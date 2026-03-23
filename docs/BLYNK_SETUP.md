# Hướng Dẫn Cấu Hình Ứng Dụng Blynk

Hướng dẫn đầy đủ thiết lập bảng điều khiển Blynk trên di động cho giám sát kho hàng.

## 1. Thiết Lập Tài Khoản Blynk

### 1.1 Tạo Tài Khoản
1. Tải Blynk app (iOS/Android)
   - iOS: App Store
   - Android: Google Play Store
2. Hoặc truy cập https://blynk.cloud trên trình duyệt
3. Đăng ký bằng email
4. Xác minh email

### 1.2 Tạo Tổ Chức (Tùy chọn)
1. Đăng nhập https://blynk.cloud
2. Workspaces → Tạo tổ chức mới
3. Tên: "Giám Sát Kho Hàng"
4. Bỏ qua thiết lập thanh toán (có gói miễn phí)

## 2. Cấu Hình Template

### 2.1 Tạo Template Trên Web Console
1. Truy cập https://blynk.cloud
2. Developer Zone → Templates
3. Nhấn "+ Create Template"

**Cài đặt Template:**
```
Template Name: Warehouse Monitor
Description: Hệ thống giám sát kho hàng dựa trên ESP32
Hardware: ESP32 Dev Board
Connectivity: WiFi
```

4. Lưu template
5. **Copy và lưu:**
   - TEMPLATE_ID (dán vào config.h)
   - TEMPLATE_NAME

### 2.2 Cấu Hình Datastream
Datastream định nghĩa cách dữ liệu truyền giữa thiết bị và app.

1. Trong template editor, nhấn "Datastreams"
2. Tạo các datastream sau:

| Virtual Pin | Tên | Loại | Min | Max | Đơn vị |
|-------------|-----|------|-----|-----|--------|
| V0 | - | - | - | - | - |
| V1 | Điều khiển đèn | Switch | 0 | 1 | - |
| V2 | Test còi | Button | 0 | 1 | - |
| V3 | Nhiệt độ | Sensor | 10 | 50 | °C |
| V4 | Độ ẩm | Sensor | 0 | 100 | % |
| V5 | Mức khói | Sensor | 0 | 1023 | ADC |
| V6 | Công suất | Sensor | 0 | 5000 | W |
| V7 | Năng lượng | Sensor | 0 | 9999 | kWh |
| V8 | Điện áp | Sensor | 0 | 300 | V |
| V9 | Dòng điện | Sensor | 0 | 100 | A |
| V10 | Cảnh báo cháy | Indicator | 0 | 1 | - |
| V11 | Cảnh báo khói | Indicator | 0 | 1 | - |
| V12 | Trạng thái đèn | Indicator | 0 | 1 | - |

3. Lưu từng datastream

## 3. Tạo Thiết Bị

### 3.1 Thêm Thiết Bị
1. Trong Blynk.Cloud web console
2. Devices → "Create Device"
3. **Cài đặt thiết bị:**
   ```
   Template: Warehouse Monitor
   Device Name: Warehouse_ESP32_Main
   ```
4. Nhấn "Create Device"

### 3.2 Lấy Authentication Token
1. Card thiết bị xuất hiện trong danh sách
2. Nhấn vào thiết bị
3. Nhấn "Device info"
4. Copy **Auth Token** (chuỗi dài)
5. Dán vào `config.h`:
   ```cpp
   #define BLYNK_AUTH_TOKEN "token_cua_ban"
   ```

## 4. Thiết Lập Bảng Điều Khiển Di Động

### 4.1 Đăng Nhập App Di Động
1. Mở Blynk app
2. Đăng nhập bằng email/mật khẩu
3. Workspaces → Chọn workspace
4. Devices → Chọn "Warehouse_ESP32_Main"

### 4.2 Tạo Bảng Điều Khiển

