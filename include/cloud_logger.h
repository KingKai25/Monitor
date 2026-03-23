#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Google Sheets & Cloud Integration Handler
class CloudLogger {
private:
  String googleScriptUrl;
  String googleScriptId;
  HTTPClient httpClient;
  
  // Data structures for logging
  struct AccessLog {
    String employeeId;
    String employeeName;
    String fingerprint;
    String timestamp;
    String action;           // "IN" or "OUT"
    String status;           // "SUCCESS" or "FAILED"
  };
  
  struct AlertLog {
    String alertType;        // "FIRE", "SMOKE", "TEMP"
    String value;
    String timestamp;
    String severity;         // "CRITICAL", "WARNING"
  };

public:
  CloudLogger();
  
  // Initialize with Google Script URL
  void begin(String scriptUrl, String scriptId);
  
  // Logging functions
  bool logAccess(String empId, String empName, String action, bool success);
  bool logAlert(String alertType, float value, String severity);
  bool logDeviceStatus(String status, String details);
  
  // Blynk Event Logging (alternative/backup)
  void blynkLogAccess(String empName, String action);
  void blynkLogAlert(String alertType, String value);
  
  // Data functions for future expansion
  bool uploadDataToFirebase(String path, String jsonData);
  bool queryEmployeeDatabase(String empId);
  
  // Status
  bool isConnected();
  bool testConnection();
  
private:
  // Helper functions
  String buildWebhookUrl(String action, String params);
  String jsonEncode(String key, String value);
  String currentTimestamp();
  bool makeHttpRequest(String url);
};

// External instance
extern CloudLogger cloudLogger;
