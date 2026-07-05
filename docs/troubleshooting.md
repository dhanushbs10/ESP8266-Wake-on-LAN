# Troubleshooting Guide

Comprehensive diagnostics and solutions for common issues with the ESP8266 Wake-on-LAN device.

---

## Quick Diagnostic Checklist

Before diving into specific issues, verify these basics:

- [ ] **Power:** Device is powered (USB cable, led lights on ESP8266)
- [ ] **Serial:** Serial Monitor open at 115200 baud showing messages?
- [ ] **WiFi:** Is your WiFi network visible and working?
- [ ] **Credentials:** SSID and password are correct in `Config.h`?
- [ ] **MAC:** Target MAC address format is `{0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX}`?
- [ ] **Target PC:** WOL enabled in BIOS and OS?
- [ ] **Same network:** ESP and PC on same subnet (e.g., 192.168.1.x)?
- [ ] **Libraries:** All required Arduino libraries installed?

---

##🧪 Using Serial Monitor for Debugging

Serial output reveals what's happening:

```
=== ESP8266 Wake-on-LAN ===
Connecting to WiFi: YourSSID
.....
Connected! IP: 192.168.1.100
WiFi connected. Ready.
Touch detected! Sending WOL...
Magic packet sent!
```

**Common serial patterns:**

| Output | Meaning |
|--------|---------|
| `ERROR: Failed to initialize OLED display` | I2C wiring/address problem |
| `ERROR: WiFi connection timeout` | Wrong SSID/password or too far from router |
| Periods `.....` | WiFi associating (normal) |
| No output at all | ESP not powered, wrong board selected, or bad USB cable |

---

## Issues & Solutions

### 1. OLED Display Issues

#### **Symptom:** Display stays blank, no text

**Possible causes and fixes:**

1. **Wrong I2C address**
   - Most SSD1306 displays use `0x3C` or `0x3D`
   - Check your display's silkscreen or documentation
   - Try both in `Config.h`:
     ```cpp
     const int OLED_I2C_ADDRESS = 0x3C;  // Try 0x3D if this fails
     ```

2. **Incorrect wiring**
   - Verify SDA → GPIO4 (D2), SCL → GPIO5 (D1) for NodeMCU
   - Ensure 4 wires connected: VCC, GND, SDA, SCL
   - Check for loose connections

3. **Wrong voltage**
   - **3.3V displays:** Must use ESP's 3.3V pin, NOT 5V!
   - **5V displays:** Rare, check specs. May damage ESP8266 (3.3V logic)

4. **Missing pull-up resistors**
   - Some displays need 2.2K-4.7K ohm pull-ups on SDA/SCL to VCC
   - Most modern modules have them onboard

5. **Library conflict**
   - Remove old Adafruit libraries
   - Install latest from Library Manager:
     - "Adafruit GFX Library"
     - "Adafruit SSD1306"

**Test:** Upload I2C scanner sketch to find address:
```cpp
#include <Wire.h>
void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Scanning I2C...");
  byte error, address;
  int nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("Found at 0x");
      if (address<16) Serial.print("0");
      Serial.print(address,HEX);
      Serial.println();
      nDevices++;
    }
  }
  if (nDevices == 0) Serial.println("No I2C devices found");
}
void loop() {}
```

---

### 2. WiFi Connection Failures

#### **Symptom:** Display shows "NO WiFi" or serial says "WiFi connection timeout"

**Possible causes and fixes:**

1. **Wrong SSID or password**
   - Check case sensitivity (passwords are case-sensitive!)
   - Verify no extra spaces in `Config.h`
   - Try connecting with phone to same network

2. **Too far from router**
   - ESP8266 has limited range (~30m through walls)
   - Move closer temporarily to test
   - Consider external antenna (if board has connector)

3. **WiFi channel issues**
   - Older ESP8266 cores may not support DFS channels (52-144)
   - Set router to 2.4GHz, channels 1-11
   - Avoid 5GHz-only networks

