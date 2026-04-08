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

// EEPROM layout: [0]=magic(0xAB), [1]=count, [2..21]=pairs(op,id), max 10 entries
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

// ========== BLYNK HANDLERS ==========

// V3: Terminal - Nhập ID vân tay (+ID = thêm, -ID = xóa, all = liệt kê)
BLYNK_WRITE(V3) {
  String input = param.asStr();
  input.trim();

  // Lenh "all" - liet ke tat ca van tay
  if (input.equalsIgnoreCase("all")) {
    listAllFingerprints();
    return;
  }

  // Lenh "rst" - xoa toan bo van tay
  if (input.equalsIgnoreCase("rst")) {
    resetAllFingerprints();
    return;
  }

  // Lenh "push" - day toan bo ID len Google Sheet
  if (input.equalsIgnoreCase("push")) {
    pushAllToSheet();
    return;
  }

  int id = input.toInt();
  
  if (id == 0) {
    blynkTerminal.println("Lenh: +ID=THEM, -ID=XOA");
    blynkTerminal.println("all=LIET KE, rst=RESET, push=DAY LEN SHEET");
    blynkTerminal.flush();
    return;
  }

  if (id > 0) {
    // Kiem tra ID da ton tai chua
    if (fingerprint.isIdStored((uint8_t)id)) {
      char msg[60];
      snprintf(msg, sizeof(msg), "ID #%d da duoc them truoc do.", id);
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
  } else {
    uint8_t delId = (uint8_t)(-id);
    if (fingerprint.deleteFingerprint(delId)) {
      syncStaffDelete(delId);
    }
  }
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
void setup() {
  Serial.begin(SERIAL_MONITOR_BAUD);
  delay(1000);
  Serial.println("\n========== WAREHOUSE MONITOR ==========");

  EEPROM.begin(EEPROM_SIZE);
  if (EEPROM.read(0) != EEPROM_MAGIC_VAL) {
    EEPROM.write(0, EEPROM_MAGIC_VAL);
    EEPROM.write(1, 0);
    EEPROM.commit();
  }

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FIRE_SENSOR_PIN, INPUT);
  pinMode(MQ2_PIN, INPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  setupWiFi();
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  dht.begin();
  fingerprint.begin();
  fingerprint.setTerminalCallback(terminalPrint);
  powerMonitor.begin(PZEM_BAUD);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, FALLING);

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect(5000);

  Serial.println("Setup complete!");
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
void setupWiFi() {
  Serial.print("[WiFi] Connecting to: ");
  Serial.println(WIFI_SSID);
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
    Serial.print("\n[WiFi] Connected! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WiFi] Connection failed");
  }
}

// ========== DHT ==========
void readDHTSensors() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("[DHT] Read failed");
    return;
  }

  Serial.print("[DHT] T: ");
  Serial.print(temperature);
  Serial.print("°C | H: ");
  Serial.print(humidity);
  Serial.println("%");
}

// ========== MQ2 ==========
void readMQ2Smoke() {
  // Chỉ dùng digitalRead (KHÔNG dùng analogRead - ADC2 conflict WiFi)
  int dVal = digitalRead(MQ2_PIN);
  Serial.print("[MQ2] D=");
  Serial.println(dVal);
  
  // DO: LOW = có khói, HIGH = bình thường
  bool currentSmoke = (dVal == LOW);

  if (currentSmoke && !smokeAlert) {
    smokeAlert = true;
    soundBuzzer(300);
    if (Blynk.connected()) {
      Blynk.virtualWrite(V5, 255);
      Blynk.setProperty(V5, "color", "#FFA500");
      Blynk.logEvent("smoke_alert", "CANH BAO KHOI!");
    }
    Serial.println("[SMOKE] CANH BAO KHOI!");
  } else if (!currentSmoke && smokeAlert) {
    smokeAlert = false;
    if (Blynk.connected()) {
      Blynk.virtualWrite(V5, 255);
      Blynk.setProperty(V5, "color", "#FFFFFF");
    }
  }
}

// ========== PZEM ==========
void readPZEMData() {
  float v, i, e;
  powerMonitor.readVoltage(v);
  powerMonitor.readCurrent(i);
  powerMonitor.readPower(power);
  powerMonitor.readEnergy(e);

  Serial.print("[PZEM] Power: ");
  Serial.print(power);
  Serial.println("W");
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

  // V1: Công suất (W) - SuperChart
  Blynk.virtualWrite(V1, power);

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
    String timeStr = getTimeString();
    char msg[80];
    snprintf(msg, sizeof(msg), "Vân tay hợp lệ: ID #%d | %s", id, timeStr.c_str());
    Serial.println(msg);

    if (Blynk.connected()) {
      blynkTerminal.println(msg);
      blynkTerminal.flush();
    }

    sendToGoogleSheet(id);
  } else if (id == -1) {
    // getImage failed = no finger or bad read, ignore silently
  }
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
  return (httpCode == 200 && (resp == "OK" || resp == "UPDATED"));
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

// ========== GOOGLE SHEETS ==========
void sendToGoogleSheet(int fingerId) {
  if (strlen(GOOGLE_SCRIPT_URL) == 0) return;
  if (WiFi.status() != WL_CONNECTED) return;

  String timeStr = getTimeString();
  String url = String(GOOGLE_SCRIPT_URL) + "?id=" + String(fingerId) + "&time=" + timeStr;
  url.replace(" ", "%20");

  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.begin(url);
  int httpCode = http.GET();
  http.end();

  if (httpCode == 200) {
    Serial.printf("[Sheet] Sent ID #%d OK\n", fingerId);
  } else {
    Serial.printf("[Sheet] Error: %d\n", httpCode);
  }
}