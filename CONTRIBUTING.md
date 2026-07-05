# Contributing to ESP8266 Wake-on-LAN

Thank you for your interest in contributing! This project is open source and welcomes contributions from the community.

---

## 📜 Code of Conduct

- Be respectful and inclusive
- Welcome newcomers and help them get started
- Focus on constructive feedback
- Accept responsibility and apologize for mistakes

---

## 🤔 How Can I Contribute?

- **Bug fixes:** Report issues or submit pull requests
- **New features:** Propose ideas via GitHub Issues first
- **Documentation:** Improve README, add tutorials, translate
- **Hardware:** Share your custom wiring diagrams or PCB designs
- **Testing:** Test with different ESP8266 boards and OLED displays

---

## 🐛 Reporting Bugs

1. **Search existing issues** first to avoid duplicates
2. **Use the bug report template** when creating a new issue
3. Include:
   - ESP8266 board type (NodeMCU, Wemos, etc.)
   - OLED display model
   - Arduino IDE version
   - Full error output from Serial Monitor (115200 baud)
   - Steps to reproduce
   - Expected vs actual behavior

---

## 💡 Feature Requests

1. **Open a GitHub Issue** with "Feature Request" label
2. Describe:
   - What problem does it solve?
   - Proposed solution/implementation
   - Alternatives considered
   - Mockups/wireframes if applicable

---

## 🔧 Development Workflow

### Prerequisites

- Arduino IDE or VS Code + PlatformIO
- ESP8266 board support installed
- Required libraries (see README.md)

### Making Changes

1. **Fork the repository**
2. **Clone your fork:**
   ```bash
   git clone https://github.com/YOUR-USERNAME/ESP8266-Wake-on-LAN.git
   cd ESP8266-Wake-on-LAN
   ```
3. **Create a branch:**
   ```bash
   git checkout -b feature/my-new-feature
   ```
4. **Make your changes:**
   - Edit code in `Config.h` for configuration changes
   - Edit `ESP8266-Wake-on-LAN.ino` for logic changes
   - Update documentation as needed
   - Follow Doxygen commenting style
5. **Test thoroughly** on actual hardware
6. **Commit with clear messages:**
   ```bash
   git add .
   git commit -m "feat: add power-saving deep sleep mode"
   ```
7. **Push to your fork:**
   ```bash
   git push origin feature/my-new-feature
   ```
8. **Open a Pull Request** against `main` branch

---

## 📝 Code Style

- **Naming:** camelCase for variables/functions, PascalCase for classes
- **Indentation:** 2 spaces, no tabs
- **Braces:** K&R style (opening brace on same line)
- **Comments:** Doxygen-style `/** */` for functions and classes
- **Constants:** UPPER_SNAKE_CASE with `const` or `#define`
- **Tabs vs Spaces:** Never mix; use spaces only

**Example:**
```cpp
/**
 * @brief Initialize WiFi connection
 * @return true if connected, false on timeout
 */
bool connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // ...
}
```

---

## 🧪 Testing

- Test on actual ESP8266 hardware (not just simulator)
- Verify OLED display works
- Test touch sensor debouncing
- Confirm WOL packets arrive (use Wireshark or router logs)
- Check WiFi reconnect behavior (unplug/replug)
- Test with both Ethernet and WiFi adapters on target PC

---

## 📚 Documentation

- Update `README.md` for user-facing changes
- Add new sections to `docs/` for complex features
- Include wiring diagrams for hardware changes
- Use Doxygen comments in code (`/** @brief ... */`)
- Update `CHANGELOG.md` with every release

---

## 🔀 Pull Request Process

1. **Ensure PR passes all checks** (Arduino syntax, lint if available)
2. **Update documentation** before submitting
3. **Squash commits** if multiple tiny commits (`git rebase -i`)
4. **Fill PR template completely**
5. **Link to related issues** (`Closes #123`, `Fixes #456`)
6. **Wait for review** - maintainers will provide feedback
7. **Address review comments** and update the PR
8. **Once approved,** maintainer will merge

---

## 🏷️ Versioning

We follow [Semantic Versioning](https://semver.org/):

- **MAJOR:** Breaking API changes
- **MINOR:** New features, backward compatible
- **PATCH:** Bug fixes, documentation updates

---

## 📄 License

By contributing, you agree your contributions will be licensed under the MIT License (see LICENSE file).

---

## 🙋 Questions?

- Open a [GitHub Discussion](https://github.com/YOUR-USERNAME/ESP8266-Wake-on-LAN/discussions)
- Email: dhanush@example.com (replace with your email)

---

**Happy hacking! 🚀**
