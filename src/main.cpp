/*
 * ============================================================
 *  HỆ THỐNG GIÁM SÁT KHO HÀNG - Firmware ESP32
 * ============================================================
 *  Chức năng:
 *  - Đo nhiệt độ, độ ẩm (DHT11)
 *  - Cảnh báo khói (MQ2), cháy (Flame sensor)
 *  - Đo điện năng (PZEM-004T v3 qua Modbus RTU)
 *  - Quản lý vân tay (AS608) - đăng kí, xóa, chấm công
 *  - Điều khiển đèn relay qua Blynk + nút nhấn
 *  - Đồng bộ dữ liệu nhân viên & log chấm công lên Google Sheet
 *  - Điều khiển từ xa qua Blynk Terminal (menu, +ID, -ID, all, rst, push)
 *
 *  Blynk Virtual Pins:
 *  V0 = Nhiệt độ + Độ ẩm (String)
 *  V1 = Công suất (W) - SuperChart
 *  V2 = LED báo cháy (đổi màu)
 *  V3 = Terminal điều khiển vân tay
 *  V4 = Switch bật/tắt đèn
 *  V5 = LED báo khói (đổi màu)
 * ============================================================
 */

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#define BLYNK_PRINT Serial
#include "config.h"
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <HardwareSerial.h>
#include "fingerprint_sensor.h"
#include "power_monitor.h"
#include <EEPROM.h>

// Bố cục EEPROM: [0]=magic(0xAB), [1]=số lượng pending, [2..21]=cặp(op,id), tối đa 10 entries
#define EEPROM_SIZE      24
#define EEPROM_MAGIC_VAL 0xAB
#define MAX_PENDING_SYNC 10
#define SYNC_OP_ADD      1
#define SYNC_OP_DEL      2

// ========== BLYNK TERMINAL ==========
WidgetTerminal blynkTerminal(V3);

// ========== HARDWARE SERIAL ==========
HardwareSerial SerialFinger(2);
HardwareSerial SerialPZEM(1);

// ========== DRIVER INSTANCES ==========
FingerprintSensor fingerprint(SerialFinger);
PowerMonitor powerMonitor(SerialPZEM);
DHT dht(DHT11_PIN, DHT11);

// ========== TERMINAL CALLBACK ==========
void terminalPrint(const char* msg) {
  blynkTerminal.println(msg);
  blynkTerminal.flush();
}

// ========== GLOBALS ==========
float temperature = 0;
float humidity = 0;
float power = 0;
bool lightState = false;
bool fireAlert = false;
bool prevFireAlert = false;
bool smokeAlert = false;

// Trạng thái chờ xác nhận lệnh RST (xóa toàn bộ vân tay)
bool waitingRstConfirm = false;
unsigned long rstConfirmTime = 0;

unsigned long lastDHTRead = 0;
unsigned long lastPZEMRead = 0;
unsigned long lastMQ2Read = 0;
unsigned long lastBlynkUpdate = 0;
unsigned long lastFingerScan = 0;

volatile bool buttonPressed = false;
unsigned long lastWifiRetry = 0;
unsigned long lastBlynkRetry = 0;
unsigned long lastPendingFlush = 0;

// ========== FUNCTION DECLARATIONS ==========
void setupWiFi();
void readDHTSensors();
void readMQ2Smoke();
void readPZEMData();
void checkFireAlert();
void controlLight(bool state);
void soundBuzzer(int duration);
void updateBlynk();
void scanFingerprintLoop();
void sendToGoogleSheet(int fingerId);
String getTimeString();
void listAllFingerprints();
void resetAllFingerprints();
void pushAllToSheet();
bool sendStaffAction(int id, const char* action);
void syncStaffAdd(int id);
void syncStaffDelete(int id);
void eepromSavePending(uint8_t op, uint8_t id);
void eepromRemoveFirst();
void flushPendingSync();
void showMenu();

// ========== BLYNK HANDLERS ==========

