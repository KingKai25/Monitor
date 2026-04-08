#pragma once

// WiFi Configuration
#define WIFI_SSID "V.M.N.DEC"
#define WIFI_PASSWORD ""

// Blynk Configuration
#define BLYNK_TEMPLATE_ID "TMPL62490Ytpd"
#define BLYNK_TEMPLATE_NAME "Warehouse Monitor"
#define BLYNK_AUTH_TOKEN "8DCoq8k9dRpCYK5CanmygDgTEMG5Tkfd"

// Pin Mapping
#define DHT11_PIN 32              // DHT11 (D32)
#define MQ2_PIN 25                // MQ2 analog (D25)
#define FIRE_SENSOR_PIN 33        // Fire sensor (D33)
#define BUTTON_PIN 26             // Button (D26)
#define BUZZER_PIN 4              // Buzzer (D4)
#define RELAY_PIN 23              // Relay đèn (D23)

// UART Pins
#define PZEM_RX 19
#define PZEM_TX 18
#define FINGERPRINT_RX 17
#define FINGERPRINT_TX 16
#define TCH_PIN 21                // AS608 touch detect (D21)

// Sensor Thresholds
#define SMOKE_THRESHOLD 400
#define TEMP_THRESHOLD 40
#define FIRE_THRESHOLD 500

// Reading Intervals (ms)
#define DHT_READ_INTERVAL 5000
#define MQ2_READ_INTERVAL 1000
#define PZEM_READ_INTERVAL 2000
#define BLYNK_UPDATE_INTERVAL 2000    // 2s - cập nhật nhanh
#define FINGER_SCAN_INTERVAL 500      // 500ms - quét vân tay

// Blynk Virtual Pins (Free Plan - 5 datastreams)
#define V_TEMP_HUMID V0           // String: "T:xx°C | H:xx%"
#define V_POWER V1                // Double: Công suất (W) - SuperChart
#define V_FIRE_ALERT V2           // Integer: LED báo cháy (đổi màu qua setProperty)
#define V_FINGER_TERM V3          // String: Terminal - nhập ID vân tay
#define V_LIGHT_SW V4             // Integer: 0=Tắt đèn, 1=Bật đèn (Switch)
#define V_SMOKE_ALERT V5           // Integer: LED báo khói (đổi màu qua setProperty)

// Fingerprint
#define FINGERPRINT_BAUD 67200
#define DEFAULT_ENROLLMENT_ATTEMPTS 3

// PZEM-004T
#define PZEM_BAUD 9600

// Google Sheets (thay URL sau khi deploy Apps Script)
#define GOOGLE_SCRIPT_URL "https://script.google.com/macros/s/AKfycbw0ktkUoFzwpU-gwnFXvpEwQeq8mxdNVev1Hv66EjkUuArPwdKCbNok3PzmO40UUKfZIQ/exec"

// NTP Time
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC (7 * 3600)     // GMT+7 Vietnam
#define DAYLIGHT_OFFSET_SEC 0

// System
#define SERIAL_MONITOR_BAUD 115200
