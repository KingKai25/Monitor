# 🚀 START HERE - Quick Start Guide

Welcome to your Warehouse Monitoring System! This document will get you started in the next 30 minutes.

## ⚠️ IntelliSense Errors Are Normal

You may see red squiggly lines in VS Code about missing includes like:
- `#include <BlynkSimpleEsp32.h>`
- `#include <DHT.h>`

**This is OK!** IntelliSense hasn't downloaded the libraries yet. PlatformIO will automatically install them when you compile.

---

## Step 1: Configure Credentials (5 minutes)

### Edit config.h with Your Information

Open file: `include/config.h`

Replace these 3 lines with YOUR actual values:

```cpp
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
#define GOOGLE_SCRIPT_ID "YOUR_GOOGLE_SCRIPT_ID"
```

### Where to Get These Values

**WiFi SSID/Password:**
- Your home/office WiFi network name and password

**Blynk Token & Template ID:**
- 📖 See detailed guide: [docs/GET_CREDENTIALS.md](docs/GET_CREDENTIALS.md) (English)
- 📖 Hướng dẫn tiếng Việt: [docs/GET_CREDENTIALS.md](docs/GET_CREDENTIALS.md) (Vietnamese)

**Google Script ID:**
- 📖 Full instructions in the same credential guide above

### 🔗 Quick Links for Getting Credentials

1. **Create Blynk Account** → https://blynk.cloud/ → Sign Up
2. **Create Google Apps Script** → https://script.google.com/ → New Project
3. **Read Full Guide** → See [docs/GET_CREDENTIALS.md](docs/GET_CREDENTIALS.md)

---

## Step 2: Connect Hardware (10 minutes)

### Updated Pin Mapping (According to Your Schematic)

```
ESP32 Pin  →  Component            Description
────────────────────────────────────────────────────
GPIO32     →  DHT11               Temperature/Humidity (D32)
GPIO25     →  MQ2 Sensor          Smoke detection (D25)
GPIO33     →  Fire Sensor         Heat detection (D33)
GPIO26     →  Button              Physical switch (D26)
GPIO23     →  Relay               Light control (D23)
GPIO4      →  Buzzer              Alert sound (D4)

UART Serial Connections:
GPIO18     →  PZEM TX             Power monitor (D18 on Serial1)
GPIO19     →  PZEM RX             Power monitor (D19 on Serial1)
GPIO17     →  AS608 TX            Fingerprint (TX2 on Serial2)
GPIO16     →  AS608 RX            Fingerprint (RX2 on Serial2)

Power Connections:
3.3V       →  DHT11 VCC, AS608 VCC
5V         →  PZEM VCC, MQ2 VCC, Fire Sensor VCC, Relay VCC
GND        →  All Ground pins
```

**📋 Full Pin Diagram:**
See detailed diagrams in: [docs/PIN_MAPPING_VI.md](docs/PIN_MAPPING_VI.md)

**✅ Check these before proceeding:**
- [ ] All ground connections (GND) are soldered
- [ ] 3.3V and 5V are not mixed up
- [ ] UART wires are crossed (TX→RX, RX→TX)
- [ ] Relay NO goes to light, COM to phase

---

## Step 3: Upload Firmware (10 minutes)

### In VS Code:

