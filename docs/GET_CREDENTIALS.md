# Hướng Dẫn Lấy Blynk Token và Google Script ID

Hướng dẫn chi tiết để lấy các thông tin cần thiết cho dự án của bạn.

---

## 🔵 Phần 1: Lấy Blynk Auth Token

### Bước 1: Tạo Tài Khoản Blynk

1. Truy cập https://blynk.cloud/
2. Nhấp **"Sign Up"** (Đăng ký)
3. Điền thông tin:
   - Email: Email của bạn
   - Password: Mật khẩu mạnh
   - Confirm Password: Nhập lại mật khẩu
4. Nhấp **"Sign Up"**
5. Xác minh email (kiểm tra hộp thư)

### Bước 2: Tạo Template (Mẫu)

1. Login vào https://blynk.cloud/ bằng email & password
2. Trên menu bên trái, nhấp **"Developer Zone"**
3. Nhấp **"Templates"**
4. Nhấp nút **"+ Create Template"** (màu xanh)
5. Điền thông tin:
   ```
   Template Name: Warehouse Monitor
   Description: Hệ thống giám sát kho hàng với esp32
   Hardware: ESP32 Dev Board
   Connectivity: WiFi
   ```
6. Nhấp **"Create"**

### Bước 3: Lấy Template ID

1. Sau khi create template, bạn sẽ vào trang template editor
2. Trên phía trên, tìm **"TEMPLATE_ID"**
3. **Copy Template ID** (dài khoảng 24 ký tự)
4. **Lưu thông tin này** - sẽ dùng sau

```
Ví dụ Template ID:
BLYNK1234567890ABCDEF
```

### Bước 4: Tạo Device (Thiết Bị)

1. Quay lại trang chủ blynk.cloud
2. Nhấp **"Devices"** trên menu bên trái
3. Nhấp **"+ Create Device"** (màu xanh)
4. Chọn:
   ```
   Template: Warehouse Monitor (vừa tạo)
   Device Name: Warehouse_ESP32_Main
   ```
5. Nhấp **"Create Device"**
6. **Đợi khoảng 5 giây** để device được tạo

### Bước 5: Lấy Auth Token

Sau khi device được tạo, bạn sẽ thấy một card hiển thị device:

1. Nhấp vào device card **"Warehouse_ESP32_Main"**
2. Nhấp nút **"Device info"** (i dấu hỏi)
3. Một cửa sổ hiện lên, tìm **"Auth Token"**
4. **Copy toàn bộ Auth Token** (dài khoảng 32 ký tự)
5. **Lưu Auth Token này** vào file `include/config.h`

```
Ví dụ Auth Token:
xQ_2hV4mN6pL8kJ0vF3sM9wX1yZ5aB7c
```

### ✅ Cách dán vào config.h:

Mở file: `include/config.h`

Tìm dòng này:
```cpp
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
```

Sửa thành:
```cpp
#define BLYNK_TEMPLATE_ID "BLYNK1234567890ABCDEF"           // ← Thay bằng Template ID của bạn
#define BLYNK_AUTH_TOKEN "xQ_2hV4mN6pL8kJ0vF3sM9wX1yZ5aB7c" // ← Thay bằng Auth Token của bạn
```

---

## 📊 Phần 2: Lấy Google Script ID

### Bước 1: Tạo Google Apps Script

1. Truy cập https://script.google.com/
2. Nhấp **"New project"** (Dự án mới)
3. Đặt tên project: `Warehouse_Monitor`
4. Nhấp **"Create"**

### Bước 2: Dán Code Google Apps Script

1. Xóa code mẫu có sẵn (nếu có)
2. Mở file từ dự án của bạn: `docs/google_sheets_script.gs`
3. **Copy toàn bộ nội dung** của file này
4. Quay lại Google Apps Script editor
5. **Paste** code vào editor

### Bước 3: Lưu Script

