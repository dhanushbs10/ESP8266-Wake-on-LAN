# Security Policy

## Supported Versions

| Version | Supported          |
|---------|-------------------|
| 1.x     | :white_check_mark: |
| < 1.0   | :x:                |

---

## Reporting a Vulnerability

We take security seriously. Please report vulnerabilities privately.

**DO NOT open GitHub issues for security vulnerabilities.**

Instead, email security@yourdomain.com (replace with your email) with:
- Description of vulnerability
- Steps to reproduce
- Potential impact
- Suggested fix (if any)

We will respond within 48 hours and work with you on a fix before public disclosure.

---

## Security Considerations

### WiFi Credentials

⚠️ **Important:** This project contains WiFi SSID and password in `Config.h`. To avoid exposing credentials:

1. Never commit real WiFi password to public repositories
2. Use placeholder values in the repository:
   ```cpp
   const char* WIFI_PASSWORD = "YourWiFiPassword";
   ```
3. Keep your actual credentials local (never share `Config.h` with real passwords)
4. Consider using environment variables or EEPROM for production deployments

### MAC Address Privacy

The target MAC address is less sensitive but still consider:
- It's a device identifier on your network
- Don't share MAC addresses of public/shared computers
- Obfuscate in documentation (use `AA:BB:CC:...` placeholders)

### Network Security

- Wake-on-LAN packets are **not encrypted**
- Anyone on your local network can send a WOL magic packet
- Consider implementing a simple authentication mechanism for sensitive environments
- Keep ESP8266 firmware updated for security patches

### Physical Security

The device has no authentication - anyone with physical access to the touch sensor can wake the PC. Ensure physical security if waking critical systems.

---

## Best Practices

- Regularly update ESP8266 Arduino core to latest version
- Use strong WiFi password (WPA2/WPA3)
- Place the ESP8266 on a trusted VLAN if possible
- Monitor network for unexpected WOL packets
- Consider logging WOL events on the target PC (event viewer on Windows)

---

## Responsible Disclosure

We follow responsible disclosure:
1. Report received privately → email security@yourdomain.com
2. We confirm and assess vulnerability (72 hours max)
3. We develop and test fix (1-2 weeks typical)
4. Coordinated public disclosure with CVE if applicable
5. Credit reporter in changelog (unless anonymous)

---

**Last updated:** 2025-07-05