1. **Open Terminal**: Press `Ctrl+`` (backtick)

2. **Compile Firmware**:
```bash
pio run
```

Wait for completion. You should see:
```
Building...
✓ Project Compiled Successfully
```

3. **Connect ESP32** to computer via USB cable

4. **Upload**:
```bash
pio run -t upload
```

You should see:
```
Uploading...
Hard resetting via RTS pin...
✓ Upload Complete
```

5. **Monitor Serial Output**:
```bash
pio device monitor --baud 115200
```

You'll see boot messages:

```
========== WAREHOUSE MONITOR STARTING ==========
[WiFi] Connecting to YOUR_SSID...
[WiFi] Connected! IP: 192.168.1.100
[DHT] DHT11 initialized
[PZEM] PZEM-004T initialized
[Fingerprint] AS608 initialized
Setup complete!
```

---

## Step 4: Configure Blynk App (5 minutes)

### On Your Phone:

1. **Download Blynk App**
   - iOS: App Store
   - Android: Google Play

2. **Log In** with email you used at blynk.cloud

3. **Select Your Device**
   - Should show "Warehouse_ESP32_Main"
   - Status should be "Online" (blue dot)

4. **Create Dashboard**
   - Click "+" button
   - Add widgets:
     - Button for V1 (Light control)
     - Gauge for V3 (Temperature)
     - Gauge for V4 (Humidity)
     - Chart for V6 (Power)
     - LED for V10 (Fire alert)

📱 See [docs/BLYNK_SETUP.md](docs/BLYNK_SETUP.md) for detailed widget setup

---

## Step 5: Test Everything (5 minutes)

### Quick Test Sequence:

**1. Check Temperature Reading**
```
Serial Monitor → "Temperature: 23.5°C | Humidity: 55%"
Blynk App      → Gauge shows ~23.5°C
```

**2. Test Button Press**
```
Push Button    → Relay clicks
Light Turns    → ON (verify physically)
Blynk App      → V1 switch shows ON
```

**3. Test Smoke Sensor**
```
Wave hand over MQ2 sensor
Serial shows   → "Smoke Level: 600"
Should trigger → Alert if > 400
```

**4. Check Google Sheets**
```
Press button or trigger alert
Open Google Sheet "Warehouse_Monitor"
AccessLog tab should show new entry
```

---

## 🎯 What You've Just Built

```
┌─────────────────────────────────────────┐
│      WAREHOUSE MONITORING SYSTEM         │
├─────────────────────────────────────────┤
│                                         │
│  📱 Blynk Mobile App (Live Dashboard)   │
│      ├─ Light ON/OFF control           │
│      ├─ Temperature graph               │
│      ├─ Power consumption chart         │
│      └─ Fire/Smoke alerts              │
│                                         │
│  🌩️ WiFi Cloud Connectivity             │
│      ├─ Real-time syncing              │
│      ├─ Push notifications             │
│      └─ Global access                  │
│                                         │
│  📊 Google Sheets Logging               │
│      ├─ Access log (employee tracking) │
│      ├─ Alert history                  │
│      └─ Device status                  │
│                                         │
│  🔐 Fingerprint Access Control          │
│      ├─ AS608 sensor (GPIO9/10)        │
│      ├─ 81 employee capacity            │
│      └─ Automatic timestamping         │
│                                         │
│  ⚡ Power Monitoring                     │
│      ├─ PZEM-004T (GPIO16/17)          │
│      ├─ Real-time kWh tracking         │
│      └─ Cost calculations              │
│                                         │
│  🌡️ Environmental Sensors               │
│      ├─ DHT11: Temp/Humidity           │
│      ├─ MQ2: Smoke detection           │
│      └─ Fire sensor: Heat alert        │
│                                         │
└─────────────────────────────────────────┘
```

---

## 📚 Next Steps & Resources

### Immediate Priorities
1. [ ] Finish hardware wiring
2. [ ] Successfully upload firmware
3. [ ] Verify serial monitor shows good output
4. [ ] Test light toggle
5. [ ] Confirm Blynk shows temperature

### Then Configure
1. [ ] Enroll first fingerprint employee
2. [ ] Verify Google Sheets logging
3. [ ] Calibrate sensor thresholds
4. [ ] Set up employee database

### Reference Guides
- Full Setup: [docs/SETUP_GUIDE.md](docs/SETUP_GUIDE.md)
- Blynk App: [docs/BLYNK_SETUP.md](docs/BLYNK_SETUP.md)
- Fingerprint: [docs/AS608_GUIDE.md](docs/AS608_GUIDE.md)
- Power Monitor: [docs/PZEM_GUIDE.md](docs/PZEM_GUIDE.md)
- Cloud Script: [docs/google_sheets_script.gs](docs/google_sheets_script.gs)

---

## ❌ Troubleshooting Quick Fixes

### "Cannot upload firmware"
**Solution**: 
1. Try different USB port
2. Install CH340 drivers: https://sparks.gogo.co.nz/ch340.html
3. Check Device Manager for ESP32 COM port

### "WiFi won't connect"
**Solution**:
1. Check SSID/password in config.h (case sensitive!)
2. Verify WiFi is 2.4GHz (some networks are 5GHz only)
3. Check router allows this device

### "Temperature shows 0 or wrong value"
**Solution**:
1. Check DHT11 wiring (GPIO25)
2. Verify 10kΩ pull-up resistor on data line
3. Make sure sensor has 3.3V power

### "Blynk device shows Offline"
**Solution**:
1. Verify auth token in config.h matches Blynk app
2. Check WiFi connection in serial monitor
3. Restart Blynk app

### "Google Sheets not logging"
**Solution**:
1. Verify script is deployed (not just saved)
2. Check webhook URL has right script ID
3. Try test function in Google Script editor

---

## 🔐 Security Reminder

Before any public deployment:
1. ⚠️ Change PZEM default slave ID (currently 0xF8)
2. ⚠️ Use strong WiFi password
3. ⚠️ Don't share auth tokens
4. ⚠️ Backup Google Sheets regularly
5. ⚠️ Monitor access logs for unauthorized use

---

## 💡 Pro Tips

**Tip 1: Monitor Serial Output**
```bash
# Watch live sensor readings while testing
pio device monitor --baud 115200
```

**Tip 2: Test Sensors Individually**
- Only enroll one fingerprint first
- Use known load for power testing
- Place hand near sensors to test

**Tip 3: Adjust Thresholds**
Edit in `include/config.h`:
```cpp
#define SMOKE_THRESHOLD 400    // Lower = more sensitive
#define TEMP_THRESHOLD 40      // Adjust to your environment
```

**Tip 4: Use Blynk Web Console**
Too busy for mobile? Monitor at:
https://blynk.cloud/ (same device, different interface)

---

## ✅ Success Checklist

You've succeeded when:
- ✅ Firmware compiles without errors
- ✅ Serial monitor shows initialization messages
- ✅ Blynk device appears "Online"
- ✅ Button press toggles light
- ✅ Temperature appears in Blynk app
- ✅ Pressing button logs to Google Sheets
- ✅ Fingerprint enrollment works

---

## 🎉 Congratulations!

You've successfully built a professional-grade IoT warehouse monitoring system with:

✓ **Access Control** - Fingerprint-based employee tracking  
✓ **Cloud Monitoring** - Real-time dashboards  
✓ **Power Analytics** - Detailed energy consumption  
✓ **Environmental Alerts** - Fire, smoke, temperature  
✓ **Data Logging** - Automatic Google Sheets records  
✓ **Mobile Control** - Blynk app integration  

---

## 📞 Need More Help?

**Problem** | **See**
-----------|--------
Setup issues | [SETUP_GUIDE.md](docs/SETUP_GUIDE.md)
Blynk not working | [BLYNK_SETUP.md](docs/BLYNK_SETUP.md)
Fingerprint errors | [AS608_GUIDE.md](docs/AS608_GUIDE.md)
Power monitor issues | [PZEM_GUIDE.md](docs/PZEM_GUIDE.md)
Overall overview | [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md)

---

**Ready to Begin?** 

👉 Start with config.h and follow Step 1 above.

Happy building! 🚀

---

**Questions?** Every guide has a troubleshooting section. Check docs/ folder for specialized guides.

**Last Updated**: March 2026  
**Status**: Ready for Deployment ✅
