# Hệ Thống Giám Sát Kho Hàng - Mục Lục Tài Liệu

Tài liệu đầy đủ cho hệ thống giám sát và kiểm soát truy cập kho hàng dựa trên ESP32.

## Liên Kết Nhanh

- **🚀 Bắt đầu**: [SETUP_GUIDE.md](SETUP_GUIDE.md) - Thiết lập phần cứng & phần mềm đầy đủ
- **📱 Ứng dụng di động**: [BLYNK_SETUP.md](BLYNK_SETUP.md) - Cấu hình ứng dụng Blynk
- **🔐 Vân tay**: [AS608_GUIDE.md](AS608_GUIDE.md) - Kiểm soát truy cập nhân viên
- **⚡ Giám sát điện**: [PZEM_GUIDE.md](PZEM_GUIDE.md) - Theo dõi tiêu thụ điện năng
- **☁️ Tích hợp Cloud**: [google_sheets_script.gs](google_sheets_script.gs) - Ghi log Google Sheets

## Cấu Trúc Tài Liệu

### 1. Thiết Lập & Cài Đặt
- **Đối tượng**: Người dùng lần đầu, tích hợp phần cứng
- **Nội dung**: [SETUP_GUIDE.md](SETUP_GUIDE.md)
  - Giai đoạn 1: Nối dây phần cứng (chân, kết nối)
  - Giai đoạn 2: Cài đặt phần mềm (PlatformIO, thư viện)
  - Giai đoạn 3: Cấu hình credentials (WiFi, Blynk, Google)
  - Giai đoạn 4: Biên dịch & nạp firmware
  - Giai đoạn 5: Kiểm tra hệ thống
  - Giai đoạn 6: Đăng ký vân tay
  - Giai đoạn 7: Triển khai production

### 2. Lấy Thông Tin Xác Thực
- **Đối tượng**: Người cần Blynk token & Google Script ID
- **Nội dung**: [GET_CREDENTIALS.md](GET_CREDENTIALS.md)
  - Hướng dẫn từng bước tạo tài khoản & lấy token Blynk
  - Deploy Google Apps Script
  - Cấu hình Google Sheets
  - Kiểm tra kết nối
  - **Có sẵn bằng tiếng Việt**

### 3. Sơ Đồ Chân & Nối Dây
- **Đối tượng**: Tham chiếu tích hợp phần cứng
- **Nội dung**: [PIN_MAPPING_VI.md](PIN_MAPPING_VI.md)
  - Sơ đồ chân GPIO cập nhật theo schematic
  - Sơ đồ kết nối trực quan
  - Checklist tất cả kết nối
  - Cấu hình cổng UART Serial
  - Xử lý sự cố nối dây
  - Tổng quan giao thức AS608
  - Cấu hình chân và UART
  - Quy trình đăng ký (từng bước)
  - Xác thực & nhận dạng
  - Tích hợp cơ sở dữ liệu nhân viên
  - Xử lý lỗi & sự cố
  - Kiểm tra chẩn đoán

### 4. Giám Sát Điện Năng
- **Đối tượng**: Lập trình viên, kỹ sư bảo trì
- **Nội dung**: [PZEM_GUIDE.md](PZEM_GUIDE.md)
  - Tổng quan PZEM-004T v3
  - Chi tiết giao thức Modbus RTU
  - Bảng register và trích xuất dữ liệu
  - Tính toán CRC
  - Ví dụ thực tế
  - Cấu hình biến dòng CT
  - Hiệu chuẩn & kiểm tra

### 5. Tích Hợp Cloud
- **Đối tượng**: Quản trị viên hệ thống, lập trình viên
- **Nội dung**: [google_sheets_script.gs](google_sheets_script.gs)
  - Các hàm Google Apps Script
  - Xử lý webhook
  - Ghi dữ liệu lên Google Sheets
  - Thông báo email
  - Quản lý log truy cập
  - Ghi log cảnh báo

### 6. README Dự Án Chính
- **Đối tượng**: Tổng quan và tham chiếu nhanh
- **Nội dung**: [../README.md](../README.md)
  - Tính năng dự án
  - Linh kiện phần cứng
  - Sơ đồ chân
  - Tổng quan cài đặt
  - Tham chiếu virtual pin Blynk
  - Tham chiếu API

## Tài Liệu Theo Tính Năng

### Kiểm Soát Truy Cập
```
Quản Lý Nhân Viên:
├─ Đăng ký vân tay (AS608)
├─ Theo dõi chấm công (Google Sheets)
├─ Ghi log vào/ra với timestamp
├─ Hỗ trợ nhiều người dùng (tối đa 81+ nhân viên)
└─ Điều khiển nút vật lý + ứng dụng di động
```

Xem: [AS608_GUIDE.md](AS608_GUIDE.md) - Giao thức & triển khai đầy đủ

### Giám Sát Môi Trường
```
Giám Sát Cảm Biến:
├─ Nhiệt độ/Độ ẩm (DHT11)
├─ Phát hiện khói (MQ2)
├─ Cảnh báo cháy
├─ Ngưỡng có thể cấu hình
└─ Thông báo đẩy
```

