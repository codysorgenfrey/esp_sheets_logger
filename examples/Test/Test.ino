#ifdef ESP32
    #include <WiFi.h>
#endif
#ifdef ESP8266
    #include <ESP8266WiFi.h>
#endif
#include <SheetsLogger.h>
#include "secrets.h"

#ifdef ESP32
    #define LOG(message, ...) sl_printf(SHEETS_URL, SHEETS_KEY, NULL, true, "Sheets Logger", ">>> [%10lu][%.5fkb] Sheets Logger: " message "\n", millis(), (ESP.getFreeHeap() * 0.001f), ##__VA_ARGS__)
#endif
#ifdef ESP8266
    #define LOG(message, ...) sl_printf(SHEETS_URL, SHEETS_KEY, NULL, true, "Sheets Logger", ">>> [%10lu][%.5fkb] Sheets Logger: " message "\n", millis(), (system_get_free_heap_size() * 0.001f), ##__VA_ARGS__)
#endif

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }; // wait for serial
    Serial.println("Starting...");

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    Serial.println("Connected.");

    LOG("This is a test log from ESP32.");
}

void loop() {
    if (millis() % 30000 == 0) {
        LOG("It's been awhile...");
    }
}