// V3: Terminal - Nhập lệnh điều khiển vân tay
// Lệnh: menu, +ID, -ID(,ID,...), all, rst, push
BLYNK_WRITE(V3) {
  String input = param.asStr();
  input.trim();

  // --- Xử lý trạng thái chờ xác nhận RST ---
  if (waitingRstConfirm) {
    if (input.equalsIgnoreCase("yes")) {
      waitingRstConfirm = false;
      resetAllFingerprints();
    } else if (input.equalsIgnoreCase("no")) {
      waitingRstConfirm = false;
      blynkTerminal.println("Đã hủy lệnh xóa.");
      blynkTerminal.flush();
    } else {
      blynkTerminal.println("Nhập 'yes' hoặc 'no'.");
      blynkTerminal.flush();
    }
    return;
  }

  // --- Lệnh "menu" - hiển thị danh sách lệnh ---
  if (input.equalsIgnoreCase("menu")) {
    showMenu();
    return;
  }

  // --- Lệnh "all" - liệt kê tất cả vân tay ---
  if (input.equalsIgnoreCase("all")) {
    listAllFingerprints();
    return;
  }

  // --- Lệnh "rst" - xóa toàn bộ vân tay (cần xác nhận) ---
  if (input.equalsIgnoreCase("rst")) {
    waitingRstConfirm = true;
    rstConfirmTime = millis();
    blynkTerminal.println("Lệnh này sẽ xóa hết vân tay!");
    blynkTerminal.println("Nhập 'yes' để tiếp tục, 'no' để dừng lại.");
    blynkTerminal.flush();
    return;
  }

  // --- Lệnh "push" - đẩy toàn bộ ID lên Google Sheet ---
  if (input.equalsIgnoreCase("push")) {
    pushAllToSheet();
    return;
  }

  // --- Lệnh xóa: -ID hoặc -ID1, ID2, ID3 ---
  if (input.startsWith("-")) {
    String body = input.substring(1); // bỏ dấu '-' đầu
    int startIdx = 0;
    bool hasDeleted = false;
    while (startIdx <= (int)body.length()) {
      int commaIdx = body.indexOf(',', startIdx);
      if (commaIdx == -1) commaIdx = body.length();
      String token = body.substring(startIdx, commaIdx);
      token.trim();
      int idVal = token.toInt();
      if (idVal > 0) {
        uint8_t delId = (uint8_t)idVal;
        if (!fingerprint.isIdStored(delId)) {
          char msg[60];
          snprintf(msg, sizeof(msg), "ID #%d chưa được đăng kí!", delId);
          blynkTerminal.println(msg);
        } else {
          if (fingerprint.deleteFingerprint(delId)) {
            syncStaffDelete(delId);
          }
          hasDeleted = true;
        }
      }
      startIdx = commaIdx + 1;
    }
    blynkTerminal.flush();
    (void)hasDeleted;
    return;
  }

  // --- Lệnh thêm: +ID ---
  int id = input.toInt();
  if (id > 0) {
    if (id > 64) {
      blynkTerminal.println("Nhập ID trong khoảng 1-64.");
      blynkTerminal.flush();
      return;
    }
    if (fingerprint.isIdStored((uint8_t)id)) {
      char msg[60];
      snprintf(msg, sizeof(msg), "ID #%d đã được thêm trước đó.", id);
      blynkTerminal.println(msg);
      blynkTerminal.flush();
      return;
    }
    if (fingerprint.enrollNewFingerprint((uint8_t)id)) {
      char msg[80];
      snprintf(msg, sizeof(msg), "Đã thêm ID #%d thành công - Đang chờ đồng bộ...", id);
      blynkTerminal.println(msg);
      blynkTerminal.flush();
      syncStaffAdd(id);
    }
    return;
  }

  // --- Lệnh không hợp lệ ---
  blynkTerminal.println("Lệnh không hợp lệ.");
  blynkTerminal.println("Gõ 'menu' để xem hướng dẫn.");
  blynkTerminal.flush();
}

// V4: Bật/Tắt đèn (0=Tắt, 1=Bật)
BLYNK_WRITE(V4) {
  int state = param.asInt();
  controlLight(state == 1);
  Serial.println(state == 1 ? "[Light] Bật đèn" : "[Light] Tắt đèn");
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V4);
  Blynk.virtualWrite(V2, 255);
  Blynk.setProperty(V2, "color", "#FFFFFF");
  Blynk.virtualWrite(V5, 255);
  Blynk.setProperty(V5, "color", "#FFFFFF");
}

// ========== BUTTON ISR ==========
void IRAM_ATTR buttonInterrupt() {
  buttonPressed = true;
}

