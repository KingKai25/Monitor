# Setup & Configuration Guide

Complete step-by-step guide to get your warehouse monitoring system running.

## Phase 1: Hardware Setup

### 1.1 Wiring Configuration

Based on your schematic diagram:

```
ESP32 Pin    | Component         | Signal Type | Notes
-------------|-------------------|-------------|------------------
GPIO25       | DHT11             | 1-Wire      | Temp/Humidity
GPIO35       | MQ2               | Analog in   | Smoke detection
GPIO36       | Fire Sensor       | Analog in   | Heat detection
GPIO34       | Button            | Digital in  | Debounced
GPIO33       | Buzzer            | Digital out | Active high
GPIO32       | Relay             | Digital out | Light control
GPIO9        | AS608 RX          | UART1       | 57600 baud
GPIO10       | AS608 TX          | UART1       | 57600 baud
GPIO16       | PZEM RX           | UART2       | 9600 baud
GPIO17       | PZEM TX           | UART2       | 9600 baud
GND          | All commons       | -           | Shared ground
5V           | Relay, Buzzer     | Power       | From PSU
3.3V         | Sensors, AS608    | Power       | ESP32 output
```

### 1.2 Component Connections

**AS608 Fingerprint Module** (Using Serial1)
```
AS608 Pin   | ESP32 Pin | Notes
------------|-----------|------------------
VCC (3.3V)  | 3.3V      | Power supply
GND         | GND       | Common ground
RX          | GPIO9     | Serial receive
TX          | GPIO10    | Serial transmit
```

**PZEM-004T Power Monitor** (Using Serial2)
```
PZEM Pin    | ESP32 Pin | Notes
------------|-----------|------------------
VCC (5V)    | 5V        | Power supply
GND         | GND       | Common ground
RX          | GPIO16    | Serial receive
TX          | GPIO17    | Serial transmit
A/B (CT)    | Light     | Current transformer
L/N         | AC mains  | Via relay
```

**DHT11** (GPIO25)
```
Pin 1 (VCC)  → 3.3V
Pin 2 (DAT)  → GPIO25 + 10kΩ pull-up to 3.3V
Pin 3 (NULL) → Not used
Pin 4 (GND)  → GND
```

**MQ2 Smoke Sensor** (GPIO35 ADC)
```
VCC → 5V
GND → GND
A0  → GPIO35 (ADC input)
D0  → (Optional, not used)
```

**Fire Sensor** (GPIO36 ADC)
```
VCC → 5V
GND → GND
AO  → GPIO36 (ADC input)
DO  → (Optional, not used)
```

**Button** (GPIO34)
```
Button 1 → 3.3V (via 10kΩ pull-up)
Button 2 → GPIO34
Button 3 → GND
Capacitor 100nF between GPIO34 and GND (optional, for noise filtering)
```

**Relay Module** (GPIO32)
```
VCC → 5V
GND → GND
IN  → GPIO32 (Active HIGH)
COM → Main AC Phase
NO  → Light phase
NC  → Not used
```

**Buzzer** (GPIO33)
```
+ (Long leg) → GPIO33
- (Short leg) → GND
Optional: Add 1N4007 diode across buzzer for protection
```

### 1.3 Power Supply

**Requirements:**
- ESP32: 3.3V @ 500mA max
- Relays: 5V @ 100mA
- PZEM: 5V @ 100mA
- Light loads: Through relay (AC power)

**Recommended PSU:**
- Main power: 5V/2A AC adapter

## Phase 2: Software Setup

### 2.1 Install PlatformIO

1. **Install VS Code**: https://code.visualstudio.com/
2. **Install PlatformIO Extension**:
   - Open VS Code
   - Extensions → Search "PlatformIO"
   - Install "PlatformIO IDE" by Ivan Kravets
3. **Reload VS Code**

### 2.2 Clone/Open Project

```bash
# Option 1: Clone from git
git clone <repository-url>
cd Monitor

# Option 2: Open existing folder
# File → Open Folder → Select Monitor directory
```

### 2.3 Verify Configuration Files

Check these files exist:
- `platformio.ini` - Build configuration
- `include/config.h` - Pin definitions
- `src/main.cpp` - Main firmware
- `README.md` - Documentation

