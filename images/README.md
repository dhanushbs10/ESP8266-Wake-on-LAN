# Images Directory

This folder contains all images referenced in the README.md and documentation.

---

## Required Images

Place the following image files here:

### 1. `wiring-diagram.png` (600px wide recommended)
Fritzing-style wiring diagram showing:
- ESP8266 NodeMCU (with pin labels)
- SSD1306 OLED display (4-pin I2C)
- Touch sensor module (TTP223 or similar)
- Jumper wire connections

**Suggested sources:**
- Use [Fritzing](http://fritzing.org/) to create diagram
- Or provide hand-drawn scan/photo of wiring

### 2. `device-photo.jpg` (400px wide recommended)
Photograph of the assembled device:
- ESP8266 + OLED + touch sensor
- Clear view of all components
- Good lighting, focus on connections

**Tips:**
- Show device powered on (OLED displaying status)
- Include scale reference (coin, ruler)
- Clean background

### 3. `windows-settings.png` (500px wide recommended)
Screenshot of Windows network adapter WOL settings:
- Power Management tab (checkboxes checked)
- Advanced tab (Wake on Magic Packet enabled)

### 4. (Optional) `pc-bios-setup.png`
BIOS/UEFI screenshot showing WOL enable option

### 5. (Optional) `wireshark-capture.png`
Wireshark capture showing WOL magic packet (for advanced docs)

---

## Image Guidelines

- **Format:** PNG or JPG (no SVG for GitHub compatibility)
- **Resolution:** At least 300 dpi for clear text
- **Size:** Keep under 500KB each
- **Aspect ratio:** Consistent (landscape recommended)
- **Licensing:** Only include images you own or have rights to

---

## Create Your Own Images

### Quick Diagram with draw.io
1. Go to https://app.diagrams.net/
2. Select "Device" template
3. Drag ESP8266, OLED, touch sensor
4. Draw connections
5. Export as PNG

### Annotated Screenshots (Windows/Mac)
- Windows: `Win + Shift + S` → select area → paste into Paint → save
- macOS: `Cmd + Shift + 4` → select area → save to desktop
- Linux: `PrtSc` or `gnome-screenshot -a`

### Photo Tips
- Use phone camera in good light
- Hold steady, avoid blur
- Show entire device + parts
- Take multiple shots, pick best

---

## Placeholder Images

If images don't exist yet, the README will show broken image links. Replace this section with actual images before publishing.

---

**Submission:** After adding images, commit them to Git:
```bash
git add images/
git commit -m "Add device photos and wiring diagram"
git push
```
