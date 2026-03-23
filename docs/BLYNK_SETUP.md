# Blynk App Configuration Guide

Complete guide to setting up the Blynk mobile app dashboard for warehouse monitoring.

## 1. Blynk Account Setup

### 1.1 Create Account
1. Download Blynk app (iOS/Android)
   - iOS: App Store
   - Android: Google Play Store
2. Or go to https://blynk.cloud in browser
3. Sign up with email address
4. Verify email

### 1.2 Create Organization (Optional)
1. Log in to https://blynk.cloud
2. Workspaces → Create new organization
3. Name: "Warehouse Monitoring"
4. Skip payment setup (free tier available)

## 2. Template Configuration

### 2.1 Create Template in Web Console
1. Go to https://blynk.cloud
2. Developer Zone → Templates
3. Click "+ Create Template"

**Template Settings:**
```
Template Name: Warehouse Monitor
Description: ESP32-based warehouse monitoring system
Hardware: ESP32 Dev Board
Connectivity: WiFi
```

4. Save template
5. **Copy and save:**
   - TEMPLATE_ID (paste into config.h)
   - TEMPLATE_NAME

### 2.2 Configure Data Streams
Data streams define how data flows between device and app.

1. In template editor, click "Datastreams"
2. Create following datastreams:

| Virtual Pin | Name | Type | Min | Max | Unit |
|-------------|------|------|-----|-----|------|
| V0 | - | - | - | - | - |
| V1 | Light Control | Switch | 0 | 1 | - |
| V2 | Buzzer Test | Button | 0 | 1 | - |
| V3 | Temperature | Sensor | 10 | 50 | °C |
| V4 | Humidity | Sensor | 0 | 100 | % |
| V5 | Smoke Level | Sensor | 0 | 1023 | ADC |
| V6 | Power | Sensor | 0 | 5000 | W |
| V7 | Energy | Sensor | 0 | 9999 | kWh |
| V8 | Voltage | Sensor | 0 | 300 | V |
| V9 | Current | Sensor | 0 | 100 | A |
| V10 | Fire Alert | Indicator | 0 | 1 | - |
| V11 | Smoke Alert | Indicator | 0 | 1 | - |
| V12 | Light Status | Indicator | 0 | 1 | - |

3. Save each datastream

## 3. Device Creation

### 3.1 Add Device
1. In Blynk.Cloud web console
2. Devices → "Create Device"
3. **Device Settings:**
   ```
   Template: Warehouse Monitor
   Device Name: Warehouse_ESP32_Main
   ```
4. Click "Create Device"

### 3.2 Get Authentication Token
1. Device card appears in list
2. Click the device
3. Click "Device info"
4. Copy **Auth Token** (long string)
5. Paste into `config.h`:
   ```cpp
   #define BLYNK_AUTH_TOKEN "your_token_here"
   ```

## 4. Mobile App Dashboard Setup

### 4.1 Login to Mobile App
1. Open Blynk app
2. Log in with your email/password
3. Workspaces → Select your workspace
4. Devices → Select "Warehouse_ESP32_Main"

### 4.2 Create Dashboard

#### Widget Types Available:
- **Button**: Digital ON/OFF control
- **Slider**: Analog value 0-255
- **Switch**: Toggle control
- **Gauge**: Display analog value with needle
- **Value Display**: Show numeric/text value
- **Chart**: Time-series graph
- **LED**: Visual indicator (on/off)
- **Input**: Text input field
- **Timer**: Schedule actions
- **Map**: GPS location (optional)

### 4.3 Add Widgets for Light Control

**Widget 1: Light Switch**
1. Click "+" to add widget
2. Select "Switch"
3. **Settings:**
   - Name: "Light Control"
   - Virtual Pin: V1
   - Send ON value: 1
   - Send OFF value: 0
4. Save

**Widget 2: Light Status Indicator**
1. Click "+" to add widget
2. Select "LED"
3. **Settings:**
   - Name: "Light Status"
   - Virtual Pin: V12
   - Color: Yellow
4. Save