## Phase 3: Credentials Configuration

### 3.1 Edit config.h

```cpp
// File: include/config.h

#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourPassword"

#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

#define GOOGLE_SCRIPT_ID "YOUR_SCRIPT_ID"
```

**Find your credentials:**

1. **WiFi SSID/Password**: Your home/office WiFi
2. **Blynk**: See section 3.2
3. **Google Script**: See section 3.3

### 3.2 Setup Blynk

1. **Create Account**:
   - Visit https://blynk.cloud/
   - Sign up with email

2. **Create Business/IoT Device**:
   - Organization → "New Organization"
   - Skip billing (you can use free tier)

3. **Create Template**:
   - Developer Zone → Templates
   - Click "Create New Template"
   - **Template Name**: Warehouse Monitor
   - **Select Hardware**: ESP32 Dev Board
   - **Select Connectivity**: WiFi
   - **Save**

4. **Create Auth Token**:
   - In template, find "Auth Token"
   - Copy entire token (includes numbers/letters)
   - Paste into `config.h` → `BLYNK_AUTH_TOKEN`

5. **Get Template ID**:
   - Template settings → Copy "TEMPLATE_ID"
   - Paste into `config.h` → `BLYNK_TEMPLATE_ID`

6. **Create Device**:
   - Devices → Create New Device
   - Select template "Warehouse Monitor"
   - Name: "Warehouse_ESP32"
   - **Save**

7. **Configure Virtual Pins** (in Blynk app):
   - V1: Button (Light ON/OFF)
   - V2: Button (Buzzer test)
   - V3-V9: Gauges/charts for sensors
   - V10-V12: LED indicators for alerts

### 3.3 Setup Google Sheets

1. **Create Google Apps Script**:
   - Visit https://script.google.com/
   - Create new project: "Warehouse Logger"

2. **Paste Script Code**:
   ```javascript
   // Script from docs/google_sheets_script.gs
   // Copy entire content
   ```

3. **Deploy Script**:
   - Deploy → New deployment
   - Type: "Web app"
   - Execute as: Your account
   - Who has access: "Anyone"
   - **Deploy**
   - Copy Script ID

4. **Save to config.h**:
   ```cpp
   #define GOOGLE_SCRIPT_ID "YOUR_COPIED_SCRIPT_ID"
   ```

5. **Create Google Sheet**:
   - Create new sheet in Drive
   - Name: "Warehouse_Monitor"
   - Create sheets:
     - "AccessLog" (columns: ID, Name, Action, Timestamp)
     - "AlertLog" (columns: Type, Value, Severity, Timestamp)
     - "DeviceStatus" (columns: Status, Details, Timestamp)

### 3.4 Test Configuration

Edit `include/config.h` sensors thresholds (optional):

```cpp
// Adjust based on your environment
#define SMOKE_THRESHOLD 400      // Lower = more sensitive
#define TEMP_THRESHOLD 40        // In Celsius
#define FIRE_THRESHOLD 500       // Adjust based on sensor testing
#define HUMIDITY_WARNING 80      // Percent
```

## Phase 4: Compilation & Upload

### 4.1 Build Firmware

```bash
# In PlatformIO terminal (Ctrl+`)
pio run

# Or in VS Code:
# Click PlatformIO icon → Project Tasks → Build
```

**Expected output:**
```
Building...
Linking...
Checking size...
Program size: ~300KB
RAM: ~50KB
Memory use: 35%
```

### 4.2 Connect ESP32

1. Connect ESP32 to computer via USB cable
2. Check COM port:
   - Windows: Device Manager → Ports (COM3, COM4, etc)
   - Linux: `ls /dev/ttyUSB*`
   - Mac: `ls /dev/tty.usbserial*`

3. Update `platformio.ini` (if needed):
   ```ini
   upload_port = COM3  ; Change to your port
   ```

### 4.3 Upload Firmware

```bash
# PlatformIO: Upload
pio run -t upload

# Monitor output:
# Should see "Uploading..." then "Done"
```

**Troubleshooting upload:**
- Try USB 2.0 port instead of USB 3.0
- Reduce upload speed in `platformio.ini`:
  ```ini
  upload_speed = 115200
  ```
- Install CH340 drivers (ESP32 USB chip: https://sparks.gogo.co.nz/ch340.html)

### 4.4 Monitor Serial Output

```bash
# Watch serial output
pio device monitor --baud 115200

