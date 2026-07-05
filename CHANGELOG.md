# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/).

---

## [1.0.0] - 2025-07-05

### Added
- Initial public release
- Core functionality: Wake-on-LAN magic packet sending
- **16x2 I2C LCD status display** (PCF8574 backpack, address 0x27)
- **TTP223 capacitive touch sensor** on GPIO12 (NodeMCU D6) with debouncing
- WiFi connection with auto-reconnect
- Non-blocking `millis()`-based timing throughout
- Comprehensive error handling
- Doxygen-style code documentation
- Configuration separated into `Config.h`
- Complete README with wiring diagrams, troubleshooting, and LCD-specific instructions
- MIT License
- GitHub repository structure

### Hardware
- **Board:** NodeMCU ESP8266 (ESP-12E)
- **Display:** 16x2 LCD with I2C interface (5V power required)
- **Sensor:** TTP223 capacitive touch module (3.3V power)
- **Pin mapping:** Touch → D6 (GPIO12), LCD SDA → D2 (GPIO4), LCD SCL → D1 (GPIO5)
- **LCD I2C address:** Default 0x27 (configurable, 0x3F also common)

### Technical Details
- Refactored from original Arduino sketch (button proj.txt)
- State machine design for application flow
- Serial debug output at 115200 baud
- LCD displays two lines: WiFi status + current action
- Display caching prevents flicker (only updates when content changes)
- Configurable debounce (200ms default) and timing constants
- Works with any ESP8266 board, but NodeMCU pinouts documented
- I2C LCD support (LiquidCrystal_I2C library)

---

## [Planned] - Future Releases

### v1.1.0 - Power Saving & UX Improvements
- [ ] Deep sleep mode between touch events (battery optimization)
- [ ] RGB status LED integration
- [ ] Improved power-on reset detection
- [ ] OTA firmware update support

### v1.2.0 - Advanced Features
- [ ] Web configuration portal (captive portal)
- [ ] Multiple target PC support (different tap patterns)
- [ ] Action toggle (wake / sleep / shutdown)
- [ ] REST API for smart home integration
- [ ] mDNS discovery (`wol.local`)

### v1.3.0 - Hardware Enhancements
- [ ] PCB design for custom boards (LCD + ESP-12E)
- [ ] Battery management for portable use (charging circuit)
- [ ] Push-button alternative to touch
- [ ] External antenna support for better WiFi range

---

## Migration Guides

### Upgrading from Version 0.x (Original) to 1.0.0

Version 1.0.0 is a complete refactor with hardware change from SSD1306 OLED to 16x2 LCD.

Key changes:
1. **Display:** OLED replaced with 16x2 I2C LCD
2. **Touch pin:** Changed from D5 (GPIO14) to D6 (GPIO12)
3. **LCD I2C address:** Now configurable in `Config.h` (default 0x27)
4. **Libraries:** Requires `LiquidCrystal_I2C` instead of `Adafruit_SSD1306`
5. **Power:** LCD needs 5V (use VIN), touch needs 3.3V
6. **WiFi auto-reconnect:** Automatic, no configuration needed
7. **All code rewritten** with state machine and `millis()` timing

If upgrading from original:
- Update wiring to match new pinout
- Change libraries in Arduino IDE
- Edit `Config.h` with your settings
- Upload new sketch

---

## Deprecated

- `delay()` calls replaced with `millis()`-based timing (v1.0.0)
- Hard-coded configuration values moved to `Config.h` (v1.0.0)
- SSD1306 OLED replaced with 16x2 I2C LCD (v1.0.0)
- Single monolithic sketch structure (v0.x)

---

## Known Issues

- LCD shows garbled text if I2C address wrong (try 0x27 or 0x3F)
- Touch sensor may be overly sensitive on some modules - adjust debounce in `Config.h`
- Very long WiFi outages (>30s) require manual reset (configurable timeout)
- LCD I2C modules vary - some need external pull-up resistors
- Windows fast startup can interfere with WOL (disable for reliability)
- LCD backlight may be dim if powered from 3.3V - always use 5V

---

**Last updated:** 2025-07-05
