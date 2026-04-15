#pragma once

// ============================================================
//  Module đo điện năng PZEM-004T v3
//  Sử dụng thư viện PZEM004Tv30 (mandulka/PZEM-004T-v30)
//  - Slave ID mặc định: 0x01 | Function code: 0x04
//  - Thư viện tự xử lý CRC16 Modbus và giao tiếp UART
// ============================================================

#include <Arduino.h>
#include <HardwareSerial.h>
#include <PZEM004Tv30.h>
#include "config.h"

class PowerMonitor {
private:
  HardwareSerial& _serial;
  PZEM004Tv30 pzem;

public:
  PowerMonitor(HardwareSerial& ser) : _serial(ser), pzem(ser, PZEM_RX, PZEM_TX) {}

  void begin(uint32_t baudrate = 9600);
  bool isConnected();

  bool readVoltage(float& voltage);
  bool readCurrent(float& current);
  bool readPower(float& power);
  bool readEnergy(float& energy);
  bool readFrequency(float& frequency);
  bool readPowerFactor(float& pf);
  bool readAll(float& v, float& i, float& p, float& e);

  bool resetEnergy();
  void printStatus();
};