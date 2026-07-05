/**
 * @file ESP8266-Wake-on-LAN.ino
 * @brief ESP8266-based wireless PC wake-up device with OLED display
 *
 * This project uses an ESP8266 microcontroller to send Wake-on-LAN magic packets
 * to wake a desktop PC over the network. The device connects to WiFi and displays
 * its status on an SSD1306 OLED screen. A touch sensor (or button) triggers the
 * wake-up sequence.
 *
 * Features:
 * - WiFi connection with auto-reconnect
 * - SSD1306 OLED status display (128x64)
 * - Touch sensor with debouncing
 * - Wake-on-LAN magic packet broadcasting
 * - Non-blocking millis() based timing
 * - Comprehensive error handling
 * - Doxygen-style documentation
 *
 * @author Dhanush
 * @version 1.0.0
 * @date 2025-07-05
 * @license MIT
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Include user configuration
#include "Config.h"

// ============================================================================
// Global Objects
// ============================================================================

/** @brief OLED display object (128x64) */
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

/** @brief UDP socket for Wake-on-LAN */
WiFiUDP udp;

/** @brief Wake-on-LAN helper */
WakeOnLan WOL(udp);

// ============================================================================
// State Enumerations
// ============================================================================

/**
 * @brief Application state machine states
 */
enum class AppState {
  INITIALIZING,       ///< Starting up, initializing hardware
  WIFI_CONNECTING,    ///< Attempting to connect to WiFi
  WIFI_CONNECTED,     ///< WiFi connected and ready
  WIFI_DISCONNECTED,  ///< WiFi lost, attempting reconnect
  ERROR,              ///< Error state (display shows error message)
  WAKE_TRIGGERED      ///< Touch pressed, sending WOL packet
};

/**
 * @brief Display modes for the OLED
 */
enum class DisplayMode {
  NORMAL,             ///< Showing normal status
  WAKING,             ///< Showing wake-up in progress
  ERROR               ///< Showing an error message
};

// ============================================================================
// Global State Variables
// ============================================================================

/** @brief Current application state */
static AppState currentState = AppState::INITIALIZING;

/** @brief Current display mode */
static DisplayMode displayMode = DisplayMode::NORMAL;

/** @brief Last time the touch sensor state changed (for debouncing) */
static unsigned long lastTouchChangeTime = 0;

/** @brief Last time WiFi status was checked (for auto-reconnect) */
static unsigned long lastWifiCheckTime = 0;

/** @brief Timestamp when we entered the current display mode */
static unsigned long displayModeStartTime = 0;

/** @brief Last known touch sensor reading */
static int lastTouchReading = LOW;

/** @brief Error message to display (when in ERROR mode) */
static String errorMessage = "";

/** @brief Cool-down timer after wake trigger */
static unsigned long cooldownEndTime = 0;

/** @brief Whether we're currently in cooldown */
static bool inCooldown = false;

// ============================================================================
// Function Prototypes
// ============================================================================

/**
 * @brief Initialize the OLED display
 * @return true if display initialized successfully, false otherwise
 */
bool initializeDisplay();

/**
 * @brief Update the OLED display with current status
 * @param mode Display mode to show
 * @param customMessage Optional custom message to display (used for errors)
 */
void updateDisplay(DisplayMode mode, const String& customMessage = "");

/**
 * @brief Attempt to connect to WiFi with timeout
 * @return true if connected, false if timeout
 */
bool connectToWiFi();

/**
 * @brief Check WiFi connection and auto-reconnect if disconnected
 */
void handleWiFiConnection();

/**
 * @brief Process touch sensor input with debouncing
 * @return true if valid press detected, false otherwise
 */
bool checkTouchSensor();

/**
 * @brief Send Wake-on-LAN magic packet to target PC
 */
void sendWakePacket();

/**
 * @brief Enter error state with specified message
 * @param message Error message to display
 */
void setError(const String& message);

/**
 * @brief Handle application state machine transitions
 */
void updateState();

// ============================================================================
// Arduino Setup Function
// ============================================================================

/**
 * @brief Arduino initialization hook
 *
 * Runs once at startup. Initializes serial, display, WiFi, and hardware.
 */
void setup() {
  Serial.begin(115200);
  Serial.println(F("\n=== ESP8266 Wake-on-LAN ==="));

  // Initialize touch sensor pin
  pinMode(TOUCH_PIN, INPUT);

  // Initialize display
  if (!initializeDisplay()) {
    Serial.println(F("ERROR: Failed to initialize OLED display"));
    setError(F("Display Error"));
    currentState = AppState::ERROR;
    return;
  }

  // Show initial state
  updateDisplay(DisplayMode::NORMAL, F("Connecting..."));

  // Configure Wake-on-LAN
  WOL.setRepeat(WOL_REPEAT_COUNT, WOL_REPEAT_DELAY);

  // Connect to WiFi
  if (connectToWiFi()) {
    currentState = AppState::WIFI_CONNECTED;
    WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
    Serial.println(F("WiFi connected. Ready."));
    updateDisplay(DisplayMode::NORMAL, F("Ready to Wake"));
  } else {
    currentState = AppState::ERROR;
    setError(F("WiFi Failed"));
    Serial.println(F("ERROR: WiFi connection timeout"));
  }
}

// ============================================================================
// Arduino Main Loop
// ============================================================================

/**
 * @brief Arduino main loop
 *
 * Runs continuously after setup(). Handles WiFi auto-reconnect,
 * touch debouncing, and state transitions.
 */
