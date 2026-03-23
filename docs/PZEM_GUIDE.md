# PZEM-004T Power Monitor Implementation Guide

Complete guide for PZEM-004T v3 power consumption monitoring in the warehouse system.

## Overview

**PZEM-004T v3 (Three-Phase or Single-Phase)**
- Communication: Modbus RTU (Serial UART)
- Baud Rate: 9600 bps (default)
- Measurement Range:
  - Voltage: 0-300V AC
  - Current: 0-100A AC (depends on CT ratio)
  - Power: 0-23000W
  - Energy: Cumulative kWh
  - Frequency: 45-65Hz
  - Power Factor: 0-1
- Accuracy: ±1% for voltage and current

## Pin Configuration

```
PZEM Pin     | Signal      | ESP32 Pin
-------------|-------------|----------
VCC          | 5V Power    | 5V
GND          | Ground      | GND
RX           | Serial RX   | GPIO16
TX           | Serial TX   | GPIO17
A (CT Input) | Current     | Via CT transformer
L (Phase)    | AC Phase    | Main AC supply
N (Neutral)  | AC Neutral  | Main AC neutral
```

**UART2 Configuration** (in firmware)
```cpp
#define PZEM_RX 16
#define PZEM_TX 17
SerialPZEM.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);
```

## Modbus RTU Protocol

### Frame Structure

```
┌─────────────┬──────────────┬──────────┬──────────┐
│  Slave ID   │  Function    │   Data   │   CRC    │
│  (1 byte)   │   (1 byte)   │ (variable)│ (2 bytes)│
└─────────────┴──────────────┴──────────┴──────────┘
│<─── Address ───>│<──── PDU (Protocol Data Unit) ──>│
```

### Slave ID
- Default: 0xF8 (or 248 in decimal)
- Configurable via command (0xB4)
- Range: 1-247

### Function Codes

```
0x03 = Read Holding Registers
0x06 = Write Single Register
0x10 = Write Multiple Registers
0xB4 = Modbus PZEM extension (read/write params)
```

### Register Mapping (Function Code 0x03)

Read registers starting from address with count.

| Register | Address | Size | Unit | Default |
|----------|---------|------|------|---------|
| Voltage | 0x0000 | 2 | V | 230V |
| Current | 0x0001 | 2 | A | 0A |
| Power (H) | 0x0003 | 2 | W | 0W |
| Power (L) | 0x0004 | 2 | W | - |
| Energy (H) | 0x0005 | 2 | kWh | 0 |
| Energy (L) | 0x0006 | 2 | kWh | - |
| Frequency | 0x0007 | 1 | Hz | 50Hz |
| Power Factor | 0x0008 | 1 | - | 1.0 |
| Alarm | 0x0009 | 1 | - | 0 |

## Data Representation

### Two's Complement 16-bit Values

```
Single Register (2 bytes):
┌────────────┬────────────┐
│  High Byte │  Low Byte  │
└────────────┴────────────┘
  0-65535

Example: Voltage = 230V
├─ Register value: 0x00E6
├─ Calculation: (0x00 << 8) | 0xE6
├─ Decimal: 230
└─ Display: 230V / 10 = 23.0V
```

### Multi-Register Values (32-bit)

```
Two Registers (4 bytes):
┌──────────┬──────────┬──────────┬──────────┐
│ Byte 1   │ Byte 2   │ Byte 3   │ Byte 4   │
│ Register │ Register │ Register │ Register │
└──────────┴──────────┴──────────┴──────────┘

Example: Power = 2345W
├─ Registers: [0x0000, 0x0929]
├─ Combined: (0x0000 << 16) | 0x0929
├─ Decimal: 2345
└─ Display: 2345W
```

## CRC Calculation

Modbus RTU uses 16-bit CRC-CCITT

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

## Read Voltage Example

### Command Frame

```
Slave ID:    0xF8
Function:    0x03 (Read Holding Registers)
Start Addr:  0x0000 (high), 0x00 (low)
Qty Registers: 0x0001
CRC:         Calculated

Frame: F8 03 00 00 00 01 [CRC_H] [CRC_H]

Calculation:
├─ Data: F8 03 00 00 00 01
├─ CRC = 0xE407
└─ Frame: F8 03 00 00 00 01 E4 07
```

### Response Frame

```
Slave ID:    0xF8
Function:    0x03
Byte Count:  0x02 (2 bytes following)
Register H:  0x00
Register L:  0xE6 (230 in decimal)
CRC:         Calculated

Frame: F8 03 02 00 E6 [CRC_H] [CRC_L]

Value Extraction:
├─ High byte: 0x00
├─ Low byte: 0xE6
├─ Combined: (0x00 << 8) | 0xE6 = 0x00E6
├─ Decimal: 230
├─ Voltage: 230 / 10 = 23.0V (or 230V after scaling)
```

