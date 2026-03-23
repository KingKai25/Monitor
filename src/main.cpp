#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <HardwareSerial.h>
#include <time.h>
#include "cloud_logger.h"
#include "fingerprint_sensor.h"
#include "power_monitor.h"

// ========== BLYNK CONFIGURATION (from config.h) ==========
#define BLYNK_PRINT Serial

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

// ========== PIN DEFINITIONS (from config.h) ==========
#define DHT_PIN DHT11_PIN
#define DHT_TYPE DHT11

// ========== GLOBALS & THRESHOLDS ==========
DHT dht(DHT_PIN, DHT_TYPE);
HardwareSerial SerialFinger(2);  // UART2 for AS608
HardwareSerial SerialPZEM(1);    // UART1 for PZEM

// ========== DRIVER INSTANCES ==========
CloudLogger cloudLogger;                    // Google Sheets logger
FingerprintSensor fingerprint(SerialFinger); // AS608 via Serial2
PowerMonitor powerMonitor(SerialPZEM);      // PZEM-004T via Serial1

float temperature = 0;
float humidity = 0;
float voltage = 0;
float current = 0;
float power = 0;
float energy = 0;
int smokeLevel = 0;
bool lightState = false;
bool fireAlert = false;
bool smokeAlert = false;

// Thresholds (from config.h)

unsigned long lastDHTRead = 0;
unsigned long lastPZEMRead = 0;
unsigned long lastMQ2Read = 0;
unsigned long dhtReadInterval = 5000;     // 5 seconds
unsigned long pzemReadInterval = 2000;    // 2 seconds
unsigned long mq2ReadInterval = 1000;     // 1 second

// Button debounce & reconnect timing
volatile bool buttonPressed = false;
unsigned long lastWifiRetry = 0;
unsigned long lastBlynkRetry = 0;
const unsigned long wifiRetryInterval = 5000;    // 5 seconds
const unsigned long blynkRetryInterval = 3000;   // 3 seconds
const unsigned long debounceDelay = 200;

// ========== FUNCTION DECLARATIONS ==========
void setupWiFi();
void setupDHT();
void setupFingerprintSensor();
void setupPZEM();
void setupMQ2();
void readDHTSensors();
void readMQ2Smoke();
void readPZEMData();
void checkFireAlert();
void controlLight(bool state);
void soundBuzzer(int duration);
void buttonInterrupt();
void handleFingerprintEnrollment();
void handleFingerprintVerification();
void logToGoogleSheets(String name, String action);
void updateBlynkValues();

// ========== BLYNK VIRTUAL PINS ==========
BLYNK_WRITE(V1) {  // Light control from app
  int value = param.asInt();
  controlLight(value);
}

BLYNK_WRITE(V2) {  // Buzzer test from app
  soundBuzzer(500);
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V1);
  Blynk.logEvent("device_online", "ESP32 Warehouse Monitor is online");
}

// ========== SETUP FUNCTION ==========
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("========== WAREHOUSE MONITOR STARTING ==========");
  
  // Pin initialization
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FIRE_SENSOR_PIN, INPUT);
  pinMode(MQ2_PIN, INPUT);
  
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Initialize sensors
  setupWiFi();
  setupDHT();
  setupPZEM();
  setupMQ2();
  setupFingerprintSensor();
  
  // Initialize cloud & sensor drivers
  cloudLogger.begin(GOOGLE_SHEET_WEBHOOK, GOOGLE_SCRIPT_ID);
  fingerprint.begin(FINGERPRINT_BAUD);
  powerMonitor.begin(PZEM_BAUD);
  
  // Attach button interrupt
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, FALLING);
  
  // Initialize Blynk (separate config and connect)
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect(5000);
  
  Serial.println("Setup complete!");
}

