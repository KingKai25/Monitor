# Hướng Dẫn Triển Khai Cảm Biến Vân Tay AS608

Hướng dẫn đầy đủ cho xác thực vân tay AS608 trong hệ thống giám sát kho hàng.

## Tổng Quan

**Module Vân Tay Quang Học AS608**
- Tốc độ Baud: 57600 bps
- Giao tiếp: UART Serial
- Chế độ: Xác thực 1:1 và Nhận dạng 1:N
- Dung lượng lưu trữ: 163 vân tay (cấu hình theo thư viện)
- Kích thước mẫu: 256 byte mỗi vân tay

## Cấu Hình Chân

```
Chân AS608   | Tín Hiệu   | Chân ESP32
-------------|-------------|----------
1 (VCC)      | Nguồn 3.3V | 3.3V
2 (GND)      | Ground     | GND
3 (RX)       | Serial RX  | GPIO17 (TX2)
4 (TX)       | Serial TX  | GPIO16 (RX2)
(Vỏ)         | Ground     | GND
```

**Cấu hình UART2 (Serial2)** (trong firmware)
```cpp
#define FINGERPRINT_RX 16
#define FINGERPRINT_TX 17
SerialFinger.begin(57600, SERIAL_8N1, FINGERPRINT_RX, FINGERPRINT_TX);
```

## Cấu Trúc Giao Thức

### 1. Định Dạng Khung Lệnh

```
Header (2 byte) → 0xEF 0x01
Địa chỉ (4 byte) → Địa chỉ thiết bị (mặc định: 0xFFFFFFFF)
ID Gói (1 byte) → Loại Lệnh/Phản hồi
Độ dài dữ liệu (2 byte) → Độ dài dữ liệu + checksum
Dữ liệu (Biến đổi) → Tham số lệnh
Checksum (2 byte) → Xác minh
```

### 2. Loại Gói

```
0x01 = Gói lệnh
0x02 = Gói dữ liệu
0x07 = Gói phản hồi (Ack)
0x08 = Gói lỗi
```

### 3. Mã Lệnh

| Mã | Chức Năng | Tham Số |
|----|-----------|---------|
| 0x01 | Thông tin hệ thống | Không |
| 0x20 | Chụp ảnh | Không |
| 0x21 | Ảnh ra Buffer | ID Buffer (1-2) |
| 0x22 | Tải ảnh xuống | ID Buffer |
| 0x23 | Upload ảnh | ID Buffer |
| 0x30 | So khớp 1:1 | ID Template |
| 0x32 | So khớp 1:N | ID bắt đầu, Số lượng |
| 0x40 | Đăng ký Template | ID Buffer (1,2) |
| 0x41 | Lưu Template | ID Model (0-162) |
| 0x42 | Xóa Template | ID Model |
| 0x43 | Xóa tất cả | Không |
| 0x44 | Đếm Template | Không |
| 0x45 | Đọc Template | ID Model |
| 0x46 | Nạp Template | ID Model, ID Buffer |

## Quy Trình Đăng Ký

### Đăng Ký Từng Bước

```
Nhập người dùng: "ENROLL 001"
│
├─ Kiểm tra ID 001 còn trống không
│
├─ Hướng dẫn: "Đặt ngón tay"
│  └─ Chụp ảnh 1
│     └─ Lưu vào buffer 1
│
├─ Hướng dẫn: "Nhấc ngón tay"
│  └─ Chờ 1-2 giây
│
├─ Hướng dẫn: "Đặt lại ngón tay"
│  └─ Chụp ảnh 2
│     └─ Lưu vào buffer 2
│
├─ So sánh đặc điểm
│  └─ Nếu điểm khớp > ngưỡng
│     ├─ Tạo template
│     ├─ Lưu vào model ID 001
│     └─ Thành công!
│     
└─ Kết quả: "Vân tay đã đăng ký - ID 001"
```

### Code Triển Khai