// ========== SETUP ==========
// Khởi tạo toàn bộ phần cứng và kết nối: WiFi, NTP, cảm biến, Blynk
void setup() {
  Serial.begin(SERIAL_MONITOR_BAUD);
  delay(1000);
  Serial.println();
  Serial.println("========================================");
  Serial.println("     HỆ THỐNG GIÁM SÁT KHO HÀNG");
  Serial.println("========================================");

  EEPROM.begin(EEPROM_SIZE);
  if (EEPROM.read(0) != EEPROM_MAGIC_VAL) {
    EEPROM.write(0, EEPROM_MAGIC_VAL);
    EEPROM.write(1, 0);
    EEPROM.commit();
  } else {
    // Xóa các entry rác (id == 0 hoặc id > 64) khỏi động mọi lần
    uint8_t count = EEPROM.read(1);
    uint8_t clean = 0;
    // Copy các entry hợp lệ vào đầu
    uint8_t tmpOp[MAX_PENDING_SYNC], tmpId[MAX_PENDING_SYNC];
    for (uint8_t i = 0; i < count && i < MAX_PENDING_SYNC; i++) {
      uint8_t op = EEPROM.read(2 + i * 2);
      uint8_t id = EEPROM.read(2 + i * 2 + 1);
      if (id >= 1 && id <= 64 && (op == SYNC_OP_ADD || op == SYNC_OP_DEL)) {
        tmpOp[clean] = op; tmpId[clean] = id; clean++;
      } else {
        Serial.printf("[EEPROM] Xóa entry rác: op=%d id=%d\n", op, id);
      }
    }
    if (clean != count) {
      for (uint8_t i = 0; i < clean; i++) {
        EEPROM.write(2 + i * 2, tmpOp[i]);
        EEPROM.write(2 + i * 2 + 1, tmpId[i]);
      }
      EEPROM.write(1, clean);
      EEPROM.commit();
    }
  }
  Serial.println("[EEPROM] Đã khởi tạo");

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FIRE_SENSOR_PIN, INPUT);
  pinMode(MQ2_PIN, INPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  setupWiFi();
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  Serial.println("[DHT] Đã khởi tạo");
  dht.begin();

  fingerprint.begin();
  fingerprint.setTerminalCallback(terminalPrint);

  powerMonitor.begin(PZEM_BAUD);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, FALLING);

  Serial.println("[Blynk] Đang kết nối...");
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect(5000);
  if (Blynk.connected()) {
    Serial.println("[Blynk] Đã kết nối!");
  } else {
    Serial.println("[Blynk] Kết nối thất bại - sẽ thử lại");
  }

  Serial.println("========================================");
  Serial.println("  Hệ thống sẵn sàng!");
  Serial.println("========================================");
}

// ========== LOOP ==========
void loop() {
  unsigned long now = millis();

  if (buttonPressed) {
    buttonPressed = false;
    controlLight(!lightState);
  }

  if (WiFi.status() != WL_CONNECTED && now - lastWifiRetry >= 5000) {
    lastWifiRetry = now;
    setupWiFi();
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!Blynk.connected() && now - lastBlynkRetry >= 3000) {
      lastBlynkRetry = now;
      Blynk.connect(3000);
    }
    Blynk.run();
  }

  if (now - lastDHTRead >= DHT_READ_INTERVAL) {
    readDHTSensors();
    lastDHTRead = now;
  }

  if (now - lastMQ2Read >= MQ2_READ_INTERVAL) {
    readMQ2Smoke();
    lastMQ2Read = now;
  }

  if (now - lastPZEMRead >= PZEM_READ_INTERVAL) {
    readPZEMData();
    lastPZEMRead = now;
  }

  checkFireAlert();

  // Kiểm tra timeout xác nhận lệnh RST (30 giây)
  if (waitingRstConfirm && now - rstConfirmTime > 30000) {
    waitingRstConfirm = false;
    if (Blynk.connected()) {
      blynkTerminal.println("Hết thời gian. Đã hủy lệnh xóa.");
      blynkTerminal.flush();
    }
    Serial.println("[RST] Timeout - đã hủy lệnh xóa toàn bộ");
  }

  if (now - lastPendingFlush >= 30000) {
    lastPendingFlush = now;
    flushPendingSync();
  }

  // Auto-scan vân tay khi phát hiện ngón tay trên cảm biến
  if (now - lastFingerScan >= FINGER_SCAN_INTERVAL) {
    lastFingerScan = now;
    scanFingerprintLoop();
  }

  if (now - lastBlynkUpdate >= BLYNK_UPDATE_INTERVAL) {
    updateBlynk();
    lastBlynkUpdate = now;
  }

  delay(100);
}

