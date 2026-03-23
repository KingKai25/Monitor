# Warehouse Monitoring System - Documentation Index

Complete documentation for the ESP32-based warehouse monitoring and access control system.

## Quick Start Links

- **🚀 Getting Started**: [SETUP_GUIDE.md](SETUP_GUIDE.md) - Complete hardware & software setup
- **📱 Mobile App**: [BLYNK_SETUP.md](BLYNK_SETUP.md) - Blynk app configuration
- **🔐 Fingerprint**: [AS608_GUIDE.md](AS608_GUIDE.md) - Employee access control
- **⚡ Power Monitor**: [PZEM_GUIDE.md](PZEM_GUIDE.md) - Power consumption tracking
- **☁️ Cloud Integration**: [google_sheets_script.gs](google_sheets_script.gs) - Google Sheets logging

## Documentation Structure

### 1. Setup & Installation
- **Target**: First-time users, hardware integration
- **Content**: [SETUP_GUIDE.md](SETUP_GUIDE.md)
  - Phase 1: Hardware wiring (pins, connections)
  - Phase 2: Software installation (PlatformIO, dependencies)
  - Phase 3: Credentials configuration (WiFi, Blynk, Google)
  - Phase 4: Firmware compilation & upload
  - Phase 5: System testing
  - Phase 6: Fingerprint enrollment
  - Phase 7: Production deployment

### 2. Getting Credentials (NEW!)
- **Target**: Users needing Blynk token & Google Script ID
- **Content**: [GET_CREDENTIALS.md](GET_CREDENTIALS.md)
  - Step-by-step Blynk account & token setup
  - Google Apps Script deployment
  - Google Sheets configuration
  - Testing connection verification
  - **Available in both English and Vietnamese**

### 3. Pin Mapping & Wiring (UPDATED!)
- **Target**: Hardware integration reference
- **Content**: [PIN_MAPPING_VI.md](PIN_MAPPING_VI.md)
  - Updated GPIO assignments per your schematic
  - Visual connection diagram
  - Checklist for all connections
  - UART Serial port configuration
  - Troubleshooting wiring issues
  - AS608 protocol overview
  - Pin configuration and UART
  - Enrollment process (step-by-step)
  - Verification & identification
  - Employee database integration
  - Error handling & troubleshooting
  - Diagnostic testing

### 4. Power Monitoring
- **Target**: Developers, maintenance engineers
- **Content**: [PZEM_GUIDE.md](PZEM_GUIDE.md)
  - PZEM-004T v3 overview
  - Modbus RTU protocol details
  - Register mapping and data extraction
  - CRC calculation
  - Real-world examples
  - CT transformer configuration
  - Calibration & testing

### 5. Cloud Integration
- **Target**: System administrators, developers
- **Content**: [google_sheets_script.gs](google_sheets_script.gs)
  - Google Apps Script functions
  - Webhook handling
  - Data logging to Google Sheets
  - Email notifications
  - Access log management
  - Alert logging

### 6. Main Project README
- **Target**: Overview and quick reference
- **Content**: [../README.md](../README.md)
  - Project features
  - Hardware components
  - Pin mapping
  - Installation overview
  - Blynk virtual pin reference
  - API reference

## Feature Documentation

### Access Control
```
Employee Management:
├─ Fingerprint enrollment (AS608)
├─ Attendance tracking (Google Sheets)
├─ Check-in/Out logging with timestamp
├─ Multiple user support (up to 81+ employees)
└─ Physical button + mobile app control
```

See: [AS608_GUIDE.md](AS608_GUIDE.md) - Complete protocol & implementation

### Environmental Monitoring
```
Sensor Monitoring:
├─ Temperature/Humidity (DHT11)
├─ Smoke Detection (MQ2)
├─ Fire Alerting
├─ Configurable thresholds
└─ Push notifications
```

See: [SETUP_GUIDE.md](SETUP_GUIDE.md) - Phase 5 Testing

### Power Consumption
```
Energy Tracking:
├─ Real-time power measurement (PZEM-004T)
├─ Voltage, current, frequency monitoring
├─ Energy consumption in kWh
├─ Power factor calculations
└─ Historical graphs
```

See: [PZEM_GUIDE.md](PZEM_GUIDE.md) - Complete Modbus protocol

### Mobile Application
```
Blynk Dashboard:
├─ Real-time sensor graphs
├─ Light ON/OFF control
├─ Alert notifications
├─ Historical data charts
└─ Remote monitoring
```

See: [BLYNK_SETUP.md](BLYNK_SETUP.md) - App configuration

##  File Organization

```
Monitor/
├── docs/
│   ├── README.md                    ← You are here
│   ├── SETUP_GUIDE.md               ← Hardware & software setup
│   ├── BLYNK_SETUP.md               ← Mobile app configuration
│   ├── AS608_GUIDE.md               ← Fingerprint sensor details
│   ├── PZEM_GUIDE.md                ← Power monitor details
│   └── google_sheets_script.gs      ← Cloud logging script
│
├── src/
│   ├── main.cpp                     ← Main firmware
│   ├── fingerprint_sensor.cpp       ← AS608 implementation
│   ├── power_monitor.cpp            ← PZEM-004T implementation
│   └── cloud_logger.cpp             ← Google Sheets logging
│
├── include/
│   ├── config.h                     ← Pin definitions & config
│   ├── fingerprint_sensor.h         ← AS608 header
│   ├── power_monitor.h              ← PZEM-004T header
│   └── cloud_logger.h               ← Google Sheets header
│
├── platformio.ini                   ← Build configuration
├── README.md                        ← Project overview
└── docs/ (this folder)             ← Documentation
```