**Widget 3: Buzzer Test Button**
1. Click "+" to add widget
2. Select "Button"
3. **Settings:**
   - Name: "Test Buzzer"
   - Virtual Pin: V2
   - Send ON only: Checked
4. Save

### 4.4 Add Widgets for Temperature Monitoring

**Widget 4: Temperature Gauge**
1. Click "+" to add widget
2. Select "Gauge"
3. **Settings:**
   - Name: "Temperature"
   - Virtual Pin: V3
   - Min: 10
   - Max: 50
   - Unit: °C
   - Color: Blue
4. Save

**Widget 5: Temperature Chart**
1. Click "+" to add widget
2. Select "Chart"
3. **Settings:**
   - Name: "Temperature History"
   - Virtual Pin: V3
   - Unit: °C
   - Time span: 24 hours (or custom)
4. Save

### 4.5 Add Widgets for Humidity

**Widget 6: Humidity Gauge**
1. Click "+" to add widget
2. Select "Gauge"
3. **Settings:**
   - Name: "Humidity"
   - Virtual Pin: V4
   - Min: 0
   - Max: 100
   - Unit: %
   - Color: Cyan
4. Save

### 4.6 Add Widgets for Smoke Detection

**Widget 7: Smoke Level Gauge**
1. Click "+" to add widget
2. Select "Gauge"
3. **Settings:**
   - Name: "Smoke Level"
   - Virtual Pin: V5
   - Min: 0
   - Max: 1023
   - Color: Orange
4. Save

**Widget 8: Smoke Alert Indicator**
1. Click "+" to add widget
2. Select "LED"
3. **Settings:**
   - Name: "Smoke Alert"
   - Virtual Pin: V11
   - Color: Orange (alert) / Green (safe)
   - On Text: "ALERT"
   - Off Text: "SAFE"
4. Save

### 4.7 Add Widgets for Power Monitoring

**Widget 9: Power Consumption Gauge**
1. Click "+" to add widget
2. Select "Gauge"
3. **Settings:**
   - Name: "Power (W)"
   - Virtual Pin: V6
   - Min: 0
   - Max: 5000
   - Unit: W
   - Color: Red
4. Save

**Widget 10: Power Chart**
1. Click "+" to add widget
2. Select "Chart"
3. **Settings:**
   - Name: "Power History"
   - Virtual Pin: V6
   - Unit: W
   - Time span: 24 hours
4. Save

**Widget 11: Energy Counter**
1. Click "+" to add widget
2. Select "Value Display"
3. **Settings:**
   - Name: "Energy Used"
   - Virtual Pin: V7
   - Unit: kWh
4. Save

**Widget 12: Voltage Display**
1. Click "+" to add widget
2. Select "Gauge"
3. **Settings:**
   - Name: "Voltage"
   - Virtual Pin: V8
   - Min: 0
   - Max: 300
   - Unit: V
   - Color: Green
4. Save

**Widget 13: Current Display**
1. Click "+" to add widget
2. Select "Gauge"
3. **Settings:**
   - Name: "Current"
   - Virtual Pin: V9
   - Min: 0
   - Max: 100
   - Unit: A
   - Color: Purple
4. Save

### 4.8 Add Fire Alert Widget

**Widget 14: Fire Alert LED**
1. Click "+" to add widget
2. Select "LED" (large preferred)
3. **Settings:**
   - Name: "FIRE ALERT"
   - Virtual Pin: V10
   - Color: Red (alert) / Green (safe)
   - On Text: "🔥 FIRE DETECTED!"
   - Off Text: "✓ Safe"
   - Font size: LARGE
4. Save

## 5. Advanced App Features

### 5.1 Events & Notifications
1. Go to device settings
2. Enable "Device notifications"
3. Push notifications will alert you of:
   - Temperature > 40°C
   - Smoke > 400 ADC
   - Fire detected
   - Light toggled

### 5.2 Event Logging
1. Device → View Events
2. Logs all state changes with timestamps:
   - Light on/off
   - Alert triggers
   - Connection status

### 5.3 Device Timeline
1. Device → Timeline tab
2. Shows history of all events
3. Can be exported