// ========== WIFI ==========
// Kết nối WiFi, thử tối đa 20 lần (10 giây)
void setupWiFi() {
  Serial.printf("[WiFi] Đang kết nối: %s...\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);

  if (strlen(WIFI_PASSWORD) == 0) {
    WiFi.begin(WIFI_SSID);
  } else {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\n[WiFi] Đã kết nối! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WiFi] Kết nối thất bại!");
  }
}

// ========== DHT ==========
// Đọc nhiệt độ, độ ẩm từ cảm biến DHT11
void readDHTSensors() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("[DHT] Lỗi đọc cảm biến!");
    return;
  }

  Serial.printf("[DHT] Nhiệt độ: %.1f°C | Độ ẩm: %.1f%%\n", temperature, humidity);
}

// ========== MQ2 ==========
// Đọc cảm biến khói MQ2 (chỉ dùng digitalRead - ADC2 xung đột với WiFi)
void readMQ2Smoke() {
  int dVal = digitalRead(MQ2_PIN);
  
  // DO: LOW = có khói, HIGH = bình thường
  bool currentSmoke = (dVal == LOW);

  if (currentSmoke) {
    Serial.println("[MQ2] CẢNH BÁO: Phát hiện khói!");
  } else {
    Serial.println("[MQ2] Trạng thái: Bình thường");
  }

  if (currentSmoke && !smokeAlert) {
    smokeAlert = true;
    soundBuzzer(300);
    if (Blynk.connected()) {
      Blynk.virtualWrite(V5, 255);
      Blynk.setProperty(V5, "color", "#FFA500");
      Blynk.logEvent("smoke_alert", "CANH BAO KHOI!");
    }
  } else if (!currentSmoke && smokeAlert) {
    smokeAlert = false;
    if (Blynk.connected()) {
      Blynk.virtualWrite(V5, 255);
      Blynk.setProperty(V5, "color", "#FFFFFF");
    }
  }
}

// ========== PZEM ==========
// Đọc dữ liệu điện năng từ PZEM-004T v3 qua Modbus RTU
void readPZEMData() {
  float v = 0, i = 0, e = 0;
  float f = 0;
  powerMonitor.readVoltage(v);
  powerMonitor.readCurrent(i);
  powerMonitor.readPower(power);
  powerMonitor.readEnergy(e);
  powerMonitor.readFrequency(f);

  Serial.printf("[PZEM] %.1fV | %.2fA | %.1fW | %.3fkWh | %.1fHz\n", v, i, power, e, f);
}

// ========== FIRE CHECK ==========
void checkFireAlert() {
  // Chân DO: LOW = có lửa, HIGH = bình thường
  bool currentFire = (digitalRead(FIRE_SENSOR_PIN) == LOW) || (temperature > TEMP_THRESHOLD);

  if (currentFire && !fireAlert) {
    fireAlert = true;
    soundBuzzer(300);
    if (Blynk.connected()) {
      Blynk.virtualWrite(V2, 255);
      Blynk.setProperty(V2, "color", "#FF0000");
      Blynk.logEvent("fire_alert", "CANH BAO CHAY!");
    }
    Serial.println("[FIRE] CANH BAO CHAY!");
  } else if (!currentFire && fireAlert) {
    fireAlert = false;
    if (Blynk.connected()) {
      Blynk.virtualWrite(V2, 255);
      Blynk.setProperty(V2, "color", "#FFFFFF");
    }
  }
}

// ========== RELAY ==========
void controlLight(bool state) {
  lightState = state;
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);
  Serial.print("[Light] ");
  Serial.println(state ? "ON" : "OFF");
}

// ========== BUZZER ==========
void soundBuzzer(int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

// ========== BLYNK UPDATE ==========
void updateBlynk() {
  if (!Blynk.connected()) return;

  // V0: Nhiệt độ + Độ ẩm (String)
  String tempHumid = "T:" + String(temperature, 1) + "°C | H:" + String(humidity, 1) + "%";
  Blynk.virtualWrite(V0, tempHumid);

  // V1: Công suất (W) - SuperChart (chỉ ghi nếu giá trị hợp lệ, tránh spike)
  if (power >= 0 && power < 25000) {
    Blynk.virtualWrite(V1, power);
  }

  // V2: Báo cháy - LED luôn sáng, đổi màu (trắng=an toàn, đỏ=cháy)
  Blynk.virtualWrite(V2, 255);
  Blynk.setProperty(V2, "color", fireAlert ? "#FF0000" : "#FFFFFF");

  // V5: Báo khói - LED luôn sáng, đổi màu (trắng=an toàn, cam=khói)
  Blynk.virtualWrite(V5, 255);
  Blynk.setProperty(V5, "color", smokeAlert ? "#FFA500" : "#FFFFFF");
}

// ========== NTP TIME ==========
String getTimeString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 1000)) {
    return "N/A";
  }
  char buf[20];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buf);
}

