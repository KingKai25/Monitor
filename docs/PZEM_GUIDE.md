# Hướng Dẫn Triển Khai Giám Sát Điện PZEM-004T

Hướng dẫn đầy đủ cho giám sát tiêu thụ điện năng PZEM-004T v3 trong hệ thống kho hàng.

## Tổng Quan

**PZEM-004T v3 (Đơn pha hoặc Ba pha)**
- Giao tiếp: Modbus RTU (UART Serial)
- Tốc độ Baud: 9600 bps (mặc định)
- Phạm vi đo:
  - Điện áp: 0-300V AC
  - Dòng điện: 0-100A AC (phụ thuộc tỷ số CT)
  - Công suất: 0-23000W
  - Năng lượng: Tích lũy kWh
  - Tần số: 45-65Hz
  - Hệ số công suất: 0-1
- Độ chính xác: ±1% cho điện áp và dòng điện

## Cấu Hình Chân

```
Chân PZEM    | Tín Hiệu   | Chân ESP32
-------------|-------------|----------
VCC          | Nguồn 5V   | 5V
GND          | Ground     | GND
RX           | Serial RX  | GPIO18 (D18)
TX           | Serial TX  | GPIO19 (D19)
A (CT Input) | Dòng điện  | Qua biến dòng CT
L (Pha)      | Pha AC     | Nguồn AC chính
N (Trung tính)| Trung tính AC | Trung tính AC chính
```

**Cấu hình UART1 (Serial1)** (trong firmware)
```cpp
#define PZEM_RX 19
#define PZEM_TX 18
SerialPZEM.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);
```

## Giao Thức Modbus RTU

### Cấu Trúc Khung

```
+-------------+--------------+----------+----------+
|  Slave ID   |  Function    |   Data   |   CRC    |
|  (1 byte)   |   (1 byte)   | (biến đổi)| (2 byte) |
+-------------+--------------+----------+----------+
|<--- Địa chỉ --->|<---- PDU (Protocol Data Unit) -->|
```

### Slave ID
- Mặc định: 0xF8 (hoặc 248 thập phân)
- Có thể cấu hình qua lệnh (0xB4)
- Phạm vi: 1-247

### Mã Chức Năng

```
0x03 = Đọc Holding Register
0x06 = Ghi Single Register
0x10 = Ghi Multiple Register
0xB4 = Mở rộng Modbus PZEM (đọc/ghi tham số)
```

### Bảng Register (Mã chức năng 0x03)

Đọc register bắt đầu từ địa chỉ với số lượng.

| Register | Địa Chỉ | Kích Thước | Đơn Vị | Mặc Định |
|----------|---------|------------|--------|----------|
| Điện áp | 0x0000 | 2 | V | 230V |
| Dòng điện | 0x0001 | 2 | A | 0A |
| Công suất (H) | 0x0003 | 2 | W | 0W |
| Công suất (L) | 0x0004 | 2 | W | - |
| Năng lượng (H) | 0x0005 | 2 | kWh | 0 |
| Năng lượng (L) | 0x0006 | 2 | kWh | - |
| Tần số | 0x0007 | 1 | Hz | 50Hz |
| Hệ số công suất | 0x0008 | 1 | - | 1.0 |
| Cảnh báo | 0x0009 | 1 | - | 0 |

## Biểu Diễn Dữ Liệu

### Giá Trị 16-bit Bù 2

```
Một Register (2 byte):
+------------+------------+
|  Byte Cao  |  Byte Thấp |
+------------+------------+
  0-65535

Ví dụ: Điện áp = 230V
├─ Giá trị register: 0x00E6
├─ Tính toán: (0x00 << 8) | 0xE6
├─ Thập phân: 230
└─ Hiển thị: 230V / 10 = 23.0V
```

### Giá Trị Đa Register (32-bit)

```
Hai Register (4 byte):
+----------+----------+----------+----------+
| Byte 1   | Byte 2   | Byte 3   | Byte 4   |
| Register | Register | Register | Register |
+----------+----------+----------+----------+

Ví dụ: Công suất = 2345W
├─ Register: [0x0000, 0x0929]
├─ Kết hợp: (0x0000 << 16) | 0x0929
├─ Thập phân: 2345
└─ Hiển thị: 2345W
```

## Tính Toán CRC

