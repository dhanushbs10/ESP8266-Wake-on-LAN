/**
 * @file Config.h
 * @brief User configuration for ESP8266 Wake-on-LAN project
 *
 * This file contains all user-specific settings for the NodeMCU ESP8266
 * with 16x2 I2C LCD display and TTP223 touch sensor.
 *
 * IMPORTANT: Do not share this file or commit sensitive information (WiFi password,
 * MAC addresses) to public repositories. Keep this file private or use environment
 * variables in production.
 */

#pragma once

// ============================================================================
// WiFi Configuration
// ============================================================================

/** @brief Your WiFi network SSID */
const char* WIFI_SSID = "YourWiFiSSID";

/** @brief Your WiFi network password */
const char* WIFI_PASSWORD = "YourWiFiPassword";

// ============================================================================
// Target PC Configuration
// ============================================================================

/**
 * @brief MAC address of the target PC to wake
 *
 * This is the network interface MAC address of the computer you want to wake.
 * Format: {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX}
 *
 * Find your MAC address:
 * - Windows: ipconfig /all | findstr "Physical"
 * - Linux: ip link show
 * - macOS: ifconfig | grep "ether"
 */
const byte TARGET_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// ============================================================================
// Hardware Pin Configuration (NodeMCU ESP8266)
// ============================================================================

/**
 * @brief GPIO pin connected to the TTP223 touch sensor output
 *
 * NodeMCU pin mapping:
 * - D1 = GPIO5  (I2C SCL)
 * - D2 = GPIO4  (I2C SDA)
 * - D6 = GPIO12 (Touch sensor OUT)
 *
 * Power: Touch VCC → 3.3V, Touch GND → GND
 */
const int TOUCH_PIN = 12;  // GPIO12 (NodeMCU D6)

// ============================================================================
// LCD Display Configuration (16x2 I2C)
// ============================================================================

/**
 * @brief I2C address of the 16x2 LCD display
 *
 * Common I2C addresses for 16x2 LCD with PCF8574 backpack:
 * - 0x27 (most common)
 * - 0x3F (some modules)
 *
 * Run an I2C scanner sketch if unsure.
 */
const int LCD_I2C_ADDRESS = 0x27;

/**
 * @brief Number of columns on the LCD (16 for 16x2)
 */
const int LCD_COLS = 16;

/**
 * @brief Number of rows on the LCD (2 for 16x2)
 */
const int LCD_ROWS = 2;

// ============================================================================
// Wake-on-LAN Configuration
// ============================================================================

/**
 * @brief Number of times to send the magic packet (for reliability)
 */
const int WOL_REPEAT_COUNT = 3;

/**
 * @brief Delay between WOL packet repeats in milliseconds
 */
const int WOL_REPEAT_DELAY = 100;

// ============================================================================
// Touch Sensor Configuration
// ============================================================================

/**
 * @brief Debounce delay for the touch sensor in milliseconds
 *
 * Prevents multiple triggers from a single press due to contact bouncing.
 * 200ms provides a good balance between responsiveness and debouncing.
 */
const int TOUCH_DEBOUNCE_MS = 200;

/**
 * @brief Extra delay after waking to prevent accidental re-triggering
 *
 * After sending the magic packet, we show a "WAKING" message for this
 * duration before returning to the ready state.
 */
const int POST_WAKE_DELAY_MS = 3000;

/**
 * @brief Cool-down period after waking before accepting new touches
 *
 * Prevents rapid successive wake attempts.
 */
const int COOLDOWN_MS = 1000;

// ============================================================================
// WiFi Connection Configuration
// ============================================================================

/**
 * @brief WiFi connection timeout in milliseconds
 *
 * How long to wait for WiFi connection before timing out.
 * Set to 0 for infinite wait.
 */
const unsigned long WIFI_TIMEOUT_MS = 30000;

/**
 * @brief WiFi connection check interval in milliseconds
 */
const int WIFI_CHECK_INTERVAL_MS = 500;

// ============================================================================
// Display Behavior Configuration
// ============================================================================

/**
 * @brief How long to show connection failures before retrying (ms)
 */
const int ERROR_DISPLAY_DURATION_MS = 3000;
