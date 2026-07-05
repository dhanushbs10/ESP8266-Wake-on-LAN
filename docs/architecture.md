# Project Architecture

This document explains the software architecture, design patterns, and code structure of the ESP8266 Wake-on-LAN project.

---

## System Overview

```
┌─────────────────────────────────────────────────────────────┐
│                      ESP8266 Microcontroller                 │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┐        ┌────────────────────────────┐ │
│  │    Config.h     │◄───────┤    ESP8266-Wake-on-LAN     │ │
│  │ (User Settings) │        │        .ino File            │ │
│  └─────────────────┘        │                            │ │
│                              │  ┌──────────────────────┐  │ │
│                              │  │   State Machine      │  │ │
│                              │  │  ┌──────────────┐    │  │ │
│                              │  │  │ INITIALIZING │    │  │ │
│                              │  │  │ WIFI_        │    │  │ │
│                              │  │  │ CONNECTING   │◄───┘  │ │
│                              │  │  │ WIFI_        │      │ │
│                              │  │  │ CONNECTED    │      │ │
│                              │  │  │ WIFI_        │      │ │
│                              │  │  │ DISCONNECTED │      │ │
│                              │  │  │ WAKE_        │      │ │
│                              │  │  │ TRIGGERED    │      │ │
│                              │  │  │ ERROR        │      │ │
│                              │  │  └──────────────┘    │  │
│                              │  └──────────────────────┘  │ │
│                              │         │                  │ │
│                              │         ▼                  │ │
│                              │  ┌──────────────────────┐  │ │
│                              │  │  Touch Debouncer     │  │ │
│                              │  │  WiFi Handler        │  │ │
│                              │  │  Display Manager     │  │ │
│                              │  │  WOL Sender          │  │ │
│                              │  └──────────────────────┘  │ │
│                              └─────────────┬──────────────┘ │
│                                            │                │
├────────────────────────────────────────────┼────────────────┤
│                 Hardware Layer               │                │
├────────────────────────────────────────────┼────────────────┤
│  Touch Sensor    OLED Display     WiFi      │   WOL Packets  │
└────────────────────────────────────────────┴────────────────┘
```

---

## Code Structure

### Main Components

#### 1. **Config.h** - Configuration Layer
All user-customizable settings in one place:
- WiFi credentials
- Target MAC address
- Pin assignments
- Timing constants
- Display settings

**Design principle:** Configuration separated from logic for easy customization.

#### 2. **ESP8266-Wake-on-LAN.ino** - Application Layer

##### Global Objects
```cpp
Adafruit_SSD1306 display  // OLED display driver
WiFiUDP udp                // UDP socket
WakeOnLan WOL(udp)         // WOL packet generator
```

##### State Machine (`AppState` enum)
The application uses a **finite state machine** (FSM) design:

| State | Description | Transitions To |
|-------|-------------|----------------|
| `INITIALIZING` | Startup, hardware init | WIFI_CONNECTING (or ERROR) |
| `WIFI_CONNECTING` | WiFi connection attempt | WIFI_CONNECTED or ERROR |
| `WIFI_CONNECTED` | Ready state, waiting for touch | WAKE_TRIGGERED |
| `WIFI_DISCONNECTED` | WiFi lost, reconnect active | WIFI_CONNECTING |
| `WAKE_TRIGGERED` | Sending WOL packet | WIFI_CONNECTED |
| `ERROR` | Hardware/config error | Manual reset |

**Benefits:** Predictable behavior, easy to debug, clear flow.

##### Display Modes (`DisplayMode` enum)
- `NORMAL`: Shows WiFi status and idle message
- `WAKING`: Shows wake-up animation
- `ERROR`: Shows error text

##### Key Functions

| Function | Purpose | Blocking? |
|----------|---------|-----------|
| `setup()` | Initialize hardware, connect WiFi | Yes (short) |
| `loop()` | Main state machine, non-blocking | No |
| `updateDisplay()` | Render OLED with caching | No (partial) |
| `checkTouchSensor()` | Debounced edge detection | No |
| `sendWakePacket()` | Send WOL magic packet | Yes (brief) |
| `handleWiFiConnection()` | Auto-reconnect logic | No |
| `connectToWiFi()` | Blocking WiFi connect with timeout | Yes |

---

## Design Patterns

### 1. **State Pattern**
The `AppState` and `DisplayMode` enums encapsulate different application states, making the code easier to extend and modify.

### 2. **Singleton Display**
The OLED display is a global object initialized once. Draw operations use caching (compare before clear) to prevent flicker.

### 3. **Debounced Input**
Touch sensor uses edge detection + time-based debounce:
```cpp
if (currentReading == HIGH && lastReading == LOW) {
  if (millis() - lastChange >= DEBOUNCE_MS) {
    // Valid press
  }
}
```