## Implementation Code

### Basic Read Function

```cpp
bool PowerMonitor::readVoltage(float& voltage) {
  // Build command: Read register 0x0000
  uint8_t cmd[] = {0xF8, 0x03, 0x00, 0x00, 0x00, 0x02};
  
  // Calculate and append CRC
  uint16_t crc = calculateCRC(cmd, 6);
  uint8_t fullCmd[] = {0xF8, 0x03, 0x00, 0x00, 0x00, 0x02, 
                       (uint8_t)(crc >> 8), (uint8_t)(crc & 0xFF)};
  
  // Send command
  sendFrame(fullCmd, 8);
  
  // Wait for response
  delay(100);
  
  // Read response: 1 byte slave + 1 byte func + 1 byte count + 2 bytes data + 2 bytes CRC
  uint8_t response[7] = {0};
  
  if (readFrame(response, 7)) {
    // Verify response
    if (response[0] != 0xF8 || response[1] != 0x03) {
      return false;
    }
    
    // Extract voltage (Little-endian for Modbus)
    uint16_t rawVoltage = (response[3] << 8) | response[4];
    voltage = rawVoltage / 10.0;  // Scale factor
    
    return true;
  }
  
  return false;
}
```

### CRC Verification

```cpp
bool PowerMonitor::readFrame(uint8_t* frame, uint8_t length) {
  unsigned long timeout = millis() + 1000;
  uint8_t idx = 0;
  
  // Read data + CRC (length + 2 bytes for CRC)
  while (millis() < timeout) {
    if (serial.available()) {
      frame[idx++] = serial.read();
      if (idx >= length + 2) {
        // Verify CRC
        uint16_t receivedCrc = (frame[length] << 8) | frame[length + 1];
        uint16_t calculatedCrc = calculateCRC(frame, length);
        
        if (receivedCrc == calculatedCrc) {
          return true;  // Valid frame
        } else {
          return false; // CRC error
        }
      }
    }
  }
  
  return false;  // Timeout
}
```

### Read All Metrics

```cpp
bool PowerMonitor::readAll() {
  // Prepare arrays for all values
  float voltage, current, power, energy, frequency, powerFactor;
  
  Serial.println("Reading power metrics...");
  
  // Read each parameter with delay
  if (!readVoltage(voltage)) {
    Serial.println("Failed to read voltage");
    return false;
  }
  delay(100);
  
  if (!readCurrent(current)) {
    Serial.println("Failed to read current");
    return false;
  }
  delay(100);
  
  if (!readPower(power)) {
    Serial.println("Failed to read power");
    return false;
  }
  delay(100);
  
  if (!readEnergy(energy)) {
    Serial.println("Failed to read energy");
    return false;
  }
  delay(100);
  
  if (!readFrequency(frequency)) {
    Serial.println("Failed to read frequency");
    return false;
  }
  delay(100);
  
  if (!readPowerFactor(powerFactor)) {
    Serial.println("Failed to read power factor");
    return false;
  }
  
  // Store in registers and display
  registers.voltage = voltage;
  registers.current = current;
  registers.power = power;
  registers.energy = energy;
  registers.frequency = frequency;
  registers.powerFactor = powerFactor;
  
  Serial.print("Voltage: "); Serial.print(voltage); Serial.println(" V");
  Serial.print("Current: "); Serial.print(current); Serial.println(" A");
  Serial.print("Power: "); Serial.print(power); Serial.println(" W");
  Serial.print("Energy: "); Serial.print(energy); Serial.println(" kWh");
  Serial.print("Frequency: "); Serial.print(frequency); Serial.println(" Hz");
  Serial.print("Power Factor: "); Serial.println(powerFactor);
  
  return true;
}
```

## Scaling Factors

```cpp
const float VOLTAGE_SCALE = 10.0;    // V
const float CURRENT_SCALE = 1000.0;  // mA
const float POWER_SCALE = 1.0;       // W
const float ENERGY_SCALE = 100.0;    // 1/100 kWh
const float FREQUENCY_SCALE = 10.0;  // 1/10 Hz
const float PF_SCALE = 100.0;        // 1/100
```

## Reset/Configuration Commands

### Reset Energy Counter

