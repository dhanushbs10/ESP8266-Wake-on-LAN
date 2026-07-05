# Advanced Configuration

Advanced customization options, power optimization, and integration ideas for the ESP8266 Wake-on-LAN project.

---

## Power Saving Modes

### Deep Sleep Between Wake Events

To dramatically reduce power consumption (battery operation), implement deep sleep:

```cpp
// In loop(), after cooldown period:
if (!inCooldown && currentState == AppState::WIFI_CONNECTED) {
  Serial.println(F("Entering deep sleep..."));
  ESP.deepSleep(10e6);  // Sleep 10 seconds
}
```

**Requirements:**
- Connect `RST` pin to `D0` (GPIO16) for wake-on-interrupt
- Touch trigger wakes from deep sleep automatically

**Power consumption comparison:**

| Mode | Current Draw | Battery Life (2000mAh) |
|------|--------------|------------------------|
| Active (WiFi connected) | ~70mA | ~28 hours |
| WiFi modem sleep | ~15mA | ~5 days |
| Deep sleep | ~20µA | ~4+ years |

**Trade-off:** Deep sleep adds 1-2 second wake delay (WiFi reconnect).

---

## Multiple Target PCs

Support waking different PCs with tap patterns:

```cpp
// Add to Config.h
const int TAP_COUNT_MAX = 3;  // Support 1-tap, 2-taps, 3-taps
const byte TARGET_MACS[TAP_COUNT_MAX][6] = {
  {0x94, 0xDE, 0x80, 0x7B, 0x2F, 0x99},  // PC 1
  {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF},  // PC 2
  // Add more as needed
};

// Add tap detection logic to loop()
static int tapCount = 0;
static unsigned long lastTapTime = 0;

// When touch detected:
if (millis() - lastTapTime < 500) {
  tapCount++;
} else {
  tapCount = 1;  // New tap sequence
}
lastTapTime = millis();

// Display tap count
updateDisplay(DisplayMode::NORMAL, "Tap #" + String(tapCount));

// After 500ms of no touches, execute:
if (millis() - lastTapTime >= 500 && tapCount > 0) {
  if (tapCount <= TAP_COUNT_MAX) {
    sendWakePacket(TARGET_MACS[tapCount - 1]);  // Use selected MAC
  }
  tapCount = 0;
}
```

---

## Web Configuration Portal (Captive Portal)

For devices without a computer to configure, create a WiFi AP with web UI:

1. Add AsyncWebServer library:
   ```cpp
   #include <ESP8266WebServer.h>
   #include <DNSServer.h>
   const byte DNS_PORT = 53;
   DNSServer dnsServer;
   ESP8266WebServer webServer(80);
   ```

2. In `setup()`, start AP mode if no WiFi config:
   ```cpp
   WiFi.mode(WIFI_AP_STA);
   WiFi.softAP("ESP-WOL-Setup");
   dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
   webServer.begin();
   ```

3. Add routes for `/` (form) and `/save` (POST credentials)

4. Save to EEPROM or SPIFFS

This is an advanced topic - consider [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) library.

---

## OTA (Over-The-Air) Updates

Update firmware wirelessly, no USB cable:

```cpp
#include <ArduinoOTA.h>

void setup() {
  // Add after WiFi.begin()
  ArduinoOTA.setHostname("esp-wol");
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  // ... rest of loop
}
```

**Usage:**
1. Install ArduinoOTA library
2. Upload sketch once with new code
3. Future updates via Arduino IDE's "Upload Using Programmer" (Ctrl+Shift+U)

---

## LED Status Indicators

Add visual feedback using onboard LED or external RGB:

```cpp
const int LED_PIN = LED_BUILTIN;  // NodeMCU: D4/GPIO2 (active LOW)

// Add to setup():
pinMode(LED_PIN, OUTPUT);

// Add helper:
void setLED(bool on) {
  digitalWrite(LED_PIN, on ? LOW : HIGH);  // Active LOW on NodeMCU
}

// Use in states:
case AppState::WIFI_CONNECTING: setLED(true); break;  // Fast blink in loop
case AppState::WIFI_CONNECTED: setLED(false); break; // Solid OFF = ready
case AppState::ERROR: blinkLED(3, 100); break;
```

---

## mDNS Support

Make the device discoverable on the local network:

```cpp
#include <ESP8266mDNS.h>

void setup() {
  // After WiFi connected:
  if (MDNS.begin("wol-device")) {
    Serial.println("mDNS responder started: http://wol-device.local");
  }
}

// Can now ping: ping wol-device.local
```

---

## REST API Endpoints

For smart home integration (Home Assistant, Node-RED):

```cpp
#include <ArduinoJson.h>

ESP8266WebServer server(80);

void setup() {
  // ... existing setup

  server.on("/wake", HTTP_GET, []() {
    sendWakePacket();
    server.send(200, "application/json", "{\"status\":\"waking\"}");
  });

  server.on("/status", HTTP_GET, []() {
    DynamicJsonDocument doc(256);
    doc["wifi"] = WiFi.status() == WL_CONNECTED;
    doc["ip"] = WiFi.localIP().toString();
    doc["rssi"] = WiFi.RSSI();
    doc["uptime"] = millis();
    String out;
    serializeJson(doc, out);
    server.send(200, "application/json", out);
  });

  server.begin();
}

void loop() {
  server.handleClient();
  // ... rest of loop
}
```

