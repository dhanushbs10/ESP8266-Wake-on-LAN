# ESP8266 Wake-on-LAN Remote Boot Button

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: ESP8266](https://img.shields.io/badge/Platform-ESP8266-blue.svg)](https://www.espressif.com/en/products/chips/ESP8266)
[![Arduino](https://img.shields.io/badge/Arduino-1.0.0-00979D.svg)](https://www.arduino.cc)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()

---

## 📋 Overview

A wireless remote PC power-on button using an ESP8266, Wake-on-LAN (WOL), and an OLED display. Press a touch sensor to wake your desktop computer over the network without leaving your chair!

**Perfect for:** Home servers, gaming PCs, workstations, or any desktop you want to power on remotely.

<div align="center">
  <img src="images/device-photo.jpg" alt="ESP8266 WOL Device" width="400"/>
  <p><em>Fig 1: Complete device with OLED display and touch sensor</em></p>
</div>

---

## ✨ Features

- ✅ **Wireless wake-up** - No cables to your PC
- ✅ **OLED status display** - See connection status at a glance
- ✅ **Touch sensor** - Modern, clean activation
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
| ESP8266 (NodeMCU or Wemos D1 Mini) | 1 | Any ESP8266 board works |
| SSD1306 OLED Display (128x64) | 1 | I2C interface (4-pin) |
| Touch Sensor Module | 1 | TTP223 or similar digital touch module |
| Jumper Wires | Several | Male-to-male or male-to-female |
| Micro-USB cable | 1 | For programming |

**Estimated cost:** $10-15 USD

---

## 🔌 Wiring Diagram

<div align="center">
  <img src="images/wiring-diagram.png" alt="Wiring Diagram" width="600"/>
  <p><em>Fig 2: Fritzing-style wiring diagram</em></p>
</div>

### Pin Connections

| Component | ESP8266 Pin | ESP8266 GPIO | Notes |
|-----------|-------------|--------------|-------|
| OLED SCL | D1 | GPIO5 | Pull-up to 3.3V |
| OLED SDA | D2 | GPIO4 | Pull-up to 3.3V |
| OLED VCC | 3.3V | - | Do not use 5V! |
| OLED GND | GND | - | |
| Touch Sensor OUT | D5 | GPIO14 | Signal output |
| Touch Sensor VCC | 3.3V | - | Check module spec |
| Touch Sensor GND | GND | - | |

⚠️ **Important:** Many SSD1306 displays are **3.3V only**. Using 5V may damage them!

---

## 📦 Software Dependencies

### Arduino Libraries

Install these libraries via Arduino Library Manager (`Sketch → Include Library → Manage Libraries`):

1. **Adafruit GFX Library** by Adafruit
2. **Adafruit SSD1306** by Adafruit
3. **ESP8266WiFi** (built-in with ESP8266 board support)
4. **WakeOnLan** by Team-Groump

### Board Support

1. Install ESP8266 board support:
   - Open Arduino IDE → `File → Preferences`
   - Add to "Additional Boards Manager URLs":
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - `Tools → Board → Boards Manager` → Search "ESP8266" → Install

2. Select your board:
   - `Tools → Board → NodeMCU 1.0 (ESP-12E Module)` (or your specific board)

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

// Pin connected to touch sensor (default: GPIO14/D5)
const int TOUCH_PIN = 14;
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
  <p><em>Fig 3: Windows network adapter WOL settings</em></p>
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
   - Select your board (`Tools → Board`)
   - Select correct port (`Tools → Port`)
   - Click "Upload" (or `Ctrl+U`)

2. **Monitor serial output (optional):**
   - Open Serial Monitor (`Tools → Serial Monitor`) at 115200 baud
   - You'll see connection status and debug messages

3. **Wake your PC:**
   - Touch the sensor (or press the button)
   - OLED displays: `WAKING PC... 🔥`
   - Magic packet is sent
   - Your PC should power on!

**Status Messages on OLED:**
- `CONNECTED / Ready to Wake` - All good, ready to use
- `NO WiFi / Reconnecting...` - WiFi lost, attempting reconnect
- `WAKING PC...` - Sending magic packet
- `ERROR` - Hardware or configuration issue (check serial monitor)

---

## 📊 Project Structure

```
ESP8266-Wake-on-LAN/
├── ESP8266-Wake-on-LAN.ino  # Main sketch (setup + loop)
├── Config.h                 # User configuration (WiFi, MAC, pins)
├── README.md                # This file
├── LICENSE                  # MIT License
├── .gitignore               # Files to exclude from Git
├── CONTRIBUTING.md          # How to contribute
├── CHANGELOG.md             # Version history
├── images/                  # Wiring diagram, device photos
│   ├── wiring-diagram.png
│   └── device-photo.jpg
└── docs/                    # Additional documentation
    ├── architecture.md      # Code architecture notes
    ├── troubleshooting.md   # Detailed troubleshooting guide
    └── advanced.md          # Advanced configuration options
```

---

## 🔮 Future Improvements

- [ ] Add mDNS support for easy identification (`wol.local`)
- [ ] Web configuration portal ( captive portal )
- [ ] Multiple PC support (double-tap for different MACs)
- [ ] Power consumption optimization (deep sleep between touches)
- [ ] LED status indicator (RGB or single color)
- [ ] Action button mode toggle (wake vs. sleep)
- [ ] OTA (Over-The-Air) updates
- [ ] REST API endpoint for smart home integration

---

## 🐛 Troubleshooting

### " OLED display stays blank"
- Check I2C address (try `0x3C` or `0x3D`)
- Verify SDA/SCL connections match Config.h
- Ensure display is 3.3V, not 5V
- Run I2C scanner sketch to find address

### "WiFi connects but no wake"
- Verify MAC address is correct (no colons, proper hex format)
- Ensure target PC has WOL enabled in BIOS + OS
- Check that PC and ESP are on same subnet/VLAN
- Disable PC firewall temporarily for testing
- Try enabling "Wake on pattern match" in adapter settings

### "ESP8266 crashes on touch"
- Check touch sensor is 3.3V compatible
- Add 10µF capacitor across touch sensor VCC-GND
- Increase `TOUCH_DEBOUNCE_MS` in Config.h

### "Display shows 'ERROR'"
- Check serial monitor at 115200 baud for error details
- Verify display I2C address and wiring
- Ensure display is powered from 3.3V, not 5V

### "WiFi disconnect/reconnect loop"
- Check SSID/password in Config.h
- Move ESP closer to router
- Reduce `WIFI_TIMEOUT_MS` if needed

---

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙋 Support & Contributing

- **Issues:** [GitHub Issues](link-to-repo/issues) for bug reports and feature requests
- **Discussions:** [GitHub Discussions](link-to-repo/discussions) for questions and ideas
- **Contributing:** See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines

---

## 🙏 Acknowledgments

- **WakeOnLan library** by Team-Groump
- **Adafruit** for GFX and SSD1306 libraries
- **Espressif** for ESP8266 SDK and Arduino core

---

**Built with ❤️ and open-source tools**