## Common Tasks

### Task: Get Blynk Token and Google Script ID
See: [GET_CREDENTIALS.md](GET_CREDENTIALS.md) - Complete step-by-step guide (Vietnamese & English)

### Task: Check Pin Assignments
See: [PIN_MAPPING_VI.md](PIN_MAPPING_VI.md) - Updated schematic pin mapping

### Task: Enroll a new employee
See: [AS608_GUIDE.md](AS608_GUIDE.md) → Enrollment Process

### Task: Configure Blynk app for monitoring
See: [BLYNK_SETUP.md](BLYNK_SETUP.md) → Mobile App Dashboard Setup

### Task: Check power consumption of light
See: [PZEM_GUIDE.md](PZEM_GUIDE.md) → Real-World Example

### Task: Set up Google Sheets logging
See: [SETUP_GUIDE.md](SETUP_GUIDE.md) → Phase 3.3 & google_sheets_script.gs

### Task: Adjust sensor thresholds
See: [SETUP_GUIDE.md](SETUP_GUIDE.md) → Phase 3.4

### Task: Troubleshoot fingerprint sensor
See: [AS608_GUIDE.md](AS608_GUIDE.md) → Troubleshooting

### Task: Understand power readings
See: [PZEM_GUIDE.md](PZEM_GUIDE.md) → Protocol & Examples

## Technology Stack

```
Hardware:
├─ ESP32 Dev Board (Main controller)
├─ AS608 (Fingerprint sensor)
├─ PZEM-004T v3 (Power monitor)
├─ DHT11 (Temperature/Humidity)
├─ MQ2 (Smoke detector)
└─ Relay + Buzzer (Outputs)

Software:
├─ PlatformIO IDE (Development)
├─ Arduino Framework (Code)
├─ Blynk (Mobile app & cloud)
├─ Google Sheets (Data logging)
└─ Serial UART (Device communication)

Protocols:
├─ WiFi (ESP32 connectivity)
├─ Modbus RTU (PZEM communication)
├─ UART Serial (AS608 & PZEM)
├─ DHT1-Wire (DHT11)
└─ Analog ADC (MQ2, Fire sensor)
```

## Getting Help

### Issue: Firmware won't compile
1. Check [SETUP_GUIDE.md](SETUP_GUIDE.md) Phase 4
2. Verify all libraries installed
3. Check platformio.ini

### Issue: Device not connecting to WiFi
1. See [SETUP_GUIDE.md](SETUP_GUIDE.md) Phase 3.1
2. Verify credentials in config.h
3. Check WiFi signal strength

### Issue: Fingerprint sensor not working
1. See [AS608_GUIDE.md](AS608_GUIDE.md) Troubleshooting
2. Verify baud rate: 57600
3. Check RX/TX pins (GPIO9/10)

### Issue: Blynk app not updating
1. See [BLYNK_SETUP.md](BLYNK_SETUP.md)
2. Verify auth token in config.h
3. Check Blynk server status

### Issue: Power readings incorrect
1. See [PZEM_GUIDE.md](PZEM_GUIDE.md) Troubleshooting
2. Verify CT ratio configuration
3. Check baud rate: 9600

## Document Status & Version

| Document | Version | Status | Last Updated |
|----------|---------|--------|--------------|
| README.md | 1.0.0 | ✅ Complete | Mar 2026 |
| SETUP_GUIDE.md | 1.0.0 | ✅ Complete | Mar 2026 |
| BLYNK_SETUP.md | 1.0.0 | ✅ Complete | Mar 2026 |
| AS608_GUIDE.md | 1.0.0 | ✅ Complete | Mar 2026 |
| PZEM_GUIDE.md | 1.0.0 | ✅ Complete | Mar 2026 |
| google_sheets_script.gs | 1.0.0 | ✅ Complete | Mar 2026 |
| docs/README.md | 1.0.0 | ✅ Complete | Mar 2026 |

## Roadmap & Future Documentation

- [ ] Advanced automation guide (scheduled actions, interlocking)
- [ ] Security hardening guide (encrypted communication, access control)
- [ ] Maintenance manual (sensor cleaning, calibration)
- [ ] Video tutorials (enrollment, app setup)
- [ ] API reference (Cloud Logger, Power Monitor classes)
- [ ] Performance optimization guide
- [ ] Data analysis and reporting
- [ ] Multi-device management

## Contributing

Found an issue or want to improve documentation?
1. Note the section and specific problem
2. Document the fix/improvement
3. Submit for review

## License

All documentation is provided under MIT License. Feel free to use, modify, and distribute.

---

**Warehouse Monitor System**  
Comprehensive IoT Solution for Access Control, Environmental Monitoring, and Power Management  
**Version**: 1.0.0  
**Last Updated**: March 2026

For the main project overview, see [../README.md](../README.md)
