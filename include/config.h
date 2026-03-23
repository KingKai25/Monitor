#pragma once

// WiFi Configuration
#define WIFI_SSID "V.M.N.DEC"
#define WIFI_PASSWORD ""

// Blynk Configuration
#define BLYNK_TEMPLATE_ID "TMPL62490Ytpd"
#define BLYNK_TEMPLATE_NAME "Warehouse Monitor"
#define BLYNK_AUTH_TOKEN "8DCoq8k9dRpCYK5CanmygDgTEMG5Tkfd"
#define BLYNK_PRINT Serial

// Pin Mapping (Based on your schematic - Updated)
#define DHT11_PIN 32              // Temperature/Humidity sensor (D32)
#define MQ2_PIN 25                // Smoke analog input (D25)
#define FIRE_SENSOR_PIN 33        // Fire detection (D33)
#define BUTTON_PIN 26             // Physical button (D26)
#define BUZZER_PIN 4              // Buzzer alert (D4)
#define RELAY_PIN 23              // Light relay control (D23)

// UART Pin Mapping
#define PZEM_RX 19                // PZEM-004T RX (D19 on Serial1)
#define PZEM_TX 18                // PZEM-004T TX (D18 on Serial1)
#define FINGERPRINT_RX 16         // AS608 RX (RX2 on Serial2)
#define FINGERPRINT_TX 17         // AS608 TX (TX2 on Serial2)

// Sensor Thresholds
#define SMOKE_THRESHOLD 400       // MQ2 analog value
#define TEMP_THRESHOLD 40         // Temperature in Celsius
#define FIRE_THRESHOLD 500        // Fire sensor analog threshold
#define HUMIDITY_WARNING 80       // High humidity warning level

// Reading Intervals (milliseconds)
#define DHT_READ_INTERVAL 5000    // 5 seconds
#define MQ2_READ_INTERVAL 1000    // 1 second
#define PZEM_READ_INTERVAL 2000   // 2 seconds
#define BUTTON_DEBOUNCE 200       // ms

// Blynk Virtual Pins
#define V_LIGHT_CONTROL V1        // Light ON/OFF
#define V_BUZZER_TEST V2          // Test buzzer
#define V_TEMPERATURE V3          // Temp reading
#define V_HUMIDITY V4             // Humidity reading
#define V_SMOKE_LEVEL V5          // Smoke level
#define V_POWER V6                // Power consumption
#define V_ENERGY V7               // Energy consumed
#define V_VOLTAGE V8              // Voltage
#define V_CURRENT V9              // Current
#define V_FIRE_ALERT V10          // Fire status
#define V_SMOKE_ALERT V11         // Smoke status
#define V_LIGHT_STATUS V12        // Light state indicator

// Google Sheets Integration
#define GOOGLE_SCRIPT_ID "AKfycby1xw6E5SZI3QbD-ENx8ygVtkFo6qqXPNdQyUrAU680L-LhrzGXpxGPDU3W8rRZXOK0Dg"
#define GOOGLE_SHEET_WEBHOOK "https://script.google.com/macros/d/AKfycby1xw6E5SZI3QbD-ENx8ygVtkFo6qqXPNdQyUrAU680L-LhrzGXpxGPDU3W8rRZXOK0Dg/usercallback"

// Fingerprint Configuration
#define FINGERPRINT_BAUD 57600
#define MAX_FINGERPRINTS 10       // Maximum enrolled fingerprints
#define DEFAULT_ENROLLMENT_ATTEMPTS 3

// PZEM-004T Configuration
#define PZEM_BAUD 9600
#define PZEM_SLAVE_ID 0xF8

// System Configuration
#define WIFI_RECONNECT_INTERVAL 30000  // 30 seconds
#define BLYNK_RECONNECT_INTERVAL 5000  // 5 seconds
#define SERIAL_MONITOR_BAUD 115200
#define MAX_ALERT_BUZZER_CYCLES 3      // Number of buzzer iterations

// Time Zone (for timestamp logging)
#define TIMEZONE 7                // UTC+7 for Vietnam
#define DST_OFFSET 0              // No daylight saving
