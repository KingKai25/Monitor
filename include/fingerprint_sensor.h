#pragma once

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include "config.h"

// Callback for sending messages to Blynk Terminal
typedef void (*TerminalPrintFunc)(const char* msg);

class FingerprintSensor {
private:
  HardwareSerial& serial;
  Adafruit_Fingerprint finger;
  TerminalPrintFunc termPrint;

  void tprint(const char* msg);

public:
  FingerprintSensor(HardwareSerial& ser) : serial(ser), finger(&ser), termPrint(nullptr) {}

  void begin();
  void setTerminalCallback(TerminalPrintFunc cb) { termPrint = cb; }

  bool enrollNewFingerprint(uint8_t id);
  bool deleteFingerprint(uint8_t id);
  int  scanFingerprint();   // returns matched ID (>=1) or -1
  bool isTouching();
  bool isConnected();
  bool isIdStored(uint8_t id);
  bool deleteAll();
  uint16_t getFingerCount();
  void listAllIds(uint8_t* ids, uint8_t& count, uint8_t maxCount = 127);
};
