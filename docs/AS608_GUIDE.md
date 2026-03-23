# AS608 Fingerprint Sensor Implementation Guide

Complete guide for AS608 fingerprint authentication in the warehouse monitoring system.

## Overview

**AS608 Optical Fingerprint Module**
- Baud Rate: 57600 bps
- Communication: Serial UART
- Mode: 1:1 verification and 1:N identification
- Storage Capacity: 163 fingerprints (configurable by library)
- Template Size: 256 bytes per fingerprint

## Pin Configuration

```
AS608 Pin    | Signal      | ESP32 Pin
-------------|-------------|----------
1 (VCC)      | 3.3V Power  | 3.3V
2 (GND)      | Ground      | GND
3 (RX)       | Serial RX   | GPIO9
4 (TX)       | Serial TX   | GPIO10
(Shield)     | Ground      | GND
```

**UART1 Configuration** (in firmware)
```cpp
#define FINGERPRINT_RX 9
#define FINGERPRINT_TX 10
SerialFinger.begin(57600, SERIAL_8N1, FINGERPRINT_RX, FINGERPRINT_TX);
```

## Protocol Structure

### 1. Command Frame Format

```
Header (2 bytes) → 0xEF 0x01
Address (4 bytes) → Device address (default: 0xFFFFFFFF)
Package Identifier (1 byte) → Command/Response type
Data Length (2 bytes) → Length of data + checksum
Data (Variable) → Command parameters
Checksum (2 bytes) → Verification
```

### 2. Package Types

```
0x01 = Command package
0x02 = Data package 
0x07 = Ack package (response)
0x08 = Error package
```

### 3. Command Codes

| Code | Function | Parameters |
|------|----------|-----------|
| 0x01 | System Info | None |
| 0x20 | Image Capture | None |
| 0x21 | Image to Buffer | Buffer ID (1-2) |
| 0x22 | Image Download | Buffer ID |
| 0x23 | Image Upload | Buffer ID |
| 0x30 | Match 1:1 | Template ID |
| 0x32 | Match 1:N | Start ID, Match Count |
| 0x40 | Register Template | Buffer IDs (1,2) |
| 0x41 | Store Template | Model ID (0-162) |
| 0x42 | Delete Template | Model ID |
| 0x43 | Clear All | None |
| 0x44 | Template Count | None |
| 0x45 | Read Template | Model ID |
| 0x46 | Load Template | Model ID, Buffer ID |

## Enrollment Process

### Step-by-Step Enrollment

```
User Input: "ENROLL 001"
│
├─ Check if ID 001 is empty
│
├─ Prompt: "Place finger"
│  └─ Capture image 1
│     └─ Store in buffer 1
│
├─ Prompt: "Lift finger"
│  └─ Wait 1-2 seconds
│
├─ Prompt: "Place same finger again"
│  └─ Capture image 2
│     └─ Store in buffer 2
│
├─ Compare features
│  └─ If match score > threshold
│     ├─ Create template
│     ├─ Store in model ID 001
│     └─ Success!
│     
└─ Result: "Fingerprint enrolled - ID 001"
```

### Implementation Code

```cpp
bool enrollFingerprints(uint8_t id) {
  Serial.print("Enrolling ID: ");
  Serial.println(id);
  
  // Step 1: Check if ID exists
  uint16_t templateCount = getTemplateCount();
  if (id < 1 || id > templateCount + 10) {
    Serial.println("Invalid ID");
    return false;
  }
  
  // Step 2: First image capture
  Serial.println(">> Place finger on sensor");
  if (!captureImage()) {
    return false; // Capture failed
  }
  
  if (!convertImage(0x01)) {  // Store in buffer 1
    return false;
  }
  
  // Step 3: Wait and second capture
  delay(2000);
  Serial.println(">> Place same finger again");
  
  if (!captureImage()) {
    return false;
  }
  
  if (!convertImage(0x02)) {  // Store in buffer 2
    return false;
  }
  
  // Step 4: Create and store template
  if (!registerTemplate()) {
    return false; // Feature mismatch
  }
  
  if (!storeTemplate(id)) {
    return false;
  }
  
  Serial.println("✓ Fingerprint enrolled successfully!");
  return true;
}
```

## Verification Process

### 1:1 Verification (Specific ID)

```
User places finger
│
├─ Capture image
├─ Convert to features
└─ Compare with stored template ID
    ├─ Score > Threshold? 
    │  └─ YES: Match!
    └─ NO: No match
```

