#include <WiFi.h>
#include <SheetsLogger.h>
#include "secrets.h"

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }; // wait for serial
    Serial.println("Starting...");

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    Serial.println("Connected.");

    sheetLog(SHEETS_URL, "Test.ino", "This is a test log from ESP32.");
}

void loop() {}