# Troubleshooting Guide

Comprehensive diagnostics and solutions for common issues with the ESP8266 Wake-on-LAN device (NodeMCU + 16x2 I2C LCD + TTP223 touch sensor).

---

## Quick Diagnostic Checklist

Before diving into specific issues, verify these basics:

- [ ] **Power:** Device powered (USB cable, blue LED on ESP8266 lit)
- [ ] **Serial:** Serial Monitor open at 115200 baud showing messages?
- [ ] **WiFi:** Is your WiFi network working?
- [ ] **Credentials:** SSID and password correct in `Config.h`?
- [ ] **MAC:** Target MAC address format is `{0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX}`?
- [ ] **Target PC:** WOL enabled in BIOS and OS?
- [ ] **Same network:** ESP and PC on same subnet (e.g., 192.168.1.x)?
- [ ] **Libraries:** LiquidCrystal_I2C and WakeOnLan installed?
- [ ] **LCD I2C address:** Set correctly in `Config.h` (default 0x27)?

---

## Using Serial Monitor for Debugging

Serial output reveals what's happening:

```
=== ESP8266 Wake-on-LAN (16x2 LCD) ===
Connecting to WiFi: YourSSID
.....
Connected! IP: 192.168.1.100
WiFi connected. Ready.
Touch detected!
Sending WOL to MAC: 94:DE:80:7B:2F:99
Magic packet sent!
```

**Common serial patterns:**

| Output | Meaning |
|--------|---------|
| `ERROR: LCD init failed` | LCD not found at I2C address, wiring issue |
| `ERROR: WiFi connection timeout` | Wrong SSID/password or too far from router |
| `WiFi lost, reconnecting...` | WiFi dropped, auto-reconnect active |
| Periods `.....` | WiFi associating (normal during connection) |
| No output at all | ESP not powered, bad USB cable, or wrong board selected |

---

## Issues & Solutions

### 1. LCD Display Issues

#### **Symptom:** LCD stays blank, shows nothing or garbled characters

**Possible causes and fixes:**

1. **Wrong I2C address**
   - Most 16x2 LCD PCF8574 backpacks use `0x27` or `0x3F`
   - Check your module's documentation or silkscreen
   - Try both in `Config.h`:
     ```cpp
     const int LCD_I2C_ADDRESS = 0x27;  // Try 0x3F if this fails
     ```
   - Use I2C scanner (see below) to detect automatically

2. **Incorrect wiring**
   - Verify SDA → D2 (GPIO4), SCL → D1 (GPIO5)
   - Ensure all 4 wires connected: VCC, GND, SDA, SCL
   - Check for loose connections or breadboard issues

3. **Wrong voltage/power**
   - **LCD needs 5V** - Use VIN pin on NodeMCU (NOT 3.3V!)
   - **Touch sensor needs 3.3V** - Use 3.3V pin
   - Check power LED on LCD backpack is lit

4. **Missing pull-up resistors**
   - Some LCD modules need external 2.2K-4.7KΩ pull-ups on SDA/SCL to VCC
   - Most modern modules have them onboard

5. **Library conflict or wrong library**
   - Remove old LiquidCrystal libraries
   - Install "LiquidCrystal I2C" by Frank de Brabander or "LiquidCrystal_I2C" by Malpartida
   - Do NOT use "LiquidCrystal" (non-I2C version)

**Test:** Upload I2C scanner sketch to find address:
```cpp
#include <Wire.h>
void setup() {
  Serial.begin(115200);
  Wire.begin();  // D1=SCL, D2=SDA on NodeMCU
  Serial.println("Scanning I2C bus...");
  byte error, address;
  int nDevices = 0;
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("Device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  if (nDevices == 0) Serial.println("No I2C devices found");
}
void loop() {}
```

Should show: `Device found at 0x27` or `0x3F`.

---

#### **Symptom:** LCD shows but text is dim or backlight off

- Call `lcd.backlight()` in `initDisplay()` (already in code)
- Some modules have a jumper to enable backlight - check it's set
- Increase contrast potentiometer on LCD backpack (small blue pot)

---

### 2. WiFi Connection Failures

#### **Symptom:** Serial shows timeout, LCD shows "ERROR" or stays on "Connecting WiFi..."

**Possible causes and fixes:**

1. **Wrong SSID or password**
   - Check case sensitivity (passwords ARE case-sensitive!)
   - Verify no extra spaces in `Config.h`
   - Test connecting phone to same network

