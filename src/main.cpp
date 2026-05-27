#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include "config.h"
#include "storage.h"
#include "buttons.h"
#include "display.h"
#include "wifi_setup.h"
#include "stock_fetcher.h"

namespace {
    StockConfig cfg;

    int      current_index    = 0;
    uint32_t last_advance_ms  = 0;
    uint32_t last_refresh_ms  = 0;
    uint32_t last_fetch_step_ms = 0;
    uint32_t last_status_ms   = 0;
    uint32_t dwell_ms         = 0;

    bool     refresh_active   = true;   // first cycle runs immediately at boot
    bool     have_data        = false;

    WiFiUDP   ntp_udp;
    NTPClient ntp(ntp_udp, NTP_SERVER, NTP_OFFSET_SECONDS, NTP_UPDATE_MS);

    void redraw_current() {
        if (have_data) {
            display::draw_stock(stock_fetcher::quote(current_index));
        } else {
            display::draw_no_data();
        }
    }

    void advance(bool forward) {
        int next = forward
            ? stock_fetcher::next_valid_index(current_index)
            : stock_fetcher::prev_valid_index(current_index);
        if (next < 0) return;
        current_index = next;
        display::draw_stock(stock_fetcher::quote(current_index));
    }

    // Returns true if the user held D0 long enough to trigger a factory reset.
    bool check_boot_reset_trigger() {
        if (digitalRead(PIN_BTN_D0) != LOW) return false;
        for (uint8_t s = (BOOT_HOLD_MS / 1000); s >= 1; --s) {
            display::show_factory_reset_countdown(s);
            uint32_t t0 = millis();
            while (millis() - t0 < 1000) {
                if (digitalRead(PIN_BTN_D0) != LOW) return false;
                delay(20);
            }
        }
        display::show_boot_message("Resetting...", "Releasing setup mode");
        wifi_setup::factory_reset();
        delay(500);
        return true;
    }
}

void setup() {
    Serial.begin(115200);
    delay(100);

    buttons::begin();
    display::begin();
    storage::begin();

    display::show_boot_message("StockTicker", "booting...");
    delay(300);

    check_boot_reset_trigger();

    if (!storage::load(cfg)) {
        wifi_setup::run_portal();  // does not return
    }

    dwell_ms = (uint32_t)cfg.dwell_s * 1000UL;

    display::show_boot_message("Connecting wifi", WiFi.SSID().c_str());
    if (!wifi_setup::connect(20000)) {
        display::show_boot_message("Wifi failed", "Entering setup");
        delay(1500);
        wifi_setup::run_portal();  // does not return
    }

    display::show_boot_message("Loading prices", cfg.symbols_csv.c_str());
    stock_fetcher::begin(cfg.symbols, cfg.symbol_count);
    ntp.begin();
    ntp.update();

    display::draw_no_data();

    uint32_t now = millis();
    last_advance_ms = now;
    last_refresh_ms = now;
    last_fetch_step_ms = 0;
    last_status_ms = 0;
}

void loop() {
    uint32_t now = millis();

    // Schedule a new refresh cycle.
    if (!refresh_active && now - last_refresh_ms >= REFRESH_INTERVAL_MS) {
        refresh_active = true;
    }

    // Step the refresh state machine — at most ONE HTTP request per loop iteration
    // (bounded by HTTP_TIMEOUT_MS < the loopTask WDT).
    if (refresh_active && now - last_fetch_step_ms >= FETCH_STEP_GAP_MS) {
        bool cycle_done = stock_fetcher::refresh_next();
        last_fetch_step_ms = now;

        // First successful fetch in the lifetime of the device: pivot the display
        // off the "loading" splash and start showing real data.
        if (!have_data && stock_fetcher::has_any_valid()) {
            int first = stock_fetcher::quote(current_index).valid
                            ? current_index
                            : stock_fetcher::next_valid_index(current_index);
            if (first >= 0) current_index = first;
            have_data = true;
            redraw_current();
            last_advance_ms = now;
        }

        if (cycle_done) {
            refresh_active = false;
            last_refresh_ms = now;
            // Re-draw current symbol so any updated price shows.
            if (have_data) redraw_current();
        }
    }

    stock_fetcher::update_staleness();

    // Auto-advance.
    if (have_data && now - last_advance_ms >= dwell_ms) {
        advance(true);
        last_advance_ms = now;
    }

    // Buttons.
    ButtonEvent ev = buttons::poll();
    if (ev == BTN_D2_PRESSED) {
        advance(true);
        last_advance_ms = now;
    } else if (ev == BTN_D1_PRESSED) {
        advance(false);
        last_advance_ms = now;
    } else if (ev == BTN_D0_PRESSED) {
        refresh_active = true;          // force a fresh cycle
        last_fetch_step_ms = 0;
    }

    // Status bar (wifi + clock) once a second.
    if (now - last_status_ms >= 1000) {
        bool wifi_ok = (WiFi.status() == WL_CONNECTED);
        if (!wifi_ok) WiFi.reconnect();   // nudge ESP32 auto-reconnect when it stalls
        ntp.update();
        String hhmm;
        if (wifi_ok && ntp.getEpochTime() > 100000) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%02d:%02d", ntp.getHours(), ntp.getMinutes());
            hhmm = buf;
        }
        display::draw_status_bar(wifi_ok, hhmm);
        last_status_ms = now;
    }

    delay(10);
}