// ========== MAIN LOOP ==========
void loop() {
  unsigned long now = millis();
  
  // Handle button press from ISR
  if (buttonPressed) {
    buttonPressed = false;
    controlLight(!lightState);
  }
  
  // Retry WiFi connection on interval (not every loop)
  if (WiFi.status() != WL_CONNECTED && now - lastWifiRetry >= wifiRetryInterval) {
    lastWifiRetry = now;
    Serial.println("\n[WiFi] Attempting to reconnect...");
    setupWiFi();
  }
  
  // Only run Blynk if WiFi is connected
  if (WiFi.status() == WL_CONNECTED) {
    if (!Blynk.connected() && now - lastBlynkRetry >= blynkRetryInterval) {
      lastBlynkRetry = now;
      Serial.println("[Blynk] Attempting to reconnect...");
      Blynk.connect(3000);
    }
    
    Blynk.run();
  }
  
  unsigned long currentTime = millis();
  
  // Read DHT sensors
  if (currentTime - lastDHTRead >= dhtReadInterval) {
    readDHTSensors();
    lastDHTRead = currentTime;
  }
  
  // Read MQ2 smoke sensor
  if (currentTime - lastMQ2Read >= mq2ReadInterval) {
    readMQ2Smoke();
    lastMQ2Read = currentTime;
  }
  
  // Read PZEM power data
  if (currentTime - lastPZEMRead >= pzemReadInterval) {
    readPZEMData();
    lastPZEMRead = currentTime;
  }
  
  // Check fire alert
  checkFireAlert();
  
  // Update Blynk values
  updateBlynkValues();
  
  delay(100);
}

// ========== WIFI SETUP ==========
void setupWiFi() {
  Serial.print("[WiFi] Connecting to: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  
  // Handle open networks (no password)
  if (strlen(pass) == 0) {
    Serial.println("[WiFi] Open network detected (no password)");
    WiFi.begin(ssid);
  } else {
    WiFi.begin(ssid, pass);
  }
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] ✓ Connected!");
    Serial.print("[WiFi] IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WiFi] ✗ Connection failed");
  }
}

// ========== DHT SENSOR SETUP & READ ==========
void setupDHT() {
  dht.begin();
  Serial.println("DHT11 initialized");
}

void readDHTSensors() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C | Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  
  // Check temperature threshold for fire alert
  if (temperature > TEMP_THRESHOLD) {
    smokeAlert = true;
    soundBuzzer(200);
  }
}

// ========== MQ2 SMOKE SENSOR ==========
void setupMQ2() {
  pinMode(MQ2_PIN, INPUT);
  Serial.println("MQ2 smoke sensor initialized");
}

void readMQ2Smoke() {
  smokeLevel = analogRead(MQ2_PIN);
  
  Serial.print("Smoke Level: ");
  Serial.println(smokeLevel);
  
  if (smokeLevel > SMOKE_THRESHOLD) {
    smokeAlert = true;
    soundBuzzer(100);
    Blynk.logEvent("smoke_alert", "High smoke levels detected: " + String(smokeLevel));
  } else {
    smokeAlert = false;
  }
}

// ========== PZEM-004T POWER MONITORING ==========
void setupPZEM() {
  SerialPZEM.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);
  Serial.println("PZEM-004T power monitor initialized at pins PZEM_RX=" + String(PZEM_RX) + ", PZEM_TX=" + String(PZEM_TX));
}

void readPZEMData() {
  // Read from PZEM-004T via powerMonitor driver
  // Methods return bool (success/fail), need to pass references
  powerMonitor.readVoltage(voltage);
  powerMonitor.readCurrent(current);
  powerMonitor.readPower(power);
  powerMonitor.readEnergy(energy);
  
  Serial.print("[PZEM] Voltage: ");
  Serial.print(voltage);
  Serial.print("V | Current: ");
  Serial.print(current);
  Serial.print("A | Power: ");
  Serial.print(power);
  Serial.print("W | Energy: ");
  Serial.print(energy);
  Serial.println("kWh");
}

// ========== FINGERPRINT SENSOR SETUP ==========
void setupFingerprintSensor() {
  // Fingerprint is initialized via fingerprint.begin() in setup()
  Serial.println("[AS608] Fingerprint sensor initialized");
  Serial.println("[AS608] Module ready for enrollment/verification");
}

