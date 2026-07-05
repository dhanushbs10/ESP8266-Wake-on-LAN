# ESP8266 Wake-on-LAN Remote Boot Button

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: ESP8266](https://img.shields.io/badge/Platform-NodeMCU_ESP8266-blue.svg)](https://www.espressif.com/en/products/chips/ESP8266)
[![Arduino](https://img.shields.io/badge/Arduino-1.0.0-00979D.svg)](https://www.arduino.cc)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()

---

## 📋 Overview

A wireless remote PC power-on button using a **NodeMCU ESP8266**, **Wake-on-LAN (WOL)**, and a **16x2 I2C LCD display**. Press a capacitive touch sensor to wake your desktop computer over the network.

**Perfect for:** Home servers, gaming PCs, workstations, or any desktop you want to power on remotely.

<div align="center">
  <img src="images/device-photo.jpg" alt="ESP8266 WOL Device" width="400"/>
  <p><em>Fig 1: Complete device with 16x2 LCD and touch sensor</em></p>
</div>

---

## ✨ Features

- ✅ **Wireless wake-up** - No cables to your PC
- ✅ **16x2 I2C LCD status** - Clear text feedback
- ✅ **Capacitive touch sensor** - Modern, clean activation
- ✅ **Auto WiFi reconnect** - Handles network drops gracefully
- ✅ **Non-blocking code** - Uses `millis()` timing, no `delay()` bottlenecks
- ✅ **Debounced input** - No false triggers
- ✅ **Broadcast support** - Works with most routers/switches
- ✅ **Secure** - No cloud dependency, local network only
- ✅ **Open source** - MIT licensed, fully customizable

---

## 🛠️ Hardware Required

| Component | Quantity | Notes |
|-----------|----------|-------|
| NodeMCU ESP8266 (ESP-12E) | 1 | Board specifically for ESP8266 |
| 16x2 I2C LCD Display (PCF8574 backpack) | 1 | I2C interface, typically address 0x27 |
| TTP223 Capacitive Touch Sensor | 1 | Digital output module |
| Jumper Wires | Several | Male-to-male or male-to-female |
| Micro-USB cable | 1 | For programming and power |

**Estimated cost:** $8-12 USD

---

## 🔌 Wiring Diagram

### ASCII Wiring Diagram

```
                NodeMCU ESP8266
             +-------------------+
             |                   |
    D1 (GPIO5) --+--> LCD SCL   |
    D2 (GPIO4) --+--> LCD SDA   |
    D6 (GPIO12) -+--> Touch OUT |
      3V3 --------+--> Touch VCC|
      VIN (5V) --+--> LCD VCC   |
      GND -------+--> LCD GND   |
      GND -------+--> Touch GND |
             +-------------------+
```

### Pin Connections Table

| Component | NodeMCU Pin | GPIO | Power | Notes |
|-----------|-------------|------|-------|-------|
| LCD SCL | D1 | GPIO5 | - | I2C clock |
| LCD SDA | D2 | GPIO4 | - | I2C data |
| LCD VCC | VIN | - | 5V | LCD typically needs 5V power |
| LCD GND | GND | - | GND | |
| Touch OUT | D6 | GPIO12 | - | Digital signal output |
| Touch VCC | 3.3V | - | 3.3V | Check module specifications |
| Touch GND | GND | - | GND | |

⚠️ **Important:**
- **LCD display needs 5V** - Use VIN pin on NodeMCU (not 3.3V!)
- **Touch sensor uses 3.3V** - Use 3.3V pin on NodeMCU
- Ensure all GND connections are common (shared)

---

## 📦 Software Dependencies

### Arduino Libraries

Install these via Arduino Library Manager (`Sketch → Include Library → Manage Libraries`):

1. **LiquidCrystal I2C** by Frank de Brabander (or **LiquidCrystal_I2C** by Malpartida)
   - Search: "LiquidCrystal I2C"
2. **WakeOnLan** by Team-Groump
   - Search: "WakeOnLan"
3. **ESP8266WiFi** (built-in when ESP8266 board support is installed)

### Board Support

1. Install ESP8266 board support:
   - Open Arduino IDE → `File → Preferences`
   - Add to "Additional Boards Manager URLs":
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - `Tools → Board → Boards Manager` → Search "ESP8266" → Install

2. Select your board:
   - `Tools → Board → NodeMCU 1.0 (ESP-12E Module)`

---

## ⚙️ Configuration

### 1. Configure `Config.h`

Open `Config.h` and edit these settings:

```cpp
// Your WiFi credentials
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASSWORD = "YourWiFiPassword";

// Target PC MAC address (format: {0x94, 0xDE, 0x80, 0x7B, 0x2F, 0x99})
const byte TARGET_MAC[6] = {0x94, 0xDE, 0x80, 0x7B, 0x2F, 0x99};

// Touch sensor pin (default: GPIO12 / D6 on NodeMCU)
const int TOUCH_PIN = 12;
```

### 2. Find Your PC's MAC Address

**Windows:**
```cmd
ipconfig /all | findstr "Physical"
```
Copy the 6-byte hex value (without colons).

**Linux/macOS:**
```bash
ip link show  # Look for "link/ether"
# or
ifconfig | grep ether
```

Format the MAC address as: `{0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX}`

**Example:** MAC `94:DE:80:7B:2F:99` becomes `{0x94, 0xDE, 0x80, 0x7B, 0x2F, 0x99}`.

### 3. LCD I2C Address (if needed)

If your display shows garbage or stays blank, the I2C address may be different. The most common addresses are `0x27` and `0x3F`.

Upload an I2C scanner sketch to find the address:

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

Then set `LCD_I2C_ADDRESS` in `Config.h` accordingly (default: `0x27`).

---

## 🔧 Enabling Wake-on-LAN on Your PC

### BIOS/UEFI Setup

1. Reboot your PC and enter BIOS/UEFI (typically `Del`, `F2`, or `F10`)
2. Find "Power Management" or "Advanced" settings
3. Enable **Wake-on-LAN**, **PCI Wake-on-LAN**, or **PME Event Wake Up**
4. Save and exit

### Windows Network Adapter Settings

1. `Win + R` → `devmgmt.msc`
2. Expand "Network adapters"
3. Right-click your Ethernet/WiFi adapter → Properties
4. Go to "Advanced" tab
5. Find and enable:
   - **Wake on Magic Packet** → Enabled
   - **Wake on pattern match** → Enabled (optional)
   - **Shutdown Wake-on-LAN** → Enabled (for Windows 10/11)
6. Go to "Power Management" tab
7. Check:
   - ✅ "Allow this device to wake the computer"
   - ✅ "Only allow a magic packet to wake the computer"

<div align="center">
  <img src="images/windows-settings.png" alt="Windows WOL Settings" width="500"/>
  <p><em>Fig 2: Windows network adapter WOL settings</em></p>
</div>

### Linux (Optional)

```bash
# Enable WoL
sudo ethtool -s eth0 wol g

# Make persistent (add to /etc/network/interfaces or systemd service)
sudo ethtool -s eth0 wol g
```

---

## 🚀 Usage

1. **Upload the code:**
   - Open `ESP8266-Wake-on-LAN.ino` in Arduino IDE
   - Select board: `Tools → Board → NodeMCU 1.0 (ESP-12E Module)`
   - Select correct port: `Tools → Port`
   - Click "Upload" (or `Ctrl+U`)

2. **Monitor serial output (optional):**
   - Open Serial Monitor (`Tools → Serial Monitor`) at 115200 baud
   - You'll see connection status and debug messages

3. **LCD Display Messages:**
   - **Line 1:** Shows WiFi status (`WiFi Connected`, `NO WiFi`, `ERROR`)
   - **Line 2:** Shows current action (`Ready to Touch`, `WAKING PC...`, `Packet Sent!`, `Reconnecting...`)

4. **Wake your PC:**
   - Touch the sensor
   - LCD shows: "WAKING PC..."
   - Magic packet sent, briefly shows "Packet Sent!"
   - Your PC should power on!

**Expected LCD flow:**
```
Line 1: Connecting WiFi...
Line 2: (blank or waiting)

-> After WiFi connects:
Line 1: WiFi Connected
Line 2: Ready to Touch

-> After touch:
Line 1: WiFi Connected
Line 2: WAKING PC...

-> After packet sent:
Line 1: WiFi Connected
Line 2: Packet Sent!

-> Returns to:
Line 1: WiFi Connected
Line 2: Ready to Touch
```

---

## 📊 Project Structure

```
ESP8266-Wake-on-LAN/
├── ESP8266-Wake-on-LAN.ino  # Main sketch (setup + loop + state machine)
├── Config.h                 # User configuration (WiFi, MAC, pins, LCD)
├── README.md                # This file
├── LICENSE                  # MIT License
├── .gitignore               # Files to exclude from Git
├── CONTRIBUTING.md          # How to contribute
├── CHANGELOG.md             # Version history
├── SECURITY.md              # Security policy
├── docs/                    # Detailed documentation
│   ├── architecture.md      # Code architecture, state machine design
│   ├── troubleshooting.md   # Comprehensive diagnostic guide
│   └── advanced.md          # Deep sleep, OTA, REST API, etc.
└── images/                  # Images for README
    ├── wiring-diagram.png   # (Replace with actual wiring diagram)
    ├── device-photo.jpg     # (Replace with actual photo)
    └── windows-settings.png # (Replace with Windows settings screenshot)
```

---

## 🔮 Future Improvements

- [ ] Add mDNS support for easy identification (`wol.local`)
- [ ] Web configuration portal (captive portal)
- [ ] Multiple PC support (different tap patterns for different MACs)
- [ ] Power consumption optimization (deep sleep between touches)
- [ ] RGB LED status indicator
- [ ] Action button mode toggle (wake / sleep / shutdown)
- [ ] OTA (Over-The-Air) updates
- [ ] REST API endpoint for smart home integration
- [ ] OLED version for smaller footprint

---

## 🐛 Troubleshooting

### "LCD stays blank or shows garbage characters"
- Check I2C address (try `0x27` or `0x3F` in `Config.h`)
- Verify SDA → D2 (GPIO4), SCL → D1 (GPIO5)
- Ensure LCD powered from **5V** (VIN), not 3.3V
- Run I2C scanner sketch to detect address
- Check backlight is enabled (code calls `lcd.backlight()`)

### "WiFi connects but PC doesn't wake"
- Verify MAC address format: `{0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX}` (hex bytes, not decimal!)
- Ensure target PC has WOL enabled in BIOS + OS
- Check that ESP and PC are on same subnet/VLAN (broadcasts don't cross routers)
- Disable PC firewall temporarily for testing
- Try enabling "Wake on pattern match" in adapter settings

### "Touch triggers multiple times or erratic"
- Debounce already set to 200ms in `Config.h`
- If still bouncing, increase `TOUCH_DEBOUNCE_MS` to 300-500ms
- Add 0.1µF capacitor across touch module VCC-GND
- Check touch module sensitivity (some have potentiometer)

### "Display shows ERROR"
- Check Serial Monitor at 115200 baud for error details
- Verify LCD I2C address and wiring
- Ensure LCD gets **5V** (not 3.3V)
- Check wires are secure, no loose connections

### "WiFi disconnect/reconnect loop"
- Check SSID/password are correct in `Config.h`
- Move ESP closer to router (weak signal)
- Verify router is broadcasting 2.4GHz (ESP8266 doesn't support 5GHz)
- Consider reducing `WIFI_TIMEOUT_MS` if needed

---

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙋 Support & Contributing

- **Issues:** [GitHub Issues](https://github.com/dhanushbs10/ESP8266-Wake-on-LAN/issues) for bug reports and feature requests
- **Discussions:** [GitHub Discussions](https://github.com/dhanushbs10/ESP8266-Wake-on-LAN/discussions) for questions and ideas
- **Contributing:** See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines

---

## 🙏 Acknowledgments

- **WakeOnLan library** by Team-Groump
- **LiquidCrystal_I2C** library by Frank de Brabander
- **Espressif** for ESP8266 SDK and Arduino core

---

**Built with ❤️ for the maker community**
