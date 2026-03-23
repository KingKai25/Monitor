# 📞 Thẻ Tham Chiếu Nhanh - Quick Reference Card

**In ra và dán trên máy tính để tiện tham khảo!**

---

## 🔌 Chân GPIO - Pin Assignment (Mới)

```
┌─────────────────────────────────────────┐
│         ESP32 GPIO PIN MAP              │
├─────────────┬──────────┬────────────┬───┤
│ GPIO#       │ Chân     │ Thiết Bị   │Loại│
├─────────────┼──────────┼────────────┼───┤
│ GPIO4   → D4   │ Buzzer      │OUT│
│ GPIO16  → RX2  │ AS608 RX    │IN │
│ GPIO17  → TX2  │ AS608 TX    │OUT│
│ GPIO18  → D18  │ PZEM TX     │OUT│
│ GPIO19  → D19  │ PZEM RX     │IN │
│ GPIO23  → D23  │ Relay       │OUT│
│ GPIO25  → D25  │ MQ2         │IN │
│ GPIO26  → D26  │ Button      │IN │
│ GPIO32  → D32  │ DHT11       │IN │
│ GPIO33  → D33  │ Fire        │IN │
└─────────────┴──────────┴────────────┴───┘
```

---

## ⚡ Kết Nối Điện - Power

```
3.3V   → DHT11 VCC, AS608 VCC
5V     → PZEM, MQ2, Fire, Relay, Buzzer
GND    → TẤT CẢ (chủ yếu!)
```

---

## 🔐 Credentials - Mã Xác Thực

### Bước 1: Lấy WiFi
```
SSID:     [________________]
Password: [________________]
```

### Bước 2: Lấy Blynk (https://blynk.cloud)
```
Template ID: [_______________________________]
Auth Token:  [_______________________________]
```

### Bước 3: Lấy Google Script
```
Script ID: [_______________________________]
```

### Bước 4: Dán vào config.h
```cpp
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASS"
#define BLYNK_TEMPLATE_ID "[Template ID]"
#define BLYNK_AUTH_TOKEN "[Auth Token]"
#define GOOGLE_SCRIPT_ID "[Script ID]"
```

---

## 🚀 Các Bước Chạy

```
1. Sửa config.h       ← Dán credentials
2. Kết nối dây        ← Theo sơ đồ mới
3. Upload firmware    → pio run -t upload
4. Mở serial monitor  → pio device monitor --baud 115200
5. Kiểm tra boot      ← Xem lỗi gì
6. Mở Blynk app       → Device phải "Online"
7. Test button        → Relay phải click
```

---

## 🧪 Kiểm Tra

| Thử Nghiệm | Kết Quả ✅ | Kết Quả ❌ |
|----------|----------|----------|
| Nhấn button | Relay click, đèn toggle | Không có gì xảy ra |
| Wave hand MQ2 | Giá trị > 400 | Giá trị không đổi |
| DHT11 | Còi khí T°C | Hiển thị 0 hoặc sai |
| Blynk app | "Online" xanh | "Offline" đỏ |
| Google Sheet | Có dòng mới | Không có dòng |

---

## 🔧 Troubleshooting

### Device Offline?
- [ ] WiFi SSID/password đúng không?
- [ ] Auth token khớp không?
- [ ] Restart ESP32
- [ ] Upload lại firmware

### Chân GPIO không hoạt động?
- [ ] Chân kết nối đúng không?
- [ ] GND được nối chung không?
- [ ] 3.3V/5V không bị đảo?
- [ ] Kiểm tra serial monitor lỗi gì

### Google Sheets không ghi log?
- [ ] Script đã Deploy chưa?
- [ ] Quyền = "Anyone" chưa?
- [ ] Script ID đúng không?
- [ ] WiFi có internet không?

---

## 📞 Liên Hệ

**Cần trợ giúp?**
```
Setup:        SETUP_GUIDE.md
Credentials:  GET_CREDENTIALS.md
Chân GPIO:    PIN_MAPPING_VI.md
Blynk app:    BLYNK_SETUP.md
Vân tay:      AS608_GUIDE.md
Điện lực:     PZEM_GUIDE.md
```

---

## 💾 Terminal Commands

```bash
# Compile firmware
pio run

# Upload to ESP32
pio run -t upload

# View serial output
pio device monitor --baud 115200

# Clean build
pio run -t clean
```

---

## ✅ Pre-Flight Checklist

**Trước khi upload:**
- [ ] WiFi available & working
- [ ] config.h đã sửa credentials
- [ ] Tất cả dây đã kết nối
- [ ] GND được nối chung
- [ ] 3.3V/5V không bị lộn
- [ ] Blynk app setup xong
- [ ] Google Scripts deployed

**Sau khi upload:**
- [ ] Serial monitor boot OK
- [ ] Blynk device "Online"
- [ ] Button → Relay click ✓
- [ ] DHT11 → Temp reading ✓
- [ ] Google Sheet → New row ✓

---

## 🎯 Success Metrics

```
✅ Firmware compiled
✅ Device online on Blynk
✅ Temperature shows in app
✅ Button toggles light
✅ Data logged to Google Sheets
✅ All 10 sensors working
→ SYSTEM READY FOR PRODUCTION
```

---

## 📚 Tài liệu Chính

### Cần Làm Ngay
1. GET_CREDENTIALS.md (15 phút)
2. Kết nối dây (10 phút)
3. config.h (5 phút)
4. Upload (5 phút)

### Sau Đó
- PIN_MAPPING_VI.md (Nếu có vấn đề)
- SETUP_GUIDE.md (Nếu muốn chi tiết)
- BLYNK_SETUP.md (Nếu app không work)

---

## 💡 Pro Tips

```
1. Giữ serial monitor mở để debug
2. Test từng sensor riêng lẻ
3. Keep credentials safe (kopya backup)
4. Check GND connections first!
5. Restart ESP32 nếu lỗi lạ
```

---

## 📞 Emergency Support

**Nếu hệ thống không chạy:**
1. Xem serial monitor (dòng lỗi gì?)
2. Check GET_CREDENTIALS.md
3. Check PIN_MAPPING_VI.md
4. Mở SETUP_GUIDE.md troubleshooting

**Nếu chân GPIO sai:**
1. Xem PIN_MAPPING_VI.md
2. So sánh với sơ đồ của bạn
3. Kết nối lại dây
4. Upload firmware lại

---

**Phiên bản:** 1.0.1  
**Cập nhật:** March 1, 2026  
**In ngày:** _______________

---

### HÀNH ĐỘNG TIẾP THEO:

```
[ ] 1. Đọc GET_CREDENTIALS.md
[ ] 2. Lấy Blynk token
[ ] 3. Tạo Google Script
[ ] 4. Sửa config.h
[ ] 5. Kết nối dây theo PIN_MAPPING_VI.md
[ ] 6. Upload: pio run -t upload
[ ] 7. Test tất cả sensors
[ ] 8. Enjoy! 🎉
```

---

**Lưu ý:** In hoặc save file này vào desktop để tiện sử dụng!
