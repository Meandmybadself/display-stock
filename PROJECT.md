## Hardware
Adafruit Feather ESP32-S2 Rev. TFT, attached via USB.

## Project Description
Using the screen on the Adafruit Feather, take a list of stock symbols & display one at a time:
- The stock symbol
- The current stock price
- Its daily percentage change

At startup, it should:
- Check to see if it has wifi credentials persisted.
  - If it does not, it should spawn a captive portal to capture:
    - wifi ssid
    - wifi password
    - stock symbols, separated by comma
    - time (in seconds) to show each stock
  - If it does, it should go immediately to display mode.

## Tech
- Use platform io to interact with the board.
  - Create helper scripts to write to board.  It should wait for the board to become available & then write.
- Use Yahoo Finance to load stock data
