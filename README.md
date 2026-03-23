# Warehouse Monitoring System (ESP32)

A comprehensive IoT-based warehouse monitoring system with employee access control, environmental monitoring, and power consumption tracking.

## Features

### 🔐 Access Control
- **Fingerprint Recognition**: AS608 fingerprint sensor for employee authentication
- **Automatic Logging**: Employee entry/exit recorded to Google Sheets with timestamp
- **Employee Database**: Store up to 10 enrolled fingerprints (configurable)

### 📊 Environmental Monitoring
- **Temperature & Humidity**: DHT11 sensor with real-time monitoring
- **Smoke Detection**: MQ2 analog sensor for air quality
- **Fire Detection**: Dedicated fire sensor with automatic alerts
- **Temperature-based Alerts**: Triggers buzzer on high temperature

### ⚡ Power Monitoring
- **PZEM-004T v3**: Real-time power consumption measurement
  - Voltage (0-300V)
  - Current (0-100A)
  - Power (0-23000W)
  - Energy consumption (kWh)
  - Frequency (Hz)
  - Power factor

### 💡 Light Control
- **Relay Control**: ON/OFF control via physical button or mobile app
- **Remote Control**: Blynk app integration for wireless control
- **Power Tracking**: Monitor light bulb power consumption

### 📱 Mobile App & Cloud Integration
- **Blynk Integration**: Real-time sensor dashboard
  - Temperature graph
  - Humidity display
  - Smoke/fire alerts
  - Power consumption charts
  - Light control switch
- **Google Sheets Logging**: Automatic data backup
- **Push Notifications**: Alert notifications on mobile

### 🚨 Alert System
- **Buzzer Notifications**: Audio alerts for critical events
- **Multi-level Alerts**: Fire, smoke, high temperature detection
- **Event Logging**: All alerts logged to cloud

## Hardware Components

```
📦 Main Controller
└── ESP32 Development Board

🎯 Sensors & Inputs
├── AS608 Fingerprint Module (UART1)
├── DHT11 Temperature/Humidity (GPIO25)
├── MQ2 Smoke Sensor (ADC GPIO35)
├── Fire Detection Sensor (GPIO36)
└── Physical Button (GPIO34)

⚡ Power Management
├── PZEM-004T Power Monitor (UART2)
└── Relay Module (GPIO32)

🔊 Output Devices
├── Buzzer/Piezo Speaker (GPIO33)
└── 5V DC Power Supply

📡 Connectivity
└── WiFi (Built-in ESP32)
```

## Pin Mapping

| Component | Pin | Interface | Baudrate |
|-----------|-----|-----------|----------|
| DHT11 | GPIO25 | 1-Wire | - |
| MQ2 | GPIO35 | Analog ADC | - |
| Fire Sensor | GPIO36 | Analog ADC | - |
| Button | GPIO34 | Digital | - |
| Buzzer | GPIO33 | Digital PWM | - |
| Relay | GPIO32 | Digital | - |
| AS608 | TX:GPIO10, RX:GPIO9 | UART1 | 57600 |
| PZEM-004T | TX:GPIO17, RX:GPIO16 | UART2 | 9600 |

## Installation & Setup

### 1. PlatformIO Setup
```bash
# Install VS Code + PlatformIO extension
# Clone/open this project
cd /path/to/Monitor
```

### 2. Configure Credentials
Edit `include/config.h`:
```cpp
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"
#define GOOGLE_SCRIPT_ID "YOUR_GOOGLE_SCRIPT_ID"
```

### 3. Upload Firmware
```bash
# In PlatformIO terminal
pio run -t upload

# Or in VS Code: PlatformIO: Upload
```

### 4. Monitor Serial Output
```bash
pio device monitor --baud 115200
```

## Configuration Guide

### Blynk Setup
1. **Create Blynk Account**: https://blynk.cloud/
2. **Create Template**: 
   - Name: "Warehouse Monitor"
   - Board: ESP32 Dev Board
3. **Configure Virtual Pins**:
   - V0-V12: Mapped in source code
4. **Create Device** from template
5. **Copy Auth Token** to `config.h`

### Google Sheets Integration
1. **Create Google Apps Script**:
   - Go to https://script.google.com/
   - Create new project
   - Paste script from `docs/google_sheets_script.gs`
   - Deploy as web app (public access)
   - Copy script ID to `config.h`

2. **Create Google Sheet**:
   - Create new sheet for logging
   - Script will write to "AccessLog", "AlertLog", "DeviceStatus" sheets