1. Nhấp **"Save"** (Ctrl+S)
2. Gõ tên: `Warehouse Logger`
3. Nhấp **"OK"**

### Bước 4: Deploy Script (Triển khai)

1. Nhấp nút **"Deploy"** (trên cùng)
2. Chọn **"New deployment"**
3. Nhấp biểu tượng **"⚙️"** (bánh xe)
4. Chọn **"Web app"**
5. Điền thông tin:
   ```
   Description: Warehouse Monitoring Webhook
   Execute as: Chọn email của bạn
   Who has access: Anyone
   ```
6. Nhấp **"Deploy"**

### Bước 5: Cho phép Quyền Truy Cập

1. Một cửa sổ yêu cầu quyền sẽ hiện lên
2. Nhấp **"Review permissions"** (Xem lại quyền)
3. Chọn tài khoản Google của bạn
4. Nhấp **"Advanced"** (Nâng cao)
5. Nhấp **"Go to Warehouse Logger (unsafe)"**
6. Nhấp **"Allow"** (Cho phép)

### Bước 6: Lấy Script ID

Sau khi Deploy thành công, một cửa sổ popup sẽ hiện lên:

1. Tìm dòng **"Deployment ID"**
2. **Copy Deployment ID** (dài khoảng 40 ký tự)

Hoặc nếu bạn đóng popup:
1. Nhấp **"Deployments"** (trên cùng)
2. Tìm deployment vừa tạo
3. **Copy ID** từ danh sách

```
Ví dụ Script ID (Deployment ID):
AKfycbxW1y2z3A4B5C6D7E8F9G0H1I2J3K
```

### ✅ Cách dán vào config.h:

Mở file: `include/config.h`

Tìm dòng này:
```cpp
#define GOOGLE_SCRIPT_ID "YOUR_GOOGLE_SCRIPT_ID"
```

Sửa thành:
```cpp
#define GOOGLE_SCRIPT_ID "AKfycbxW1y2z3A4B5C6D7E8F9G0H1I2J3K" // ← Thay bằng Script ID của bạn
```

---

## 🌐 Phần 3: Lấy Google Sheets Webhook URL

### Tạo Google Sheet để Lưu Dữ Liệu

1. Truy cập https://sheets.google.com/
2. Nhấp **"+ Untitled spreadsheet"** (Bảng tính mới)
3. Đặt tên: `Warehouse_Monitor`
4. **Tạo 3 sheet** bằng cách nhấp nút "+" dưới cùng:
   - **Sheet 1**: Đổi tên thành `AccessLog` (Nhật ký truy cập)
   - **Sheet 2**: Đổi tên thành `AlertLog` (Nhật ký cảnh báo)
   - **Sheet 3**: Đổi tên thành `DeviceStatus` (Trạng thái thiết bị)

### Thêm Tiêu Đề Cột

**AccessLog sheet:**
```
A1: Timestamp
B1: Employee ID
C1: Employee Name
D1: Action
E1: Status
F1: Device
```

**AlertLog sheet:**
```
A1: Timestamp
B1: Alert Type
C1: Value
D1: Severity
```

**DeviceStatus sheet:**
```
A1: Timestamp
B1: Status
C1: Details
D1: Signal Strength
```

### Lấy Webhook URL

URL webhook được tạo tự động từ Google Apps Script:

```
https://script.google.com/macros/d/{SCRIPT_ID}/usercallback
```

Thay `{SCRIPT_ID}` bằng Script ID bạn vừa copy.

**Ví dụ:**
```
https://script.google.com/macros/d/AKfycbxW1y2z3A4B5C6D7E8F9G0H1I2J3K/usercallback
```

---

## 🧪 Bước 4: Kiểm Tra Kết Nối

### Test Blynk Connection

1. Upload firmware lên ESP32
2. Mở **Blynk app** trên điện thoại
3. Chọn device **"Warehouse_ESP32_Main"**
4. **Status** phải hiển thị **"Online"** (dấu xanh)

