#include "fingerprint_sensor.h"
#include "config.h"

// Global instance
FingerprintSensor fingerprint(Serial1);

void FingerprintSensor::begin(uint32_t baudrate) {
  serial.begin(baudrate, SERIAL_8N1, FINGERPRINT_RX, FINGERPRINT_TX);
  delay(1000);
  
  Serial.println("[Fingerprint] AS608 Initialized at 57600 baud");
  delay(100);
  
  if (isConnected()) {
    Serial.println("[Fingerprint] ✓ AS608 Connected!");
    printStatus();
  } else {
    Serial.println("[Fingerprint] ✗ AS608 Not responding!");
  }
}

bool FingerprintSensor::enrollNewFingerprint(uint8_t id) {
  Serial.print("[Fingerprint] Starting enrollment for ID: ");
  Serial.println(id);
  
  // Implement AS608 enrollment process
  // This is a simplified version - needs full AS608 protocol implementation
  
  // Step 1: Send ENROLL command
  uint8_t cmd[] = {0x01, id};  // Command: ENROLL, Param: ID
  sendCommand(0x01, cmd, 2);
  
  // Step 2: Wait for finger placement and capture
  Serial.println("[Fingerprint] Please place finger on sensor...");
  
  // Step 3: Verify capture quality
  uint8_t response[256];
  uint16_t len = 0;
  
  for (int attempt = 0; attempt < DEFAULT_ENROLLMENT_ATTEMPTS; attempt++) {
    delay(500);
    if (readResponse(response, len)) {
      Serial.print("[Fingerprint] Attempt ");
      Serial.print(attempt + 1);
      Serial.println(" - Image captured");
    }
  }
  
  Serial.println("[Fingerprint] Enrollment complete!");
  enrollCount++;
  return true;
}

bool FingerprintSensor::verifyFingerprint(uint8_t& matchedId) {
  Serial.println("[Fingerprint] Please place finger on sensor...");
  
  // Implement AS608 verification process
  delay(500);
  
  // Send VERIFY command
  uint8_t cmd[] = {0x00};  // Command: VERIFY
  sendCommand(0x00, cmd, 1);
  
  // Read response with matched ID
  uint8_t response[256];
  uint16_t len = 0;
  
  if (readResponse(response, len)) {
    matchedId = response[0];
    Serial.print("[Fingerprint] Match found! ID: ");
    Serial.println(matchedId);
    return true;
  }
  
  Serial.println("[Fingerprint] No match found");
  return false;
}

bool FingerprintSensor::deleteFingerprint(uint8_t id) {
  Serial.print("[Fingerprint] Deleting ID: ");
  Serial.println(id);
  
  uint8_t cmd[] = {0x02, id};  // Command: DELETE
  sendCommand(0x02, cmd, 2);
  
  return true;
}

bool FingerprintSensor::deleteAllFingerprints() {
  Serial.println("[Fingerprint] Deleting all fingerprints...");
  
  uint8_t cmd[] = {0x03};  // Command: DELETE_ALL
  sendCommand(0x03, cmd, 1);
  
  enrollCount = 0;
  return true;
}

uint16_t FingerprintSensor::getFingerCount() {
  // Query module for current fingerprint count
  return enrollCount;
}

bool FingerprintSensor::setSecurityLevel(uint8_t level) {
  if (level < 1 || level > 5) {
    Serial.println("[Fingerprint] Invalid security level (1-5)");
    return false;
  }
  
  Serial.print("[Fingerprint] Setting security level to: ");
  Serial.println(level);
  
  uint8_t cmd[] = {0x04, level};  // Command: SET_LEVEL
  sendCommand(0x04, cmd, 2);
  
  return true;
}

bool FingerprintSensor::isConnected() {
  // Send PING command
  uint8_t cmd[] = {0xFF};  // PING command
  sendCommand(0xFF, cmd, 1);
  
  uint8_t response[256];
  uint16_t len = 0;
  
  delay(500);
  return readResponse(response, len);
}

void FingerprintSensor::sendCommand(uint8_t cmd, uint8_t* data, uint16_t dataLen) {
  // Simplified command sending - implement full AS608 protocol
  serial.write(cmd);
  serial.write(data, dataLen);
  serial.flush();
}

bool FingerprintSensor::readResponse(uint8_t* response, uint16_t& len) {
  len = 0;
  unsigned long timeout = millis() + 1000;
  
  while (millis() < timeout) {
    if (serial.available()) {
      response[len++] = serial.read();
      if (len >= 256) break;
    }
  }
  
  return len > 0;
}

uint16_t FingerprintSensor::calculateChecksum(uint8_t* data, uint16_t len) {
  uint16_t sum = 0;
  for (uint16_t i = 0; i < len; i++) {
    sum += data[i];
  }
  return sum;
}

void FingerprintSensor::printStatus() {
  Serial.println("\n=== Fingerprint Sensor Status ===");
  Serial.print("Connected: ");
  Serial.println(isConnected() ? "Yes" : "No");
  Serial.print("Enrolled IDs: ");
  Serial.println(enrollCount);
  Serial.println("===================================\n");
}
