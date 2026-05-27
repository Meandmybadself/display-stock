#pragma once

#include <Arduino.h>
#include <Adafruit_ST77xx.h>

// --- Board pins (Adafruit Feather ESP32-S2 Reverse TFT) ---
constexpr uint8_t PIN_TFT_BACKLIGHT  = TFT_BACKLITE;
constexpr uint8_t PIN_TFT_I2C_POWER  = TFT_I2C_POWER;
constexpr uint8_t PIN_BTN_D0         = 0;  // active LOW (BOOT button, internal pull-up)
constexpr uint8_t PIN_BTN_D1         = 1;  // active HIGH
constexpr uint8_t PIN_BTN_D2         = 2;  // active HIGH

// --- Display geometry (240x135 landscape rotation 3) ---
constexpr uint16_t TFT_WIDTH         = 240;
constexpr uint16_t TFT_HEIGHT        = 135;
constexpr uint8_t  TFT_ROTATION      = 3;

// --- Captive portal ---
constexpr const char* PORTAL_AP_SSID = "StockTicker-Setup";
constexpr uint16_t PORTAL_TIMEOUT_S  = 600;  // 10 min then reboot to retry

// --- Defaults the portal pre-fills ---
constexpr const char* DEFAULT_SYMBOLS = "AAPL,MSFT,GOOGL,NVDA,TSLA";
constexpr const char* DEFAULT_DWELL_S = "8";

// --- Bounds ---
constexpr uint8_t  MAX_SYMBOLS       = 16;
constexpr uint8_t  MAX_SYMBOL_LEN    = 10;
constexpr uint16_t MIN_DWELL_S       = 2;
constexpr uint16_t MAX_DWELL_S       = 600;

// --- Network / data ---
constexpr uint32_t REFRESH_INTERVAL_MS = 60UL * 1000UL;
constexpr uint32_t STALE_THRESHOLD_MS  = 5UL * 60UL * 1000UL;
// HTTP timeout MUST stay below the loopTask WDT (~5s) since fetch is called from loop().
constexpr uint32_t HTTP_TIMEOUT_MS     = 4000;
constexpr uint32_t FETCH_STEP_GAP_MS   = 250;  // pacing between per-symbol fetches
constexpr const char* YAHOO_HOST       = "query1.finance.yahoo.com";
constexpr const char* YAHOO_PATH_FMT   = "/v8/finance/chart/%s?interval=1d&range=1d";
constexpr const char* USER_AGENT       = "Mozilla/5.0 (StockTickerESP32)";

// --- NTP ---
constexpr const char* NTP_SERVER       = "pool.ntp.org";
constexpr long NTP_OFFSET_SECONDS      = 0;  // display in UTC; user can edit if they want local
constexpr unsigned long NTP_UPDATE_MS  = 60UL * 60UL * 1000UL;  // hourly resync

// --- Reset-trigger detection ---
constexpr uint32_t BOOT_HOLD_MS        = 2000;

// --- Colors (RGB565) ---
constexpr uint16_t COLOR_BG            = 0x0000;   // black
constexpr uint16_t COLOR_FG            = 0xFFFF;   // white
constexpr uint16_t COLOR_DIM           = 0x7BEF;   // gray
constexpr uint16_t COLOR_UP            = 0x07E0;   // green
constexpr uint16_t COLOR_DOWN          = 0xF800;   // red
constexpr uint16_t COLOR_ACCENT        = 0x07FF;   // cyan
constexpr uint16_t COLOR_WARN          = 0xFD20;   // amber