2. **Too far from router**
   - ESP8266 range ~30m through walls
   - Move closer temporarily to test
   - Consider external antenna if board has connector

3. **5GHz network**
   - ESP8266 only supports 2.4GHz
   - Ensure router is broadcasting 2.4GHz band

4. **WiFi channel issues**
   - Older ESP cores may not support DFS channels (52-144)
   - Set router to channel 1-11

5. **MAC filtering enabled**
   - Disable router's "MAC address filtering" or add ESP's MAC
   - Note: ESP8266 uses random MAC by default unless set (not implemented)

6. **Router DHCP pool exhausted**
   - Too many devices connected
   - Restart router or disconnect unused devices

7. **Captive portal / Enterprise WiFi**
   - This sketch only supports WPA2-Personal
   - Not compatible with hotel/airport portals

---

### 3. Wake-on-LAN Not Working

#### **Symptom:** Touch triggers, but PC doesn't power on

**Possible causes and fixes:**

1. **Wrong MAC address**
   - Verify using `ipconfig /all` (Windows) or `ip link` (Linux/macOS)
   - Format must be exactly 6 hex bytes (0-padded):
     ```cpp
     // Correct:
     const byte TARGET_MAC[6] = {0x94, 0xDE, 0x80, 0x7B, 0x2F, 0x99};
     // WRONG (decimal):
     const byte TARGET_MAC[6] = {94, 222, 128, 123, 47, 153};
     ```
   - Remove colons, convert to hex with leading zeros

2. **WOL not enabled in BIOS**
   - Reboot PC, enter BIOS/UEFI (Del/F2/F10)
   - Find "Power Management" → Enable "Wake-on-LAN", "PCIe Wake-on-LAN", or "PME"
   - Save and exit

3. **WOL not enabled in OS**
   - **Windows:** Network adapter properties → Advanced → Enable "Wake on Magic Packet"
   - **Linux:** `sudo ethtool -s eth0 wol g`
   - **macOS:** System Preferences → Network → Advanced → "Wake for network access"

4. **PC and ESP on different subnets/VLANs**
   - WOL uses UDP broadcast
   - Broadcasts typically blocked across subnets/VLANs
   - Must be on same L2 network (same router LAN)
   - If using guest WiFi or IoT VLAN, WOL won't work

5. **Firewall blocking UDP port 9**
   - Temporarily disable firewall to test
   - Or add rule allowing inbound UDP port 9 (discard) or 7 (echo)

6. **Target PC in wrong sleep state**
   - **S3 (Sleep):** WOL works ✓
   - **S4 (Hibernate):** May work depending on hardware
   - **S5 (Shutdown):** Should work if BIOS/OS configured
   - **Fast Startup (Windows):** Can interfere - disable in Power Options

7. **Router blocking broadcast packets**
   - Some routers isolate WiFi clients ("AP Isolation")
   - Disable "Wireless Isolation" or "AP Isolation"
   - Toggle "IGMP Snooping" (sometimes helps, sometimes hurts)

8. **Network adapter doesn't support WOL**
   - Check adapter specs or test with another PC
   - Some USB-Ethernet adapters lack WOL support

**Debug:** Use Wireshark on another computer:
- Filter: `udp.port == 9`
- Look for packet with destination `FF:FF:FF:FF:FF:FF`
- Should see 102 bytes total (6×FF + 16×MAC)

---

### 4. Touch Sensor Problems

#### **Symptom:** Touch triggers multiple times or doesn't trigger reliably

**Possible causes and fixes:**

1. **Contact bounce (multiple triggers)**
   - Debounce already set to 200ms in `Config.h`
   - If still bouncing, increase `TOUCH_DEBOUNCE_MS` to 300-500
   - Add 0.1µF capacitor across touch module VCC-GND

2. **Sensor too sensitive** (always HIGH)
   - Check wiring - some modules need pull-down resistor
   - TTP223 modules have sensitivity potentiometer - adjust it
   - Move hand further from sensor

3. **Sensor not sensitive enough**
   - Adjust sensitivity potentiometer (if present)
   - Ensure power supply is stable (3.3V)
   - Check wire connected to correct pin (D6 = GPIO12)

4. **Wrong pin in Config.h**
   - Verify `TOUCH_PIN = 12` matches your wiring
   - If you wired to D5, change to 14; if D7, change to 13