4. **MAC filtering enabled**
   - Disable router's "MAC address filtering" or add ESP's MAC to whitelist
   - ESP's MAC is random unless you set it (not implemented yet)

5. **Router limits (DHCP pool exhausted)**
   - Too many devices connected
   - Restart router or disconnect unused devices

6. **Captive portal / Enterprise WiFi**
   - This sketch only supports WPA2-Personal
   - Not compatible with hotel/coffee shop portals

**Test:** Serial monitor shows connecting dots `....` If dots appear but never "Connected" → credentials or visibility issue.

---

### 3. Wake-on-LAN Not Working

#### **Symptom:** Touch triggers, but PC doesn't power on

**Possible causes and fixes:**

1. **Wrong MAC address**
   - Verify using `ipconfig /all` (Windows) or `ip link` (Linux/macOS)
   - Format must be exactly 6 hex bytes without colons:
     ```cpp
     // Correct:
     const byte TARGET_MAC[6] = {0x94, 0xDE, 0x80, 0x7B, 0x2F, 0x99};
     // WRONG:
     const byte TARGET_MAC[6] = {94, 222, 128, 123, 47, 153};  // Decimal!
     ```
   - Remove colons from output manually: `94:DE:80:7B:2F:99` → `94, DE, 80, 7B, 2F, 99`

2. **WOL not enabled in BIOS**
   - Reboot PC, enter BIOS/UEFI (Del/F2/F10)
   - Find "Power Management" → Enable "Wake-on-LAN", "PCIe Wake-on-LAN", or "PME"
   - Save and exit

3. **WOL not enabled in OS**
   - **Windows:** Network adapter properties → Advanced → Enable "Wake on Magic Packet" and "Wake on pattern match"
   - **Linux:** Run `sudo ethtool -s eth0 wol g`
   - **macOS:** System Preferences → Network → Advanced → Wake for network access

4. **PC and ESP on different subnets/VLANs**
   - WOL uses UDP broadcast
   - Broadcasts typically blocked across subnets/VLANs
   - Must be on same L2 network (same router LAN)
   - If using guest WiFi or IoT VLAN, WOL won't work

5. **Firewall blocking UDP port 9**
   - Temporarily disable firewall to test
   - Or add rule allowing inbound UDP port 9 (discard) or 7 (echo)

6. **Target PC in wrong sleep state**
   - **S3 (Sleep):** WOL works
   - **S4 (Hibernate):** May work depending on hardware
   - **S5 (Shutdown):** Should work if BIOS/OS configured
   - **Fast Startup (Windows):** Can interfere - disable in Power Options

7. **Router blocking broadcast packets**
   - Some routers isolate WiFi clients (AP isolation)
   - Disable "Wireless Isolation" or "AP Isolation"
   - Toggle "IGMP Snooping" (sometimes helps)

8. **Network adapter doesn't support WOL**
   - Check adapter specs or test with another PC
   - Some USB-Ethernet adapters lack WOL support

**Debug:** Use Wireshark on another computer to capture packets:
- Filter: `udp.port == 9`
- Look for packet with `FF:FF:FF:FF:FF:FF` destination
- Should see 102 bytes total (6×FF + 16×MAC)

---

### 4. Touch Sensor Problems

#### **Symptom:** Touch triggers multiple times or doesn't trigger reliably

**Possible causes and fixes:**

1. **Contact bounce (multiple triggers)**
   - Increase debounce time in `Config.h`:
     ```cpp
     const int TOUCH_DEBOUNCE_MS = 300;  // Try 300-500ms
     ```
   - Add 0.1µF capacitor across touch module VCC-GND

2. **Sensor too sensitive** (constantly HIGH)
   - Check wiring - some modules need specific pull-down
   - TTP2331/TTP223 modules have sensitivity adjustment (small potentiometer)
   - Move hand further from sensor

3. **Sensor not sensitive enough**
   - Adjust touch module's sensitivity pot (if present)
   - Ensure power supply is stable (3.3V, not 5V)
   - Check I2C? Wait, touch is GPIO, ensure wire connected to correct pin

