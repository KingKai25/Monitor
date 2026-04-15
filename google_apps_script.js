/**
 * ============================================================
 *  HỆ THỐNG GIÁM SÁT KHO HÀNG - Google Apps Script
 * ============================================================
 *  Script xử lý các yêu cầu từ ESP32 qua HTTP GET:
 *  - action=add    : Thêm nhân viên (vân tay) vào sheet "Nhân viên"
 *  - action=delete : Đánh dấu nhân viên đã xóa
 *  - action=log    : Ghi log chấm công vào sheet "Chấm công"
 *
 *  Cách deploy:
 *  1. Mở Google Sheets → Extensions → Apps Script
 *  2. Dán toàn bộ code này vào editor (xóa code cũ)
 *  3. Deploy → New deployment → Web app
 *     - Execute as: Me
 *     - Who has access: Anyone
 *  4. Copy URL → dán vào config.h (GOOGLE_SCRIPT_URL)
 *  5. Mỗi lần sửa code → Deploy → Manage deployments → Edit → New version
 * ============================================================
 */

function doGet(e) {
  try {
    var ss = SpreadsheetApp.getActiveSpreadsheet();
    var action = (e.parameter.action || "").toLowerCase();
    var id = parseInt(e.parameter.id || "0");

    // ===================================================
    //  ACTION = ADD: Thêm ID vào Sheet "Nhân viên"
    //  - Nếu ID đã tồn tại → cập nhật trạng thái "Đã đăng kí"
    //  - Nếu chưa có → thêm dòng mới
    // ===================================================
    if (action === "add") {
      var sheet = ss.getSheetByName("Nhân viên");
      if (!sheet) {
        // Tự tạo sheet nếu chưa có
        sheet = ss.insertSheet("Nhân viên");
        sheet.appendRow(["ID", "Tên", "Trạng thái"]);
      }
      var data = sheet.getDataRange().getValues();

      // Tìm dòng đã có ID này → cập nhật trạng thái
      for (var i = 1; i < data.length; i++) {
        if (data[i][0] == id) {
          sheet.getRange(i + 1, 3).setValue("Đã đăng kí");
          SpreadsheetApp.flush(); // Force ghi ngay lập tức
          return ContentService.createTextOutput("UPDATED");
        }
      }

      // Chưa có → thêm dòng mới: ID, (tên để trống), trạng thái
      sheet.appendRow([id, "", "Đã đăng kí"]);
      SpreadsheetApp.flush();
      return ContentService.createTextOutput("OK");
    }

    // ===================================================
    //  ACTION = DELETE: Đổi trạng thái thành "Đã xóa"
    //  - Tìm ID trong sheet "Nhân viên"
    //  - Nếu tìm thấy → đổi trạng thái
    //  - Nếu không tìm thấy → trả NOT_FOUND
    // ===================================================
    if (action === "delete") {
      var sheet = ss.getSheetByName("Nhân viên");
      if (!sheet) return ContentService.createTextOutput("NOT_FOUND");

      var data = sheet.getDataRange().getValues();
      for (var i = 1; i < data.length; i++) {
        if (data[i][0] == id) {
          sheet.getRange(i + 1, 3).setValue("Đã xóa");
          SpreadsheetApp.flush();
          return ContentService.createTextOutput("UPDATED");
        }
      }
      return ContentService.createTextOutput("NOT_FOUND");
    }

    // ===================================================
    //  ACTION = LOG: Ghi log chấm công vào sheet "Chấm công"
    //  - Nhận ID + time từ ESP32
    //  - Nếu time rỗng/N/A → dùng thời gian server (GMT+7)
    //  - Tự tạo header nếu sheet trống
    // ===================================================
    if (action === "log") {
      var time = e.parameter.time || "";
      var sheet = ss.getSheetByName("Chấm công");

      // Tự tạo sheet + header nếu chưa có
      if (!sheet) {
        sheet = ss.insertSheet("Chấm công");
        sheet.appendRow(["ID", "Tên nhân viên", "Thời gian"]);
      } else if (sheet.getLastRow() === 0) {
        sheet.appendRow(["ID", "Tên nhân viên", "Thời gian"]);
      }

      // Nếu ESP32 không gửi time hoặc NTP lỗi → dùng thời gian server
      if (time === "" || time === "N/A") {
        time = Utilities.formatDate(new Date(), "Asia/Ho_Chi_Minh", "yyyy-MM-dd HH:mm:ss");
      }

      // Tra cứu tên nhân viên từ sheet "Nhân viên" theo ID
      var ten = "";
      var sheetNV = ss.getSheetByName("Nhân viên");
      if (sheetNV) {
        var dataNV = sheetNV.getDataRange().getValues();
        for (var i = 1; i < dataNV.length; i++) {
          if (dataNV[i][0] == id) {
            ten = dataNV[i][1] || "";
            break;
          }
        }
      }

      sheet.appendRow([id, ten, time]);
      SpreadsheetApp.flush(); // Force commit ngay — tránh log bị chậm/mất
      return ContentService.createTextOutput("OK");
    }

    // ===================================================
    //  FALLBACK: Backward compatible — nếu không có action
    //  nhưng có id > 0 → coi như log chấm công (deprecated)
    // ===================================================
    if (id > 0) {
      var time = e.parameter.time || "";
      var sheet = ss.getSheetByName("Chấm công");

      if (!sheet) {
        sheet = ss.insertSheet("Chấm công");
        sheet.appendRow(["ID", "Tên nhân viên", "Thời gian"]);
      }

      if (time === "" || time === "N/A") {
        time = Utilities.formatDate(new Date(), "Asia/Ho_Chi_Minh", "yyyy-MM-dd HH:mm:ss");
      }

      // Tra cứu tên nhân viên (fallback)
      var ten = "";
      var sheetNV = ss.getSheetByName("Nhân viên");
      if (sheetNV) {
        var dataNV = sheetNV.getDataRange().getValues();
        for (var i = 1; i < dataNV.length; i++) {
          if (dataNV[i][0] == id) { ten = dataNV[i][1] || ""; break; }
        }
      }

      sheet.appendRow([id, ten, time]);
      SpreadsheetApp.flush();
      return ContentService.createTextOutput("OK");
    }

    // Không có action hợp lệ
    return ContentService.createTextOutput("NO_ACTION");

  } catch (err) {
    // Bắt mọi lỗi, trả về message để ESP32 debug qua Serial
    return ContentService.createTextOutput("ERROR: " + err.message);
  }
}