Xem: [SETUP_GUIDE.md](SETUP_GUIDE.md) - Giai đoạn 5 Kiểm Tra

### Tiêu Thụ Điện Năng
```
Theo Dõi Năng Lượng:
├─ Đo công suất thời gian thực (PZEM-004T)
├─ Giám sát điện áp, dòng điện, tần số
├─ Tiêu thụ năng lượng kWh
├─ Tính hệ số công suất
└─ Biểu đồ lịch sử
```

Xem: [PZEM_GUIDE.md](PZEM_GUIDE.md) - Giao thức Modbus đầy đủ

### Ứng Dụng Di Động
```
Bảng Điều Khiển Blynk:
├─ Biểu đồ cảm biến thời gian thực
├─ Điều khiển bật/tắt đèn
├─ Thông báo cảnh báo
├─ Biểu đồ dữ liệu lịch sử
└─ Giám sát từ xa
```

Xem: [BLYNK_SETUP.md](BLYNK_SETUP.md) - Cấu hình ứng dụng

## Tổ Chức File

```
Monitor/
├── docs/
│   ├── README.md                    ← Bạn đang ở đây
│   ├── SETUP_GUIDE.md               ← Thiết lập phần cứng & phần mềm
│   ├── BLYNK_SETUP.md               ← Cấu hình ứng dụng di động
│   ├── AS608_GUIDE.md               ← Chi tiết cảm biến vân tay
│   ├── PZEM_GUIDE.md                ← Chi tiết giám sát điện
│   ├── GET_CREDENTIALS.md           ← Hướng dẫn lấy credentials
│   ├── PIN_MAPPING_VI.md            ← Sơ đồ chân GPIO
│   └── google_sheets_script.gs      ← Script ghi log cloud
│
├── src/
│   ├── main.cpp                     ← Firmware chính
│   ├── fingerprint_sensor.cpp       ← Triển khai AS608
│   ├── power_monitor.cpp            ← Triển khai PZEM-004T
│   └── cloud_logger.cpp             ← Ghi log Google Sheets
│
├── include/
│   ├── config.h                     ← Định nghĩa chân & cấu hình
│   ├── fingerprint_sensor.h         ← Header AS608
│   ├── power_monitor.h              ← Header PZEM-004T
│   └── cloud_logger.h               ← Header Google Sheets
│
├── platformio.ini                   ← Cấu hình biên dịch
├── README.md                        ← Tổng quan dự án
└── docs/ (thư mục này)             ← Tài liệu
```

## Các Tác Vụ Thường Gặp

### Tác vụ: Lấy Blynk Token và Google Script ID
Xem: [GET_CREDENTIALS.md](GET_CREDENTIALS.md) - Hướng dẫn chi tiết từng bước

### Tác vụ: Kiểm tra sơ đồ chân
Xem: [PIN_MAPPING_VI.md](PIN_MAPPING_VI.md) - Sơ đồ chân cập nhật theo schematic

### Tác vụ: Đăng ký nhân viên mới
Xem: [AS608_GUIDE.md](AS608_GUIDE.md) → Quy Trình Đăng Ký

### Tác vụ: Cấu hình Blynk app để giám sát
Xem: [BLYNK_SETUP.md](BLYNK_SETUP.md) → Thiết Lập Bảng Điều Khiển

### Tác vụ: Kiểm tra tiêu thụ điện của đèn
Xem: [PZEM_GUIDE.md](PZEM_GUIDE.md) → Ví Dụ Thực Tế

### Tác vụ: Thiết lập ghi log Google Sheets
Xem: [SETUP_GUIDE.md](SETUP_GUIDE.md) → Giai đoạn 3.3 & google_sheets_script.gs

### Tác vụ: Điều chỉnh ngưỡng cảm biến
Xem: [SETUP_GUIDE.md](SETUP_GUIDE.md) → Giai đoạn 3.4

### Tác vụ: Xử lý sự cố cảm biến vân tay
Xem: [AS608_GUIDE.md](AS608_GUIDE.md) → Xử Lý Sự Cố

### Tác vụ: Hiểu số liệu đo điện
Xem: [PZEM_GUIDE.md](PZEM_GUIDE.md) → Giao Thức & Ví Dụ

## Công Nghệ Sử Dụng

```
Phần cứng:
├─ Board ESP32 Dev (Bộ điều khiển chính)
├─ AS608 (Cảm biến vân tay)
├─ PZEM-004T v3 (Giám sát điện)
├─ DHT11 (Nhiệt độ/Độ ẩm)
├─ MQ2 (Phát hiện khói)
└─ Relay + Còi (Đầu ra)

Phần mềm:
├─ PlatformIO (Hệ thống biên dịch)
├─ Arduino Framework (Thư viện lập trình)
├─ Blynk IoT (Nền tảng cloud)
├─ Google Apps Script (Ghi log)
└─ C++ (Ngôn ngữ firmware)
```

---

**Cập nhật lần cuối**: Tháng 3, 2026