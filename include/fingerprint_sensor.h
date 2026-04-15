#pragma once

// ============================================================
//  Module cảm biến vân tay AS608
//  - Đăng kí vân tay mới (2 bước: chụp 2 lần → tạo mẫu → lưu)
//  - Xóa vân tay theo ID hoặc xóa toàn bộ
//  - Quét nhận dạng vân tay (trả về ID đã đăng kí)
//  - Phát hiện ngón tay chạm qua chân TCH
//  - Gửi thông báo lên Blynk Terminal qua callback
// ============================================================

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include "config.h"

// Kiểu hàm callback để gửi message lên Blynk Terminal
typedef void (*TerminalPrintFunc)(const char* msg);

class FingerprintSensor {
private:
  HardwareSerial& serial;          // UART giao tiếp với AS608
  Adafruit_Fingerprint finger;     // Driver thư viện Adafruit
  TerminalPrintFunc termPrint;     // Callback in lên Terminal

  // In message ra cả Serial và Blynk Terminal
  void tprint(const char* msg);

public:
  FingerprintSensor(HardwareSerial& ser) : serial(ser), finger(&ser), termPrint(nullptr) {}

  void begin();                                    // Khởi tạo, kiểm tra kết nối AS608
  void setTerminalCallback(TerminalPrintFunc cb) { termPrint = cb; }

  bool enrollNewFingerprint(uint8_t id);           // Đăng kí vân tay mới (2 lần quét)
  bool deleteFingerprint(uint8_t id);              // Xóa vân tay theo ID
  int  scanFingerprint();                          // Quét nhận dạng, trả về ID (>=1) hoặc -1
  bool isTouching();                               // Kiểm tra có ngón tay chạm cảm biến
  bool isConnected();                              // Kiểm tra AS608 còn kết nối
  bool isIdStored(uint8_t id);                     // Kiểm tra ID đã được đăng kí chưa
  bool deleteAll();                                // Xóa toàn bộ database vân tay
  uint16_t getFingerCount();                       // Đếm số vân tay đã lưu
  void listAllIds(uint8_t* ids, uint8_t& count, uint8_t maxCount = 127); // Liệt kê tất cả ID
};
