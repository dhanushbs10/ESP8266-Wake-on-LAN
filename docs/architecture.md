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
│                              │  │  │ SETUP        │    │  │ │
│                              │  │  │ WIFI_CONNECT │◄───┘  │ │
│                              │  │  │ READY        │      │ │
│                              │  │  │ WAKING       │      │ │
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
│  Touch Sensor    16x2 LCD       WiFi      │   WOL Packets  │
└────────────────────────────────────────────┴────────────────┘
```

---

## Code Structure

### Main Components

#### 1. **Config.h** - Configuration Layer
All user-customizable settings in one place:
- WiFi credentials (`WIFI_SSID`, `WIFI_PASSWORD`)
- Target MAC address (`TARGET_MAC[6]`)
- Pin assignments (`TOUCH_PIN = 12` for D6)
- LCD settings (`LCD_I2C_ADDRESS`, `LCD_COLS`, `LCD_ROWS`)
- Timing constants (debounce, delays, timeouts)

**Design principle:** Configuration separated from logic for easy customization. Users never need to modify the `.ino` file.

#### 2. **ESP8266-Wake-on-LAN.ino** - Application Layer

##### Global Objects
```cpp
LiquidCrystal_I2C lcd    // 16x2 LCD driver (I2C address 0x27)
WiFiUDP udp              // UDP socket
WakeOnLan WOL(udp)       // WOL packet generator
```

##### State Machine (`State` enum)
The application uses a **finite state machine** (FSM) design:

| State | Description | Transitions To |
|-------|-------------|----------------|
| `SETUP` | Initialization | `WIFI_CONNECT` or `ERROR` |
| `WIFI_CONNECT` | Attempting WiFi connection | `READY` or `ERROR` |
| `READY` | WiFi connected, waiting for touch | `WAKING` |
| `WAKING` | Sending WOL packet, showing message | `READY` (or `WIFI_CONNECT` if WiFi lost) |
| `ERROR` | Hardware/config error | Manual reset |

**Benefits:** Predictable behavior, easy to debug, clear flow.

##### Display Modes (`DispMode` enum)
- `NORMAL`: Shows WiFi status on line 1, state on line 2
- `WAKING`: Shows "WiFi Connected" / "WAKING PC..."
- `ERR`: Shows "ERROR" and error message

##### Key Functions

| Function | Purpose | Blocking? |
|----------|---------|-----------|
| `setup()` | Initialize hardware, connect WiFi | Yes (short, acceptable) |
| `loop()` | Main state machine, non-blocking | No |
| `updateDisplay()` | Render LCD with caching | No (only if content changed) |
| `touchPressed()` | Debounced edge detection | No |
| `sendWol()` | Send WOL magic packet | Yes (brief, ~300ms) |
| `wifiMonitor()` | Auto-reconnect logic | No |
| `wifiConnect()` | Blocking WiFi connect with timeout | Yes, but with yield |

---

## Design Patterns

### 1. **State Pattern**
The `State` and `DispMode` enums encapsulate different application states, making the code easy to extend and modify.

### 2. **Display Caching**
The LCD driver is a global object. Draw operations cache the last strings and only update when content changes to prevent flicker:

```cpp
if (line1 != lastLine1 || line2 != lastLine2) {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print(line1);
  lcd.setCursor(0,1); lcd.print(line2);
  lastLine1 = line1; lastLine2 = line2;
}
```

### 3. **Debounced Input**
Touch sensor uses edge detection + time-based debounce:
```cpp
if (currentReading == HIGH && lastTouchReading == LOW) {
  if (millis() - lastTouchMs >= DEBOUNCE_MS) {
    return true;  // Valid press
  }
}
```

### 4. **Timer-Based Non-Blocking Code**
All delays use `millis()` instead of `delay()`:
```cpp
// Instead of: delay(3000);
wakeEndMs = millis() + 3000;
// Later:
if (millis() >= wakeEndMs) { /* done */ }
```

This allows WiFi background tasks to run smoothly and keeps the system responsive.

### 5. **Configuration Centralization**
All tunable parameters live in `Config.h`. No magic numbers scattered in code. Users can adjust:
- WiFi credentials
- MAC address
- Pin mapping
- Timing constants
- LCD I2C address

---

## WiFi Auto-Reconnect Mechanism

```
┌────────────────────────────────────────────────┐
│   Each loop():                                 │
│   ┌──────────────────────────────────────────┐│
│   │ WiFi.status() == WL_CONNECTED?          ││
│   │       YES → State = READY               ││
│   │       NO  → Enter reconnect mode       ││
│   └──────────────────────────────────────────┘│
│                                                │
│   Reconnect attempts:                          │
│   - Try every 1000ms                          │
│   - Call wifiConnect() with 30s timeout      │
│   - Success: Update WOL broadcast, return    ││
│     to READY state                            │
└────────────────────────────────────────────────┘
```

---

## Wake-on-LAN Packet Structure

WakeOnLan library handles packet construction. For reference:

**Magic Packet Format:**
- 6 bytes of `0xFF` (sync stream)
- Target MAC repeated 16 times (96 bytes total)
- UDP port 9 (discard) or 7 (echo)
- Broadcast to network

Our configuration:
```cpp
WOL.setRepeat(3, 100);  // Send 3 packets, 100ms between each
```

---

## LCD Display Layout

16x2 character LCD organized as:

```
Line 1: Status (WiFi Connected, NO WiFi, ERROR, Connecting WiFi...)
Line 2: Current Action (Ready to Touch, WAKING PC..., Packet Sent!, Reconnecting...)
```

Example flow:
```
Line 1: Connecting WiFi...    Line 2: (blank)
  ↓ (WiFi connected)
