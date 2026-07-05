# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/).

---

## [1.0.0] - 2025-07-05

### Added
- Initial public release
- Core functionality: Wake-on-LAN magic packet sending
- SSD1306 OLED status display (128x64)
- Touch sensor input with debouncing
- WiFi connection with auto-reconnect
- Non-blocking `millis()`-based timing
- Comprehensive error handling
- Doxygen-style code documentation
- Configuration separated into `Config.h`
- Complete README with wiring diagrams and troubleshooting
- MIT License
- GitHub repository structure

### Technical Details
- Refactored from original Arduino sketch
- State machine design for application flow
- Serial debug output at 115200 baud
- OLED display shows WiFi status and wake progress
- Configurable debounce and timing constants
- Supports any ESP8266 board (NodeMCU, Wemos D1 Mini, etc.)
- I2C OLED display (SSD1306) support

---

## [Planned] - Future Releases

### v1.1.0 - Power Saving & UX Improvements
- [ ] Deep sleep mode between touch events (battery optimization)
- [ ] RGB status LED integration
- [ ] Improved power-on reset detection
- [ ] OTA firmware update support

### v1.2.0 - Advanced Features
- [ ] Web configuration portal (captive portal)
- [ ] Multiple target PC support
- [ ] Action toggle (wake / sleep / shutdown)
- [ ] REST API for smart home integration
- [ ] mDNS discovery (`wol.local`)

### v1.3.0 - Hardware Enhancements
- [ ] PCB design for custom boards
- [ ] Battery management for portable use
- [ ] Push-button alternative to touch
- [ ] External antenna support for better range

---

## Migration Guides

### Upgrading from Version 0.x to 1.0.0

Version 1.0.0 is a complete refactor. If you have an older version:

1. Update `Config.h` with your WiFi and MAC settings
2. Review pin assignments (TOUCH_PIN moved to Config.h)
3. Upload new sketch (may require Arduino IDE 1.8.19+)
4. OLED I2C address is now configurable (default 0x3C)
5. WiFi auto-reconnect is automatic - no code changes needed

---

## Deprecated

- `delay()` calls replaced with `millis()`-based timing (v1.0.0)
- Hard-coded configuration values moved to Config.h (v1.0.0)
- Single monolithic sketch structure (v0.x)

---

## Known Issues

- Touch sensor may be overly sensitive on some modules - adjust debounce in Config.h
- Very long WiFi outages ( > 30s) require manual reset (configurable timeout)
- OLED displays with 0x3D I2C address need manual config change
- Windows fast startup can interfere with WOL (disable for reliability)
