# Images Directory

This folder contains all images referenced in the README.md and documentation.

---

## Required Images

Place the following image files here:

### 1. `wiring-diagram.png` (600px wide recommended)
Fritzing-style wiring diagram showing:
- NodeMCU ESP8266 (with pin labels)
- 16x2 I2C LCD display (PCF8574 backpack, 4-pin)
- TTP223 capacitive touch sensor
- Jumper wire connections with correct pins: D1 (SCL), D2 (SDA), D6 (Touch), VIN (LCD 5V), 3.3V (Touch), GND

**Suggested sources:**
- Use [Fritzing](http://fritzing.org/) - select NodeMCU ESP8266 part, add LCD and touch module
- Or provide hand-drawn scan/photo of wiring diagram
- Include the ASCII diagram from README.md as inspiration

### 2. `device-photo.jpg` (400px wide recommended)
Photograph of the assembled device:
- ESP8266 NodeMCU + 16x2 LCD + touch sensor
- Clear view of all components and wiring
- Good lighting, focus on connections
- Show device powered on (LCD displaying "WiFi Connected / Ready to Touch")

**Tips:**
- Include scale reference (coin, ruler)
- Clean background
- Show both front and back if possible

### 3. `windows-settings.png` (500px wide recommended)
Screenshot of Windows network adapter WOL settings:
- Power Management tab (both checkboxes checked)
- Advanced tab (Wake on Magic Packet enabled, Wake on pattern match enabled)

**How to capture:**
1. Open Network Adapter properties
2. Use Snipping Tool or `Win+Shift+S`
3. Crop to show both tabs clearly

---

## Image Guidelines

- **Format:** PNG or JPG (no SVG for GitHub README compatibility)
- **Resolution:** At least 300 dpi for clear text (or high quality screenshots)
- **Size:** Keep under 500KB each (compress if needed)
- **Aspect ratio:** Consistent (landscape recommended) for uniform look
- **Licensing:** Only include images you own or have rights to

---

## Create Your Own Images

### Quick Wiring Diagram with draw.io
1. Go to https://app.diagrams.net/
2. Select "Device" template or blank diagram
3. Search for "NodeMCU", "LCD", "Touch" shapes
4. Draw connections: D1→SCL, D2→SDA, D6→Touch OUT, VIN→LCD VCC, 3.3V→Touch VCC, GND→both
5. Label pins clearly
6. Export as PNG (File → Export as → PNG)

### Annotated Screenshots (Windows)
- Press `Win + Shift + S` → select window → click popup to save to Captures folder
- Or use Snipping Tool → New → drag rectangle → Save as PNG
- Crop to show only relevant settings

### Photo Tips
- Use phone camera in good light, avoid shadows
- Hold steady, avoid blur (use both hands or prop phone)
- Show entire device + power cable + connection points
- Take multiple shots from different angles, pick best
- Consider neutral background (desk, poster board)

---

## Placeholder Images

This repo currently contains SVG placeholders for:
- `wiring-diagram.svg`
- `device-photo.svg`
- `windows-settings.svg`

**Before publishing on GitHub:**
1. Replace `.svg` files with actual `.png` or `.jpg` images
2. Delete the `.svg` files after copying content
3. Update README.md image references if you change filenames

---

## Submission

After adding real images:
```bash
cd "C:\Users\Dhanush\Desktop\Project 1\hell naaa\esp projj\ESP8266-Wake-on-LAN"
git add images/
git commit -m "Add actual device photos and wiring diagram"
git push origin master
```

---

**Thanks for improving the documentation with real visuals!**