```cpp
bool enrollFingerprints(uint8_t id) {
  Serial.print("Dang ky ID: ");
  Serial.println(id);
  
  // Buoc 1: Kiem tra ID ton tai
  uint16_t templateCount = getTemplateCount();
  if (id < 1 || id > templateCount + 10) {
    Serial.println("ID khong hop le");
    return false;
  }
  
  // Buoc 2: Chup anh lan 1
  Serial.println(">> Dat ngon tay len cam bien");
  if (!captureImage()) {
    return false;
  }
  
  if (!convertImage(0x01)) {  // Luu vao buffer 1
    return false;
  }
  
  // Buoc 3: Cho va chup lan 2
  delay(2000);
  Serial.println(">> Dat lai ngon tay");
  
  if (!captureImage()) {
    return false;
  }
  
  if (!convertImage(0x02)) {  // Luu vao buffer 2
    return false;
  }
  
  // Buoc 4: Tao va luu template
  if (!registerTemplate()) {
    return false;
  }
  
  if (!storeTemplate(id)) {
    return false;
  }
  
  Serial.println("Van tay dang ky thanh cong!");
  return true;
}
```

## Quy Trình Xác Thực

### Xác thực 1:1 (ID cụ thể)

```
Người dùng đặt ngón tay
│
├─ Chụp ảnh
├─ Chuyển đổi thành đặc điểm
└─ So sánh với template ID đã lưu
    ├─ Điểm > Ngưỡng?
    │  └─ CÓ: Khớp!
    └─ KHÔNG: Không khớp
```

### Nhận dạng 1:N (Tìm kiếm tất cả)

```
Người dùng đặt ngón tay
│
├─ Chụp ảnh
├─ Chuyển đổi thành đặc điểm
└─ So sánh với TẤT CẢ template đã lưu (0-162)
    ├─ Tìm thấy trong buffer 0-162?
    │  └─ CÓ: Khớp! Trả về ID
    └─ KHÔNG: Không có trong cơ sở dữ liệu
```

### Code Triển Khai

```cpp
bool verifyFingerprint(uint8_t targetId, uint16_t& score) {
  Serial.println("Dat ngon tay len cam bien");
  
  // Chup anh moi
  if (!captureImage()) {
    Serial.println("Chup that bai");
    return false;
  }
  
  // Chuyen doi thanh dac diem (buffer 1)
  if (!convertImage(0x01)) {
    return false;
  }
  
  // Nap template da luu (targetId)
  if (!loadTemplate(targetId, 0x02)) {
    return false;
  }
  
  // So sanh dac diem buffer 1 vs buffer 2
  if (compareTemplates(score)) {
    Serial.print("Khop! Diem: ");
    Serial.println(score);
    return true;
  }
  
  return false;
}

bool searchFingerprint(uint16_t& foundId, uint16_t& score) {
  Serial.println("Dat ngon tay len cam bien");
  
  // Chup anh moi
  if (!captureImage()) {
    return false;
  }
  
  // Chuyen doi thanh dac diem
  if (!convertImage(0x01)) {
    return false;
  }
  
  // Tim kiem trong tat ca template da luu
  if (searchTemplate(0, 162, foundId, score)) {
    Serial.print("Tim thay: ID ");
    Serial.print(foundId);
    Serial.print(" Diem: ");
    Serial.println(score);
    return true;
  }
  
  return false;
}
```

## Xử Lý Lỗi

### Các Lỗi Thường Gặp

| Mã | Ý Nghĩa | Giải Pháp |
|----|----------|-----------|
| 0x00 | OK | Không lỗi |
| 0x01 | Lỗi nhận dữ liệu | Kiểm tra kết nối serial |
| 0x02 | Không phát hiện ngón tay | Yêu cầu đặt ngón tay |
| 0x03 | Xử lý ảnh thất bại | Chất lượng kém, thử lại |
| 0x04 | Đặc điểm không rõ ràng | Cảm biến bẩn hoặc vị trí sai |
| 0x05 | Template không khớp | ID không có trong database |
| 0x06 | Không tìm thấy | Không khớp khi tìm 1:N |
| 0x07 | Tham số không hợp lệ | ID hoặc chỉ mục sai |
| 0x08 | Xung đột template | Hai ngón tay quá giống |
| 0x09 | Tràn địa chỉ | ID > dung lượng tối đa |
| 0x0A | Template trống | Không có dữ liệu tại ID đó |
| 0x0B | Lỗi đọc/ghi Flash | Vấn đề phần cứng |

