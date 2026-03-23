#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>

// AS608 Fingerprint Sensor Handler
class FingerprintSensor {
private:
  HardwareSerial& serial;
  uint8_t enrollCount;
  
  // AS608 Command structure
  struct Command {
    uint8_t header[2];    // 0xEF, 0x01
    uint8_t addr[4];      // Module address (default FFFFFFFF)
    uint8_t pid;          // Packet ID type
    uint8_t length[2];    // Data length
    uint8_t data[256];    // Command data
    uint8_t checksum[2];  // Checksum
  };

  // Helper functions
  void sendCommand(uint8_t cmd, uint8_t* data, uint16_t dataLen);
  bool readResponse(uint8_t* response, uint16_t& len);
  uint16_t calculateChecksum(uint8_t* data, uint16_t len);
  
public:
  FingerprintSensor(HardwareSerial& ser) : serial(ser), enrollCount(0) {}
  
  // Initialization
  void begin(uint32_t baudrate = 57600);
  
  // Fingerprint operations
  bool enrollNewFingerprint(uint8_t id);
  bool verifyFingerprint(uint8_t& matchedId);
  bool deleteFingerprint(uint8_t id);
  bool deleteAllFingerprints();
  
  // System operations
  uint16_t getFingerCount();
  bool setSecurityLevel(uint8_t level);  // 1=highest, 5=lowest
  
  // Status
  bool isConnected();
  void printStatus();
};

// External instance
extern FingerprintSensor fingerprint;
