#pragma once

#include <Arduino.h>

struct StockQuote;  // fwd

namespace display {
    void begin();

    // Boot/portal screens
    void show_boot_message(const char* line1, const char* line2 = nullptr);
    void show_portal_instructions(const char* ssid, const IPAddress& ip);
    void show_factory_reset_countdown(uint8_t seconds_remaining);

    // Main display
    void draw_stock(const StockQuote& q);
    void draw_no_data();
    void draw_status_bar(bool wifi_ok, const String& hhmm);
}