### AS608 Fingerprint Setup
1. **Calibrate Sensor**:
   - Power on module
   - Place finger firmly for 3 seconds
   - Lift and repeat
2. **Enrollment**:
   - Send UART command via app/serial monitor
   - Place finger 3 times when prompted
3. **Verification**:
   - System auto-checks on device power-on

## Usage

### Employee Access Control
1. Employee places finger on AS608 sensor
2. System captures and verifies fingerprint
3. If match found:
   - ✓ Logs entry/exit to Google Sheets
   - ✓ Records timestamp
   - ✓ Buzzer sounds (success tone)
4. If no match:
   - ✗ Buzzer sounds (failure tone)
   - ✗ Alert logged

### Light Control - Physical Button
- **Short Press**: Toggle light ON/OFF
- **Long Press** (2s): Emergency shutdown (future)

### Light Control - Mobile App
- Open Blynk app
- Toggle "Light Control" switch (V1)
- See power consumption in real-time

### Environmental Monitoring
- **Temperature**: Displayed on app graph
- **Humidity**: Shows in dashboard
- **Smoke Level**: Analog value 0-1023
- **Fire**: Red alert if triggered
- All readings update every 5 seconds

### Power Monitoring
- **Live Metrics**:
  - Voltage (V)
  - Current (A)
  - Power (W)
  - Energy (kWh)
  - Frequency (Hz)
  - Power Factor
- **Graphs**: Energy consumption over time

## Blynk Virtual Pin Mapping

| V# | Function | Type | Description |
|----|----------|------|-------------|
| V0 | - | - | *Reserved* |
| V1 | Light Control | Button/Switch | ON/OFF light relay |
| V2 | Buzzer Test | Button | Test buzzer output |
| V3 | Temperature | Gauge/Chart | °C reading |
| V4 | Humidity | Gauge | % RH reading |
| V5 | Smoke Level | Gauge | ADC 0-1023 |
| V6 | Power | Gauge/Chart | Watts |
| V7 | Energy | Gauge | kWh consumed |
| V8 | Voltage | Gauge | Volts |
| V9 | Current | Gauge | Amps |
| V10 | Fire Alert | LED | Critical alert |
| V11 | Smoke Alert | LED | Warning alert |
| V12 | Light Status | LED | Light state indicator |

## Threshold Configuration

Edit `include/config.h`:

```cpp
#define SMOKE_THRESHOLD 400      // MQ2 analog value
#define TEMP_THRESHOLD 40        // °C
#define FIRE_THRESHOLD 500       // ADC value
#define HUMIDITY_WARNING 80      // % RH
```

## Troubleshooting

### AS608 Not Responding
- Check baud rate: 57600
- Verify RX/TX pins (GPIO9/GPIO10)
- Test with serial monitor at 57600 baud

### PZEM Not Reading
- Check baud rate: 9600
- Verify module slave ID (default 0xF8)
- Load test (turn on light) to see readings

### WiFi Connection Failed
- Check SSID/password in `config.h`
- Verify ESP32 antenna connection
- Check WiFi signal strength

### Blynk Not Syncing
- Verify auth token
- Check internet connection
- Restart app and device
- Re-paste token if needed

### Google Sheets Not Logging
- Test webhook in browser
- Check script deployment is active
- Verify sheet URL in config
- Check internet latency

## Future Enhancements

- [ ] Database for employee fingerprint storage
- [ ] RFID card alternative authentication
- [ ] Motion detection sensors
- [ ] UDP/MQTT protocols for faster updates
- [ ] Local web dashboard (mDNS)
- [ ] Multi-zone temperature monitoring
- [ ] Historical data analytics
- [ ] Scheduled automation (time-based actions)
- [ ] Email alerts for critical events
- [ ] Video integration with camera module

## API Reference

### Main Functions
```cpp
// Light control
controlLight(bool state);

// Sensor reading
readDHTSensors();
readMQ2Smoke();
readPZEMData();
checkFireAlert();

// Fingerprint
fingerprint.enrollNewFingerprint(uint8_t id);
fingerprint.verifyFingerprint(uint8_t& matchedId);

// Cloud logging
cloudLogger.logAccess(empId, empName, action, success);
cloudLogger.logAlert(type, value, severity);

// Power monitoring
powerMonitor.readAll(v, i, p, e);
```

## License

MIT License - See LICENSE file for details

## Support

For issues, questions, or feature requests, please open an issue in the repository.

## Author

Built with ESP32, PlatformIO, and Blynk

---

**Last Updated**: March 2026
**Firmware Version**: 1.0.0