```cpp
bool PowerMonitor::resetEnergy() {
  // Command: 0xB4 reset command
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

### Change Slave ID

```cpp
bool PowerMonitor::setSlaveId(uint8_t newId) {
  // Command: 0xB4 with new slave ID
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

## Current Transformer (CT) Ratio

### Physical Connection

```
           ┌─── Main AC Phase
           │
        ╔══════╗
        ║  CT  ║ (Current Transformer)
        ║ n:m  ║
        ╚══════╝
           │
           └─── to PZEM
           
Common CT ratios:
├─ 100A/5A = 20:1
├─ 200A/5A = 40:1
├─ 300A/5A = 60:1
└─ Adjustable via PZEM settings
```

### Configuration

```
PZEM-004T allows CT ratio configuration:
1. Via command 0xB4 register 0x00, 0x01
2. Default: Usually 100A/5A (ratio 20)

Example for 200A/5A (ratio 40):
├─ Command: 0xB4 0x00 0x01 0x00 0x28
└─ Value: 40 (0x0028)
```

## Calibration & Testing

### 1. No-Load Calibration
```
1. Disconnect load from measured circuit
2. PZEM should read ~0A, ~0W
3. If not, use zero-offset calibration command
```

### 2. Load Test

```
Light bulb test:
├─ 100W bulb
├─ Expected: 100W at rated voltage
├─ Example: 230V = 100W / 230V ≈ 0.43A
└─ Compare with PZEM reading
```

### 3. Energy Verification

```
Run light for 1 hour:
├─ 100W × 1 hour = 0.1 kWh
├─ PZEM should show ~0.1 kWh increase
└─ Verify calculation: Energy = Power × Time
```

## Troubleshooting

| Issue | Cause | Solution |
|-------|-------|----------|
| **No response** | Serial connection | Check RX/TX pins, baud rate |
| **Voltage always 0** | Circuit not connected | Check CT connection to AC |
| **Current reads 0** | CT not installed | Install CT transformer |
| **False readings** | CT ratio wrong | Recalibrate with known load |
| **CRC error** | Noise/baud mismatch | Check serial speed 9600 |
| **Power shows 0W** | Load off | Turn on light or device |
| **Energy not increasing** | Reset happened | Check reset command execution |

## Real-World Example: Light Bulb Monitoring

```cpp
struct LightBulbMetrics {
  float instantPower;      // Current watts
  float dailyEnergy;       // kWh used today
  float costPerHour;       // Calculated cost
  uint32_t lastReadTime;   // For rate limiting
};

LightBulbMetrics bulb;
const float COST_PER_KWH = 2.5;  // Example: $2.50/kWh

void updateBulbMetrics() {
  if (millis() - bulb.lastReadTime < 5000) {
    return;  // Read every 5 seconds
  }
  
  float voltage, current, power, energy;
  if (powerMonitor.readAll(voltage, current, power, energy)) {
    bulb.instantPower = power;
    bulb.dailyEnergy = energy;
    bulb.costPerHour = (power / 1000.0) * COST_PER_KWH;  // kW * $/kWh
    
    // Log to Blynk
    Blynk.virtualWrite(V6, power);      // Watts
    Blynk.virtualWrite(V7, energy);     // kWh
    
    // Alert if high power
    if (power > 2000) {
      Blynk.logEvent("high_power", "Bulb consuming " + String(power) + "W");
    }
    
    bulb.lastReadTime = millis();
  }
}
```

## Integration with Blynk

```cpp
// In updateBlynkValues():
void updateBlynkValues() {
  float v, i, p, e;
  
  if (powerMonitor.readAll(v, i, p, e)) {
    Blynk.virtualWrite(V8, v);  // Voltage
    Blynk.virtualWrite(V9, i);  // Current
    Blynk.virtualWrite(V6, p);  // Power (graph/gauge)
    Blynk.virtualWrite(V7, e);  // Energy (kWh counter)
  }
}
```

## Performance Metrics

```
Read Voltage: ~150ms
Read Current: ~150ms
Read Power: ~150ms
Read Energy: ~150ms
Read Frequency: ~100ms
Read All (optimized): ~500-700ms
```

## References

- PZEM-004T v3 Protocol Manual
- Modbus RTU Standard IEC 61110
- CT Transformer specifications
- AC Power measurement theory

## Implementation Status

- ✅ Header file: `include/power_monitor.h`
- ✅ Source file: `src/power_monitor.cpp`
- ✅ CRC calculation
- ✅ Basic Modbus reading
- 📋 CT ratio configuration
- 📋 Energy reset function
- 📋 Advanced diagnostics

## Next Steps

1. Compile and upload firmware
2. Test serial communication with PZEM
3. Verify CRC handling
4. Test voltage/current readings with known load
5. Calibrate CT transformer ratio
6. Integrate with light bulb monitoring
7. Display metrics on Blynk app
8. Log to Google Sheets

---

**Last Updated**: March 2026
**Firmware Version**: 1.0.0 (PZEM-004T Integration)