Modbus RTU sử dụng CRC-CCITT 16-bit

```cpp
uint16_t calculateCRC(uint8_t* frame, uint8_t length) {
  uint16_t crc = 0xFFFF;
  
  for (uint8_t i = 0; i < length; i++) {
    crc ^= frame[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 1) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  
  return crc;
}
```

## Ví Dụ Đọc Điện Áp

### Khung Lệnh

```
Slave ID:    0xF8
Function:    0x03 (Đọc Holding Register)
Đ/c bắt đầu: 0x0000 (cao), 0x00 (thấp)
Số register:  0x0001
CRC:         Tính toán

Khung: F8 03 00 00 00 01 [CRC_H] [CRC_L]

Tính toán:
├─ Dữ liệu: F8 03 00 00 00 01
├─ CRC = 0xE407
└─ Khung: F8 03 00 00 00 01 E4 07
```

### Khung Phản Hồi

```
Slave ID:    0xF8
Function:    0x03
Số byte:     0x02 (2 byte tiếp theo)
Register H:  0x00
Register L:  0xE6 (230 thập phân)
CRC:         Tính toán

Khung: F8 03 02 00 E6 [CRC_H] [CRC_L]

Trích xuất giá trị:
├─ Byte cao: 0x00
├─ Byte thấp: 0xE6
├─ Kết hợp: (0x00 << 8) | 0xE6 = 0x00E6
├─ Thập phân: 230
├─ Điện áp: 230 / 10 = 23.0V (hoặc 230V sau chia tỷ lệ)
```

## Code Triển Khai

### Hàm Đọc Cơ Bản

```cpp
bool PowerMonitor::readVoltage(float& voltage) {
  // Tao lenh: Doc register 0x0000
  uint8_t cmd[] = {0xF8, 0x03, 0x00, 0x00, 0x00, 0x02};
  
  // Tinh va them CRC
  uint16_t crc = calculateCRC(cmd, 6);
  uint8_t fullCmd[] = {0xF8, 0x03, 0x00, 0x00, 0x00, 0x02, 
                       (uint8_t)(crc >> 8), (uint8_t)(crc & 0xFF)};
  
  // Gui lenh
  sendFrame(fullCmd, 8);
  
  // Cho phan hoi
  delay(100);
  
  // Doc phan hoi: 1 byte slave + 1 byte func + 1 byte count + 2 byte data + 2 byte CRC
  uint8_t response[7] = {0};
  
  if (readFrame(response, 7)) {
    // Xac minh phan hoi
    if (response[0] != 0xF8 || response[1] != 0x03) {
      return false;
    }
    
    // Trich xuat dien ap
    uint16_t rawVoltage = (response[3] << 8) | response[4];
    voltage = rawVoltage / 10.0;  // He so ty le
    
    return true;
  }
  
  return false;
}
```

### Xác Minh CRC

```cpp
bool PowerMonitor::readFrame(uint8_t* frame, uint8_t length) {
  unsigned long timeout = millis() + 1000;
  uint8_t idx = 0;
  
  // Doc du lieu + CRC (length + 2 byte cho CRC)
  while (millis() < timeout) {
    if (serial.available()) {
      frame[idx++] = serial.read();
      if (idx >= length + 2) {
        // Xac minh CRC
        uint16_t receivedCrc = (frame[length] << 8) | frame[length + 1];
        uint16_t calculatedCrc = calculateCRC(frame, length);
        
        if (receivedCrc == calculatedCrc) {
          return true;  // Khung hop le
        } else {
          return false; // Loi CRC
        }
      }
    }
  }
  
  return false;  // Het thoi gian cho
}
```

### Đọc Tất Cả Thông Số