### 4. **Timer-Based Non-Blocking Code**
All delays use `millis()` instead of `delay()`:
```cpp
// Instead of: delay(3000);
if (millis() - startTime >= 3000) { /* done */ }
```

This allows WiFi background tasks to run smoothly.

### 5. **Configuration Centralization**
All tunable parameters live in `Config.h`. No magic numbers scattered in code.

---

## WiFi Auto-Reconnect Mechanism

```
┌────────────────────────────────────────────────┐
│   Every loop():                                │
│   ┌──────────────────────────────────────────┐│
│   │ WiFi.status() == WL_CONNECTED?          ││
│   │       YES → State = WIFI_CONNECTED      ││
│   │       NO  → Try reconnect every 1000ms  ││
│   └──────────────────────────────────────────┘│
│                                                │
│   If reconnect succeeds:                       │
│   - Update WOL broadcast address              │
│   - Change state to WIFI_CONNECTED            │
│   - Update display                            │
└────────────────────────────────────────────────┘
```

---

## Wake-on-LAN Packet Structure

WakeOnLan library handles this, but for reference:

**Magic Packet Format:**
- 6 bytes of `0xFF` (sync stream)
- Target MAC repeated 16 times (96 bytes total)
- UDP port 9 (discard port) or 7 (echo)

Our configuration:
```cpp
WOL.setRepeat(3, 100);  // Send 3 times, 100ms between packets
```

---

## Display Refresh Strategy

To prevent OLED flicker and reduce CPU:

1. **Caching:** Last status/action strings are stored
2. **Conditional update:** Only redraw if content changed
3. **Batch operations:** Clear once, write all text, then display()

```cpp
if (statusText != lastStatus || actionText != lastAction) {
  clearDisplay();
  drawEverything();
  display();
}
```

---

## Error Handling Strategy

| Error Type | Detection | Response |
|------------|-----------|----------|
| OLED init fail | `display.begin()` returns false | Set ERROR state, halt |
| WiFi timeout | `connectToWiFi()` returns false | Set ERROR state |
| WiFi disconnect | `WiFi.status() != WL_CONNECTED` | Auto-reconnect state |
| Invalid pin | `digitalRead()` on unconfigured pin | Undefined - avoid |

Errors are logged to Serial at 115200 baud.

---

## Memory Usage Estimates

| Resource | Usage | Notes |
|----------|-------|-------|
| Flash | ~150-200 KB | Code + libraries |
| RAM | ~15-25 KB | Global buffers, display cache |
| Stack | ~2 KB | Function call overhead |

**Note:** ESP8266 has ~80KB RAM, ~4MB flash. This sketch is well within limits.

---

## Performance Considerations

- **Loop time:** ~1-10ms (mostly WiFi background processing)
- **Touch latency:** Debounce (200ms) + state processing (<10ms)
- **WOL send:** ~300ms (3 packets × 100ms interval)
- **Display refresh:** ~50ms per update
- **WiFi reconnect:** Up to 30s timeout (configurable)

---

## Testing Strategy

### Unit Testing (not implemented)
- Test debounce logic with simulated inputs
- Test state transitions programmatically
- Validate MAC address parsing

### Integration Testing (manual)
1. Upload sketch, verify Serial output
2. Verify OLED shows "CONNECTED" after WiFi
3. Press touch, verify WOL packet on network analyzer
4. Unplug WiFi, verify auto-reconnect
5. Disconnect touch sensor, verify ERROR state

---

## Extensibility Points

Easy modifications for users:

| Feature | Where to modify | Difficulty |
|---------|----------------|------------|
| Change touch pin | `Config.h: TOUCH_PIN` | Easy |
| Adjust debounce | `Config.h: TOUCH_DEBOUNCE_MS` | Easy |
| Change OLED I2C address | `Config.h: OLED_I2C_ADDRESS` | Easy |
| Add LED indicator | Add `pinMode(LED_PIN, OUTPUT)` in setup() | Easy |
| Deep sleep mode | Modify `loop()` to call `ESP.deepSleep()` | Medium |
| Web interface | Add AsyncWebServer library, handlers | Hard |
| OTA updates | Add ArduinoOTA library calls | Medium |
| Multiple PCs | State tracking, different MACs per tap count | Medium |

---

## References

- [ESP8266 Arduino Core](https://arduino-esp8266.readthedocs.io/)
- [Adafruit SSD1306](https://learn.adafruit.com/monochrome-oled-breakouts/overview)
- [WakeOnLan Library](https://github.com/Team-Groump/WakeOnLan)
- [Wake-on-LAN RFC](https://tools.ietf.org/html/rfc5427)

---

**Architecture version:** 1.0.0 (2025-07-05)