#### Các loại Widget có sẵn:
- **Button**: Điều khiển bật/tắt số
- **Slider**: Giá trị analog 0-255
- **Switch**: Công tắc bật/tắt
- **Gauge**: Hiển thị giá trị analog với kim
- **Value Display**: Hiển thị giá trị số/chữ
- **Chart**: Biểu đồ theo thời gian
- **LED**: Chỉ báo trực quan (bật/tắt)
- **Input**: Trường nhập văn bản
- **Timer**: Lên lịch hành động
- **Map**: Vị trí GPS (tùy chọn)

### 4.3 Thêm Widget Điều Khiển Đèn

**Widget 1: Công tắc đèn**
1. Nhấn "+" để thêm widget
2. Chọn "Switch"
3. **Cài đặt:**
   - Tên: "Điều Khiển Đèn"
   - Virtual Pin: V1
   - Giá trị BẬT: 1
   - Giá trị TẮT: 0
4. Lưu

**Widget 2: Chỉ báo trạng thái đèn**
1. Nhấn "+" để thêm widget
2. Chọn "LED"
3. **Cài đặt:**
   - Tên: "Trạng Thái Đèn"
   - Virtual Pin: V12
   - Màu: Vàng
4. Lưu

**Widget 3: Nút test còi**
1. Nhấn "+" để thêm widget
2. Chọn "Button"
3. **Cài đặt:**
   - Tên: "Test Còi"
   - Virtual Pin: V2
   - Chỉ gửi BẬT: Đánh dấu
4. Lưu

### 4.4 Thêm Widget Giám Sát Nhiệt Độ

**Widget 4: Đồng hồ nhiệt độ**
1. Nhấn "+" để thêm widget
2. Chọn "Gauge"
3. **Cài đặt:**
   - Tên: "Nhiệt Độ"
   - Virtual Pin: V3
   - Min: 10
   - Max: 50
   - Đơn vị: °C
   - Màu: Xanh dương
4. Lưu

**Widget 5: Biểu đồ nhiệt độ**
1. Nhấn "+" để thêm widget
2. Chọn "Chart"
3. **Cài đặt:**
   - Tên: "Lịch Sử Nhiệt Độ"
   - Virtual Pin: V3
   - Đơn vị: °C
   - Khoảng thời gian: 24 giờ (hoặc tùy chỉnh)
4. Lưu

### 4.5 Thêm Widget Độ Ẩm

**Widget 6: Đồng hồ độ ẩm**
1. Nhấn "+" để thêm widget
2. Chọn "Gauge"
3. **Cài đặt:**
   - Tên: "Độ Ẩm"
   - Virtual Pin: V4
   - Min: 0
   - Max: 100
   - Đơn vị: %
   - Màu: Cyan
4. Lưu

### 4.6 Thêm Widget Phát Hiện Khói

**Widget 7: Đồng hồ mức khói**
1. Nhấn "+" để thêm widget
2. Chọn "Gauge"
3. **Cài đặt:**
   - Tên: "Mức Khói"
   - Virtual Pin: V5
   - Min: 0
   - Max: 1023
   - Màu: Cam
4. Lưu

**Widget 8: Chỉ báo cảnh báo khói**
1. Nhấn "+" để thêm widget
2. Chọn "LED"
3. **Cài đặt:**
   - Tên: "Cảnh Báo Khói"
   - Virtual Pin: V11
   - Màu: Cam (cảnh báo) / Xanh (an toàn)
   - Chữ BẬT: "CẢNH BÁO"
   - Chữ TẮT: "AN TOÀN"
4. Lưu

### 4.7 Thêm Widget Giám Sát Điện

**Widget 9: Đồng hồ công suất**
1. Nhấn "+" để thêm widget
2. Chọn "Gauge"
3. **Cài đặt:**
   - Tên: "Công Suất (W)"
   - Virtual Pin: V6
   - Min: 0
   - Max: 5000
   - Đơn vị: W
   - Màu: Đỏ
4. Lưu

**Widget 10: Biểu đồ công suất**
1. Nhấn "+" để thêm widget
2. Chọn "Chart"
3. **Cài đặt:**
   - Tên: "Lịch Sử Công Suất"
   - Virtual Pin: V6
   - Đơn vị: W
   - Khoảng thời gian: 24 giờ
