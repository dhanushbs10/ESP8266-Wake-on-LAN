/**
 * @file ESP8266-Wake-on-LAN.ino
 * @brief NodeMCU ESP8266 + 16x2 I2C LCD Wake-on-LAN Remote Boot Button
 *
 * Refactored from original `button proj.txt` for 16x2 I2C LCD hardware.
 *
 * Hardware:
 * - NodeMCU ESP8266 (ESP-12E)
 * - 16x2 I2C LCD display (PCF8574 backpack, address 0x27)
 * - TTP223 capacitive touch sensor
 *
 * Features:
 * - WiFi connection with auto-reconnect
 * - 16x2 LCD status display (2 lines)
 * - Touch sensor on D6 (GPIO12) with debouncing
 * - Wake-on-LAN magic packet broadcasting
 * - Non-blocking millis() based timing throughout
 * - Comprehensive error handling
 * - State machine design
 * - Doxygen documentation
 *
 * @author Original: Unknown | Refactored: Dhanush
 * @version 1.0.0
 * @date 2025-07-05
 * @license MIT
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "Config.h"

// ============================================================================
// Hardware Objects
// ============================================================================
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLS, LCD_ROWS);
WiFiUDP udp;
WakeOnLan WOL(udp);

// ============================================================================
// State Machine
// ============================================================================
enum State { SETUP, WIFI_CONNECT, READY, WAKING, ERROR };
enum DispMode { NORMAL, WAKING, ERR };

static State state = SETUP;
static DispMode dispMode = NORMAL;

// ============================================================================
// Timing Variables (millis-based, no delay())
// ============================================================================
static unsigned long lastTouchMs = 0;
static unsigned long wakeEndMs = 0;
static unsigned long cooldownEndMs = 0;
static unsigned long lastWifiCheckMs = 0;
static unsigned long wifiStartMs = 0;

// ============================================================================
// Touch Debounce
// ============================================================================
static int lastTouch = LOW;
static const int DEBOUNCE_MS = TOUCH_DEBOUNCE_MS;

// ============================================================================
// Display Caching
// ============================================================================
static String lastLine1 = "";
static String lastLine2 = "";

// ============================================================================
// Error Tracking
// ============================================================================
static String errMsg = "";

// ============================================================================
// Function Prototypes
// ============================================================================
void updateDisplay(DispMode mode, const String& line1 = "", const String& line2 = "");
bool initDisplay();
bool wifiConnect();
void wifiMonitor();
bool touchPressed();
void sendWol();
void setError(const String& msg);
void runStateMachine();

// ============================================================================
// Setup
// ============================================================================
void setup() {
  Serial.begin(115200);
  Serial.println(F("\n=== ESP8266 Wake-on-LAN (16x2 LCD) ==="));

  pinMode(TOUCH_PIN, INPUT);

  if (!initDisplay()) {
    Serial.println(F("ERROR: LCD init failed"));
    setError(F("LCD Error"));
    state = ERROR;
    return;
  }

  updateDisplay(NORMAL, "Connecting WiFi...", "");

  if (wifiConnect()) {
    state = READY;
    WOL.setRepeat(WOL_REPEAT_COUNT, WOL_REPEAT_DELAY);
    WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
    Serial.println(F("WiFi connected. Ready."));
    updateDisplay(NORMAL, "WiFi Connected", "Ready to Touch");
  } else {
    state = ERROR;
    setError(F("WiFi Failed"));
  }
}

// ============================================================================
// Main Loop - Non-blocking
// ============================================================================
void loop() {
  wifiMonitor();
  runStateMachine();

  if (touchPressed() && state == READY && millis() >= cooldownEndMs) {
    state = WAKING;
    wakeEndMs = millis() + POST_WAKE_DELAY_MS;
    cooldownEndMs = millis() + COOLDOWN_MS;
  }

  // No delay() - allows WiFi background tasks
}

// ============================================================================
// Display Functions (16x2 LCD)
// ============================================================================
/**
 * @brief Update LCD with caching to reduce flicker
 *
 * LCD layout:
 * Line 1: Status (WiFi status or mode)
 * Line 2: Current action/state
 *
 * Original OLED code cleared display every update causing flicker.
 * This version only updates when content changes.
 */
