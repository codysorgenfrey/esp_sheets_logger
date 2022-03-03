#include "SheetsLogger.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <stdarg.h>

void sheetLog(const char *url, const char *ns, const char *message) {
    if (WiFi.status() != WL_CONNECTED) {
        SL_ERROR_LINE("Not connected to WiFi.");
        SL_ERROR_LINE("Message: %s", message);
        return;
    }

    WiFiClientSecure *client = new WiFiClientSecure();
    HTTPClient *https = new HTTPClient();

    client->setCACert(SL_CERT);
    if (!https->begin(*client, url)) {
        SL_ERROR_LINE("Could not connect to %s", url);
        return;
    }

    https->addHeader("Content-Type", "application/json");

    String payload = "{\"device\":\"" + String(ns) + "\",\"message\":\"" + message + "\",\"secret\":\"" + SL_SECRET + "\"}";

    payload.replace("\r\n", " ");
    payload.replace("\n", " ");

    SL_INFO_LINE("Posting payload: %s", payload.c_str());
    int res = https->POST(payload);

    if (res < 200 || res > 299) {
        SL_ERROR_LINE("Error posting payload. Response code: %i", res);
        return;
    } else {
        SL_INFO_LINE("%s", https->getString().c_str());
    }

    client->stop();
    https->end();
    delete client;
    delete https;
}

int sl_printf(const char *url, const char *ns, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int size = vsnprintf(NULL, 0, format, args);
    SL_INFO_LINE("Size: %i", size + 1);
    char cloudMsg[size + 1]; // +1 for the terminating char
    SL_INFO_LINE("Size of cldMsg: %i", sizeof(cloudMsg));
    size = vsnprintf(cloudMsg, sizeof(cloudMsg), format, args);
    if (size >= 0) {
        sheetLog(url, ns, cloudMsg);
        printf(cloudMsg);
    }

    va_end(args);
    return size;
}