4. **Wrong pin in Config.h**
   - Verify `TOUCH_PIN = 14` matches your wiring
   - NodeMCU D5 = GPIO14, but if you used D6, change to 12

5. **Bad ground connection**
   - Ensure touch module GND connected to ESP GND
   - Shared ground with power supply

**Test:** Upload simple touch test sketch:
```cpp
void setup() {
  Serial.begin(115200);
  pinMode(14, INPUT);
}
void loop() {
  if (digitalRead(14) == HIGH) Serial.println("TOUCH!");
  delay(100);
}
```

---

### 5. Display Flickering or Unstable

**Possible causes and fixes:**

1. **Power supply noise**
   - Use dedicated power source (not USB hub)
   - Add 100uF capacitor across OLED VCC-GND

2. **Loose I2C wiring**
   - Solder connections if using prototyping board
   - Check wires are fully seated

3. **Display redraw frequency too high**
   - Our code caches to avoid this, but custom modifications may cause it
   - Ensure only calling `updateDisplay()` when state changes

---

### 6. WiFi Drops Frequently

**Possible causes and fixes:**

1. **Weak signal**
   - Router too far, move ESP closer
   - Add external antenna if board supports
   - Change WiFi channel to avoid interference

2. **Power saving mode** (ESP8266 modem sleep)
   - Code currently disables modem sleep for WOL responsiveness
   - If you added power saving, ensure WiFi is active when needed

3. **Router DHCP lease timeout**
   - Increase router DHCP lease time
   - Set static IP for ESP (advanced: modify WiFi.config() in code)

4. **Multiple WiFi networks on same channel**
   - Use WiFi analyzer app to pick clearest channel

---

## Advanced Diagnostics

### Monitor Network Traffic with Wireshark

1. Set ESP and PC on same network hub/switch
2. Capture on PC's network interface
3. Filter: `wol` or `udp.port == 9`
4. Trigger touch on ESP
5. Look for magic packet (FF:FF:FF:FF:FF:FF → 94:DE:80:7B:2F:99 repeated 16x)

### Enable Extra Debug Output

In `ESP8266-Wake-on-LAN.ino`, add more Serial prints:
```cpp
Serial.print("WiFi status: ");
Serial.println(WiFi.status());
Serial.print("RSSI: ");
Serial.println(WiFi.RSSI());
```

### Check RSSI (Signal Strength)

Add to code:
```cpp
Serial.print("WiFi RSSI: ");
Serial.println(WiFi.RSSI());  // Negative dBm, -30 is excellent, -90 is poor
```

---

## Still Stuck?

1. **Check GitHub Issues:** https://github.com/YOUR-USERNAME/ESP8266-Wake-on-LAN/issues
2. **Search with exact error message**
3. **Open a new issue** with:
   - ESP board type and firmware version
   - Arduino IDE version
   - Full serial monitor output
   - Photos of wiring (if applicable)
   - What you've already tried

4. **Join Discussions:** https://github.com/YOUR-USERNAME/ESP8266-Wake-on-LAN/discussions

---

## Known Limitations

- **Not compatible with:** WiFi networks requiring captive portals (hotels, airports)
- **Range:** ~30m indoor, may vary with obstacles
- **Latency:** 1-3 seconds from touch to WOL packet (debounce + display)
- **Power:** Requires continuous 5V power (USB); not battery-optimized (yet)

---

## Tested Hardware

The following setups are verified working:

| ESP Board | OLED | Touch Module | Status |
|-----------|------|--------------|--------|
| NodeMCU 1.0 | SSD1306 128x64 (0x3C) | TTP223 | ✅ Works |
| Wemos D1 Mini | SSD1306 128x64 (0x3D) | TTP223 | ✅ Works |
| Generic ESP-12E | SSD1306 0.96" | TTP223 | ✅ Works |

*If you test with other hardware, please open a PR to add your setup!*

---

**Last updated:** 2025-07-05
