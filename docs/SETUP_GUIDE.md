# Hướng Dẫn Thiết Lập & Cấu Hình

Hướng dẫn từng bước để chạy hệ thống giám sát kho hàng.

## Giai Đoạn 1: Thiết Lập Phần Cứng

### 1.1 Cấu Hình Nối Dây

Theo sơ đồ schematic của bạn:

```
Chân ESP32   | Thiết Bị          | Loại Tín Hiệu | Ghi Chú
-------------|-------------------|----------------|------------------
GPIO32       | DHT11             | 1-Wire         | Nhiệt độ/Độ ẩm
GPIO25       | MQ2               | Analog ADC     | Phát hiện khói
GPIO33       | Cảm biến lửa      | Analog ADC     | Phát hiện nhiệt
GPIO26       | Nút nhấn          | Digital Input  | Có chống rung
GPIO4        | Còi/Buzzer        | Digital Output | Tích cực mức cao
GPIO23       | Relay             | Digital Output | Điều khiển đèn
GPIO16       | AS608 RX          | UART2 (Serial2)| 57600 baud
GPIO17       | AS608 TX          | UART2 (Serial2)| 57600 baud
GPIO18       | PZEM TX           | UART1 (Serial1)| 9600 baud
GPIO19       | PZEM RX           | UART1 (Serial1)| 9600 baud
GND          | Tất cả chung      | -              | Ground chung
5V           | Relay, Còi        | Nguồn          | Từ PSU
3.3V         | Cảm biến, AS608   | Nguồn          | Đầu ra ESP32
```

### 1.2 Kết Nối Từng Linh Kiện

**Module Vân Tay AS608** (Sử dụng Serial2)
```
Chân AS608   | Chân ESP32 | Ghi Chú
-------------|-----------|------------------
VCC (3.3V)   | 3.3V      | Nguồn cấp
GND          | GND       | Ground chung
RX           | GPIO17    | Serial truyền (TX2)
TX           | GPIO16    | Serial nhận (RX2)
```

**Module Đo Điện PZEM-004T** (Sử dụng Serial1)
```
Chân PZEM    | Chân ESP32 | Ghi Chú
-------------|-----------|------------------
VCC (5V)     | 5V        | Nguồn cấp
GND          | GND       | Ground chung
RX           | GPIO18    | Serial truyền (D18)
TX           | GPIO19    | Serial nhận (D19)
A/B (CT)     | Đèn       | Biến dòng
L/N          | Nguồn AC  | Qua relay
```

**DHT11** (GPIO32)
```
Chân 1 (VCC)  → 3.3V
Chân 2 (DAT)  → GPIO32 + Điện trở pull-up 10kΩ lên 3.3V
Chân 3 (NULL) → Không sử dụng
Chân 4 (GND)  → GND
```

**Cảm Biến Khói MQ2** (GPIO25 ADC)
```
VCC → 5V
GND → GND
A0  → GPIO25 (Đầu vào ADC)
D0  → (Tùy chọn, không sử dụng)
```

**Cảm Biến Lửa** (GPIO33 ADC)
```
VCC → 5V
GND → GND
AO  → GPIO33 (Đầu vào ADC)
DO  → (Tùy chọn, không sử dụng)
```

**Nút Nhấn** (GPIO26)
```
Nút chân 1 → 3.3V (qua điện trở pull-up 10kΩ)
Nút chân 2 → GPIO26
Nút chân 3 → GND
Tụ 100nF giữa GPIO26 và GND (tùy chọn, lọc nhiễu)
```

**Module Relay** (GPIO23)
```
VCC → 5V
GND → GND
IN  → GPIO23 (Tích cực HIGH)
COM → Pha AC chính
NO  → Pha đèn
NC  → Không sử dụng
```

**Còi/Buzzer** (GPIO4)
```
+ (Chân dài) → GPIO4
- (Chân ngắn) → GND
Tùy chọn: Thêm diode 1N4007 qua còi để bảo vệ
```

### 1.3 Nguồn Cấp

