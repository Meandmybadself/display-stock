#include "wifi_setup.h"
#include "config.h"
#include "storage.h"
#include "display.h"

#include <WiFi.h>
#include <WiFiManager.h>

namespace wifi_setup {

[[noreturn]] void run_portal() {
    WiFiManager wm;

    WiFiManagerParameter symbols_param(
        "symbols", "Stock symbols (comma-separated)",
        DEFAULT_SYMBOLS, 200);
    WiFiManagerParameter dwell_param(
        "dwell", "Seconds per symbol (2-600)",
        DEFAULT_DWELL_S, 4);
    WiFiManagerParameter hint_param(
        "<p style='font-size:12px;color:#666'>"
        "After saving, the device reboots and starts displaying."
        "</p>");

    wm.addParameter(&symbols_param);
    wm.addParameter(&dwell_param);
    wm.addParameter(&hint_param);

    wm.setConfigPortalTimeout(PORTAL_TIMEOUT_S);
    wm.setBreakAfterConfig(true);   // exit portal after wifi save (so we can persist params)
    wm.setTitle("StockTicker");

    // Draw setup instructions on TFT once the AP is actually up.
    wm.setAPCallback([](WiFiManager*) {
        display::show_portal_instructions(PORTAL_AP_SSID, WiFi.softAPIP());
    });

    display::show_boot_message("Starting setup", "AP coming up...");

    bool saved = wm.startConfigPortal(PORTAL_AP_SSID);

    if (saved) {
        String symbols = symbols_param.getValue();
        String dwell_s = dwell_param.getValue();
        symbols.trim();
        dwell_s.trim();
        if (symbols.length() == 0) symbols = DEFAULT_SYMBOLS;
        uint32_t dwell = (uint32_t)dwell_s.toInt();
        if (dwell == 0) dwell = (uint32_t)String(DEFAULT_DWELL_S).toInt();
        storage::save(symbols, (uint16_t)dwell);
        display::show_boot_message("Saved.", "Restarting...");
    } else {
        // Timed out without input.
        display::show_boot_message("Setup timed out", "Restarting...");
    }

    delay(1500);
    ESP.restart();
    while (true) { delay(1000); }
}

void factory_reset() {
    WiFiManager wm;
    wm.resetSettings();              // wipes wifi creds in NVS
    storage::clear();                // wipes symbols/dwell
}

bool connect(uint32_t timeout_ms) {
    WiFi.mode(WIFI_STA);
    WiFi.begin();                    // uses creds saved by WiFiManager / esp32 wifi nvs
    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < timeout_ms) {
        delay(200);
    }
    return WiFi.status() == WL_CONNECTED;
}

} // namespace wifi_setup