### 5.4 Automations (Premium)
Create automatic responses:
- Temperature > 40°C → Send notification + turn on fan
- Smoke detected → Log event + sound alarm
- Light on > 1 hour → Send reminder

## 6. Dashboard Layout Tips

### 6.1 Best Practices
1. **Top Section**: Critical alerts (Fire, Smoke)
2. **Middle Section**: Controls (Light switch, buttons)
3. **Lower Section**: Monitoring gauges
4. **Bottom Section**: History charts

### 6.2 Responsive Design
- Widgets auto-arrange for phone/tablet
- Drag to reorder widgets
- Resize widgets by dragging corners
- Set large widgets for important info

### 6.3 Color Coding
- 🔴 Red: Critical alerts (Fire)
- 🟠 Orange: Warnings (Smoke, High temp)
- 🟡 Yellow: Caution (High power)
- 🟢 Green: Safe/Normal
- 🔵 Blue: Information (Temperature)
- 🟣 Purple: Power metrics (Current)

## 7. Mobile App Interface

### 7.1 Main Navigation
```
Bottom Menu:
├─ Devices (Device list)
├─ Events (Activity log)
├─ Automations (Scheduled actions)
├─ Explorer (Data browser)
└─ Settings (Account/app settings)
```

### 7.2 Device Screen
```
Header:
├─ Device name: Warehouse_ESP32_Main
├─ Status: Online/Offline 
└─ Signal strength: WiFi RSSI

Widgets:
├─ Control widgets (Switches, buttons)
├─ Indicator widgets (LEDs, values)
├─ Monitoring widgets (Gauges, charts)
└─ Alert widgets (Fire, Smoke)
```

## 8. Testing the Setup

### 8.1 Test Connection
1. Upload firmware to ESP32
2. Monitor serial output for WiFi connection
3. Check Blynk app - device appears as "Online" (blue dot)

### 8.2 Test Widgets
1. **Light Control**: Toggle switch → Relay clicks
2. **Buzzer Test**: Click button → Buzzer sounds
3. **Temperature**: Check gauge updates from sensor
4. **Smoke Alert**: Test by simulating smoke input
5. **Fire Alert**: Test fire sensor trigger

### 8.3 Test Notifications
1. Enable push notifications in Blynk
2. Trigger an alert condition
3. Phone receives notification within 2-3 seconds

## 9. Troubleshooting

| Issue | Solution |
|-------|----------|
| **Device shows "Offline"** | Check WiFi connection, restart app |
| **Widgets not updating** | Check datastream virtual pins match code |
| **No notifications** | Enable notifications in device settings |
| **Switch state not syncing** | Check Blynk.virtualWrite in firmware |
| **Chart not showing data** | May take 5+ minutes to populate |

## 10. Advanced Customization

### 10.1 Custom Colors/Themes
1. Go to device → Theme settings
2. Choose dark/light mode
3. Customize widget colors

### 10.2 Multiple Users
1. Device → Sharing
2. Invite other users by email
3. Set permission levels:
   - Editor (full control)
   - Viewer (read-only)
   - Operator (control only)

### 10.3 Multiple Devices
1. Create more devices from template
2. Invite each with different auth token
3. Switch between devices in app

## 11. Blynk Web Console Features

Access at https://blynk.cloud for:
- Device management
- Template editing
- Data export (CSV)
- Usage analytics
- Team management
- Billing (if premium)

## Summary

**Key Files Updated:**
- ✅ config.h: Added BLYNK_AUTH_TOKEN and BLYNK_TEMPLATE_ID
- ✅ main.cpp: Handles all virtual pin writes
- ✅ cloud_logger.cpp: Blynk event logging

**Next Steps:**
1. Create Blynk account
2. Create template and device
3. Copy auth token to config.h
4. Upload firmware
5. Configure dashboard widgets
6. Test all functionality

**More Information:**
- Blynk Documentation: https://docs.blynk.io/
- Blynk Community: https://community.blynk.cc/
- Video Tutorials: https://www.youtube.com/c/BlynkIoT