5. **Bad ground connection**
   - Ensure touch module GND connected to ESP GND
   - Should share ground with power supply

**Test:** Upload simple touch test:
```cpp
void setup() {
  Serial.begin(115200);
  pinMode(12, INPUT);  // GPIO12
}
void loop() {
  if (digitalRead(12) == HIGH) Serial.println("TOUCH!");
  delay(100);
}
```

---

### 5. LCD Flickering or Unstable

**Possible causes and fixes:**

1. **Power supply noise**
   - Use direct USB port (not hub)
   - Add 100µF capacitor across LCD VCC-GND

2. **Loose I2C wiring**
   - Solder connections if on perfboard
   - Verify wires fully seated in headers

3. **Update frequency too high**
   - Our code already caches to avoid this
   - If you modified code, ensure `updateDisplay()` only called when state changes

---

### 6. WiFi Drops Frequently

**Possible causes and fixes:**

1. **Weak signal**
   - Move ESP closer to router
   - Change router channel to avoid interference
   - Use WiFi analyzer app to find clean channel

2. **Power saving mode**
   - Code does not use WiFi modem sleep
   - If you added power saving, ensure WiFi active when needed

3. **Router DHCP lease timeout**
   - Increase DHCP lease time on router
   - Set static IP for ESP (advanced: modify code with `WiFi.config()`)

4. **Electrical interference**
   - Keep ESP away from motors, microwaves, other 2.4GHz devices

---

## Advanced Diagnostics

### Enable Extra Debug Output

Add to `ESP8266-Wake-on-LAN.ino` in `loop()` or `wifiMonitor()`:
```cpp
Serial.print("WiFi status: ");
Serial.println(WiFi.status());
Serial.print("RSSI: ");
Serial.println(WiFi.RSSI());  // Negative dBm, -30=excellent, -90=poor
```

### Check RSSI (Signal Strength)

RSSI values:
- `-30 to -50 dBm`: Excellent
- `-50 to -60 dBm`: Good
- `-60 to -70 dBm`: Fair
- `-70 to -90 dBm`: Poor, may drop

If RSSI < -70, move ESP closer or add external antenna.

---

## Still Stuck?

1. **Check GitHub Issues:** https://github.com/dhanushbs10/ESP8266-Wake-on-LAN/issues
2. **Search with exact error message**
3. **Open a new issue** with:
   - ESP board type (NodeMCU v1, v2, v3?)
   - LCD module (PCF8574? I2C address?)
   - Touch sensor model (TTP223?)
   - Arduino IDE version
   - Full serial monitor output (copy-paste)
   - Photos of wiring (if applicable)
   - What you've already tried

4. **Join Discussions:** https://github.com/dhanushbs10/ESP8266-Wake-on-LAN/discussions

---

## Known Limitations

- **Not compatible with:** WiFi networks requiring captive portals (hotels, airports)
- **Range:** ~30m indoor, varies with obstacles
- **Latency:** ~400ms from touch to WOL (200ms debounce + display time)
- **Power:** Requires continuous 5V power (USB); not battery-optimized (yet)
- **LCD speed:** I2C at 100kHz may feel slow compared to SPI OLED
- **2.4GHz only:** ESP8266 does not support 5GHz WiFi networks

---

## Tested Hardware

The following setups are verified working:

| ESP Board | LCD Module | Touch Sensor | Status |
|-----------|------------|--------------|--------|
| NodeMCU 1.0 (ESP-12E) | 16x2 LCD (PCF8574, 0x27) | TTP223 | ✅ Works |
| NodeMCU v3 | 16x2 LCD (PCF8574, 0x3F) | TTP223 | ✅ Works |
| Generic ESP-12E | 16x2 LCD (0x27) | TTP223 | ✅ Works |

*If you test with other hardware, please open a PR to add your setup!*

---

## I2C Scanner Output Examples

**Success:**
```
Scanning I2C...
Device found at 0x27
```
→ Set `LCD_I2C_ADDRESS = 0x27` in Config.h

**No devices found:**
```
Scanning I2C...
No I2C devices found
```
→ Check wiring, power, pull-ups

**Multiple devices:**
```
Scanning I2C...
Device found at 0x27
Device found at 0x68  // Possibly an RTC module
```
→ Use the one that's your LCD (usually 0x27)

---

**Last updated:** 2025-07-05
