// Google Apps Script for Warehouse Monitor
// Deploy as web app (Execute as: Your Account, Who has access: Anyone)
// Copy the deployment URL and script ID to config.h

// Global configuration
const SPREADSHEET_ID = ""; // Leave empty - will use active sheet
const SHEET_NAMES = {
  access: "AccessLog",
  alerts: "AlertLog",
  status: "DeviceStatus"
};

/**
 * Main entry point - handles incoming webhook requests
 */
function doGet(e) {
  try {
    const spreadsheet = SpreadsheetApp.getActiveSpreadsheet();
    const params = e.parameter;
    
    // Log all parameters
    logToSheet("System", "INFO", "Webhook received: " + JSON.stringify(params));
    
    // Route based on type parameter
    const type = params.type || "unknown";
    
    switch(type) {
      case "access":
        return handleAccessLog(spreadsheet, params);
      
      case "alert":
        return handleAlertLog(spreadsheet, params);
      
      case "status":
        return handleStatusLog(spreadsheet, params);
      
      case "test":
        return handleTestConnection(spreadsheet, params);
      
      default:
        return createResponse(400, "Unknown request type: " + type);
    }
  } catch(error) {
    logToSheet("System", "ERROR", "Error in doGet: " + error.toString());
    return createResponse(500, "Internal server error: " + error.toString());
  }
}

/**
 * Handle access control logging (employee check-in/out)
 */
function handleAccessLog(spreadsheet, params) {
  try {
    const sheet = getOrCreateSheet(spreadsheet, SHEET_NAMES.access, [
      "Timestamp",
      "Employee ID",
      "Employee Name",
      "Action",
      "Status",
      "Device",
      "IP Address"
    ]);
    
    const row = [
      params.timestamp || getCurrentTimestamp(),
      params.empid || "UNKNOWN",
      params.empname || "Unknown",
      params.action || "UNKNOWN",
      params.status || "UNKNOWN",
      "ESP32_Warehouse",
      params.ip || "N/A"
    ];
    
    sheet.appendRow(row);
    
    logToSheet("AccessLog", "INFO", 
      "Logged: " + params.empname + " - " + params.action);
    
    return createResponse(200, "Access logged successfully");
  } catch(error) {
    logToSheet("AccessLog", "ERROR", error.toString());
    return createResponse(500, "Error in access logging: " + error.toString());
  }
}

/**
 * Handle alert logging (fire, smoke, temperature)
 */
function handleAlertLog(spreadsheet, params) {
  try {
    const sheet = getOrCreateSheet(spreadsheet, SHEET_NAMES.alerts, [
      "Timestamp",
      "Alert Type",
      "Value",
      "Unit",
      "Severity",
      "Threshold",
      "Action Taken"
    ]);
    
    const row = [
      params.timestamp || getCurrentTimestamp(),
      params.alerttype || "UNKNOWN",
      params.value || "N/A",
      getUnit(params.alerttype),
      params.severity || "WARNING",
      "", // Threshold - can be populated from config
      "Logged to sheet" // Action taken
    ];
    
    sheet.appendRow(row);
    
    // Send notification if critical
    if (params.severity === "CRITICAL") {
      sendEmail("CRITICAL: " + params.alerttype, 
        "Value: " + params.value + " at " + params.timestamp);
    }
    
    logToSheet("AlertLog", "INFO",
      "Alert: " + params.alerttype + " (" + params.value + ")");
    
    return createResponse(200, "Alert logged successfully");
  } catch(error) {
    logToSheet("AlertLog", "ERROR", error.toString());
    return createResponse(500, "Error in alert logging: " + error.toString());
  }
}

/**
 * Handle device status logging
 */
function handleStatusLog(spreadsheet, params) {
  try {
    const sheet = getOrCreateSheet(spreadsheet, SHEET_NAMES.status, [
      "Timestamp",
      "Device ID",
      "Status",
      "Details",
      "WiFi Signal",
      "Uptime"
    ]);
    
    const row = [
      params.timestamp || getCurrentTimestamp(),
      "ESP32_Warehouse",
      params.status || "UNKNOWN",
      params.details || "N/A",
      params.signal || "N/A",
      params.uptime || "N/A"
    ];
    
    sheet.appendRow(row);
    
    logToSheet("DeviceStatus", "INFO",
      "Status: " + params.status + " - " + params.details);
    
    return createResponse(200, "Status logged successfully");
  } catch(error) {
    logToSheet("DeviceStatus", "ERROR", error.toString());
    return createResponse(500, "Error in status logging: " + error.toString());
  }
}

/**
 * Handle test connection request
 */
function handleTestConnection(spreadsheet, params) {
  try {
    const sheet = getOrCreateSheet(spreadsheet, SHEET_NAMES.status, [
      "Timestamp",
      "Device ID",
      "Status",
      "Details",
      "WiFi Signal",
      "Uptime"
    ]);
    
    sheet.appendRow([
      params.timestamp || getCurrentTimestamp(),
      "TEST",
      "Connection Test",
      "Webhook connection verified",
      "N/A",
      "N/A"
    ]);
    
    return createResponse(200, "Connection test successful!");
  } catch(error) {
    return createResponse(500, "Connection test failed: " + error.toString());
  }
}

/**
 * Get or create a sheet with headers
 */
function getOrCreateSheet(spreadsheet, sheetName, headers) {
  let sheet = spreadsheet.getSheetByName(sheetName);
  
  if (!sheet) {
    sheet = spreadsheet.insertSheet(sheetName);
    sheet.appendRow(headers);
    
    // Format header row
    const range = sheet.getRange(1, 1, 1, headers.length);
    range.setBackground("#4285F4");
    range.setFontColor("#FFFFFF");
    range.setFontWeight("bold");
  }
  
  return sheet;
}

