#include "cloud_logger.h"
#include "config.h"

// Global instance
CloudLogger cloudLogger;

CloudLogger::CloudLogger() {
  googleScriptUrl = "";
  googleScriptId = "";
}

void CloudLogger::begin(String scriptUrl, String scriptId) {
  googleScriptUrl = scriptUrl;
  googleScriptId = scriptId;
  
  Serial.println("[CloudLog] Google Sheets integration initialized");
  Serial.print("[CloudLog] Webhook URL: ");
  Serial.println(googleScriptUrl);
}

bool CloudLogger::logAccess(String empId, String empName, String action, bool success) {
  String status = success ? "SUCCESS" : "FAILED";
  String timestamp = currentTimestamp();
  
  Serial.print("[CloudLog] Logging access: ");
  Serial.print(empName);
  Serial.print(" - ");
  Serial.print(action);
  Serial.print(" - ");
  Serial.println(status);
  
  // Build webhook URL with parameters
  String url = googleScriptUrl + "?";
  url += "type=access&";
  url += "empid=" + empId + "&";
  url += "empname=" + empName + "&";
  url += "action=" + action + "&";
  url += "status=" + status + "&";
  url += "timestamp=" + timestamp;
  
  return makeHttpRequest(url);
}

bool CloudLogger::logAlert(String alertType, float value, String severity) {
  String timestamp = currentTimestamp();
  
  Serial.print("[CloudLog] Logging alert: ");
  Serial.print(alertType);
  Serial.print(" (");
  Serial.print(value);
  Serial.print(") - ");
  Serial.println(severity);
  
  // Build webhook URL
  String url = googleScriptUrl + "?";
  url += "type=alert&";
  url += "alerttype=" + alertType + "&";
  url += "value=" + String(value) + "&";
  url += "severity=" + severity + "&";
  url += "timestamp=" + timestamp;
  
  return makeHttpRequest(url);
}

bool CloudLogger::logDeviceStatus(String status, String details) {
  String timestamp = currentTimestamp();
  
  Serial.print("[CloudLog] Logging device status: ");
  Serial.print(status);
  Serial.print(" - ");
  Serial.println(details);
  
  String url = googleScriptUrl + "?";
  url += "type=status&";
  url += "status=" + status + "&";
  url += "details=" + details + "&";
  url += "timestamp=" + timestamp;
  
  return makeHttpRequest(url);
}

void CloudLogger::blynkLogAccess(String empName, String action) {
  // Alternative: Log to Blynk Events instead
  String event = action == "IN" ? "employee_checkin" : "employee_checkout";
  // Blynk.logEvent(event, empName);
  
  Serial.print("[CloudLog] Blynk logged: ");
  Serial.print(event);
  Serial.print(" - ");
  Serial.println(empName);
}

void CloudLogger::blynkLogAlert(String alertType, String value) {
  // Alternative: Log alerts to Blynk Events
  String event = "alert_" + alertType;
  // Blynk.logEvent(event, value);
  
  Serial.print("[CloudLog] Blynk alert logged: ");
  Serial.print(event);
  Serial.print(" - ");
  Serial.println(value);
}

bool CloudLogger::uploadDataToFirebase(String path, String jsonData) {
  Serial.println("[CloudLog] Firebase integration not yet implemented");
  return false;
}

bool CloudLogger::queryEmployeeDatabase(String empId) {
  Serial.println("[CloudLog] Employee database query not yet implemented");
  return false;
}

bool CloudLogger::isConnected() {
  return WiFi.status() == WL_CONNECTED && googleScriptUrl != "";
}

bool CloudLogger::testConnection() {
  Serial.println("[CloudLog] Testing connection to Google Sheets...");
  
  String url = googleScriptUrl + "?type=test&timestamp=" + currentTimestamp();
  
  if (makeHttpRequest(url)) {
    Serial.println("[CloudLog] ✓ Connection successful!");
    return true;
  } else {
    Serial.println("[CloudLog] ✗ Connection failed!");
    return false;
  }
}

String CloudLogger::buildWebhookUrl(String action, String params) {
  return googleScriptUrl + "?" + params;
}

String CloudLogger::jsonEncode(String key, String value) {
  return "\"" + key + "\":\"" + value + "\"";
}

String CloudLogger::currentTimestamp() {
  time_t now = time(nullptr);
  struct tm timeinfo = *localtime(&now);
  
  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  
  return String(buffer);
}

bool CloudLogger::makeHttpRequest(String url) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[CloudLog] WiFi not connected!");
    return false;
  }
  
  HTTPClient http;
  http.setConnectTimeout(5000);
  http.setTimeout(10000);
  
  Serial.print("[CloudLog] Making request: ");
  Serial.println(url);
  
  if (!http.begin(url)) {
    Serial.println("[CloudLog] Failed to initialize HTTP request");
    return false;
  }
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    Serial.print("[CloudLog] Response code: ");
    Serial.println(httpResponseCode);
    
    String response = http.getString();
    Serial.print("[CloudLog] Response: ");
    Serial.println(response);
    
    http.end();
    return httpResponseCode == 200;
  } else {
    Serial.print("[CloudLog] Error code: ");
    Serial.println(httpResponseCode);
    http.end();
    return false;
  }
}