4. Lưu

**Widget 11: Hiển thị năng lượng**
1. Nhấn "+" để thêm widget
2. Chọn "Value Display"
3. **Cài đặt:**
   - Tên: "Năng Lượng Tiêu Thụ"
   - Virtual Pin: V7
   - Đơn vị: kWh
4. Lưu

**Widget 12: Hiển thị điện áp**
1. Nhấn "+" để thêm widget
2. Chọn "Gauge"
3. **Cài đặt:**
   - Tên: "Điện Áp"
   - Virtual Pin: V8
   - Min: 0
   - Max: 300
   - Đơn vị: V
   - Màu: Xanh lá
4. Lưu

**Widget 13: Hiển thị dòng điện**
1. Nhấn "+" để thêm widget
2. Chọn "Gauge"
3. **Cài đặt:**
   - Tên: "Dòng Điện"
   - Virtual Pin: V9
   - Min: 0
   - Max: 100
   - Đơn vị: A
   - Màu: Tím
4. Lưu

### 4.8 Thêm Widget Cảnh Báo Cháy

**Widget 14: LED cảnh báo cháy**
1. Nhấn "+" để thêm widget
2. Chọn "LED" (chọn kích thước lớn)
3. **Cài đặt:**
   - Tên: "CẢNH BÁO CHÁY"
   - Virtual Pin: V10
   - Màu: Đỏ (cảnh báo) / Xanh (an toàn)
   - Chữ BẬT: "🔥 PHÁT HIỆN CHÁY!"
   - Chữ TẮT: "✓ An Toàn"
   - Cỡ chữ: LỚN
4. Lưu

## 5. Tính Năng Nâng Cao

### 5.1 Sự Kiện & Thông Báo
1. Vào cài đặt thiết bị
2. Bật "Device notifications"
3. Thông báo đẩy sẽ cảnh báo:
   - Nhiệt độ > 40°C
   - Khói > 400 ADC
   - Phát hiện cháy
   - Bật/tắt đèn

### 5.2 Ghi Log Sự Kiện
1. Device → View Events
2. Ghi nhận tất cả thay đổi trạng thái với timestamp:
   - Bật/tắt đèn
   - Kích hoạt cảnh báo
   - Trạng thái kết nối

### 5.3 Timeline Thiết Bị
1. Device → Tab Timeline
2. Hiển thị lịch sử tất cả sự kiện
3. Có thể xuất file

### 5.4 Tự Động Hóa (Premium)
Tạo phản hồi tự động:
- Nhiệt độ > 40°C → Gửi thông báo + bật quạt
- Phát hiện khói → Ghi log + bật còi
- Đèn bật > 1 giờ → Gửi nhắc nhở

## 6. Mẹo Bố Trí Bảng Điều Khiển

### 6.1 Thực Hành Tốt Nhất
1. **Phần trên**: Cảnh báo nghiêm trọng (Cháy, Khói)
2. **Phần giữa**: Điều khiển (Công tắc đèn, nút)
3. **Phần dưới**: Đồng hồ giám sát
4. **Phần đáy**: Biểu đồ lịch sử

### 6.2 Thiết Kế Responsive
- Widget tự sắp xếp cho điện thoại/máy tính bảng
- Kéo để sắp xếp lại widget
- Thay đổi kích thước widget bằng kéo góc
- Đặt widget lớn cho thông tin quan trọng

### 6.3 Mã Màu
- 🔴 Đỏ: Cảnh báo nghiêm trọng (Cháy)
- 🟠 Cam: Cảnh báo (Khói, Nhiệt độ cao)
- 🟡 Vàng: Chú ý (Công suất cao)
- 🟢 Xanh lá: An toàn/Bình thường
- 🔵 Xanh dương: Thông tin (Nhiệt độ)
- 🟣 Tím: Số liệu điện (Dòng điện)

## 7. Giao Diện App Di Động