**Yêu cầu:**
- ESP32: 3.3V @ 500mA tối đa
- Relay: 5V @ 100mA
- PZEM: 5V @ 100mA
- Tải đèn: Qua relay (nguồn AC)

**Nguồn khuyến nghị:**
- Nguồn chính: Adapter AC 5V/2A

## Giai Đoạn 2: Thiết Lập Phần Mềm

### 2.1 Cài Đặt PlatformIO

1. **Cài VS Code**: https://code.visualstudio.com/
2. **Cài Extension PlatformIO**:
   - Mở VS Code
   - Extensions → Tìm "PlatformIO"
   - Cài "PlatformIO IDE" của Ivan Kravets
3. **Khởi động lại VS Code**

### 2.2 Clone/Mở Dự Án

```bash
# Cách 1: Clone từ git
git clone <repository-url>
cd Monitor

# Cách 2: Mở thư mục có sẵn
# File → Open Folder → Chọn thư mục Monitor
```

### 2.3 Kiểm Tra File Cấu Hình

Kiểm tra các file này tồn tại:
- `platformio.ini` - Cấu hình biên dịch
- `include/config.h` - Định nghĩa chân
- `src/main.cpp` - Firmware chính
- `README.md` - Tài liệu

## Giai Đoạn 3: Cấu Hình Credentials

### 3.1 Sửa config.h

```cpp
// File: include/config.h

#define WIFI_SSID "TenWiFiCuaBan"
#define WIFI_PASSWORD "MatKhauCuaBan"

#define BLYNK_TEMPLATE_ID "TEMPLATE_ID_CUA_BAN"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN_CUA_BAN"

#define GOOGLE_SCRIPT_ID "SCRIPT_ID_CUA_BAN"
```

**Tìm credentials ở đâu:**

1. **WiFi SSID/Mật khẩu**: WiFi nhà/văn phòng của bạn
2. **Blynk**: Xem phần 3.2
3. **Google Script**: Xem phần 3.3

### 3.2 Thiết Lập Blynk

1. **Tạo tài khoản**:
   - Truy cập https://blynk.cloud/
   - Đăng ký bằng email

2. **Tạo Template**:
   - Developer Zone → Templates
   - Nhấn "Create New Template"
   - **Tên**: Warehouse Monitor
   - **Chọn phần cứng**: ESP32 Dev Board
   - **Chọn kết nối**: WiFi
   - **Lưu**

3. **Lấy Auth Token**:
   - Trong template, tìm "Auth Token"
   - Copy toàn bộ token
   - Dán vào `config.h` → `BLYNK_AUTH_TOKEN`

4. **Lấy Template ID**:
   - Cài đặt template → Copy "TEMPLATE_ID"
   - Dán vào `config.h` → `BLYNK_TEMPLATE_ID`

5. **Tạo Device**:
   - Devices → Create New Device
   - Chọn template "Warehouse Monitor"
   - Tên: "Warehouse_ESP32"
   - **Lưu**

6. **Cấu hình Virtual Pin** (trong Blynk app):
   - V1: Nút (Bật/Tắt đèn)
   - V2: Nút (Test còi)
   - V3-V9: Đồng hồ/biểu đồ cho cảm biến
   - V10-V12: Chỉ báo LED cho cảnh báo

### 3.3 Thiết Lập Google Sheets

1. **Tạo Google Apps Script**:
   - Truy cập https://script.google.com/
   - Tạo dự án mới: "Warehouse Logger"

2. **Dán Code Script**:
   ```javascript
   // Script từ docs/google_sheets_script.gs
   // Copy toàn bộ nội dung
   ```

3. **Deploy Script**:
   - Deploy → New deployment
   - Loại: "Web app"
   - Thực thi dưới: Tài khoản của bạn
   - Ai có quyền truy cập: "Anyone"
   - **Deploy**
   - Copy Script ID

4. **Lưu vào config.h**:
   ```cpp
   #define GOOGLE_SCRIPT_ID "SCRIPT_ID_DA_COPY"
   ```