### Code Xử Lý Lỗi

```cpp
const char* getErrorMsg(uint8_t errorCode) {
  const char* errors[] = {
    "OK",
    "Loi nhan du lieu",
    "Khong co ngon tay tren cam bien",
    "Xu ly anh that bai",
    "Anh qua mo",
    "Van tay khong khop",
    "Khong tim thay",
    "Tham so khong hop le",
    "Xung dot template",
    "Tran dia chi",
    "Template trong",
    "Loi doc/ghi Flash"
  };
  
  if (errorCode < sizeof(errors)) {
    return errors[errorCode];
  }
  return "Loi khong xac dinh";
}

bool captureImageSafe(uint8_t maxAttempts = 3) {
  for (uint8_t attempt = 0; attempt < maxAttempts; attempt++) {
    Serial.println("Dang quet van tay...");
    
    if (captureImage()) {
      return true;
    }
    
    Serial.println("Khong phat hien ngon tay, thu lai...");
    delay(500);
  }
  
  return false;
}
```

## Quản Lý Cơ Sở Dữ Liệu

### Lưu Trữ Template

```
Bố cục bộ nhớ:
├─ Template 0-81: Pool người dùng (MAX_PROFILES)
├─ Template 82-162: Pool dự phòng/Quản trị
└─ Vùng hệ thống: Cài đặt thiết bị
```

### Thao Tác Lưu/Xóa

```cpp
// Luu template
bool storeTemplate(uint8_t modelId) {
  return sendCommand(0x41, modelId);
}

// Xoa template cu the
bool deleteTemplate(uint8_t modelId) {
  return sendCommand(0x42, modelId);
}

// Xoa tat ca template
bool deleteAllTemplates() {
  return sendCommand(0x43, 0);
}

// Dem so template
uint16_t getTemplateCount() {
  return sendCommandWithResult(0x44);
}
```

## Tích Hợp Với Hệ Thống Nhân Viên

### Cấu Trúc Cơ Sở Dữ Liệu Nhân Viên

```cpp
struct EmployeeRecord {
  uint8_t fingerprintId;      // ID template AS608 (1-81)
  String employeeName;        // "Nguyen Van A"
  String employeeId;          // "NV001"
  String department;          // "Kho hang"
  time_t enrollmentDate;      // Thoi gian dang ky
  bool active;                // Hoat dong/Khong hoat dong
};

// Luu trong EEPROM hoac cloud
EmployeeRecord employees[MAX_FINGERPRINTS] = {
  {1, "Nguyen Van A", "NV001", "Kho hang", 1698765432, true},
  {2, "Tran Thi B", "NV002", "Quan ly", 1698765432, true},
  // ...
};
```

### Xác Thực Kèm Ghi Log

```cpp
bool verifyAndLog(String& employeeName) {
  uint8_t foundId = 0;
  uint16_t score = 0;
  
  // Xac thuc van tay
  if (!searchFingerprint(foundId, score)) {
    Serial.println("Khong khop");
    return false;
  }
  
  // Tim nhan vien
  if (foundId < 1 || foundId > MAX_FINGERPRINTS) {
    return false;
  }
  
  if (!employees[foundId].active) {
    Serial.println("Nhan vien khong hoat dong");
    return false;
  }
  
  employeeName = employees[foundId].employeeName;
  String action = getLastAction() == "OUT" ? "IN" : "OUT";
  
  // Ghi log len Google Sheets
  cloudLogger.logAccess(
    employees[foundId].employeeId,
    employees[foundId].employeeName,
    action,
    true
  );
  
  // Phat am thanh cong
  soundBuzzer(200);
  
  return true;
}
```

## Kiểm Tra & Hiệu Chuẩn

### 1. Test Cảm Biến
```bash
1. Bat nguon AS608
2. Dat cac ngon tay khac nhau
3. Gia tri phai thay doi (khong co dinh 0 hoac 255)
4. Lau ong kinh cam bien neu gia tri kem
```