void updateDisplay(DispMode mode, const String& line1, const String& line2) {
  String l1, l2;

  switch (mode) {
    case NORMAL:
      l1 = line1.length() ? line1 : (WiFi.status() == WL_CONNECTED ? "WiFi Connected" : "NO WiFi");
      l2 = line2.length() ? line2 : (state == READY ? "Ready to Touch" : "Waiting...");
      break;
    case WAKING:
      l1 = "WiFi Connected";
      l2 = "WAKING PC...";
      break;
    case ERR:
      l1 = "ERROR";
      l2 = errMsg.length() ? errMsg : "Error";
      break;
  }

  // Only update if changed (prevent flicker)
  if (l1 != lastLine1 || l2 != lastLine2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(l1);
    lcd.setCursor(0, 1);
    lcd.print(l2);

    lastLine1 = l1;
    lastLine2 = l2;
  }

  dispMode = mode;
}

// ============================================================================
// LCD Initialization
// ============================================================================
bool initDisplay() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(50); // Short delay for LCD to initialize (acceptable)
  return true;
}

// ============================================================================
// WiFi Connect with Timeout
// ============================================================================
bool wifiConnect() {
  Serial.print(F("Connecting to WiFi: "));
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  wifiStartMs = millis();

  while (WiFi.status() != WL_CONNECTED) {
    if (WIFI_TIMEOUT_MS > 0 && (millis() - wifiStartMs) >= WIFI_TIMEOUT_MS) {
      Serial.println(F("\nWiFi timeout"));
      return false;
    }
    static unsigned long lastDot = 0;
    if (millis() - lastDot >= 500) {
      Serial.print(F("."));
      lastDot = millis();
    }
    delay(10); // Small yield, OK during initial connect
  }

  Serial.println();
  Serial.print(F("Connected! IP: "));
  Serial.println(WiFi.localIP());
  return true;
}

// ============================================================================
// WiFi Auto-Reconnect
// ============================================================================
void wifiMonitor() {
  if (state == ERROR || state == WAKING) return;

  if (WiFi.status() != WL_CONNECTED) {
    if (state != WIFI_CONNECT) {
      Serial.println(F("WiFi lost, reconnecting..."));
      updateDisplay(NORMAL, "WiFi Disconnected", "Reconnecting...");
      state = WIFI_CONNECT;
      lastWifiCheckMs = millis();
    }
    if (millis() - lastWifiCheckMs >= 1000) {
      lastWifiCheckMs = millis();
      if (wifiConnect()) {
        state = READY;
        WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
        updateDisplay(NORMAL, "WiFi Connected", "Ready to Touch");
      }
    }
  }
}

// ============================================================================
// Touch with Debounce
// ============================================================================
bool touchPressed() {
  int cur = digitalRead(TOUCH_PIN);
  if (cur == HIGH && lastTouch == LOW) {
    if (millis() - lastTouchMs >= DEBOUNCE_MS) {
      lastTouchMs = millis();
      lastTouch = cur;
      Serial.println(F("Touch detected!"));
      return true;
    }
  }
  lastTouch = cur;
  return false;
}

// ============================================================================
// Send Magic Packet
// ============================================================================
void sendWol() {
  Serial.print(F("Sending WOL to MAC: "));
  for (int i = 0; i < 6; i++) {
    if (i) Serial.print(F(":"));
    if (TARGET_MAC[i] < 0x10) Serial.print(F("0"));
    Serial.print(TARGET_MAC[i], HEX);
  }
  Serial.println();

  WOL.sendMagicPacket(TARGET_MAC);
  Serial.println(F("Magic packet sent!"));

  // Show "Packet Sent" briefly
  updateDisplay(NORMAL, "Packet Sent!", "");
}

// ============================================================================
// Error Handler
// ============================================================================
void setError(const String& msg) {
  errMsg = msg;
  updateDisplay(ERR, "ERROR", msg);
}

// ============================================================================
// State Machine
// ============================================================================
void runStateMachine() {
  if (state != WAKING) return;

  static bool sent = false;
  if (!sent) {
    updateDisplay(WAKING);
    sendWol();
    sent = true;
  }

  if (millis() >= wakeEndMs) {
    sent = false;
    state = (WiFi.status() == WL_CONNECTED) ? READY : WIFI_CONNECT;
    if (state == READY) {
      updateDisplay(NORMAL, "WiFi Connected", "Ready to Touch");
    }
  }
}
