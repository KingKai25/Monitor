#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>

// PZEM-004T v3 Power Monitor Handler
class PowerMonitor {
private:
  HardwareSerial& serial;
  uint8_t slaveId;
  unsigned long lastReadTime;
  
  // PZEM-004T frame structure
  struct Frame {
    uint8_t header;          // 0xB4
    uint8_t addr;            // Slave address
    uint8_t func;            // Function code
    uint8_t data[8];         // Data bytes
    uint8_t crc[2];          // CRC checksum
  };
  
  // Data holding register
  struct {
    float voltage;           // V (0-300V)
    float current;           // A (0-100A)
    float power;             // W (0-23000W)
    float energy;            // kWh (0-9999.99 kWh)
    float frequency;         // Hz
    float powerFactor;       // (0-1)
  } registers;
  
  // Helper functions
  uint16_t calculateCRC(uint8_t* frame, uint8_t length);
  void sendFrame(uint8_t* frame, uint8_t length);
  bool readFrame(uint8_t* frame, uint8_t length);
  
public:
  PowerMonitor(HardwareSerial& ser, uint8_t id = 0xF8) 
    : serial(ser), slaveId(id), lastReadTime(0) {}
  
  // Initialization
  void begin(uint32_t baudrate = 9600);
  
  // Data reading
  bool readVoltage(float& voltage);
  bool readCurrent(float& current);
  bool readPower(float& power);
  bool readEnergy(float& energy);
  bool readFrequency(float& frequency);
  bool readPowerFactor(float& pf);
  
  // Bulk read (more efficient)
  bool readAll(float& v, float& i, float& p, float& e);
  
  // Configuration
  bool setSlaveId(uint8_t newId);
  bool resetEnergy();
  
  // Status
  bool isConnected();
  void printStatus();
  
  // Get last read values
  float getVoltage() const { return registers.voltage; }
  float getCurrent() const { return registers.current; }
  float getPower() const { return registers.power; }
  float getEnergy() const { return registers.energy; }
};

// External instance
extern PowerMonitor powerMonitor;
