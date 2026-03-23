# Hệ Thống Giám Sát Kho Hàng (ESP32)

Hệ thống giám sát kho hàng IoT toàn diện với kiểm soát truy cập nhân viên, giám sát môi trường và theo dõi tiêu thụ điện năng.

## Tính Năng

### 🔐 Kiểm Soát Truy Cập
- **Nhận dạng vân tay**: Cảm biến vân tay AS608 để xác thực nhân viên
- **Ghi nhận tự động**: Lưu vào/ra của nhân viên lên Google Sheets kèm thời gian
- **Cơ sở dữ liệu nhân viên**: Lưu trữ tối đa 81 vân tay đã đăng ký (có thể cấu hình)

### 📊 Giám Sát Môi Trường
- **Nhiệt độ & Độ ẩm**: Cảm biến DHT11 giám sát thời gian thực
- **Phát hiện khói**: Cảm biến MQ2 analog kiểm tra chất lượng không khí
- **Phát hiện cháy**: Cảm biến lửa chuyên dụng với cảnh báo tự động
- **Cảnh báo nhiệt độ**: Kích hoạt còi khi nhiệt độ cao

### ⚡ Giám Sát Điện Năng
- **PZEM-004T v3**: Đo tiêu thụ điện năng thời gian thực
  - Điện áp (0-300V)
  - Dòng điện (0-100A)
  - Công suất (0-23000W)
  - Năng lượng tiêu thụ (kWh)
  - Tần số (Hz)
  - Hệ số công suất

### 💡 Điều Khiển Đèn
- **Điều khiển Relay**: Bật/tắt qua nút nhấn vật lý hoặc ứng dụng di động
- **Điều khiển từ xa**: Tích hợp ứng dụng Blynk
- **Theo dõi điện năng**: Giám sát tiêu thụ điện của bóng đèn

### 📱 Ứng Dụng Di Động & Tích Hợp Cloud
- **Tích hợp Blynk**: Bảng điều khiển cảm biến thời gian thực
  - Biểu đồ nhiệt độ
  - Hiển thị độ ẩm
  - Cảnh báo khói/cháy
  - Biểu đồ tiêu thụ điện
  - Công tắc điều khiển đèn
- **Ghi log Google Sheets**: Sao lưu dữ liệu tự động
- **Thông báo đẩy**: Cảnh báo trên điện thoại

### 🚨 Hệ Thống Cảnh Báo
- **Thông báo còi**: Cảnh báo âm thanh cho sự kiện nghiêm trọng
- **Cảnh báo đa cấp**: Phát hiện cháy, khói, nhiệt độ cao
- **Ghi log sự kiện**: Tất cả cảnh báo được ghi lên cloud

## Linh Kiện Phần Cứng

```
📦 Bộ Điều Khiển Chính
└── Board phát triển ESP32

🎯 Cảm Biến & Đầu Vào
├── Module vân tay AS608 (UART2 - Serial2)
├── Cảm biến nhiệt độ/độ ẩm DHT11 (GPIO32)
├── Cảm biến khói MQ2 (ADC GPIO25)
├── Cảm biến lửa (GPIO33)
└── Nút nhấn vật lý (GPIO26)

⚡ Quản Lý Điện Năng
├── Module đo điện PZEM-004T (UART1 - Serial1)
└── Module Relay (GPIO23)

🔊 Thiết Bị Đầu Ra
├── Còi/Buzzer (GPIO4)
└── Nguồn cấp 5V DC

📡 Kết Nối
└── WiFi (tích hợp sẵn ESP32)
```

## Sơ Đồ Chân GPIO

| Thiết Bị | Chân | Giao Tiếp | Tốc Độ |
|----------|------|-----------|--------|
| DHT11 | GPIO32 (D32) | 1-Wire | - |
| MQ2 | GPIO25 (D25) | Analog ADC | - |
| Cảm biến lửa | GPIO33 (D33) | Analog ADC | - |
| Nút nhấn | GPIO26 (D26) | Digital | - |
| Còi/Buzzer | GPIO4 (D4) | Digital | - |
| Relay | GPIO23 (D23) | Digital | - |
| AS608 | TX:GPIO17, RX:GPIO16 | UART2 (Serial2) | 57600 |
| PZEM-004T | TX:GPIO18, RX:GPIO19 | UART1 (Serial1) | 9600 |

