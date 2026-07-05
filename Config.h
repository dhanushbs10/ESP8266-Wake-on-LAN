/**
 * @file Config.h
 * @brief User configuration for ESP8266 Wake-on-LAN project
 *
 * This file contains all user-specific settings that need to be customized
 * for your local network and hardware setup.
 *
 * IMPORTANT: Do not share this file or commit sensitive information (WiFi password,
 * MAC addresses) to public repositories. This file should be kept private or
 * values should be sourced from environment variables in production builds.
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
 * Find your MAC address in Windows: ipconfig /all | findstr "Physical"
 * Find on Linux: ip link show
 * Find on macOS: ifconfig | grep "ether"
 */
const byte TARGET_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// ============================================================================
// Hardware Pin Configuration
// ============================================================================

/**
 * @brief GPIO pin connected to the touch sensor/button
 *
 * NodeMCU/ESP8266 pin mapping:
 * - D0 = GPIO16
 * - D1 = GPIO5
 * - D2 = GPIO4
 * - D3 = GPIO0
 * - D4 = GPIO2
 * - D5 = GPIO14  (commonly used for touch sensors)
 * - D6 = GPIO12
 * - D7 = GPIO13
 * - D8 = GPIO15
 */
const int TOUCH_PIN = 14;  // GPIO14 (NodeMCU D5)

// ============================================================================
// OLED Display Configuration
// ============================================================================

/**
 * @brief I2C address of the SSD1306 OLED display
 *
 * Most SSD1306 displays use 0x3C or 0x3D. Check your display's documentation.
 */
const int OLED_I2C_ADDRESS = 0x3C;

/**
 * @brief Width of the OLED display in pixels
 */
const int OLED_WIDTH = 128;

/**
 * @brief Height of the OLED display in pixels
 */
const int OLED_HEIGHT = 64;

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
