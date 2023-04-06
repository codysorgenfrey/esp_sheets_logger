#include "SheetsLogger.h"
#ifdef ESP32
    #include <WiFi.h>
    #include <HTTPClient.h>
    #include <WiFiClientSecure.h>
#endif
#ifdef ESP8266
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
    #include <WiFiClientSecureBearSSL.h>
    #include <time.h>
#endif
#include <stdarg.h>

void setClock() {
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    SL_INFO_LINE("Waiting for NTP time sync.");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        delay(500);
        now = time(nullptr);
    }
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    SL_INFO_LINE("Current time: %s", asctime(&timeinfo));
}

void sheetLog(
    const char *url,
    const char *key,
    const char *notify,
    const char *ns,
    const char *message
) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        HTTPClient https;

        bool began = false;
        #ifdef ESP32
            client.setCACert(SL_CERT);
        #else
            X509List certs;
            certs.append(SL_CERT);
            client.setTrustAnchors(&certs);
            setClock();
        #endif
        
        if (https.begin(client, url)) {
            https.addHeader("Content-Type", "application/x-www-form-urlencoded");

            String payload = "key=";
            payload += key;
            if (notify != NULL) {
                payload += "&notify=";
                payload += notify;
            }
            payload += "&device=";
            payload += ns;
            payload += "&message=";
            payload += message;
            payload.replace("\r\n", " ");
            payload.replace("\n", " ");

            SL_INFO_LINE("Posting payload: %s", payload.c_str());
            int res = https.POST(payload);

            if (res >= 200 && res <= 299) SL_INFO_LINE("%s", https->getString().c_str());
            else SL_ERROR_LINE("Error posting payload. Response code: %i", res);

            client.stop();
            https.end();
        } else {
            SL_ERROR_LINE("Could not connect to %s", url);
        }
    } else {
        SL_ERROR_LINE("Not connected to WiFi.");
    }
}

int sl_printf(
    const char *url,
    const char *key,
    const char *notify,
    const bool localPrint,
    const char *ns,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);

    int size = vsnprintf(NULL, 0, format, args);
    SL_INFO_LINE("Size: %i", size + 1);
    char cloudMsg[size + 1]; // +1 for the terminating char
    SL_INFO_LINE("Size of cldMsg: %i", sizeof(cloudMsg));
    size = vsnprintf(cloudMsg, sizeof(cloudMsg), format, args);
    if (size >= 0) {
        sheetLog(url, key, notify, ns, cloudMsg);
        if (localPrint) printf(cloudMsg);
    }

    va_end(args);
    return size;
}