### 1:N Identification (Search all)

```
User places finger
│
├─ Capture image
├─ Convert to features
└─ Compare with ALL stored templates (0-162)
    ├─ Found in buffer 0-162?
    │  └─ YES: Match! Return ID
    └─ NO: Not in database
```

### Implementation Code

```cpp
bool verifyFingerprint(uint8_t targetId, uint16_t& score) {
  Serial.println("Place finger on sensor");
  
  // Capture new image
  if (!captureImage()) {
    Serial.println("Capture failed");
    return false;
  }
  
  // Convert to features (buffer 1)
  if (!convertImage(0x01)) {
    return false;
  }
  
  // Load stored template (targetId)
  if (!loadTemplate(targetId, 0x02)) {
    return false;
  }
  
  // Compare features in buffer 1 vs buffer 2
  if (compareTemplates(score)) {
    Serial.print("Match! Score: ");
    Serial.println(score);
    return true;
  }
  
  return false;
}

bool searchFingerprint(uint16_t& foundId, uint16_t& score) {
  Serial.println("Place finger on sensor");
  
  // Capture new image
  if (!captureImage()) {
    return false;
  }
  
  // Convert to features
  if (!convertImage(0x01)) {
    return false;
  }
  
  // Search against all stored templates
  if (searchTemplate(0, 162, foundId, score)) {
    Serial.print("Found match: ID ");
    Serial.print(foundId);
    Serial.print(" Score: ");
    Serial.println(score);
    return true;
  }
  
  return false;
}
```

## Error Handling

### Common Errors

| Code | Meaning | Solution |
|------|---------|----------|
| 0x00 | OK | No error |
| 0x01 | Error receiving data | Check serial connection |
| 0x02 | No finger detected | Ask user to place finger |
| 0x03 | Fail to process image | Poor quality, try again |
| 0x04 | Feature is not clear | Dirty sensor or bad position |
| 0x05 | Not matching templates | ID not in database |
| 0x06 | Not found | No match in 1:N search |
| 0x07 | Invalid parameter | Wrong ID or index |
| 0x08 | Template conflict | Two fingers too similar |
| 0x09 | Address over-flow | ID > max capacity |
| 0x0A | Template is empty | No data at that ID |
| 0x0B | Flash read/write error | Hardware problem |

### Error Handling Implementation

```cpp
const char* getErrorMsg(uint8_t errorCode) {
  const char* errors[] = {
    "OK",
    "Error receiving data",
    "No finger on sensor",
    "Image processing failed",
    "Image too unclear",
    "Fingerprints not matching",
    "No match found",
    "Invalid parameter",
    "Template conflict",
    "Address overflow",
    "Template is empty",
    "Flash read/write error"
  };
  
  if (errorCode < sizeof(errors)) {
    return errors[errorCode];
  }
  return "Unknown error";
}

bool captureImageSafe(uint8_t maxAttempts = 3) {
  for (uint8_t attempt = 0; attempt < maxAttempts; attempt++) {
    Serial.println("Scanning fingerprint...");
    
    if (captureImage()) {
      return true;
    }
    
    Serial.println("No finger detected, try again...");
    delay(500);
  }
  
  return false;
}
```

## Database Management

### Template Storage

```
Memory Layout:
├─ Templates 0-81: User pool (MAX_PROFILES)
├─ Templates 82-162: Backup/Admin pool
└─ System area: Device settings
```

### Store/Delete Operations

```cpp
// Store template
bool storeTemplate(uint8_t modelId) {
  // Create command: 0x41 with model ID
  // Response confirms storage
  return sendCommand(0x41, modelId);
}

// Delete specific template
bool deleteTemplate(uint8_t modelId) {
  // Command: 0x42 with model ID
  return sendCommand(0x42, modelId);
}

// Delete all templates
bool deleteAllTemplates() {
  // Command: 0x43, no data
  return sendCommand(0x43, 0);
}

// Get template count
uint16_t getTemplateCount() {
  // Command: 0x44
  // Returns: count of stored templates
  return sendCommandWithResult(0x44);
}
```

## Integration with Employee System

### Employee Database Structure