5. **Tạo Google Sheet**:
   - Tạo sheet mới trong Drive
   - Tên: "Warehouse_Monitor"
   - Tạo các sheet:
     - "AccessLog" (cột: ID, Tên, Hành động, Thời gian)
     - "AlertLog" (cột: Loại, Giá trị, Mức nghiêm trọng, Thời gian)
     - "DeviceStatus" (cột: Trạng thái, Chi tiết, Thời gian)

### 3.4 Kiểm Tra Cấu Hình

Sửa ngưỡng cảm biến trong `include/config.h` (tùy chọn):

```cpp
// Điều chỉnh theo môi trường của bạn
#define SMOKE_THRESHOLD 400      // Thấp hơn = nhạy hơn
#define TEMP_THRESHOLD 40        // Độ Celsius
#define FIRE_THRESHOLD 500       // Điều chỉnh theo test cảm biến
#define HUMIDITY_WARNING 80      // Phần trăm
```

## Giai Đoạn 4: Biên Dịch & Nạp Firmware

### 4.1 Biên Dịch Firmware

```bash
# Trong terminal PlatformIO (Ctrl+`)
pio run

# Hoặc trong VS Code:
# Nhấn biểu tượng PlatformIO → Project Tasks → Build
```

**Kết quả mong đợi:**
```
Building...
Linking...
Checking size...
Program size: ~300KB
RAM: ~50KB
Memory use: 35%
```

### 4.2 Kết Nối ESP32

1. Kết nối ESP32 với máy tính qua cáp USB
2. Kiểm tra cổng COM:
   - Windows: Device Manager → Ports (COM3, COM4, v.v.)
   - Linux: `ls /dev/ttyUSB*`
   - Mac: `ls /dev/tty.usbserial*`

3. Cập nhật `platformio.ini` (nếu cần):
   ```ini
   upload_port = COM3  ; Đổi sang cổng của bạn
   ```

### 4.3 Nạp Firmware

```bash
# PlatformIO: Upload
pio run -t upload

# Theo dõi output:
# Sẽ thấy "Uploading..." rồi "Done"
```

**Xử lý lỗi nạp:**
- Thử cổng USB 2.0 thay vì USB 3.0
- Giảm tốc độ nạp trong `platformio.ini`:
  ```ini
  upload_speed = 115200
  ```
- Cài driver CH340 (chip USB ESP32: https://sparks.gogo.co.nz/ch340.html)

### 4.4 Theo Dõi Serial Output

```bash
# Xem serial output
pio device monitor --baud 115200

