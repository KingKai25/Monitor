# Sơ Đồ Chân GPIO - Pin Mapping Reference

## 📌 Bảng Chân GPIO cập nhật theo sơ đồ của bạn

### Chân ESP32 → Thiết Bị Ngoại Vi

```
ESP32 Pin  | Thiết Bị                | Tín Hiệu       | GPIO# | Loại
-----------|-------------------------|----------------|-------|--------
D4         | Buzzer (Còi báo)       | Control        | 4     | Output
D18        | PZEM-004T              | TX (Serial1)   | 18    | UART TX
D19        | PZEM-004T              | RX (Serial1)   | 19    | UART RX
D23        | Relay Module (Đèn)     | Control        | 23    | Output
D25        | MQ2 Smoke Sensor       | Analog OUT     | 25    | ADC Input
D26        | Button (Nút nhấn)      | Input          | 26    | Digital Input
D32        | DHT11                  | Data           | 32    | 1-Wire
D33        | Fire Sensor (Cảm biến) | Analog OUT     | 33    | ADC Input
RX2        | AS608 Fingerprint      | TX (Serial2)   | 16    | UART RX
TX2        | AS608 Fingerprint      | RX (Serial2)   | 17    | UART TX
GND        | Ground (Chung)         | -              | -     | -
5V         | Power 5V (Các module)  | -              | -     | -
3.3V       | Power 3.3V (DHT11)     | -              | -     | -
```

---

## 🔌 Số Hiệu Chân Chi Tiết

### Input Pins (Chân Đầu Vào)
```
GPIO4   → D4   : Buzzer (Còi báo)
GPIO25  → D25  : MQ2 Smoke Sensor (Cảm biến khí)
GPIO26  → D26  : Button (Nút nhấn)
GPIO32  → D32  : DHT11 Data (Dữ liệu nhiệt độ/độ ẩm)
GPIO33  → D33  : Fire Sensor (Cảm biến lửa/nhiệt)
GPIO16  → RX2  : AS608 RX (Cổng Serial2 RX)
GPIO19  → D19  : PZEM RX (Cổng Serial1 RX)
```

### Output Pins (Chân Đầu Ra)
```
GPIO4   → D4   : Buzzer Control (Điều khiển còi)
GPIO23  → D23  : Relay Control (Điều khiển relay đèn)
GPIO17  → TX2  : AS608 TX (Cổng Serial2 TX)
GPIO18  → D18  : PZEM TX (Cổng Serial1 TX)
```

### UART Serial Ports (Cổng Serial)
```
Serial0 (Monitor):
  TX: GPIO1 (Kết nối USB)
  RX: GPIO3 (Kết nối USB)
  Baud: 115200 (cho debug/monitor)

Serial1 (PZEM-004T):
  TX: GPIO18 (D18)
  RX: GPIO19 (D19)
  Baud: 9600

Serial2 (AS608 Fingerprint):
  TX: GPIO17 (TX2)
  RX: GPIO16 (RX2)
  Baud: 57600
```

---

## 🎯 Sơ Đồ Kết Nối

### Sơ đồ Tổng Quát Kết Nối