```cpp
struct EmployeeRecord {
  uint8_t fingerprintId;      // AS608 template ID (1-81)
  String employeeName;        // "John Doe"
  String employeeId;          // "EMP001"
  String department;          // "Warehouse"
  time_t enrollmentDate;      // Enrollment timestamp
  bool active;                // Active/Inactive
};

// Store in EEPROM or cloud
EmployeeRecord employees[MAX_FINGERPRINTS] = {
  {1, "John Doe", "EMP001", "Warehouse", 1698765432, true},
  {2, "Jane Smith", "EMP002", "Management", 1698765432, true},
  // ...
};
```

### Verification with Logging

```cpp
bool verifyAndLog(String& employeeName) {
  uint8_t foundId = 0;
  uint16_t score = 0;
  
  // Verify fingerprint
  if (!searchFingerprint(foundId, score)) {
    Serial.println("No match");
    return false;
  }
  
  // Look up employee
  if (foundId < 1 || foundId > MAX_FINGERPRINTS) {
    return false;
  }
  
  if (!employees[foundId].active) {
    Serial.println("Employee inactive");
    return false;
  }
  
  employeeName = employees[foundId].employeeName;
  String action = getLastAction() == "OUT" ? "IN" : "OUT";
  
  // Log to Google Sheets
  cloudLogger.logAccess(
    employees[foundId].employeeId,
    employees[foundId].employeeName,
    action,
    true
  );
  
  // Sound success buzzer
  soundBuzzer(200);
  
  return true;
}
```

## Testing & Calibration

### 1. Sensor Test
```bash
1. Power on AS608
2. Place different fingers
3. Should show varying values (not constant 0 or 255)
4. Clean sensor lens if values poor
```

### 2. Quality Check
```
Good image: 65+ score
Fair image: 50-64 score
Poor image: <50 score

Action: If poor, clean sensor and try again
```

### 3. Matching Threshold
```cpp
const uint8_t MATCH_THRESHOLD = 60;
// Adjust based on false positive/negative rates
// 60 = balanced
// 70+ = stricter (more false neg)
// 50- = lenient (more false pos)
```

### 4. Diagnostic Commands

```cpp
void diagnosticTest() {
  Serial.println("=== AS608 Diagnostic ===");
  
  // Test communication
  if (isConnected()) {
    Serial.println("✓ Sensor responding");
  } else {
    Serial.println("✗ No response");
    return;
  }
  
  // Get device info
  uint16_t templateCount = getTemplateCount();
  Serial.print("Stored templates: ");
  Serial.println(templateCount);
  
  // Test image capture
  Serial.println("Place finger to test capture...");
  if (captureImage()) {
    Serial.println("✓ Image captured successfully");
  } else {
    Serial.println("✗ Image capture failed");
  }
}
```

## Troubleshooting

| Problem | Cause | Solution |
|---------|-------|----------|
| **Module not responding** | Serial connection | Check RX/TX pins, baud rate |
| **"No finger" always** | Optical window dirty | Clean with soft cloth |
| **Poor image quality** | Dry fingers / bad lighting | Use different finger, moisten finger |
| **Enrollment fails** | Images too different | Place finger same way twice |
| **False negatives** | Threshold too high | Lower MATCH_THRESHOLD |
| **False positives** | Threshold too low | Raise MATCH_THRESHOLD |
| **Out of memory** | Too many templates | Delete unused entries |

## Performance Metrics

```
Capture Time: 200-300ms
Image Processing: 100-200ms
1:1 Matching: 150-200ms
1:N Searching (162 templates): 1.5-2 seconds
Enrollment Time: ~2 seconds
```

## Security Considerations

1. **Secure Enrollment**: Verify admin badge before enrollment
2. **Template Backup**: Export templates to external storage
3. **Duplicate Detection**: Check if fingerprint already enrolled
4. **Access Logs**: Keep detailed audit trail
5. **Inactive Users**: Disable old employees quickly

## References

- AS608 Protocol Documentation
- DFRobot AS608 Arduino Library
- UART Serial Communication (115200 baud typical for ESP32)
- Fingerprint Recognition Algorithms

## Implementation Status

- ✅ Header file: `include/fingerprint_sensor.h`
- ✅ Source file: `src/fingerprint_sensor.cpp`
- ⏳ Full protocol implementation (needs testing)
- 📋 Integration with main.cpp (ready)
- 📋 Employee database (to be implemented)

## Next Steps

1. Compile and upload firmware
2. Test basic serial communication
3. Enroll first fingerprint
4. Test verification/identification
5. Integrate with employee database
6. Add to Google Sheets logging
7. Test with multiple users

---

**Last Updated**: March 2026
**Firmware Version**: 1.0.0 (AS608 Integration)