Line 1: WiFi Connected        Line 2: Ready to Touch
  ↓ (touch pressed)
Line 1: WiFi Connected        Line 2: WAKING PC...
  ↓ (packet sent)
Line 1: WiFi Connected        Line 2: Packet Sent!
  ↓ (after delay)
Line 1: WiFi Connected        Line 2: Ready to Touch
```

---

## Error Handling Strategy

| Error Type | Detection | Response |
|------------|-----------|----------|
| LCD init fail | `lcd.init()` returns false | Set ERROR state, show "LCD Error" |
| WiFi timeout | `wifiConnect()` returns false | Set ERROR state, show "WiFi Failed" |
| WiFi disconnect | `WiFi.status() != WL_CONNECTED` | Auto-reconnect, show "Reconnecting..." |
| Invalid pin | `digitalRead()` on wrong pin | Undefined - avoid by correct wiring |

All errors are logged to Serial at 115200 baud for debugging.

---

## Memory Usage Estimates

| Resource | Usage | Notes |
|----------|-------|-------|
| Flash | ~180-220 KB | Code + LiquidCrystal_I2C + WakeOnLan libraries |
| RAM | ~20-30 KB | Global buffers, LCD buffer, WiFi stack |
| Stack | ~2 KB | Function call overhead |

**Note:** ESP8266 NodeMCU has ~80KB RAM, ~4MB flash. This sketch is well within limits.

---

## Performance Considerations

- **Loop time:** ~1-5ms (WiFi background tasks)
- **Touch latency:** Debounce (200ms) + edge detection (<10ms)
- **WOL send:** ~300ms total (3 packets × 100ms interval)
- **LCD refresh:** ~20ms per update (only when changed)
- **WiFi reconnect:** Attempt every 1s, up to 30s timeout

---

## Testing Strategy

### Unit Testing (not implemented)
- Test debounce logic with simulated inputs
- Test state transitions programmatically
- Validate MAC address format in Config.h

### Integration Testing (manual)
1. Upload sketch, verify Serial output
2. Verify LCD shows "WiFi Connected" after connection
3. Touch sensor triggers WOL packet (use Wireshark to verify)
4. Unplug/replug WiFi, verify auto-reconnect
5. Disconnect touch sensor wire, verify ERROR state

---

## Extensibility Points

Easy modifications for users:

| Feature | Where to modify | Difficulty |
|---------|----------------|------------|
| Change touch pin | `Config.h: TOUCH_PIN` (currently 12) | Easy |
| Adjust debounce | `Config.h: TOUCH_DEBOUNCE_MS` | Easy |
| Change LCD I2C address | `Config.h: LCD_I2C_ADDRESS` | Easy |
| Add status LED | Add `pinMode(LED_PIN, OUTPUT)` in `setup()` | Easy |
| Deep sleep mode | Modify `loop()` to call `ESP.deepSleep()` | Medium |
| Web interface | Add AsyncWebServer library, handlers | Hard |
| OTA updates | Add ArduinoOTA library calls | Medium |
| Multiple PCs | Track tap count, different MACs per count | Medium |

---

## References

- [ESP8266 Arduino Core](https://arduino-esp8266.readthedocs.io/)
- [LiquidCrystal I2C Library](https://github.com/marcoschwartz/LiquidCrystal_I2C)
- [WakeOnLan Library](https://github.com/Team-Groump/WakeOnLan)
- [Wake-on-LAN RFC](https://tools.ietf.org/html/rfc5427)
- [16x2 LCD Character Display Datasheet](https://cdn-shop.adafruit.com/datasheets/CharacterLCD_16x2.pdf)

---

**Architecture version:** 2.0.0 (2025-07-05) - Updated for 16x2 LCD hardware