```
┌─────────────────────────────────────────────────────────┐
│                      ESP32 DEV BOARD                     │
│                                                          │
│  3.3V ──→ [DHT11 VCC]                                  │
│  GND  ──→ [DHT11 GND]                                  │
│  D32  ──→ [DHT11 DATA]                                 │
│                                                          │
│  5V   ──→ [MQ2 VCC]                                    │
│  GND  ──→ [MQ2 GND]                                    │
│  D25  ──→ [MQ2 AO] (Analog Output)                     │
│                                                          │
│  5V   ──→ [Fire Sensor VCC]                            │
│  GND  ──→ [Fire Sensor GND]                            │
│  D33  ──→ [Fire Sensor AO]                             │
│                                                          │
│  5V   ──→ [Button + 10kΩ pull-up to 3.3V]             │
│  D26  ──→ [Button (khác chân)]                         │
│  GND  ──→ [Button GND]                                 │
│                                                          │
│  5V   ──→ [Relay VCC]                                  │
│  GND  ──→ [Relay GND]                                  │
│  D23  ──→ [Relay IN]                                   │
│                                                          │
│  D4   ──→ [Buzzer + (qua điện trở 470Ω)]             │
│  GND  ──→ [Buzzer - (qua diode 1N4007)]               │
│                                                          │
│  D18  ──→ [PZEM TX]                                    │
│  D19  ──→ [PZEM RX]                                    │
│  5V   ──→ [PZEM VCC]                                   │
│  GND  ──→ [PZEM GND]                                   │
│                                                          │
│  TX2  ──→ [AS608 RX]                                   │
│  RX2  ──→ [AS608 TX]                                   │
│  3.3V ──→ [AS608 VCC]                                  │
│  GND  ──→ [AS608 GND]                                  │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

---

## 📝 config.h - Các Định Nghĩa Chân

Mở file `include/config.h`, bạn sẽ thấy các dòng này:

```cpp
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
```

---

## ✅ Checklist Kết Nối Dây

Trước khi upload firmware, kiểm tra:

- [ ] DHT11 VCC → 3.3V
- [ ] DHT11 GND → GND
- [ ] DHT11 DATA → GPIO32 (D32) + Pull-up 10kΩ
- [ ] MQ2 VCC → 5V
- [ ] MQ2 GND → GND
- [ ] MQ2 AO → GPIO25 (D25)
- [ ] Fire Sensor VCC → 5V
- [ ] Fire Sensor GND → GND
- [ ] Fire Sensor AO → GPIO33 (D33)
- [ ] Button 1 pin → 3.3V (qua 10kΩ pull-up)
- [ ] Button pin khác → GPIO26 (D26)
- [ ] Button GND → GND
- [ ] Relay VCC → 5V
- [ ] Relay GND → GND
- [ ] Relay IN → GPIO23 (D23)
- [ ] Buzzer + → GPIO4 (D4) (qua điện trở 470Ω)
- [ ] Buzzer - → GND (qua diode 1N4007)
- [ ] PZEM TX → GPIO18 (D18)
- [ ] PZEM RX → GPIO19 (D19)
- [ ] PZEM VCC → 5V
- [ ] PZEM GND → GND
- [ ] AS608 RX → GPIO17 (TX2)
- [ ] AS608 TX → GPIO16 (RX2)
- [ ] AS608 VCC → 3.3V
- [ ] AS608 GND → GND
- [ ] Tất cả GND được nối với nhau

---

## 🔄 So Sánh: Cũ vs Mới

| Thiết Bị | Chân Cũ | Chân Mới | Cập Nhật |
|---------|--------|---------|--------|
| DHT11 | GPIO25 (D25) | GPIO32 (D32) | ✅ Thay đổi |
| MQ2 | GPIO35 (D35) | GPIO25 (D25) | ✅ Thay đổi |
| Fire Sensor | GPIO36 (D36) | GPIO33 (D33) | ✅ Thay đổi |
| Button | GPIO34 (D34) | GPIO26 (D26) | ✅ Thay đổi |
| Relay | GPIO32 (D32) | GPIO23 (D23) | ✅ Thay đổi |
| Buzzer | GPIO33 (D33) | GPIO4 (D4) | ✅ Thay đổi |
| PZEM RX | GPIO16 | GPIO19 (D19) | ✅ Thay đổi |
| PZEM TX | GPIO17 | GPIO18 (D18) | ✅ Thay đổi |
| AS608 RX | GPIO9 | GPIO16 (RX2) | ✅ Thay đổi |
| AS608 TX | GPIO10 | GPIO17 (TX2) | ✅ Thay đổi |

---

## 🧪 Cách Kiểm Tra Chân

### Dùng Serial Monitor để Kiểm Tra:

1. Upload firmware
2. Mở Serial Monitor bằng lệnh:
   ```bash
   pio device monitor --baud 115200
   ```

3. Bạn sẽ thấy dòng khởi động:
   ```
   ========== WAREHOUSE MONITOR STARTING ==========
   [WiFi] Connecting to YOUR_SSID...
   [DHT] DHT11 initialized
   [MQ2] MQ2 smoke sensor initialized
   [Fingerprint] AS608 fingerprint sensor initialized
   ...
   ```

### Kiểm Tra Button:
1. Nhấn button
2. Kiểm tra:
   - Relay phải click (âm thanh kích điện)
   - Light toggle ON/OFF
   - Serial monitor phải hiển thị: `Light toggled`

### Kiểm Tra Cảm Biến:
1. Kiểm tra DHT11 hiển thị nhiệt độ/độ ẩm (không phải 0)
2. Wave hand qua MQ2 → Giá trị phải tăng
3. Fire sensor → Giá trị thay đổi khi có nhiệt

---

## 🚀 Bước Tiếp Theo

1. ✅ Kiểm tra lại tất cả chân
2. ✅ Upload firmware: `pio run -t upload`
3. ✅ Mở serial monitor: `pio device monitor --baud 115200`
4. ✅ Kiểm tra các thông báo khởi động
5. ✅ Test button → Relay phải click
6. ✅ Vào Blynk app → Device phải "Online"

---

## 📞 Hỗ Trợ

**Nếu chân không hoạt động bình thường:**
1. Kiểm tra lại phần kết nối dây
2. Kiểm tra chân có bị gập không
3. Kiểm tra GND được nối chung không
4. Kiểm tra 5V và 3.3V không bị đảo
5. Restart ESP32
6. Upload lại firmware

---

**Cập nhật cuối cùng**: March 2026  
**Phiên bản**: 1.0.0  
**Trạng thái**: ✅ Sẵn sàng sử dụng