// ========== FINGERPRINT AUTO-SCAN ==========
void scanFingerprintLoop() {
  if (!fingerprint.isTouching()) return;

  int id = fingerprint.scanFingerprint();
  if (id >= 1) {
    // Vân tay hợp lệ → bíp 1 lần ngắn
    soundBuzzer(100);

    String timeStr = getTimeString();
    char msg[80];
    snprintf(msg, sizeof(msg), "Vân tay hợp lệ: ID #%d | %s", id, timeStr.c_str());
    Serial.println(msg);

    if (Blynk.connected()) {
      blynkTerminal.println(msg);
      blynkTerminal.flush();
    }

    sendToGoogleSheet(id);
  } else if (id == -2) {
    // Phát hiện ngón tay nhưng không nhận ra → bíp 2 lần
    soundBuzzer(100);
    delay(100);
    soundBuzzer(100);
    Serial.println("[Finger] Vân tay không nhận ra!");
  }
  // id == -1: không có ngón tay hoặc lỗi chụp → bỏ qua
}

// ========== SHOW MENU ==========
// Hiển thị danh sách lệnh trên Blynk Terminal, mỗi lệnh một dòng
void showMenu() {
  blynkTerminal.println("===== MENU LỆNH =====");
  blynkTerminal.println("menu  : Hiển thị menu");
  blynkTerminal.println("+ID   : Thêm vân tay (VD: +1)");
  blynkTerminal.println("-ID   : Xóa vân tay (VD: -1, 2, 3)");
  blynkTerminal.println("all   : Liệt kê vân tay");
  blynkTerminal.println("rst   : Xóa toàn bộ vân tay");
  blynkTerminal.println("push  : Đẩy ID lên Sheet");
  blynkTerminal.println("======================");
  blynkTerminal.flush();
}

// ========== LIST ALL FINGERPRINTS ==========
void listAllFingerprints() {
  uint8_t ids[127];
  uint8_t count = 0;
  fingerprint.listAllIds(ids, count);

  if (count == 0) {
    blynkTerminal.println("Chua co van tay nao.");
    blynkTerminal.flush();
    return;
  }

  char msg[80];
  snprintf(msg, sizeof(msg), "== %d van tay ==", count);
  blynkTerminal.println(msg);

  for (uint8_t i = 0; i < count; i++) {
    snprintf(msg, sizeof(msg), "  ID #%d", ids[i]);
    blynkTerminal.println(msg);
  }
  blynkTerminal.flush();
}

// ========== RESET ALL FINGERPRINTS ==========
void resetAllFingerprints() {
  if (fingerprint.deleteAll()) {
    blynkTerminal.println("Đã xóa toàn bộ vân tay.");
  } else {
    blynkTerminal.println("Lỗi: Không thể xóa!");
  }
  blynkTerminal.flush();
}

// ========== PUSH ALL IDs TO GOOGLE SHEET ==========
void pushAllToSheet() {
  if (WiFi.status() != WL_CONNECTED) {
    blynkTerminal.println("Lỗi: Chưa kết nối WiFi!");
    blynkTerminal.flush();
    return;
  }

  uint8_t ids[127];
  uint8_t count = 0;
  fingerprint.listAllIds(ids, count);

  if (count == 0) {
    blynkTerminal.println("Không có vân tay để đẩy lên.");

    blynkTerminal.flush();
    return;
  }

  char msg[80];
  snprintf(msg, sizeof(msg), "Đang đẩy %d ID lên Sheet...", count);
  blynkTerminal.println(msg);
  blynkTerminal.flush();

  uint8_t ok = 0, fail = 0;
  for (uint8_t i = 0; i < count; i++) {
    if (sendStaffAction(ids[i], "add")) {
      ok++;
    } else {
      fail++;
    }
  }

  snprintf(msg, sizeof(msg), "Hoàn tất: %d thành công, %d thất bại", ok, fail);
  blynkTerminal.println(msg);
  blynkTerminal.flush();
}

