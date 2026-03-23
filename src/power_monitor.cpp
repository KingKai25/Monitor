#include "power_monitor.h"
#include "config.h"

// Global instance
PowerMonitor powerMonitor(Serial2, 0xF8);

void PowerMonitor::begin(uint32_t baudrate) {
  serial.begin(baudrate, SERIAL_8N1, PZEM_RX, PZEM_TX);
  delay(500);
  
  Serial.println("[PowerMon] PZEM-004T v3 Initialized at 9600 baud");
  delay(1000);
  
  if (isConnected()) {
    Serial.println("[PowerMon] ✓ PZEM Connected!");
    printStatus();
  } else {
    Serial.println("[PowerMon] ✗ PZEM Not responding!");
  }
}

bool PowerMonitor::readVoltage(float& voltage) {
  // PZEM command to read voltage register
  uint8_t cmd[] = {slaveId, 0x03, 0x00, 0x00, 0x00, 0x02};
  
  sendFrame(cmd, 6);
  delay(100);
  
  uint8_t response[25] = {0};
  if (readFrame(response, 7)) {
    // Extract voltage from response (bytes 3-4)
    voltage = ((response[3] << 8) | response[4]) / 10.0;
    registers.voltage = voltage;
    return true;
  }
  
  return false;
}

bool PowerMonitor::readCurrent(float& current) {
  // PZEM command to read current register
  uint8_t cmd[] = {slaveId, 0x03, 0x00, 0x01, 0x00, 0x02};
  
  sendFrame(cmd, 6);
  delay(100);
  
  uint8_t response[25] = {0};
  if (readFrame(response, 7)) {
    // Extract current from response (bytes 3-4)
    current = ((response[3] << 8) | response[4]) / 1000.0;
    registers.current = current;
    return true;
  }
  
  return false;
}

bool PowerMonitor::readPower(float& power) {
  // PZEM command to read power register
  uint8_t cmd[] = {slaveId, 0x03, 0x00, 0x03, 0x00, 0x02};
  
  sendFrame(cmd, 6);
  delay(100);
  
  uint8_t response[25] = {0};
  if (readFrame(response, 7)) {
    // Extract power from response (bytes 3-4)
    power = ((response[3] << 8) | response[4]);
    registers.power = power;
    return true;
  }
  
  return false;
}

bool PowerMonitor::readEnergy(float& energy) {
  // PZEM command to read energy register
  uint8_t cmd[] = {slaveId, 0x03, 0x00, 0x05, 0x00, 0x02};
  
  sendFrame(cmd, 6);
  delay(100);
  
  uint8_t response[25] = {0};
  if (readFrame(response, 7)) {
    // Extract energy from response (bytes 3-4)
    energy = ((response[3] << 8) | response[4]) / 100.0;
    registers.energy = energy;
    return true;
  }
  
  return false;
}

bool PowerMonitor::readFrequency(float& frequency) {
  // PZEM command to read frequency register
  uint8_t cmd[] = {slaveId, 0x03, 0x00, 0x07, 0x00, 0x02};
  
  sendFrame(cmd, 6);
  delay(100);
  
  uint8_t response[25] = {0};
  if (readFrame(response, 7)) {
    // Extract frequency from response (bytes 3-4)
    frequency = ((response[3] << 8) | response[4]) / 10.0;
    registers.frequency = frequency;
    return true;
  }
  
  return false;
}

bool PowerMonitor::readPowerFactor(float& pf) {
  // PZEM command to read power factor register
  uint8_t cmd[] = {slaveId, 0x03, 0x00, 0x08, 0x00, 0x02};
  
  sendFrame(cmd, 6);
  delay(100);
  
  uint8_t response[25] = {0};
  if (readFrame(response, 7)) {
    // Extract power factor from response (bytes 3-4)
    pf = ((response[3] << 8) | response[4]) / 100.0;
    registers.powerFactor = pf;
    return true;
  }
  
  return false;
}

bool PowerMonitor::readAll(float& v, float& i, float& p, float& e) {
  bool success = true;
  
  success &= readVoltage(v);
  delay(100);
  success &= readCurrent(i);
  delay(100);
  success &= readPower(p);
  delay(100);
  success &= readEnergy(e);
  
  return success;
}

bool PowerMonitor::setSlaveId(uint8_t newId) {
  // Command to change slave ID
  uint8_t cmd[] = {slaveId, 0x06, 0x00, 0x02, 0x00, newId};
  
  // Add CRC
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

bool PowerMonitor::resetEnergy() {
  // Command to reset energy counter
  uint8_t cmd[] = {slaveId, 0x06, 0x00, 0xAA, 0x00, 0x00};
  
  uint16_t crc = calculateCRC(cmd, 6);
  
  uint8_t fullCmd[8];
  memcpy(fullCmd, cmd, 6);
  fullCmd[6] = crc >> 8;
  fullCmd[7] = crc & 0xFF;
  
  sendFrame(fullCmd, 8);
  delay(100);
  
  registers.energy = 0;
  return true;
}

bool PowerMonitor::isConnected() {
  float v = 0;
  return readVoltage(v);
}

void PowerMonitor::sendFrame(uint8_t* frame, uint8_t length) {
  // Add CRC and send
  uint16_t crc = calculateCRC(frame, length);
  
  serial.write(frame, length);
  serial.write((uint8_t)(crc >> 8));
  serial.write((uint8_t)(crc & 0xFF));
  serial.flush();
}

bool PowerMonitor::readFrame(uint8_t* frame, uint8_t length) {
  unsigned long timeout = millis() + 1000;
  uint8_t idx = 0;
  
  while (millis() < timeout) {
    if (serial.available()) {
      frame[idx++] = serial.read();
      if (idx >= length + 2) {  // +2 for CRC bytes
        // Verify CRC
        uint16_t receivedCrc = (frame[length] << 8) | frame[length + 1];
        uint16_t calculatedCrc = calculateCRC(frame, length);
        return receivedCrc == calculatedCrc;
      }
    }
  }
  
  return false;
}

uint16_t PowerMonitor::calculateCRC(uint8_t* frame, uint8_t length) {
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

void PowerMonitor::printStatus() {
  Serial.println("\n=== Power Monitor Status ===");
  Serial.print("Connected: ");
  Serial.println(isConnected() ? "Yes" : "No");
  Serial.print("Voltage: ");
  Serial.print(registers.voltage);
  Serial.println(" V");
  Serial.print("Current: ");
  Serial.print(registers.current);
  Serial.println(" A");
  Serial.print("Power: ");
  Serial.print(registers.power);
  Serial.println(" W");
  Serial.print("Energy: ");
  Serial.print(registers.energy);
  Serial.println(" kWh");
  Serial.println("=============================\n");
}