```cpp
bool PowerMonitor::readAll() {
  float voltage, current, power, energy, frequency, powerFactor;
  
  Serial.println("Dang doc thong so dien...");
  
  // Doc tung tham so voi do tre
  if (!readVoltage(voltage)) {
    Serial.println("Doc dien ap that bai");
    return false;
  }
  delay(100);
  
  if (!readCurrent(current)) {
    Serial.println("Doc dong dien that bai");
    return false;
  }
  delay(100);
  
  if (!readPower(power)) {
    Serial.println("Doc cong suat that bai");
    return false;
  }
  delay(100);
  
  if (!readEnergy(energy)) {
    Serial.println("Doc nang luong that bai");
    return false;
  }
  delay(100);
  
  if (!readFrequency(frequency)) {
    Serial.println("Doc tan so that bai");
    return false;
  }
  delay(100);
  
  if (!readPowerFactor(powerFactor)) {
    Serial.println("Doc he so cong suat that bai");
    return false;
  }
  
  // Luu vao register va hien thi
  registers.voltage = voltage;
  registers.current = current;
  registers.power = power;
  registers.energy = energy;
  registers.frequency = frequency;
  registers.powerFactor = powerFactor;
  
  Serial.print("Dien ap: "); Serial.print(voltage); Serial.println(" V");
  Serial.print("Dong dien: "); Serial.print(current); Serial.println(" A");
  Serial.print("Cong suat: "); Serial.print(power); Serial.println(" W");
  Serial.print("Nang luong: "); Serial.print(energy); Serial.println(" kWh");
  Serial.print("Tan so: "); Serial.print(frequency); Serial.println(" Hz");
  Serial.print("He so CS: "); Serial.println(powerFactor);
  
  return true;
}
```

## Hệ Số Tỷ Lệ

```cpp
const float VOLTAGE_SCALE = 10.0;    // V
const float CURRENT_SCALE = 1000.0;  // mA
const float POWER_SCALE = 1.0;       // W
const float ENERGY_SCALE = 100.0;    // 1/100 kWh
const float FREQUENCY_SCALE = 10.0;  // 1/10 Hz
const float PF_SCALE = 100.0;        // 1/100
```

## Lệnh Reset/Cấu Hình

### Reset Bộ Đếm Năng Lượng

```cpp
bool PowerMonitor::resetEnergy() {
  uint8_t cmd[] = {0xF8, 0xB4, 0x80, 0x11, 0x00, 0x00};
  
  uint16_t crc = calculateCRC(cmd, 6);
  uint8_t fullCmd[8];
  memcpy(fullCmd, cmd, 6);
  fullCmd[6] = crc >> 8;
  fullCmd[7] = crc & 0xFF;
  
  sendFrame(fullCmd, 8);
  delay(100);
  
  return true;
}
```

### Đổi Slave ID

```cpp
bool PowerMonitor::setSlaveId(uint8_t newId) {
  uint8_t cmd[] = {0xF8, 0xB4, 0x00, 0x02, 0x00, newId};
  
  uint16_t crc = calculateCRC(cmd, 6);
  uint8_t fullCmd[8];
  memcpy(fullCmd, cmd, 6);
  fullCmd[6] = crc >> 8;
  fullCmd[7] = crc & 0xFF;
  
  sendFrame(fullCmd, 8);
  delay(100);
  
  slaveId = newId;
  return true;
}
```

## Biến Dòng (CT) Tỷ Số

### Kết Nối Vật Lý

```
           +--- Dây pha AC chính
           |
        +======+
        |  CT  | (Biến dòng)
        | n:m  |
        +======+
           |
           +--- đến PZEM
           
Tỷ số CT phổ biến:
├─ 100A/5A = 20:1
├─ 200A/5A = 40:1
├─ 300A/5A = 60:1
└─ Có thể điều chỉnh qua cài đặt PZEM
```

### Cấu Hình

```
PZEM-004T cho phép cấu hình tỷ số CT:
1. Qua lệnh 0xB4 register 0x00, 0x01
2. Mặc định: Thường 100A/5A (tỷ số 20)

Ví dụ cho 200A/5A (tỷ số 40):
├─ Lệnh: 0xB4 0x00 0x01 0x00 0x28
└─ Giá trị: 40 (0x0028)
```

## Hiệu Chuẩn & Kiểm Tra

### 1. Hiệu Chuẩn Không Tải
```
1. Ngắt tải khỏi mạch đo
2. PZEM phải đọc ~0A, ~0W
3. Nếu không, sử dụng lệnh hiệu chuẩn offset zero
```

### 2. Test Tải

```
Test bóng đèn:
├─ Bóng đèn 100W
├─ Dự kiến: 100W ở điện áp danh định
├─ Ví dụ: 230V = 100W / 230V ≈ 0.43A
└─ So sánh với giá trị PZEM đọc được
```

### 3. Xác Minh Năng Lượng

