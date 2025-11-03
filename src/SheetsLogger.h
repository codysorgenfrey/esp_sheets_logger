#ifndef __SHEETSLOGGER_H__
#define __SHEETSLOGGER_H__

#include <Arduino.h>

#define CRITICAL_HEAP_KB 20.0f

#ifdef ESP32
#define FREE_HEAP() (esp_get_free_heap_size() * 0.001f)
#endif
#ifdef ESP8266
#define FREE_HEAP() (ESP.getFreeHeap() * 0.001f)
#endif

// Logging
#define SL_DEBUG_ERROR 0
#define SL_DEBUG_INFO 1

#define SL_DEBUG SL_DEBUG_ERROR

#if SL_DEBUG >= SL_DEBUG_ERROR
#define SL_ERROR_LINE(message, ...) printf("ERR [%7d][%.2fkb] !!!Sheets Logger!!!: " message "\n", millis(), FREE_HEAP(), ##__VA_ARGS__)
#else
#define SL_ERROR_LINE(message, ...)
#endif

#if SL_DEBUG >= SL_DEBUG_INFO
#define SL_INFO_LINE(message, ...) printf(">>> [%7d][%.2fkb] Sheets Logger: " message "\n", millis(), FREE_HEAP(), ##__VA_ARGS__)
#else
#define SL_INFO_LINE(message, ...)
#endif

int sl_printf(
  const char* host,
  const char* path,
  const char* key,
  const char* notify,
  const bool localPrint,
  const char* ns,
  const char* format,
  ...
);

#endif