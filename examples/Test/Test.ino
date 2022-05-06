#include <ESP8266WiFi.h>
#include <SheetsLogger.h>
#include "secrets.h"

#define LOG(message, ...) sl_printf(SHEETS_URL, "Sheets Logger", ">>> [%10lu][%.5fkb] Sheets Logger: " message "\n", millis(), (system_get_free_heap_size() * 0.001f), ##__VA_ARGS__)

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