### 7.1 Điều Hướng Chính
```
Menu dưới:
├─ Devices (Danh sách thiết bị)
├─ Events (Log hoạt động)
├─ Automations (Hành động theo lịch)
├─ Explorer (Duyệt dữ liệu)
└─ Settings (Cài đặt tài khoản/app)
```

### 7.2 Màn Hình Thiết Bị
```
Header:
├─ Tên thiết bị: Warehouse_ESP32_Main
├─ Trạng thái: Online/Offline
└─ Cường độ tín hiệu: WiFi RSSI

Widget:
├─ Widget điều khiển (Công tắc, nút)
├─ Widget chỉ báo (LED, giá trị)
├─ Widget giám sát (Đồng hồ, biểu đồ)
└─ Widget cảnh báo (Cháy, Khói)
```

## 8. Kiểm Tra Thiết Lập

### 8.1 Test Kết Nối
1. Nạp firmware lên ESP32
2. Theo dõi serial output xem kết nối WiFi
3. Kiểm tra Blynk app - thiết bị hiển thị "Online" (dấu chấm xanh)

### 8.2 Test Widget
1. **Điều khiển đèn**: Bật/tắt công tắc → Relay click
2. **Test còi**: Nhấn nút → Còi kêu
3. **Nhiệt độ**: Kiểm tra đồng hồ cập nhật từ cảm biến
4. **Cảnh báo khói**: Test bằng cách mô phỏng đầu vào khói
5. **Cảnh báo cháy**: Test kích hoạt cảm biến lửa

### 8.3 Test Thông Báo
1. Bật thông báo đẩy trong Blynk
2. Kích hoạt điều kiện cảnh báo
3. Điện thoại nhận thông báo trong 2-3 giây

## 9. Xử Lý Sự Cố

| Vấn Đề | Giải Pháp |
|---------|-----------|
| **Thiết bị hiển thị "Offline"** | Kiểm tra kết nối WiFi, khởi động lại app |
| **Widget không cập nhật** | Kiểm tra virtual pin datastream khớp code |
| **Không nhận thông báo** | Bật thông báo trong cài đặt thiết bị |
| **Trạng thái công tắc không đồng bộ** | Kiểm tra Blynk.virtualWrite trong firmware |
| **Biểu đồ không hiển thị dữ liệu** | Có thể mất 5+ phút để có dữ liệu |

## 10. Tùy Chỉnh Nâng Cao

### 10.1 Màu/Giao diện tùy chỉnh
1. Vào thiết bị → Cài đặt giao diện
2. Chọn chế độ tối/sáng
3. Tùy chỉnh màu widget

### 10.2 Nhiều Người Dùng
1. Device → Sharing
2. Mời người dùng khác bằng email
3. Đặt cấp quyền:
   - Editor (toàn quyền)
   - Viewer (chỉ xem)
   - Operator (chỉ điều khiển)

### 10.3 Nhiều Thiết Bị
1. Tạo thêm thiết bị từ template
2. Gán auth token khác cho mỗi thiết bị
3. Chuyển đổi giữa các thiết bị trong app

## 11. Tính Năng Blynk Web Console

Truy cập https://blynk.cloud để:
- Quản lý thiết bị
- Chỉnh sửa template
- Xuất dữ liệu (CSV)
- Phân tích sử dụng
- Quản lý nhóm
- Thanh toán (nếu premium)

## Tóm Tắt

**File đã cập nhật:**
- ✅ config.h: Đã thêm BLYNK_AUTH_TOKEN và BLYNK_TEMPLATE_ID
- ✅ main.cpp: Xử lý tất cả ghi virtual pin
- ✅ cloud_logger.cpp: Ghi log sự kiện Blynk

**Bước tiếp theo:**
1. Tạo tài khoản Blynk
2. Tạo template và thiết bị
3. Copy auth token vào config.h
4. Nạp firmware
5. Cấu hình widget bảng điều khiển
6. Test toàn bộ chức năng

**Thông tin thêm:**
- Tài liệu Blynk: https://docs.blynk.io/
- Cộng đồng Blynk: https://community.blynk.cc/
- Video hướng dẫn: https://www.youtube.com/c/BlynkIoT