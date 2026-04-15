#pragma once

// ============================================================
//  CẤU HÌNH HỆ THỐNG GIÁM SÁT KHO HÀNG
//  Chỉnh sửa các thông số bên dưới phù hợp với phần cứng
// ============================================================

// ---------- Cấu hình WiFi ----------
#define WIFI_SSID "V.M.N.DEC"
#define WIFI_PASSWORD ""

// ---------- Cấu hình Blynk IoT ----------
#define BLYNK_TEMPLATE_ID "TMPL62490Ytpd"
#define BLYNK_TEMPLATE_NAME "Warehouse Monitor"
#define BLYNK_AUTH_TOKEN "8DCoq8k9dRpCYK5CanmygDgTEMG5Tkfd"

// ---------- Sơ đồ chân GPIO ----------
#define DHT11_PIN 32              // Cảm biến nhiệt độ & độ ẩm DHT11 (D32)
#define MQ2_PIN 25                // Cảm biến khói MQ2 - chân digital (D25)
#define FIRE_SENSOR_PIN 33        // Cảm biến lửa hồng ngoại (D33)
#define BUTTON_PIN 23             // Nút nhấn bật/tắt đèn (D23)
#define BUZZER_PIN 4              // Còi báo động (D4)
#define RELAY_PIN 26             // Relay điều khiển đèn (D26)

// ---------- Chân UART ----------
#define PZEM_RX 18                // PZEM-004T RX (UART1) - thử đổi chân
#define PZEM_TX 19                // PZEM-004T TX (UART1) - thử đổi chân
#define FINGERPRINT_RX 17         // AS608 RX (UART2)
#define FINGERPRINT_TX 16         // AS608 TX (UART2)
#define TCH_PIN 22                // AS608 chân phát hiện chạm (D22)

// ---------- Ngưỡng cảnh báo ----------
#define SMOKE_THRESHOLD 400       // Ngưỡng khói (analog - không dùng do ADC2 conflict)
#define TEMP_THRESHOLD 40         // Ngưỡng nhiệt độ cảnh báo cháy (°C)
#define FIRE_THRESHOLD 500        // Ngưỡng cảm biến lửa (analog)

// ---------- Chu kỳ đọc cảm biến (ms) ----------
#define DHT_READ_INTERVAL 5000        // Đọc DHT11 mỗi 5 giây
#define MQ2_READ_INTERVAL 1000        // Đọc MQ2 mỗi 1 giây
#define PZEM_READ_INTERVAL 2000       // Đọc PZEM mỗi 2 giây
#define BLYNK_UPDATE_INTERVAL 2000    // Cập nhật Blynk mỗi 2 giây
#define FINGER_SCAN_INTERVAL 500      // Quét vân tay mỗi 500ms

// ---------- Blynk Virtual Pins (Free Plan - 5 datastreams) ----------
#define V_TEMP_HUMID V0           // String: "T:xx°C | H:xx%" (nhiệt độ + độ ẩm)
#define V_POWER V1                // Double: Công suất (W) - SuperChart
#define V_FIRE_ALERT V2           // Integer: LED báo cháy (đổi màu trắng/đỏ)
#define V_FINGER_TERM V3          // String: Terminal điều khiển vân tay
#define V_LIGHT_SW V4             // Integer: 0=Tắt đèn, 1=Bật đèn (Switch)
#define V_SMOKE_ALERT V5          // Integer: LED báo khói (đổi màu trắng/cam)

// ---------- Cảm biến vân tay AS608 ----------
#define FINGERPRINT_BAUD 57600        // Baudrate giao tiếp AS608 (thực tế 57600)
#define DEFAULT_ENROLLMENT_ATTEMPTS 3 // Số lần thử đăng kí

// ---------- Đo điện PZEM-004T v3 ----------
#define PZEM_BAUD 9600                // Baudrate Modbus RTU

// ---------- Google Sheets ----------
// Thay URL sau khi deploy Google Apps Script (xem file google_apps_script.js)
#define GOOGLE_SCRIPT_URL "https://script.google.com/macros/s/AKfycbw0ktkUoFzwpU-gwnFXvpEwQeq8mxdNVev1Hv66EjkUuArPwdKCbNok3PzmO40UUKfZIQ/exec"

// ---------- Đồng bộ thời gian NTP ----------
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC (7 * 3600)     // Múi giờ GMT+7 (Việt Nam)
#define DAYLIGHT_OFFSET_SEC 0         // Không dùng DST

// ---------- Hệ thống ----------
#define SERIAL_MONITOR_BAUD 115200    // Baudrate Serial Monitor