void loop() {
  // Update WiFi connection status
  handleWiFiConnection();

  // Run state machine
  updateState();

  // Handle touch sensor input
  if (checkTouchSensor()) {
    if (!inCooldown) {
      currentState = AppState::WAKE_TRIGGERED;
      inCooldown = true;
      cooldownEndTime = millis() + COOLDOWN_MS;
    }
  }

  // Check if cooldown period has ended
  if (inCooldown && millis() >= cooldownEndTime) {
    inCooldown = false;
  }

  // Small delay to reduce CPU usage (non-blocking, allows WiFi background tasks)
  delay(1);
}

// ============================================================================
// Function Implementations
// ============================================================================

bool initializeDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    return false;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();

  return true;
}

void updateDisplay(DisplayMode mode, const String& customMessage) {
  static String lastStatus = "";
  static String lastAction = "";

  String statusText;
  String actionText;

  switch (mode) {
    case DisplayMode::NORMAL:
      if (customMessage.length() > 0) {
        actionText = customMessage;
      } else {
        actionText = currentState == AppState::WIFI_CONNECTED ? F("Ready to Wake") : F("Waiting");
      }
      statusText = WiFi.status() == WL_CONNECTED ? F("CONNECTED") : F("NO WiFi");
      break;

    case DisplayMode::WAKING:
      actionText = F("WAKING PC... 🔥");
      statusText = WiFi.status() == WL_CONNECTED ? F("CONNECTED") : F("NO WiFi");
      break;

    case DisplayMode::ERROR:
      actionText = customMessage.length() > 0 ? customMessage : F("Error");
      statusText = F("ERROR");
      break;
  }

  // Only update if content changed (prevents flicker)
  if (statusText != lastStatus || actionText != lastAction) {
    display.clearDisplay();

    display.setCursor(0, 0);
    display.print(F("PC REMOTE BOOTER"));
    display.drawLine(0, 14, OLED_WIDTH - 1, 14, SSD1306_WHITE);

    display.setCursor(0, 24);
    display.print(F("WiFi: "));
    display.println(statusText);

    display.setCursor(0, 45);
    display.print(F("> "));
    display.println(actionText);

    display.display();

    lastStatus = statusText;
    lastAction = actionText;
  }

  displayModeStartTime = millis();
}

bool connectToWiFi() {
  Serial.print(F("Connecting to WiFi: "));
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED) {
    delay(WIFI_CHECK_INTERVAL_MS);

    if (WIFI_TIMEOUT_MS > 0 && (millis() - startTime) >= WIFI_TIMEOUT_MS) {
      Serial.println(F("\nERROR: WiFi connection timeout"));
      return false;
    }

    Serial.print(F("."));
  }

  Serial.println();
  Serial.print(F("Connected! IP: "));
  Serial.println(WiFi.localIP());

  return true;
}

void handleWiFiConnection() {
  if (currentState == AppState::ERROR) {
    return;  // Don't attempt reconnect in error state
  }

  if (WiFi.status() != WL_CONNECTED) {
    if (currentState != AppState::WIFI_DISCONNECTED) {
      Serial.println(F("WiFi disconnected. Attempting reconnect..."));
      currentState = AppState::WIFI_DISCONNECTED;
      updateDisplay(DisplayMode::NORMAL, F("Reconnecting..."));
    }

    // Try to reconnect every second
    if (millis() - lastWifiCheckTime >= 1000) {
      lastWifiCheckTime = millis();

      if (connectToWiFi()) {
        currentState = AppState::WIFI_CONNECTED;
        WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
        updateDisplay(DisplayMode::NORMAL, F("Ready to Wake"));
      }
    }
  }
}

bool checkTouchSensor() {
  int currentReading = digitalRead(TOUCH_PIN);

  // Rising edge detection (LOW to HIGH for touch sensors)
  if (currentReading == HIGH && lastTouchReading == LOW) {
    // Debounce check
    if (millis() - lastTouchChangeTime >= TOUCH_DEBOUNCE_MS) {
      lastTouchChangeTime = millis();
      lastTouchReading = currentReading;
      return true;
    }
  }

  lastTouchReading = currentReading;
  return false;
}

void sendWakePacket() {
  Serial.print(F("Sending Wake-on-LAN to MAC: "));
  for (int i = 0; i < 6; i++) {
    if (i > 0) Serial.print(F(":"));
    if (TARGET_MAC[i] < 0x10) Serial.print(F("0"));
    Serial.print(TARGET_MAC[i], HEX);
  }
  Serial.println();

  WOL.sendMagicPacket(TARGET_MAC);

  Serial.println(F("Magic packet sent!"));
}

void setError(const String& message) {
  errorMessage = message;
  displayMode = DisplayMode::ERROR;
  updateDisplay(DisplayMode::ERROR, message);
}

void updateState() {
  switch (currentState) {
    case AppState::INITIALIZING:
      // Should transition to WIFI_CONNECTING in setup
      break;

    case AppState::WAKE_TRIGGERED:
      updateDisplay(DisplayMode::WAKING, F("WAKING PC..."));
      sendWakePacket();

      // Keep "WAKING" message visible for POST_WAKE_DELAY_MS
      delay(POST_WAKE_DELAY_MS);

      currentState = WiFi.status() == WL_CONNECTED ? AppState::WIFI_CONNECTED : AppState::WIFI_DISCONNECTED;
      updateDisplay(DisplayMode::NORMAL);
      break;

    case AppState::ERROR:
    case AppState::WIFI_CONNECTING:
    case AppState::WIFI_CONNECTED:
    case AppState::WIFI_DISCONNECTED:
      // These states are handled by other functions
      break;
  }
}
