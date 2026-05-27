# display-stock

ESP32-S2 firmware that turns an [Adafruit Feather ESP32-S2 Reverse TFT](https://www.adafruit.com/product/5345) into a desktop stock ticker. Rotates through a configurable list of symbols, showing price and daily percent change on the 240×135 TFT.

## Features

- First-boot captive-portal setup: collects wifi credentials, the symbol list, and per-symbol dwell seconds in one form
- Quotes from Yahoo Finance, refreshed every 60 seconds
- Auto-rotation with manual prev/next via the side buttons
- NTP-synced clock and wifi indicator in a small status bar
- Stale-data marker if a symbol fails to refresh
- Hold the BOOT button for two seconds at power-on to wipe config and re-enter setup

## Hardware

- [Adafruit Feather ESP32-S2 Reverse TFT](https://www.adafruit.com/product/5345) (240×135 ST7789, three user buttons, native USB)
- USB-C cable

## Build & flash

Requires [PlatformIO Core](https://platformio.org/install/cli).

```sh
# Build only (no upload)
pio run

# Build + upload (waits for the board to appear on /dev/cu.usbmodem*)
scripts/flash.sh

# Open the serial monitor
scripts/monitor.sh
```

### ESP32-S2 bootloader gotcha

The S2 has native USB instead of a USB-to-serial chip, so the automatic `1200bps`-reset trick PlatformIO uses to drop into the bootloader is unreliable. If `scripts/flash.sh` fails with `Failed to connect to ESP32-S2: No serial data received`, put the board into ROM bootloader mode manually:

1. Hold **BOOT** (the D0 button)
2. Press and release **RESET** while still holding BOOT
3. Release BOOT

The port name will change (e.g. `/dev/cu.usbmodem101` → `/dev/cu.usbmodem01`). Re-run `scripts/flash.sh`; press **RESET** once when it finishes to run the new firmware.

## First-boot setup

1. After flashing, the TFT shows `Setup mode` with an AP name and `192.168.4.1`.
2. From your phone or laptop, join the `StockTicker-Setup` open wifi network.
3. The captive portal page opens automatically. Pick your home wifi, enter its password, edit the symbol list (`AAPL,MSFT,GOOGL,NVDA,TSLA` by default), and set the seconds-per-symbol dwell (default `8`).
4. Save. The device reboots and starts displaying.

## Buttons

| Button | While running |
|---|---|
| **D0** (BOOT) | Force an immediate refresh |
| **D1** | Previous symbol |
| **D2** | Next symbol |
| **D0 held at boot for 2s** | Factory reset (clears wifi + symbols, re-enters setup) |

## File layout

```
src/
  main.cpp           top-level state machine + loop
  config.h           pins, colors, URLs, defaults
  storage.h/.cpp     NVS-backed config (symbols, dwell)
  wifi_setup.h/.cpp  WiFiManager wrapper + factory reset
  stock_fetcher.h/.cpp  Yahoo v8 chart endpoint client (one fetch per loop tick)
  display.h/.cpp     ST7789 drawing
  buttons.h/.cpp     debounced edge-triggered button events
scripts/
  flash.sh           wait for port, build, upload
  monitor.sh         wait for port, open serial monitor
  _wait_for_board.sh shared port-wait helper
```

## Caveats

- **Yahoo Finance has no official free API.** This uses the unofficial `query1.finance.yahoo.com/v8/finance/chart/SYMBOL` endpoint. It has been stable for years but could break at any time.
- **TLS uses `setInsecure()`** — no certificate validation. Reasonable for a personal device on a home LAN; do not deploy outside that threat model.
- Times shown are UTC by default. Edit `NTP_OFFSET_SECONDS` in `src/config.h` to use local time.

## License

No license declared. Add one if you want to make the code reusable.