**Now you can:**
- `GET http://esp-ip/wake` → Triggers WOL
- `GET http://esp-ip/status` → Returns JSON status

---

## Custom Display Graphics

Show icons instead of text using `drawBitmap()`:

```cpp
#include <bitmaps.h>  // Create your own 16x16 monochrome bitmaps

void updateDisplay(DisplayMode mode) {
  switch (mode) {
    case DisplayMode::WAKING:
      display.drawBitmap(0, 0, icon_wifi, 16, 16, SSD1306_WHITE);
      break;
  }
}
```

Generate bitmaps at: [LCD Assistant](http://en.radzio.dxp.pl/bitmap_converter/)

---

## Bluetooth / BLE Remote

Replace touch sensor with smartphone app:

```cpp
#include <BLEDevice.h>
#include <BLEServer.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic;

void setup() {
  BLEDevice::init("ESP-WOL");
  pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristic->setCallbacks(new MyCallbacks());  // Implement to trigger WOL
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();
}
```

---

## Security Enhancements

### Rolling Code or Authentication

Current implementation sends WOL to anyone who touches. Add simple challenge-response:

1. ESP generates random 4-byte nonce, displays on OLED
2. User enters via Serial (or password button sequence)
3. ESP only sends WOL if correct

### Encrypted WiFi Credentials

Store WiFi password in EEPROM encrypted, not plain text:
```cpp
#include <Crypto.h>
#include <AES.h>

// Encrypt before storing in EEPROM, decrypt on boot
```

---

## Integration with Home Assistant

Use the REST API endpoints to add as a button:

```yaml
# configuration.yaml
rest_command:
  wol_pc:
    url: http://ESP_IP_HERE/wake
    method: GET

# Then add to dashboard:
- type: button
  tap_action:
    action: call-service
    service: rest_command.wol_pc
```

---

## PCB Design

For a permanent, compact device:

1. **KiCad or EasyEDA** for schematic and PCB
2. **2-layer board** minimum
3. **Components:**
   - ESP-12E module
   - SSD1306 OLED (with pin headers)
   - 3.3V regulator (if USB not used)
   - Touch header
   - Status LED
4. **Power options:** USB micro-B, terminal blocks, or 5V barrel jack

**Schematic tips:**
- Pull-up resistors on I2C lines (4.7K ohm to 3.3V)
- 100µF capacitor across power rails
- Optional: MOSFET for touch sensor power control (power saving)

---

## Production Checklist

Before flashing 10+ devices:

- [ ] WiFi credentials validated
- [ ] MAC address correct
- [ ] Touch debounce tuned (adjust `TOUCH_DEBOUNCE_MS`)
- [ ] OLED contrast adjusted (if display has pot)
- [ ] Auto-reconnect tested (unplug WiFi for 30s)
- [ ] Power consumption measured (should be <80mA)
- [ ] IP address reserved in router (optional but helpful)
- [ ] Case designed/3D printed (optional)
- [ ] Label applied with MAC address for each device

---

## Performance Tuning

### Reduce WiFi Latency
```cpp
// In connectToWiFi():
WiFi.setSleepMode(WIFI_NONE_SLEEP);  // Disable WiFi power saving
```

### Faster Touch Response
Reduce debounce if your sensor is clean:
```cpp
const int TOUCH_DEBOUNCE_MS = 100;  // Faster, but may false-trigger
```

### Lower Power (if always connected)
```cpp
WiFi.setSleepMode(WIFI_LIGHT_SLEEP);  // Saves ~10-20mA
```

---

## Custom Serial Commands

Add hidden debug commands triggered via Serial:

```cpp
void checkSerialCommands() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "wol") sendWakePacket();
    if (cmd == "reset") ESP.restart();
    if (cmd == "config") printConfig();
    if (cmd == "scan") WiFi.scanNetworks();  // List networks
  }
}

// Call in loop(): checkSerialCommands();
```

---

## Troubleshooting Deep Sleep Issues

**Symptom:** Device sleeps but doesn't wake on touch.

**Solution:**
- Connect `RST` pin to `D0` (GPIO16) on ESP8266
- Configure wakeup:
  ```cpp
  ESP.deepSleep(sleepTime);
  // On wake, D0 must go LOW to trigger reset
  // Touch module pulls D0 HIGH, need transistor inverting circuit
  // OR use external interrupt instead
  ```

Simpler: Use `ESP.restart()` after `deepSleep()` if touch is directly on RST (wired externally).

---

## References

- [ESP8266 Arduino Core API](https://arduino-esp8266.readthedocs.io/)
- [ESP8266 Deep Sleep Tutorial](https://randomnerdtutorials.com/esp8266-deep-sleep/)
- [Home Assistant REST Command](https://www.home-assistant.io/integrations/rest_command/)
- [KiCad PCB Design](https://kicad.org/)

---

**Advanced version:** 1.0.0-advanced (2025-07-05)