/**
 * Get unit for alert type
 */
function getUnit(alertType) {
  const units = {
    "TEMP": "°C",
    "HUMIDITY": "%",
    "SMOKE": "ppm",
    "FIRE": "ADC",
    "POWER": "W",
    "VOLTAGE": "V",
    "CURRENT": "A"
  };
  
  return units[alertType] || "N/A";
}

/**
 * Create JSON response
 */
function createResponse(code, message) {
  const response = {
    "code": code,
    "message": message,
    "timestamp": getCurrentTimestamp()
  };
  
  return ContentService
    .createTextOutput(JSON.stringify(response))
    .setMimeType(ContentService.MimeType.JSON);
}

/**
 * Log to system log sheet
 */
function logToSheet(source, level, message) {
  try {
    const spreadsheet = SpreadsheetApp.getActiveSpreadsheet();
    let logSheet = spreadsheet.getSheetByName("SystemLog");
    
    if (!logSheet) {
      logSheet = spreadsheet.insertSheet("SystemLog");
      logSheet.appendRow(["Timestamp", "Source", "Level", "Message"]);
    }
    
    logSheet.appendRow([
      getCurrentTimestamp(),
      source,
      level,
      message.substring(0, 200) // Limit message length
    ]);
    
    // Keep only last 1000 lines
    if (logSheet.getLastRow() > 1000) {
      logSheet.deleteRows(2, logSheet.getLastRow() - 1000);
    }
  } catch(e) {
    // Silent fail to avoid recursion
  }
}

/**
 * Send email notification for critical alerts
 */
function sendEmail(subject, body) {
  try {
    const userEmail = Session.getActiveUser().getEmail();
    
    GmailApp.sendEmail(userEmail, 
      "🚨 Warehouse Alert: " + subject,
      "CRITICAL ALERT!\n\n" + body + "\n\n" +
      "Time: " + getCurrentTimestamp() + "\n" +
      "Device: Warehouse Monitor (ESP32)\n\n" +
      "Please check immediately!");
  } catch(error) {
    logToSheet("Email", "ERROR", "Failed to send email: " + error.toString());
  }
}

/**
 * Get current timestamp in format: YYYY-MM-DD HH:MM:SS
 */
function getCurrentTimestamp() {
  const now = new Date();
  
  // Format: YYYY-MM-DD HH:MM:SS
  const year = now.getFullYear();
  const month = String(now.getMonth() + 1).padStart(2, '0');
  const date = String(now.getDate()).padStart(2, '0');
  const hours = String(now.getHours()).padStart(2, '0');
  const minutes = String(now.getMinutes()).padStart(2, '0');
  const seconds = String(now.getSeconds()).padStart(2, '0');
  
  return `${year}-${month}-${date} ${hours}:${minutes}:${seconds}`;
}

/**
 * Test function - run from Script Editor to verify
 */
function test() {
  const mockEvent = {
    parameter: {
      type: "access",
      empid: "001",
      empname: "John Doe",
      action: "IN",
      status: "SUCCESS",
      timestamp: getCurrentTimestamp()
    }
  };
  
  const response = doGet(mockEvent);
  Logger.log(response.getContent());
}

/**
 * Setup function - creates initial sheets
 */
function setup() {
  const spreadsheet = SpreadsheetApp.getActiveSpreadsheet();
  
  // Create base sheets
  getOrCreateSheet(spreadsheet, SHEET_NAMES.access, [
    "Timestamp", "Employee ID", "Employee Name", "Action", "Status", "Device", "IP Address"
  ]);
  
  getOrCreateSheet(spreadsheet, SHEET_NAMES.alerts, [
    "Timestamp", "Alert Type", "Value", "Unit", "Severity", "Threshold", "Action Taken"
  ]);
  
  getOrCreateSheet(spreadsheet, SHEET_NAMES.status, [
    "Timestamp", "Device ID", "Status", "Details", "WiFi Signal", "Uptime"
  ]);
  
  Logger.log("Setup complete! Sheets created successfully.");
}

/**
 * Delete data from sheets (except headers)
 */
function clearAllData() {
  const spreadsheet = SpreadsheetApp.getActiveSpreadsheet();
  
  const sheets = [SHEET_NAMES.access, SHEET_NAMES.alerts, SHEET_NAMES.status];
  sheets.forEach(sheetName => {
    const sheet = spreadsheet.getSheetByName(sheetName);
    if (sheet && sheet.getLastRow() > 1) {
      sheet.deleteRows(2, sheet.getLastRow() - 1);
    }
  });
  
  Logger.log("All data cleared!");
}

/**
 * Archive old data (older than 30 days)
 */
function archiveOldData() {
  const spreadsheet = SpreadsheetApp.getActiveSpreadsheet();
  const thirtyDaysAgo = new Date();
  thirtyDaysAgo.setDate(thirtyDaysAgo.getDate() - 30);
  
  const sheets = [SHEET_NAMES.access, SHEET_NAMES.alerts];
  sheets.forEach(sheetName => {
    const sheet = spreadsheet.getSheetByName(sheetName);
    if (!sheet) return;
    
    let rowsToDelete = [];
    const data = sheet.getDataRange().getValues();
    
    for (let i = 1; i < data.length; i++) {
      const cellDate = new Date(data[i][0]);
      if (cellDate < thirtyDaysAgo) {
        rowsToDelete.push(i + 1);
      }
    }
    
    // Delete in reverse order to maintain indices
    for (let i = rowsToDelete.length - 1; i >= 0; i--) {
      sheet.deleteRow(rowsToDelete[i]);
    }
  });
  
  Logger.log("Archive complete!");
}