// ========== STAFF SYNC ==========
bool sendStaffAction(int id, const char* action) {
  if (strlen(GOOGLE_SCRIPT_URL) == 0) return false;
  if (WiFi.status() != WL_CONNECTED) return false;

  String url = String(GOOGLE_SCRIPT_URL) + "?action=" + action + "&id=" + String(id);
  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.begin(url);
  int httpCode = http.GET();
  String resp = "";
  if (httpCode == 200) {
    resp = http.getString();
    resp.trim();
  }
  http.end();
  Serial.printf("[Sync] action=%s id=%d code=%d resp=%s\n", action, id, httpCode, resp.c_str());
  // Trả true nếu thành công HOẶC server báo không tìm thấy (đã xóa rồi)
  return (httpCode == 200 && (resp == "OK" || resp == "UPDATED" || resp == "NOT_FOUND"));
}

void syncStaffAdd(int id) {
  if (sendStaffAction(id, "add")) {
    char msg[60];
    snprintf(msg, sizeof(msg), "Đã đồng bộ ID #%d thành công", id);
    blynkTerminal.println(msg);
    blynkTerminal.flush();
  } else {
    eepromSavePending(SYNC_OP_ADD, (uint8_t)id);
    blynkTerminal.println("Đồng bộ thất bại - sẽ thử lại sau");
    blynkTerminal.flush();
  }
}

void syncStaffDelete(int id) {
  if (!sendStaffAction(id, "delete")) {
    eepromSavePending(SYNC_OP_DEL, (uint8_t)id);
  }
}

void eepromSavePending(uint8_t op, uint8_t id) {
  uint8_t count = EEPROM.read(1);
  if (count >= MAX_PENDING_SYNC) return;
  uint8_t base = 2 + count * 2;
  EEPROM.write(base,     op);
  EEPROM.write(base + 1, id);
  EEPROM.write(1, count + 1);
  EEPROM.commit();
  Serial.printf("[EEPROM] Saved pending op=%d id=%d (count=%d)\n", op, id, count + 1);
}

void eepromRemoveFirst() {
  uint8_t count = EEPROM.read(1);
  if (count == 0) return;
  // Shift all entries left by one slot
  for (uint8_t i = 0; i < count - 1; i++) {
    EEPROM.write(2 + i * 2,     EEPROM.read(2 + (i + 1) * 2));
    EEPROM.write(2 + i * 2 + 1, EEPROM.read(2 + (i + 1) * 2 + 1));
  }
  EEPROM.write(1, count - 1);
  EEPROM.commit();
}

void flushPendingSync() {
  uint8_t count = EEPROM.read(1);
  if (count == 0) return;
  if (WiFi.status() != WL_CONNECTED) return;

  uint8_t op = EEPROM.read(2);
  uint8_t id = EEPROM.read(3);
  const char* action = (op == SYNC_OP_ADD) ? "add" : "delete";

  if (sendStaffAction(id, action)) {
    eepromRemoveFirst();
    char msg[70];
    snprintf(msg, sizeof(msg), "Đồng bộ lại ID #%d (%s) thành công", id, action);
    if (Blynk.connected()) {
      blynkTerminal.println(msg);
      blynkTerminal.flush();
    }
    Serial.println(msg);
  }
}

// ========== GOOGLE SHEETS - GỬI LOG CHẤM CÔNG ==========
void sendToGoogleSheet(int fingerId) {
  if (strlen(GOOGLE_SCRIPT_URL) == 0) return;
  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("[Sheet] Không có WiFi - bỏ qua log ID #%d\n", fingerId);
    return;
  }

  String timeStr = getTimeString();

  // Tạo URL với action=log tường minh
  // Chỉ encode phần time (chứa dấu : và space), KHÔNG encode toàn bộ URL
  String timeEncoded = timeStr;
  timeEncoded.replace(":", "%3A");
  timeEncoded.replace(" ", "%20");
  String url = String(GOOGLE_SCRIPT_URL) + "?action=log&id=" + String(fingerId) + "&time=" + timeEncoded;

  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.setTimeout(10000);  // Timeout 10 giây tránh treo
  http.begin(url);
  int httpCode = http.GET();
  String resp = "";
  if (httpCode > 0) {
    resp = http.getString();
    resp.trim();
  }
  http.end();

  if (httpCode == 200 && resp == "OK") {
    Serial.printf("[Sheet] Gửi log ID #%d → OK\n", fingerId);
  } else {
    Serial.printf("[Sheet] THẤT BẠI ID #%d - code=%d resp=%s\n", fingerId, httpCode, resp.c_str());
  }
}