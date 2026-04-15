/*
 * ============================================================
 *  Module cảm biến vân tay AS608
 *  Flow đăng kí: chụp ảnh lần 1 → xử lý → chụp ảnh lần 2
 *              → xử lý → so khớp 2 mẫu → lưu vào flash
 *  Flow quét:   chụp ảnh → xử lý → tìm kiếm trong database
 *              → trả về ID đã đăng kí hoặc -1 (không tìm thấy)
 * ============================================================
 */

#include "fingerprint_sensor.h"

// ========== TERMINAL HELPER ==========
// In message ra Serial + gửi lên Blynk Terminal (nếu có callback)
void FingerprintSensor::tprint(const char* msg) {
  Serial.println(msg);
  if (termPrint) termPrint(msg);
}

// ========== INIT ==========
void FingerprintSensor::begin() {
  pinMode(TCH_PIN, INPUT);

  serial.begin(FINGERPRINT_BAUD, SERIAL_8N1, FINGERPRINT_RX, FINGERPRINT_TX);
  finger.begin(FINGERPRINT_BAUD);
  delay(500);

  if (finger.verifyPassword()) {
    Serial.println("[Finger] AS608 ket noi thanh cong!");
    finger.getTemplateCount();
    Serial.printf("[Finger] So van tay da luu: %d / %d\n", finger.templateCount, finger.capacity);
  } else {
    Serial.println("[Finger] LOI: Khong ket noi duoc AS608! Kiem tra day noi.");
  }
}

// ========== ENROLL ==========
bool FingerprintSensor::enrollNewFingerprint(uint8_t id) {
  char buf[60];
  snprintf(buf, sizeof(buf), ">> Dang ky van tay ID #%d", id);
  tprint(buf);

  // --- Lan 1 ---
  tprint("Đặt ngón tay lên cảm biến (lần 1)...");
  uint8_t p = 0xFF;
  unsigned long start = millis();
  while (millis() - start < 10000) {
    p = finger.getImage();
    if (p == FINGERPRINT_OK) break;
    delay(200);
  }
  if (p != FINGERPRINT_OK) { tprint("Lỗi: Không nhận được vân tay!"); return false; }
  tprint("Chụp ảnh 1 OK.");

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) { tprint("Lỗi: Xử lý ảnh 1 thất bại!"); return false; }

  // --- Nhấc tay ---
  tprint("Nhấc ngón tay ra...");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER) delay(100);

  // --- Lần 2 ---
  tprint("Đặt lại ngón tay (lần 2)...");
  start = millis();
  p = 0xFF;
  while (millis() - start < 10000) {
    p = finger.getImage();
    if (p == FINGERPRINT_OK) break;
    delay(200);
  }
  if (p != FINGERPRINT_OK) { tprint("Lỗi: Không nhận được vân tay lần 2!"); return false; }
  tprint("Chụp ảnh 2 OK.");

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) { tprint("Lỗi: Xử lý ảnh 2 thất bại!"); return false; }

  // --- Merge + Store ---
  p = finger.createModel();
  if (p == FINGERPRINT_ENROLLMISMATCH) {
    tprint("Lỗi: 2 lần quét KHÔNG khớp!");
    tprint("Xin hãy thử thêm lại.");
    return false;
  }
  if (p != FINGERPRINT_OK) { tprint("Lỗi: Tạo mô hình thất bại!"); return false; }

  p = finger.storeModel(id);
  if (p != FINGERPRINT_OK) { tprint("Lỗi: Lưu vân tay thất bại!"); return false; }

  snprintf(buf, sizeof(buf), ">> Đã lưu ID #%d THÀNH CÔNG!", id);
  tprint(buf);
  return true;
}

// ========== DELETE ==========
bool FingerprintSensor::deleteFingerprint(uint8_t id) {
  char buf[60];
  uint8_t p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    snprintf(buf, sizeof(buf), "Đã xóa vân tay ID #%d.", id);
    tprint(buf);
    return true;
  }
  snprintf(buf, sizeof(buf), "Lỗi: Không thể xóa ID #%d!", id);
  tprint(buf);
  return false;
}

// ========== SCAN ==========
// Quét vân tay: trả về ID (>=1) nếu khớp, -1 nếu không có ngón tay, -2 nếu không nhận ra
int FingerprintSensor::scanFingerprint() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;  // Không có ngón tay hoặc lỗi chụp

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -2;  // Có ngón tay nhưng xử lý ảnh thất bại

  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) return -2;  // Có ngón tay nhưng không nhận ra

  return finger.fingerID;
}

// ========== TOUCH DETECT ==========
bool FingerprintSensor::isTouching() {
  return digitalRead(TCH_PIN) == HIGH;
}

// ========== STATUS ==========
bool FingerprintSensor::isConnected() {
  return finger.verifyPassword();
}

bool FingerprintSensor::isIdStored(uint8_t id) {
  return finger.loadModel(id) == FINGERPRINT_OK;
}

bool FingerprintSensor::deleteAll() {
  uint8_t p = finger.emptyDatabase();
  if (p == FINGERPRINT_OK) {
    tprint("Đã xóa toàn bộ vân tay!");
    return true;
  }
  tprint("Lỗi: Không thể xóa toàn bộ!");
  return false;
}

uint16_t FingerprintSensor::getFingerCount() {
  finger.getTemplateCount();
  return finger.templateCount;
}

void FingerprintSensor::listAllIds(uint8_t* ids, uint8_t& count, uint8_t maxCount) {
  count = 0;
  for (uint8_t i = 1; i <= maxCount && count < maxCount; i++) {
    if (finger.loadModel(i) == FINGERPRINT_OK) {
      ids[count++] = i;
    }
  }
}