### 2. Kiểm Tra Chất Lượng
```
Ảnh tốt: Điểm 65+
Ảnh trung bình: Điểm 50-64
Ảnh kém: Điểm <50

Hành động: Nếu kém, lau cảm biến và thử lại
```

### 3. Ngưỡng Khớp
```cpp
const uint8_t MATCH_THRESHOLD = 60;
// Dieu chinh dua tren ty le false positive/negative
// 60 = can bang
// 70+ = nghiem ngat hon (nhieu false neg hon)
// 50- = thoang hon (nhieu false pos hon)
```

### 4. Lệnh Chẩn Đoán

```cpp
void diagnosticTest() {
  Serial.println("=== Chan Doan AS608 ===");
  
  // Test giao tiep
  if (isConnected()) {
    Serial.println("Cam bien phan hoi");
  } else {
    Serial.println("Khong co phan hoi");
    return;
  }
  
  // Lay thong tin thiet bi
  uint16_t templateCount = getTemplateCount();
  Serial.print("Template da luu: ");
  Serial.println(templateCount);
  
  // Test chup anh
  Serial.println("Dat ngon tay de test chup...");
  if (captureImage()) {
    Serial.println("Chup anh thanh cong");
  } else {
    Serial.println("Chup anh that bai");
  }
}
```

## Xử Lý Sự Cố

| Vấn Đề | Nguyên Nhân | Giải Pháp |
|---------|-------------|-----------|
| **Module không phản hồi** | Kết nối serial | Kiểm tra chân RX/TX (GPIO16/17), baud rate |
| **Luôn báo "Không có ngón tay"** | Cửa sổ quang bẩn | Lau bằng vải mềm |
| **Chất lượng ảnh kém** | Ngón tay khô/ánh sáng kém | Dùng ngón tay khác, làm ẩm ngón tay |
| **Đăng ký thất bại** | Ảnh quá khác nhau | Đặt ngón tay giống nhau 2 lần |
| **False negative** | Ngưỡng quá cao | Giảm MATCH_THRESHOLD |
| **False positive** | Ngưỡng quá thấp | Tăng MATCH_THRESHOLD |
| **Hết bộ nhớ** | Quá nhiều template | Xóa các mục không dùng |

## Chỉ Số Hiệu Suất

```
Thời gian chụp: 200-300ms
Xử lý ảnh: 100-200ms
So khớp 1:1: 150-200ms
Tìm kiếm 1:N (162 template): 1.5-2 giây
Thời gian đăng ký: ~2 giây
```

## Lưu Ý Bảo Mật

1. **Đăng ký an toàn**: Xác minh thẻ admin trước khi đăng ký
2. **Sao lưu template**: Xuất template ra bộ nhớ ngoài
3. **Phát hiện trùng lặp**: Kiểm tra vân tay đã đăng ký chưa
4. **Nhật ký truy cập**: Giữ log kiểm toán chi tiết
5. **Người dùng không hoạt động**: Vô hiệu hóa nhân viên cũ nhanh chóng

## Tham Khảo

- Tài liệu giao thức AS608
- Thư viện Arduino AS608 DFRobot
- Giao tiếp UART Serial (115200 baud cho ESP32)
- Thuật toán nhận dạng vân tay

## Trạng Thái Triển Khai

- ✅ File header: `include/fingerprint_sensor.h`
- ✅ File source: `src/fingerprint_sensor.cpp`
- ⏳ Triển khai giao thức đầy đủ (cần test)
- 📋 Tích hợp với main.cpp (sẵn sàng)
- 📋 Cơ sở dữ liệu nhân viên (cần triển khai)

## Bước Tiếp Theo

1. Biên dịch và nạp firmware
2. Test giao tiếp serial cơ bản
3. Đăng ký vân tay đầu tiên
4. Test xác thực/nhận dạng
5. Tích hợp với cơ sở dữ liệu nhân viên
6. Thêm ghi log Google Sheets
7. Test với nhiều người dùng

---

**Cập nhật lần cuối**: Tháng 3, 2026
**Phiên bản Firmware**: 1.0.1 (Tích hợp AS608)