void handleFingerprintEnrollment() {
  Serial.println("[AS608] Starting enrollment process...");
  // Call fingerprint driver to enroll
  uint8_t empId = 1;  // TODO: Get from app/database
  if (fingerprint.enrollNewFingerprint(empId)) {
    Serial.print("[AS608] Employee ");
    Serial.print(empId);
    Serial.println(" enrolled successfully");
    cloudLogger.logAccess(String(empId), "New Employee", "ENROLLED", true);
  } else {
    Serial.println("[AS608] Enrollment failed");
  }
}

void handleFingerprintVerification() {
  Serial.println("[AS608] Waiting for fingerprint...");
  uint8_t matchedId = 0;
  
  if (fingerprint.verifyFingerprint(matchedId)) {
    Serial.print("[AS608] Match found! Employee ID: ");
    Serial.println(matchedId);
    
    // Log to Google Sheets
    String empName = "Employee_" + String(matchedId);  // TODO: Get from database
    cloudLogger.logAccess(String(matchedId), empName, "IN", true);
    
    // Notify Blynk
    Blynk.logEvent("fingerprint_match", empName);
  } else {
    Serial.println("[AS608] No match found");
    cloudLogger.logAccess("UNKNOWN", "Unknown", "DENIED", false);
  }
}

// ========== FIRE SENSOR ==========
void checkFireAlert() {
  int fireValue = analogRead(FIRE_SENSOR_PIN);
  
  if (fireValue > FIRE_THRESHOLD) {
    fireAlert = true;
    soundBuzzer(300);
    Blynk.logEvent("fire_alert", "FIRE ALERT! Value: " + String(fireValue));
    Serial.println("🔥 FIRE ALERT! 🔥");
  } else {
    fireAlert = false;
  }
}

// ========== RELAY CONTROL ==========
void controlLight(bool state) {
  lightState = state;
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);
  
  if (state) {
    Serial.println("💡 Light turned ON");
    Blynk.logEvent("light_on", "Light switched on");
  } else {
    Serial.println("💡 Light turned OFF");
    Blynk.logEvent("light_off", "Light switched off");
  }
}

// ========== BUZZER ALERT ==========
void soundBuzzer(int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

// ========== BUTTON INTERRUPT ==========
void IRAM_ATTR buttonInterrupt() {
  // Set flag instead of doing heavy work in ISR
  buttonPressed = true;
}

// ========== GOOGLE SHEETS INTEGRATION ==========
void logToGoogleSheets(String name, String action) {
  // TODO: Implement Google Sheets API integration
  // Option 1: Use Google Script as webhook
  // Option 2: Use Firebase Realtime Database as middleware
  
  Serial.print("Logging to Google Sheets: ");
  Serial.print(name);
  Serial.print(" - ");
  Serial.println(action);
  
  // Example webhook URL (replace with your own):
  // String url = "https://script.google.com/macros/d/YOUR_SCRIPT_ID/usercallback?name=" + name + "&action=" + action;
  // TODO: Make HTTP GET request to webhook
}

// ========== BLYNK VALUE UPDATES ==========
void updateBlynkValues() {
  // Only update if Blynk is connected
  if (!Blynk.connected()) return;
  
  Blynk.virtualWrite(V3, temperature);    // Temperature
  Blynk.virtualWrite(V4, humidity);       // Humidity
  Blynk.virtualWrite(V5, smokeLevel);     // Smoke level
  Blynk.virtualWrite(V6, power);          // Power consumption
  Blynk.virtualWrite(V7, energy);         // Energy consumption
  Blynk.virtualWrite(V8, voltage);        // Voltage
  Blynk.virtualWrite(V9, current);        // Current
  
  // Status indicators
  Blynk.virtualWrite(V10, fireAlert ? 1 : 0);   // Fire alert
  Blynk.virtualWrite(V11, smokeAlert ? 1 : 0);  // Smoke alert
  Blynk.virtualWrite(V12, lightState ? 1 : 0);  // Light state
}