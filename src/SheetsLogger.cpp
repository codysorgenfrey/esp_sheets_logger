#include "SheetsLogger.h"
#ifdef ESP32
#include <WiFi.h>
#include <WiFiClientSecure.h>
#endif
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <time.h>
#endif
#include <stdarg.h>

const char* SL_CERT PROGMEM = "-----BEGIN CERTIFICATE-----\n\
MIIFWjCCA0KgAwIBAgIQbkepxUtHDA3sM9CJuRz04TANBgkqhkiG9w0BAQwFADBH\n\
MQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExM\n\
QzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIy\n\
MDAwMDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNl\n\
cnZpY2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEB\n\
AQUAA4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaM\n\
f/vo27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vX\n\
mX7wCl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7\n\
zUjwTcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0P\n\
fyblqAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtc\n\
vfaHszVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4\n\
Zor8Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUsp\n\
zBmkMiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOO\n\
Rc92wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYW\n\
k70paDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+\n\
DVrNVjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgF\n\
lQIDAQABo0IwQDAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNV\n\
HQ4EFgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBADiW\n\
Cu49tJYeX++dnAsznyvgyv3SjgofQXSlfKqE1OXyHuY3UjKcC9FhHb8owbZEKTV1\n\
d5iyfNm9dKyKaOOpMQkpAWBz40d8U6iQSifvS9efk+eCNs6aaAyC58/UEBZvXw6Z\n\
XPYfcX3v73svfuo21pdwCxXu11xWajOl40k4DLh9+42FpLFZXvRq4d2h9mREruZR\n\
gyFmxhE+885H7pwoHyXa/6xmld01D1zvICxi/ZG6qcz8WpyTgYMpl0p8WnK0OdC3\n\
d8t5/Wk6kjftbjhlRn7pYL15iJdfOBL07q9bgsiG1eGZbYwE8na6SfZu6W0eX6Dv\n\
J4J2QPim01hcDyxC2kLGe4g0x8HYRZvBPsVhHdljUEn2NIVq4BjFbkerQUIpm/Zg\n\
DdIx02OYI5NaAIFItO/Nis3Jz5nu2Z6qNuFoS3FJFDYoOj0dzpqPJeaAcWErtXvM\n\
+SUWgeExX6GjfhaknBZqlxi9dnKlC54dNuYvoS++cJEPqOba+MSSQGwlfnuzCdyy\n\
F62ARPBopY+Udf90WuioAnwMCeKpSwughQtiue+hMZL77/ZRBIls6Kl0obsXs7X9\n\
SQ98POyDGCBDTtWTurQ0sR8WNh8M5mQ5Fkzc4P4dyKliPUDqysU0ArSuiYgzNdws\n\
E3PYJ/HQcu51OyLemGhmW/HGY0dVHLqlCFF1pkgl\n\
-----END CERTIFICATE-----\n";

void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  SL_INFO_LINE("Waiting for NTP time sync.");
  time_t now = time(nullptr);
  while (now < 57600) {
    delay(500);
    now = time(nullptr);
  }
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  SL_INFO_LINE("Current time: %s", asctime(&timeinfo));
}

int stripNewlines(char* s) {
  int len = 0;
  char* d = s; // destination pointer
  while (*s) {
    if (*s != '\n' && *s != '\r') {
      *d++ = *s;
      len++;
    }
    s++;
  }
  *d = '\0'; // terminate
  return len;
}

void sheetLog(
  const char* host,
  const char* path,
  const char* key,
  const char* notify,
  const char* ns,
  const char* message
) {
  if (WiFi.status() != WL_CONNECTED) {
    SL_ERROR_LINE("Not connected to WiFi.");
    return;
  }
  if (FREE_HEAP() < CRITICAL_HEAP_KB) {
    SL_ERROR_LINE("Not enough heap to send log.");
    return;
  }

  WiFiClientSecure client;

  #ifdef ESP32
  client.setCACert(SL_CERT);
  #else
  // lean TLS settings
  client.setBufferSizes(512, 512); // small buffers to save RAM is why there's no response body read
  client.setSSLVersion(BR_TLS12, BR_TLS12);
  
  X509List certs;
  certs.append(SL_CERT);
  client.setTrustAnchors(&certs);
  setClock();
  #endif

  if (!client.connect(host, 443)) {
    SL_ERROR_LINE("TLS connect failed");
    return;
  }

  char body[256];
  snprintf(
    body,
    sizeof(body),
    "key=%s&notify=%s&device=%s&message=%s",
    key,
    notify != NULL ? notify : "",
    ns,
    message
  );
  int len = stripNewlines(body);
  SL_INFO_LINE("Body len: %i", len);

  // Build minimal HTTP/1.1 request
  client.printf("POST %s HTTP/1.1\r\n", path);
  client.printf("Host: %s\r\n", host);
  client.print(F("Connection: close\r\n"));
  client.print(F("Content-Type: application/x-www-form-urlencoded\r\n"));
  client.printf("Content-Length: %u\r\n\r\n", (unsigned)len);

  client.write((const uint8_t*)body, len);

  client.setTimeout(15000); // 15 seconds

  while (!client.available() && client.connected()) {
    delay(1);
  }

  // Drain & close, don't care about response
  while (client.connected() || client.available()) client.read();
  client.stop();
}

int sl_printf(
  const char* host,
  const char* path,
  const char* key,
  const char* notify,
  const bool localPrint,
  const char* ns,
  const char* format,
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
    sheetLog(host, path, key, notify, ns, cloudMsg);
    if (localPrint) printf(cloudMsg);
  }

  va_end(args);
  return size;
}