## Cài Đặt & Thiết Lập

### 1. Thiết lập PlatformIO
```bash
# Cài đặt VS Code + Extension PlatformIO
# Clone/mở dự án này
cd /path/to/Monitor
```

### 2. Cấu hình thông tin xác thực
Sửa file `include/config.h`:
```cpp
#define WIFI_SSID "TEN_WIFI_CUA_BAN"
#define WIFI_PASSWORD "MAT_KHAU_WIFI"
#define BLYNK_AUTH_TOKEN "TOKEN_BLYNK_CUA_BAN"
#define GOOGLE_SCRIPT_ID "SCRIPT_ID_GOOGLE_CUA_BAN"
```

### 3. Nạp Firmware
```bash
# Trong terminal PlatformIO
pio run -t upload

# Hoặc trong VS Code: PlatformIO: Upload
```

### 4. Theo dõi Serial
```bash
pio device monitor --baud 115200
```

## Hướng Dẫn Cấu Hình

### Thiết lập Blynk
1. **Tạo tài khoản Blynk**: https://blynk.cloud/
2. **Tạo Template**: 
   - Tên: "Warehouse Monitor"
   - Board: ESP32 Dev Board
3. **Cấu hình Virtual Pin**:
   - V0-V12: Đã ánh xạ trong mã nguồn
4. **Tạo Device** từ template
5. **Copy Auth Token** vào `config.h`

### Tích hợp Google Sheets
1. **Tạo Google Apps Script**:
   - Truy cập https://script.google.com/
   - Tạo dự án mới
   - Dán code từ `docs/google_sheets_script.gs`
   - Deploy dạng web app (truy cập công khai)
   - Copy Script ID vào `config.h`

2. **Tạo Google Sheet**:
   - Tạo sheet mới để ghi log
   - Script sẽ ghi vào các sheet "AccessLog", "AlertLog", "DeviceStatus"

### Thiết lập Cảm biến vân tay AS608
1. **Hiệu chuẩn cảm biến**:
   - Cấp nguồn module
   - Đặt ngón tay chắc chắn trong 3 giây
   - Nhấc và lặp lại
2. **Đăng ký vân tay**:
   - Gửi lệnh qua ứng dụng/serial monitor
   - Đặt ngón tay 3 lần khi được yêu cầu
3. **Xác thực**:
   - Hệ thống tự kiểm tra khi bật nguồn

## Sử Dụng

### Kiểm soát truy cập nhân viên
1. Nhân viên đặt ngón tay lên cảm biến AS608
2. Hệ thống chụp và xác thực vân tay
3. Nếu khớp:
   - ✓ Ghi lịch sử vào/ra lên Google Sheets
   - ✓ Ghi nhận thời gian
   - ✓ Còi kêu (âm thành công)
4. Nếu không khớp:
   - ✗ Còi kêu (âm thất bại)
   - ✗ Ghi log cảnh báo

### Điều khiển đèn - Nút nhấn vật lý
- **Nhấn ngắn**: Bật/tắt đèn
- **Nhấn giữ** (2 giây): Tắt khẩn cấp (tương lai)

### Điều khiển đèn - Ứng dụng di động
- Mở Blynk app
- Bật/tắt công tắc "Điều khiển đèn" (V1)
- Xem tiêu thụ điện thời gian thực

### Giám sát môi trường
- **Nhiệt độ**: Hiển thị trên biểu đồ app
- **Độ ẩm**: Hiển thị trên bảng điều khiển
- **Mức khói**: Giá trị analog 0-1023
- **Cháy**: Cảnh báo đỏ khi kích hoạt
- Tất cả số liệu cập nhật mỗi 5 giây

### Giám sát điện năng
- **Số liệu trực tiếp**:
  - Điện áp (V)
  - Dòng điện (A)
  - Công suất (W)
  - Năng lượng (kWh)
  - Tần số (Hz)
  - Hệ số công suất
- **Biểu đồ**: Tiêu thụ năng lượng theo thời gian