Nếu **Offline**:
- Kiểm tra WiFi trong config.h
- Kiểm tra Auth Token
- Restart ESP32

### Test Google Sheets Logging

1. Nhấn button trên ESP32
2. Chờ 2-3 giây
3. Mở Google Sheet `Warehouse_Monitor`
4. Kiểm tra `AccessLog` sheet
5. Phải có **dòng mới** với timestamp và thông tin nút

Nếu không có dòng mới:
- Kiểm tra Script ID trong config.h
- Kiểm tra Google Apps Script đã Deploy chưa
- Kiểm tra quyền "Anyone" cho web app

---

## 📋 Checklist Hoàn Thành

- [ ] Tạo Blynk account
- [ ] Tạo Blynk template "Warehouse Monitor"
- [ ] Copy BLYNK_TEMPLATE_ID vào config.h
- [ ] Tạo Blynk device
- [ ] Copy BLYNK_AUTH_TOKEN vào config.h
- [ ] Tạo Google Apps Script
- [ ] Dán code từ google_sheets_script.gs
- [ ] Deploy script với quyền "Anyone"
- [ ] Copy GOOGLE_SCRIPT_ID vào config.h
- [ ] Tạo Google Sheet "Warehouse_Monitor"
- [ ] Tạo 3 sheets: AccessLog, AlertLog, DeviceStatus
- [ ] Thêm tiêu đề cột
- [ ] Test Blynk app (phải hiển thị "Online")
- [ ] Test Google Sheets logging

---

## ⚠️ Lưu Ý Quan Trọng

1. **Auth Token**: Không chia sẻ token này cho ai khác
2. **Script Permissions**: Phải là "Anyone" để webhook hoạt động
3. **Google Sheet**: Phải tạo sẵn sheets trong file
4. **WiFi**: SSID và password phải chính xác (case-sensitive)
5. **Gmail**: Sử dụng Gmail để tạo Google Apps Script & Sheets

---

## 🆘 Troubleshooting

### Blynk: Device hiển thị "Offline"
**Giải pháp:**
1. Kiểm tra WiFi SSID/password trong config.h
2. Kiểm tra Auth Token khớp với device
3. Kiểm tra URL server Blynk (https://blynk.cloud/)
4. Restart ESP32

### Google Sheets: Không ghi log dữ liệu
**Giải pháp:**
1. Kiểm tra Script ID trong config.h
2. Kiểm tra script đã Deploy chưa (không chỉ Save)
3. Kiểm tra quyền = "Anyone"
4. Kiểm tra Google Sheet có 3 sheets chưa

### Không thể Deploy Google Script
**Giải pháp:**
1. Kiểm tra có dùng Gmail Google không
2. Kiểm tra quyền tài khoản Google
3. Thử dùng browser khác (Chrome, Firefox)

### Config.h: Không biết dán ở đâu
**Giải pháp:**
```cpp
// Tìm 3 dòng này:
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"      ← Dán TEMPLATE_ID ở đây
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"        ← Dán AUTH_TOKEN ở đây
#define GOOGLE_SCRIPT_ID "YOUR_GOOGLE_SCRIPT_ID"  ← Dán SCRIPT_ID ở đây
```

---

## 📞 Hỗ Trợ

**Câu hỏi thường gặp?** Xem:
- Blynk: https://docs.blynk.io/
- Google Script: https://script.google.com/
- Google Sheets: https://sheets.google.com/

**Cần upload lại firmware?**
```bash
pio run -t upload
```

**Cần xem dữ liệu real-time?**
- Mở Blynk app → Device "Warehouse_ESP32_Main"
- Hoặc vào https://blynk.cloud/ → web console

---

**Hướng dẫn hoàn thành:**  
Sau khi điền đủ cả 3 giá trị vào config.h, hãy upload firmware!  

```bash
pio run -t upload
```

✅ **Xong! Hệ thống của bạn sẵn sàng chạy.**
