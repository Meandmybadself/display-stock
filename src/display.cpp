#include "display.h"
#include "config.h"
#include "stock_fetcher.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <IPAddress.h>

namespace {
    Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);

    constexpr int16_t STATUS_BAR_H = 12;

    void center_print(int16_t y, uint8_t size, uint16_t color, const String& text) {
        tft.setTextSize(size);
        tft.setTextColor(color);
        int16_t  x1, y1;
        uint16_t w, h;
        tft.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
        int16_t x = (TFT_WIDTH - (int16_t)w) / 2 - x1;
        tft.setCursor(x, y);
        tft.print(text);
    }

    String format_price(float price) {
        char buf[16];
        if (price >= 10000.0f)     snprintf(buf, sizeof(buf), "$%.0f", price);
        else if (price >= 100.0f)  snprintf(buf, sizeof(buf), "$%.1f", price);
        else                       snprintf(buf, sizeof(buf), "$%.2f", price);
        return String(buf);
    }

    String format_pct(float pct) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%c%.2f%%", (pct >= 0 ? '+' : '-'), fabsf(pct));
        return String(buf);
    }

    // Erase the body region (everything below the status bar) without flicker on the bar itself.
    void clear_body() {
        tft.fillRect(0, STATUS_BAR_H, TFT_WIDTH, TFT_HEIGHT - STATUS_BAR_H, COLOR_BG);
    }
}

namespace display {

void begin() {
    // Reverse TFT Feather: powering the I2C/peripheral rail also enables the display backlight rail
    pinMode(PIN_TFT_I2C_POWER, OUTPUT);
    digitalWrite(PIN_TFT_I2C_POWER, HIGH);
    pinMode(PIN_TFT_BACKLIGHT, OUTPUT);
    digitalWrite(PIN_TFT_BACKLIGHT, HIGH);
    delay(10);

    tft.init(135, 240);
    tft.setRotation(TFT_ROTATION);
    tft.fillScreen(COLOR_BG);
    tft.setTextWrap(false);
}

void show_boot_message(const char* line1, const char* line2) {
    tft.fillScreen(COLOR_BG);
    center_print(TFT_HEIGHT / 2 - 20, 2, COLOR_FG, line1);
    if (line2) {
        center_print(TFT_HEIGHT / 2 + 10, 1, COLOR_DIM, line2);
    }
}

void show_portal_instructions(const char* ssid, const IPAddress& ip) {
    tft.fillScreen(COLOR_BG);
    center_print(8,  2, COLOR_ACCENT, "Setup mode");
    center_print(40, 1, COLOR_FG,     "Connect phone to wifi:");
    center_print(56, 2, COLOR_FG,     ssid);
    center_print(86, 1, COLOR_DIM,    "Then open browser to:");
    center_print(102,1, COLOR_FG,     ip.toString());
    center_print(120,1, COLOR_DIM,    "(captive portal auto-pops)");
}

void show_factory_reset_countdown(uint8_t seconds_remaining) {
    tft.fillScreen(COLOR_BG);
    center_print(20, 2, COLOR_WARN, "Hold D0 to");
    center_print(48, 2, COLOR_WARN, "reset config");
    char buf[8];
    snprintf(buf, sizeof(buf), "%u", seconds_remaining);
    center_print(85, 4, COLOR_FG, buf);
}

void draw_stock(const StockQuote& q) {
    clear_body();

    // Symbol top
    String sym = q.symbol;
    if (q.stale) sym += " *";
    center_print(STATUS_BAR_H + 6, 3, COLOR_DIM, sym);

    // Price middle (huge)
    center_print(STATUS_BAR_H + 36, 5, COLOR_FG, format_price(q.price));

    // % change bottom, color-coded
    uint16_t color = q.change_pct >= 0.0f ? COLOR_UP : COLOR_DOWN;
    String pct = format_pct(q.change_pct);
    center_print(TFT_HEIGHT - 24, 3, color, pct);
}

void draw_no_data() {
    clear_body();
    center_print(TFT_HEIGHT / 2 - 16, 2, COLOR_DIM, "Loading prices");
    center_print(TFT_HEIGHT / 2 + 8,  1, COLOR_DIM, "...");
}

void draw_status_bar(bool wifi_ok, const String& hhmm) {
    tft.fillRect(0, 0, TFT_WIDTH, STATUS_BAR_H, COLOR_BG);
    tft.setTextSize(1);
    tft.setTextColor(wifi_ok ? COLOR_ACCENT : COLOR_WARN);
    tft.setCursor(4, 2);
    tft.print(wifi_ok ? "wifi" : "off");
    if (hhmm.length() > 0) {
        tft.setTextColor(COLOR_DIM);
        int16_t  x1, y1;
        uint16_t w, h;
        tft.getTextBounds(hhmm, 0, 2, &x1, &y1, &w, &h);
        tft.setCursor(TFT_WIDTH - (int16_t)w - 4, 2);
        tft.print(hhmm);
    }
    tft.drawFastHLine(0, STATUS_BAR_H - 1, TFT_WIDTH, COLOR_DIM);
}

} // namespace display