# Tìm các thông tin:
# - Trạng thái kết nối WiFi
# - Khởi tạo cảm biến
# - Giá trị nhiệt độ/độ ẩm
# - Trạng thái vân tay
# - Số liệu đo điện
```

Nhấn Ctrl+C để thoát.

## Giai Đoạn 5: Kiểm Tra Hệ Thống

### 5.1 Kiểm Tra Phần Cứng

**Cảm biến DHT11**:
```
Serial output hiển thị:
"Temperature: 25.5°C | Humidity: 60%"
```

**Cảm biến MQ2/Lửa**:
```
Serial hiển thị giá trị analog
- Không khí sạch: ~100-200
- Khói: >400
```

**Vân tay AS608**:
```
Serial hiển thị:
"Fingerprint module ready"
Đặt ngón tay để đăng ký
```

**Đo điện PZEM**:
```
Serial hiển thị:
"Voltage: 230V | Current: 0.5A | Power: 115W"
```

**Test nút nhấn**:
```
Nhấn nút vật lý → Đèn bật/tắt
Nhấn 5 lần
Kiểm tra Blynk app cập nhật
```

**Relay/Đèn**:
```
GPIO23 điều khiển relay
Relay click khi bật/tắt
Đèn bật/tắt
```

### 5.2 Kiểm Tra Blynk App

1. Mở Blynk app
2. Chọn device "Warehouse_ESP32"
3. Kiểm tra widget:
   - Đồng hồ nhiệt độ hiển thị giá trị
   - Nút bật/tắt đèn
   - Slider cập nhật
   - Log sự kiện hiển thị hoạt động

### 5.3 Kiểm Tra Google Sheets

1. Mở Google Sheet "Warehouse_Monitor"
2. Test bằng cách nhấn nút hoặc mô phỏng cảnh báo
3. Kiểm tra sheet cập nhật trong vòng 5 giây
4. Xác nhận timestamp và hành động được ghi nhận

## Giai Đoạn 6: Đăng Ký Vân Tay

### 6.1 Đăng Ký Vân Tay Đầu Tiên

1. **Gửi lệnh đăng ký** qua serial:
   ```
   Lệnh Serial: ENROLL:001
   (Hoặc triển khai qua tổ hợp nút nhấn)
   ```

2. **Làm theo hướng dẫn**:
   - Đặt ngón tay lên cảm biến
   - Chờ tiếng bíp
   - Nhấc ngón tay
   - Lặp lại 3 lần
   - Xác nhận đăng ký

3. **Xác minh đăng ký** bằng lệnh VERIFY:
   - Đặt cùng ngón tay
   - Phải thấy "Match ID: 001"

### 6.2 Đăng Ký Thêm Người Dùng

- Lặp lại quy trình cho ID 002, 003, v.v.
- Tối đa 81 vân tay (có thể cấu hình)
- Lưu thông tin nhân viên vào Google Sheet

## Giai Đoạn 7: Triển Khai Production

### 7.1 Cấu Hình Cuối Cùng

1. **Mức điện áp**:
   - Xác nhận 3.3V cho cảm biến
   - Xác nhận 5V cho relay/còi
   - Kiểm tra kết nối ground

2. **Quản lý nhiệt**:
   - Thêm tản nhiệt nếu cần
   - Đảm bảo thông gió
   - Giám sát nhiệt độ

3. **Nguồn dự phòng**:
   - Cân nhắc UPS cho ghi log quan trọng
   - Sao lưu EEPROM cho dữ liệu quan trọng

### 7.2 Giám Sát & Bảo Trì

```
Kiểm tra hàng ngày:
- Serial monitor xem lỗi
- Blynk app phản hồi
- Google Sheets ghi log

Hàng tuần:
- Kiểm tra số liệu cảm biến
- Xác nhận mức tiêu thụ điện cơ bản
- Test hệ thống cảnh báo

Hàng tháng:
- Lau cảm biến vân tay
- Xác nhận tất cả dữ liệu ghi log chính xác
- Cập nhật credentials nếu thay đổi
```

## Tham Chiếu Xử Lý Sự Cố Nhanh

| Vấn Đề | Giải Pháp |
|---------|-----------|
| **WiFi không kết nối** | Kiểm tra SSID/mật khẩu trong config.h |
| **Cảm biến không đọc được** | Kiểm tra định nghĩa chân trong config.h |
| **Blynk không đồng bộ** | Kiểm tra internet, khởi động lại app |
| **Vân tay thất bại** | Lau ống kính cảm biến, đăng ký lại |
| **PZEM không có dữ liệu** | Kiểm tra baud rate (9600), xác nhận RX/TX |
| **Relay click nhưng đèn tắt** | Kiểm tra nối dây relay với mạch AC |
| **Serial output lỗi ký tự** | Xác nhận baud rate 115200 |
| **Lỗi bộ nhớ đầy** | Giảm FINGERPRINT_MAX xuống 5 |

## Bước Tiếp Theo

1. ✅ Hoàn thành nối dây phần cứng
2. ✅ Cài PlatformIO IDE
3. ✅ Cấu hình credentials
4. ✅ Biên dịch và nạp firmware
5. ✅ Test tất cả cảm biến
6. ✅ Đăng ký vân tay
7. ✅ Giám sát hệ thống production
8. 📝 Tùy chỉnh ngưỡng cho môi trường của bạn
9. 📝 Thêm cảm biến/tính năng bổ sung khi cần

---

**Vẫn gặp khó khăn?** Kiểm tra:
- Serial monitor output để xem thông báo lỗi
- Build flags trong `platformio.ini`
- Trạng thái online thiết bị Blynk (dấu chấm xanh)
- Trạng thái deployment Google Script