```
Chạy đèn trong 1 giờ:
├─ 100W x 1 giờ = 0.1 kWh
├─ PZEM phải hiển thị tăng ~0.1 kWh
└─ Xác minh: Năng lượng = Công suất x Thời gian
```

## Xử Lý Sự Cố

| Vấn Đề | Nguyên Nhân | Giải Pháp |
|---------|-------------|-----------|
| **Không phản hồi** | Kết nối serial | Kiểm tra chân RX/TX (GPIO18/19), baud rate |
| **Điện áp luôn = 0** | Mạch chưa kết nối | Kiểm tra kết nối CT với AC |
| **Dòng điện đọc = 0** | CT chưa lắp | Lắp biến dòng CT |
| **Giá trị sai** | Tỷ số CT sai | Hiệu chuẩn lại với tải đã biết |
| **Lỗi CRC** | Nhiễu/baud sai | Kiểm tra tốc độ serial 9600 |
| **Công suất hiển thị 0W** | Tải tắt | Bật đèn hoặc thiết bị |
| **Năng lượng không tăng** | Đã reset | Kiểm tra thực thi lệnh reset |

## Ví Dụ Thực Tế: Giám Sát Bóng Đèn

```cpp
struct LightBulbMetrics {
  float instantPower;      // Watt hien tai
  float dailyEnergy;       // kWh su dung hom nay
  float costPerHour;       // Chi phi tinh toan
  uint32_t lastReadTime;   // Gioi han toc do doc
};

LightBulbMetrics bulb;
const float COST_PER_KWH = 2500;  // Vi du: 2500 VND/kWh

void updateBulbMetrics() {
  if (millis() - bulb.lastReadTime < 5000) {
    return;  // Doc moi 5 giay
  }
  
  float voltage, current, power, energy;
  if (powerMonitor.readAll(voltage, current, power, energy)) {
    bulb.instantPower = power;
    bulb.dailyEnergy = energy;
    bulb.costPerHour = (power / 1000.0) * COST_PER_KWH;
    
    // Ghi log len Blynk
    Blynk.virtualWrite(V6, power);      // Watt
    Blynk.virtualWrite(V7, energy);     // kWh
    
    // Canh bao neu cong suat cao
    if (power > 2000) {
      Blynk.logEvent("high_power", "Den tieu thu " + String(power) + "W");
    }
    
    bulb.lastReadTime = millis();
  }
}
```

## Tích Hợp Với Blynk

```cpp
// Trong updateBlynkValues():
void updateBlynkValues() {
  float v, i, p, e;
  
  if (powerMonitor.readAll(v, i, p, e)) {
    Blynk.virtualWrite(V8, v);  // Dien ap
    Blynk.virtualWrite(V9, i);  // Dong dien
    Blynk.virtualWrite(V6, p);  // Cong suat (bieu do/dong ho)
    Blynk.virtualWrite(V7, e);  // Nang luong (kWh)
  }
}
```

## Chỉ Số Hiệu Suất

```
Đọc điện áp: ~150ms
Đọc dòng điện: ~150ms
Đọc công suất: ~150ms
Đọc năng lượng: ~150ms
Đọc tần số: ~100ms
Đọc tất cả (tối ưu): ~500-700ms
```

## Tham Khảo

- Hướng dẫn giao thức PZEM-004T v3
- Tiêu chuẩn Modbus RTU IEC 61110
- Thông số kỹ thuật biến dòng CT
- Lý thuyết đo công suất AC

## Trạng Thái Triển Khai

- ✅ File header: `include/power_monitor.h`
- ✅ File source: `src/power_monitor.cpp`
- ✅ Tính toán CRC
- ✅ Đọc Modbus cơ bản
- 📋 Cấu hình tỷ số CT
- 📋 Hàm reset năng lượng
- 📋 Chẩn đoán nâng cao

## Bước Tiếp Theo

1. Biên dịch và nạp firmware
2. Test giao tiếp serial với PZEM
3. Xác minh xử lý CRC
4. Test đọc điện áp/dòng điện với tải đã biết
5. Hiệu chuẩn tỷ số biến dòng CT
6. Tích hợp với giám sát bóng đèn
7. Hiển thị số liệu trên Blynk app
8. Ghi log lên Google Sheets

---

**Cập nhật lần cuối**: Tháng 3, 2026
**Phiên bản Firmware**: 1.0.1 (Tích hợp PZEM-004T)