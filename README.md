# movement-code

Arduino sketch for the **Arduino UNO R4 WiFi**, built and uploaded with `arduino-cli`.

## Board

| Property | Value |
|---|---|
| Board | Arduino UNO R4 WiFi |
| MCU | Renesas RA4M1 (Cortex-M4) |
| Wireless | ESP32-S3 co-processor (Wi-Fi + BLE) |
| FQBN | `arduino:renesas_uno:unor4wifi` |
| Core | `arduino:renesas_uno` |

> This board is **not** AVR-based. Do not use `arduino:avr:uno`.

## Setup
1. Install the required core:
   ```bash
   arduino-cli core update-index
   arduino-cli core install arduino:renesas_uno
   ```
2. (Optional, for Wi-Fi sketches) Install the wireless library:
   ```bash
   arduino-cli lib install WiFiS3
   ```
3. Copy `.env.example` to `.env` and configure your port
4. Run `pnpm install`

## Commands
```bash
# Build
pnpm run arduino:compile

# Upload
pnpm run arduino:upload

# Monitor
pnpm run arduino:monitor

# Clean
pnpm run arduino:clean
```

## Notes
- **Windows:** `arduino-cli` versions **0.33.0 and earlier** have a bug with the board's touch-reset during upload. Run `arduino-cli upgrade`.
- **Linux:** You may need udev rules to access the serial port without `sudo`.
- **OTA:** Standalone Over-The-Air uploads via CLI are unreliable on this board; use the Arduino Cloud for OTA.