## Bảng Ánh Xạ Virtual Pin Blynk

| V# | Chức Năng | Loại | Mô Tả |
|----|-----------|------|--------|
| V0 | - | - | *Dự phòng* |
| V1 | Điều khiển đèn | Nút/Công tắc | Bật/tắt relay đèn |
| V2 | Test còi | Nút | Thử nghiệm còi |
| V3 | Nhiệt độ | Đồng hồ/Biểu đồ | Đọc °C |
| V4 | Độ ẩm | Đồng hồ | Đọc % RH |
| V5 | Mức khói | Đồng hồ | ADC 0-1023 |
| V6 | Công suất | Đồng hồ/Biểu đồ | Watt |
| V7 | Năng lượng | Đồng hồ | kWh tiêu thụ |
| V8 | Điện áp | Đồng hồ | Volt |
| V9 | Dòng điện | Đồng hồ | Ampe |
| V10 | Cảnh báo cháy | LED | Cảnh báo nghiêm trọng |
| V11 | Cảnh báo khói | LED | Cảnh báo |
| V12 | Trạng thái đèn | LED | Chỉ báo trạng thái đèn |

## Cấu Hình Ngưỡng Cảnh Báo

Sửa `include/config.h`:

```cpp
#define SMOKE_THRESHOLD 400      // Giá trị analog MQ2
#define TEMP_THRESHOLD 40        // °C
#define FIRE_THRESHOLD 500       // Giá trị ADC
#define HUMIDITY_WARNING 80      // % RH
```

## Xử Lý Sự Cố

### AS608 không phản hồi
- Kiểm tra tốc độ baud: 57600
- Kiểm tra chân RX/TX (GPIO16/GPIO17)
- Test với serial monitor ở 57600 baud

### PZEM không đọc được
- Kiểm tra tốc độ baud: 9600
- Kiểm tra slave ID (mặc định 0xF8)
- Bật tải (bật đèn) để xem giá trị

### Kết nối WiFi thất bại
- Kiểm tra SSID/mật khẩu trong `config.h`
- Kiểm tra anten ESP32
- Kiểm tra cường độ tín hiệu WiFi

### Blynk không đồng bộ
- Kiểm tra auth token
- Kiểm tra kết nối internet
- Khởi động lại app và thiết bị
- Dán lại token nếu cần

### Google Sheets không ghi log
- Test webhook trên trình duyệt
- Kiểm tra script deployment đang hoạt động
- Kiểm tra URL sheet trong config
- Kiểm tra độ trễ internet

## Nâng Cấp Trong Tương Lai

- [ ] Cơ sở dữ liệu lưu trữ vân tay nhân viên
- [ ] Xác thực thay thế bằng thẻ RFID
- [ ] Cảm biến phát hiện chuyển động
- [ ] Giao thức UDP/MQTT cập nhật nhanh hơn
- [ ] Bảng điều khiển web cục bộ (mDNS)
- [ ] Giám sát nhiệt độ đa vùng
- [ ] Phân tích dữ liệu lịch sử
- [ ] Tự động hóa theo lịch (hành động theo thời gian)
- [ ] Cảnh báo email cho sự kiện nghiêm trọng
- [ ] Tích hợp camera video

## Tham Chiếu API

### Hàm Chính
```cpp
// Điều khiển đèn
controlLight(bool state);

// Đọc cảm biến
readDHTSensors();
readMQ2Smoke();
readPZEMData();
checkFireAlert();

// Vân tay
fingerprint.enrollNewFingerprint(uint8_t id);
fingerprint.verifyFingerprint(uint8_t& matchedId);

// Ghi log cloud
cloudLogger.logAccess(empId, empName, action, success);
cloudLogger.logAlert(type, value, severity);

// Giám sát điện
powerMonitor.readAll(v, i, p, e);
```

## Giấy Phép

Giấy phép MIT - Xem file LICENSE để biết chi tiết

## Hỗ Trợ

Nếu có vấn đề, câu hỏi hoặc yêu cầu tính năng, vui lòng mở issue trong repository.

## Tác Giả

Xây dựng với ESP32, PlatformIO và Blynk

---

**Cập nhật lần cuối**: Tháng 3, 2026