# Look for:
# - WiFi connection status
# - Sensor initialization
# - Temperature/humidity values
# - Fingerprint status
# - Power monitor readings
```

Press Ctrl+C to exit monitor.

## Phase 5: System Testing

### 5.1 Hardware Tests

**DHT11 Sensor**:
```
Serial output shows:
"Temperature: 25.5°C | Humidity: 60%"
```

**MQ2/Fire Sensor**:
```
Serial shows analog values
- Clean air: ~100-200
- Smoke: >400
```

**AS608 Fingerprint**:
```
Serial shows:
"Fingerprint module ready"
Place finger for enrollment
```

**PZEM Power Monitor**:
```
Serial shows:
"Voltage: 230V | Current: 0.5A | Power: 115W"
```

**Button Test**:
```
Press physical button → Light toggles
Press button 5 times
Check Blynk app updates
```

**Relay/Light**:
```
GPIO32 controls relay
Relay clicks when toggled
Light turns on/off
```

### 5.2 Blynk App Tests

1. Open Blynk app
2. Select "Warehouse_ESP32" device
3. Check widgets:
   - Temperature gauge shows reading
   - Button toggles light
   - Sliders update
   - Events log shows activity

### 5.3 Google Sheets Tests

1. Open Google Sheet "Warehouse_Monitor"
2. Test by pressing button or simulating alert
3. Check sheet updates within 5 seconds
4. Verify timestamp and action recorded

## Phase 6: Fingerprint Enrollment

### 6.1 Enroll First Fingerprint

1. **Send enrollment command** via serial:
   ```
   Serial command: ENROLL:001
   (Or implement via button combination)
   ```

2. **Follow prompts**:
   - Place finger on sensor
   - Wait for beep
   - Lift finger
   - Repeat 3 times
   - Confirm enrollment

3. **Verify enrollment** with VERIFY command:
   - Place same finger
   - Should see "Match ID: 001"

### 6.2 Enroll Additional Users

- Repeat process for ID 002, 003, etc.
- Maximum 10 fingerprints (configurable)
- Store employee details in Google Sheet

## Phase 7: Production Deployment

### 7.1 Final Configuration

1. **Voltage levels**:
   - Verify 3.3V to sensors
   - Verify 5V to relay/buzzer
   - Check ground connections

2. **Thermal management**:
   - Add heat sink if needed
   - Ensure ventilation
   - Monitor temperature

3. **Backup power**:
   - Consider UPS for critical logging
   - EEPROM backup for important data

### 7.2 Monitoring & Maintenance

```
Daily checks:
- Serial monitor for errors
- Blynk app responsiveness
- Google Sheets logging

Weekly:
- Check sensor readings
- Verify power consumption baseline
- Test alert system

Monthly:
- Clean fingerprint sensor
- Verify all data logged correctly
- Update credentials if changed
```

## Troubleshooting Quick Reference

| Issue | Solution |
|-------|----------|
| **WiFi won't connect** | Check SSID/password in config.h |
| **Sensors not reading** | Verify pin definitions in config.h |
| **Blynk not syncing** | Check internet, restart app |
| **Fingerprint failing** | Clean sensor lens, re-enroll |
| **PZEM no data** | Check baud rate (9600), verify RX/TX |
| **Relay clicks but light off** | Check relay wiring to AC circuit |
| **Serial garbage output** | Verify baud rate 115200 |
| **Memory full error** | Reduce FINGERPRINT_MAX to 5 |

## Next Steps

1. ✅ Complete hardware wiring
2. ✅ Install PlatformIO IDE
3. ✅ Configure credentials
4. ✅ Build and upload firmware
5. ✅ Test all sensors
6. ✅ Enroll fingerprints
7. ✅ Monitor production system
8. 📝 Customize thresholds for your environment
9. 📝 Add additional sensors/features as needed

---

**Still stuck?** Check:
- Serial monitor output for error messages
- `platformio.ini` build flags
- Blynk device online status (blue dot)
- Google Script deployment status
