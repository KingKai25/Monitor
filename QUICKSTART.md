# 🚀 BẮT ĐẦU TỪ ĐÂY - Hướng Dẫn Khởi Động Nhanh

Chào mừng bạn đến với Hệ Thống Giám Sát Kho Hàng! Tài liệu này sẽ giúp bạn khởi động trong 30 phút.

## ⚠️ Lỗi IntelliSense Là Bình Thường

Bạn có thể thấy gạch đỏ trong VS Code về thiếu include như:
- `#include <BlynkSimpleEsp32.h>`
- `#include <DHT.h>`

**Điều này bình thường!** IntelliSense chưa tải thư viện. PlatformIO sẽ tự động cài đặt khi bạn biên dịch.

---

## Bước 1: Cấu Hình Thông Tin Xác Thực (5 phút)

### Sửa config.h với thông tin của bạn

Mở file: `include/config.h`

Thay thế các dòng sau bằng giá trị THỰC của bạn:

```cpp
#define WIFI_SSID "TEN_WIFI_CUA_BAN"
#define WIFI_PASSWORD "MAT_KHAU_CUA_BAN"
#define BLYNK_TEMPLATE_ID "TEMPLATE_ID_CUA_BAN"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN_CUA_BAN"
#define GOOGLE_SCRIPT_ID "GOOGLE_SCRIPT_ID_CUA_BAN"
```

### Lấy các giá trị này ở đâu

**WiFi SSID/Mật khẩu:**
- Tên mạng WiFi và mật khẩu nhà/văn phòng của bạn

**Blynk Token & Template ID:**
- 📖 Xem hướng dẫn chi tiết: [docs/GET_CREDENTIALS.md](docs/GET_CREDENTIALS.md)

**Google Script ID:**
- 📖 Hướng dẫn đầy đủ trong cùng file hướng dẫn credentials ở trên

### 🔗 Liên kết nhanh để lấy Credentials

1. **Tạo tài khoản Blynk** → https://blynk.cloud/ → Đăng ký
2. **Tạo Google Apps Script** → https://script.google.com/ → Dự án mới
3. **Đọc hướng dẫn đầy đủ** → Xem [docs/GET_CREDENTIALS.md](docs/GET_CREDENTIALS.md)

---

## Bước 2: Kết Nối Phần Cứng (10 phút)

### Sơ Đồ Chân Cập Nhật (Theo sơ đồ schematic của bạn)

```
Chân ESP32   →  Thiết Bị             Mô Tả
────────────────────────────────────────────────────
GPIO32       →  DHT11               Nhiệt độ/Độ ẩm (D32)
GPIO25       →  Cảm biến MQ2        Phát hiện khói (D25)
GPIO33       →  Cảm biến lửa        Phát hiện nhiệt (D33)
GPIO26       →  Nút nhấn            Công tắc vật lý (D26)
GPIO23       →  Relay               Điều khiển đèn (D23)
GPIO4        →  Còi/Buzzer          Âm cảnh báo (D4)

Kết nối UART Serial:
GPIO18       →  PZEM TX             Module đo điện (D18 trên Serial1)
GPIO19       →  PZEM RX             Module đo điện (D19 trên Serial1)
GPIO17       →  AS608 TX            Vân tay (TX2 trên Serial2)
GPIO16       →  AS608 RX            Vân tay (RX2 trên Serial2)

Kết nối nguồn:
3.3V         →  DHT11 VCC, AS608 VCC
5V           →  PZEM VCC, MQ2 VCC, Cảm biến lửa VCC, Relay VCC
GND          →  Tất cả chân Ground
```

**📋 Sơ đồ chân đầy đủ:**
Xem sơ đồ chi tiết tại: [docs/PIN_MAPPING_VI.md](docs/PIN_MAPPING_VI.md)

**✅ Kiểm tra trước khi tiếp tục:**
- [ ] Tất cả kết nối ground (GND) đã được hàn
- [ ] 3.3V và 5V không bị lộn
- [ ] Dây UART được nối chéo (TX→RX, RX→TX)
- [ ] Relay NO nối tới đèn, COM nối tới pha

---

## Bước 3: Nạp Firmware (10 phút)

### Trong VS Code:

1. **Mở Terminal**: Nhấn `Ctrl+`` (dấu huyền)

2. **Biên dịch Firmware**:
```bash
pio run
```

Chờ hoàn thành. Bạn sẽ thấy:
```
Building...
✓ Project Compiled Successfully
```

3. **Kết nối ESP32** với máy tính qua cáp USB

4. **Nạp firmware**:
```bash
pio run -t upload
```

Bạn sẽ thấy:
```
Uploading...
Hard resetting via RTS pin...
✓ Upload Complete
```

5. **Theo dõi Serial Output**:
```bash
pio device monitor --baud 115200
```

Bạn sẽ thấy thông báo khởi động:

```
========== WAREHOUSE MONITOR STARTING ==========
[WiFi] Connecting to TEN_WIFI...
[WiFi] Connected! IP: 192.168.1.100
[DHT] DHT11 initialized
[PZEM] PZEM-004T initialized
[Fingerprint] AS608 initialized
Setup complete!
```

---

## Bước 4: Cấu Hình Blynk App (5 phút)

### Trên điện thoại:

1. **Tải Blynk App**
   - iOS: App Store
   - Android: Google Play

2. **Đăng nhập** với email đã dùng tại blynk.cloud

3. **Chọn thiết bị**
   - Sẽ hiển thị "Warehouse_ESP32_Main"
   - Trạng thái phải là "Online" (dấu chấm xanh)

4. **Tạo bảng điều khiển**
   - Nhấn nút "+"
   - Thêm widget:
     - Nút cho V1 (Điều khiển đèn)
     - Đồng hồ cho V3 (Nhiệt độ)
     - Đồng hồ cho V4 (Độ ẩm)
     - Biểu đồ cho V6 (Công suất)
     - LED cho V10 (Cảnh báo cháy)

📱 Xem [docs/BLYNK_SETUP.md](docs/BLYNK_SETUP.md) để cấu hình widget chi tiết

---

## Bước 5: Kiểm Tra Toàn Bộ (5 phút)

### Trình tự kiểm tra nhanh:

**1. Kiểm tra đọc nhiệt độ**
```
Serial Monitor → "Temperature: 23.5°C | Humidity: 55%"
Blynk App      → Đồng hồ hiển thị ~23.5°C
```

**2. Kiểm tra nút nhấn**
```
Nhấn nút       → Relay click
Đèn            → BẬT (xác nhận trực tiếp)
Blynk App      → Công tắc V1 hiển thị BẬT
```

**3. Kiểm tra cảm biến khói**
```
Vẫy tay qua cảm biến MQ2
Serial hiển thị → "Smoke Level: 600"
Phải kích hoạt  → Cảnh báo nếu > 400
```

**4. Kiểm tra Google Sheets**
```
Nhấn nút hoặc kích hoạt cảnh báo
Mở Google Sheet "Warehouse_Monitor"
Tab AccessLog phải có dòng mới
```

---

## 🎯 Những Gì Bạn Vừa Xây Dựng

```
┌─────────────────────────────────────────┐
│      HỆ THỐNG GIÁM SÁT KHO HÀNG       │
├─────────────────────────────────────────┤
│                                         │
│  📱 Blynk App (Bảng điều khiển)        │
│      ├─ Điều khiển bật/tắt đèn        │
│      ├─ Biểu đồ nhiệt độ              │
│      ├─ Biểu đồ tiêu thụ điện         │
│      └─ Cảnh báo cháy/khói            │
│                                         │
│  🌩️ Kết nối WiFi Cloud                 │
│      ├─ Đồng bộ thời gian thực        │
│      ├─ Thông báo đẩy                  │
│      └─ Truy cập toàn cầu             │
│                                         │
│  📊 Ghi log Google Sheets              │
│      ├─ Nhật ký truy cập (nhân viên)   │
│      ├─ Lịch sử cảnh báo              │
│      └─ Trạng thái thiết bị           │
│                                         │
│  🔐 Kiểm soát truy cập vân tay        │
│      ├─ Cảm biến AS608 (GPIO16/17)     │
│      ├─ Dung lượng 81 nhân viên        │
│      └─ Ghi nhận thời gian tự động     │
│                                         │
│  ⚡ Giám sát điện năng                  │
│      ├─ PZEM-004T (GPIO18/19)          │
│      ├─ Theo dõi kWh thời gian thực    │
│      └─ Tính chi phí điện              │
│                                         │
│  🌡️ Cảm biến môi trường                │
│      ├─ DHT11: Nhiệt độ/Độ ẩm         │
│      ├─ MQ2: Phát hiện khói            │
│      └─ Cảm biến lửa: Cảnh báo nhiệt  │
│                                         │
└─────────────────────────────────────────┘
```

---

## 📚 Bước Tiếp Theo & Tài Liệu

### Ưu tiên ngay
1. [ ] Hoàn thành nối dây phần cứng
2. [ ] Nạp firmware thành công
3. [ ] Xác nhận serial monitor hiển thị tốt
4. [ ] Test bật/tắt đèn
5. [ ] Xác nhận Blynk hiển thị nhiệt độ

### Sau đó cấu hình
1. [ ] Đăng ký vân tay nhân viên đầu tiên
2. [ ] Xác nhận Google Sheets ghi log
3. [ ] Hiệu chuẩn ngưỡng cảm biến
4. [ ] Thiết lập cơ sở dữ liệu nhân viên

### Tài liệu tham khảo
- Thiết lập đầy đủ: [docs/SETUP_GUIDE.md](docs/SETUP_GUIDE.md)
- Ứng dụng Blynk: [docs/BLYNK_SETUP.md](docs/BLYNK_SETUP.md)
- Vân tay: [docs/AS608_GUIDE.md](docs/AS608_GUIDE.md)
- Giám sát điện: [docs/PZEM_GUIDE.md](docs/PZEM_GUIDE.md)
- Script Cloud: [docs/google_sheets_script.gs](docs/google_sheets_script.gs)

---

## ❌ Xử Lý Sự Cố Nhanh

### "Không nạp được firmware"
**Giải pháp**: 
1. Thử cổng USB khác
2. Cài driver CH340: https://sparks.gogo.co.nz/ch340.html
3. Kiểm tra Device Manager tìm cổng COM ESP32

### "WiFi không kết nối được"
**Giải pháp**:
1. Kiểm tra SSID/mật khẩu trong config.h (phân biệt hoa thường!)
2. Xác nhận WiFi là 2.4GHz (một số mạng chỉ 5GHz)
3. Kiểm tra router cho phép thiết bị này

### "Nhiệt độ hiển thị 0 hoặc sai"
**Giải pháp**:
1. Kiểm tra nối dây DHT11 (GPIO32)
2. Xác nhận có điện trở pull-up 10kΩ trên chân data
3. Đảm bảo cảm biến có nguồn 3.3V

### "Thiết bị Blynk hiển thị Offline"
**Giải pháp**:
1. Kiểm tra auth token trong config.h khớp với Blynk app
2. Kiểm tra kết nối WiFi trong serial monitor
3. Khởi động lại Blynk app

### "Google Sheets không ghi log"
**Giải pháp**:
1. Kiểm tra script đã Deploy (không chỉ Save)
2. Kiểm tra webhook URL có đúng Script ID
3. Thử chạy hàm test trong Google Script editor

---

## 🔐 Lưu Ý Bảo Mật

Trước khi triển khai:
1. ⚠️ Đổi slave ID mặc định PZEM (hiện tại 0xF8)
2. ⚠️ Sử dụng mật khẩu WiFi mạnh
3. ⚠️ Không chia sẻ auth token
4. ⚠️ Sao lưu Google Sheets thường xuyên
5. ⚠️ Giám sát log truy cập phát hiện sử dụng trái phép

---

## 💡 Mẹo Hay

**Mẹo 1: Theo dõi Serial Output**
```bash
# Xem số liệu cảm biến trực tiếp khi test
pio device monitor --baud 115200
```

**Mẹo 2: Test cảm biến từng cái**
- Chỉ đăng ký một vân tay trước
- Dùng tải đã biết để test công suất
- Đưa tay gần cảm biến để thử nghiệm

**Mẹo 3: Điều chỉnh ngưỡng**
Sửa trong `include/config.h`:
```cpp
#define SMOKE_THRESHOLD 400    // Thấp hơn = nhạy hơn
#define TEMP_THRESHOLD 40      // Điều chỉnh theo môi trường
```

**Mẹo 4: Dùng Blynk Web Console**
Bận không có điện thoại? Giám sát tại:
https://blynk.cloud/ (cùng thiết bị, giao diện khác)

---

## ✅ Checklist Thành Công

Bạn đã thành công khi:
- ✅ Firmware biên dịch không lỗi
- ✅ Serial monitor hiển thị thông báo khởi tạo
- ✅ Thiết bị Blynk hiển thị "Online"
- ✅ Nhấn nút bật/tắt đèn thành công
- ✅ Nhiệt độ hiển thị trong Blynk app
- ✅ Nhấn nút ghi log vào Google Sheets
- ✅ Đăng ký vân tay hoạt động

---

## 🎉 Chúc Mừng!

Bạn đã xây dựng thành công hệ thống giám sát kho hàng IoT chuyên nghiệp với:

✓ **Kiểm soát truy cập** - Theo dõi nhân viên bằng vân tay  
✓ **Giám sát Cloud** - Bảng điều khiển thời gian thực  
✓ **Phân tích điện năng** - Chi tiết tiêu thụ năng lượng  
✓ **Cảnh báo môi trường** - Cháy, khói, nhiệt độ  
✓ **Ghi log dữ liệu** - Tự động ghi vào Google Sheets  
✓ **Điều khiển di động** - Tích hợp Blynk app  

---

## 📞 Cần Thêm Trợ Giúp?

**Vấn đề** | **Xem**
-----------|--------
Lỗi thiết lập | [SETUP_GUIDE.md](docs/SETUP_GUIDE.md)
Blynk không hoạt động | [BLYNK_SETUP.md](docs/BLYNK_SETUP.md)
Lỗi vân tay | [AS608_GUIDE.md](docs/AS608_GUIDE.md)
Lỗi đo điện | [PZEM_GUIDE.md](docs/PZEM_GUIDE.md)
Tổng quan | [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md)

---

**Sẵn sàng bắt đầu?** 

👉 Bắt đầu với config.h và làm theo Bước 1 ở trên.

Chúc xây dựng thành công! 🚀

---

**Cập nhật lần cuối**: Tháng 3, 2026  
**Trạng thái**: Sẵn sàng triển khai ✅