/*
 * ============================================================
 *  Module đo điện năng PZEM-004T v3
 *  Dùng thư viện mandulka/PZEM-004T-v30
 *  Slave ID mặc định: 0x01 | Function code: 0x04
 * ============================================================
 */

#include "power_monitor.h"
#include "config.h"

void PowerMonitor::begin(uint32_t baudrate) {
  // Khởi tạo lại UART tại đây (trong setup) để ghi đè lên global constructor
  _serial.begin(baudrate, SERIAL_8N1, PZEM_RX, PZEM_TX);
  delay(2000);  // PZEM cần ~2 giây để khởi động
  Serial.println("[PowerMon] PZEM-004T v3 đã khởi tạo (slave 0x01)");
  if (isConnected()) {
    Serial.println("[PowerMon] ✓ PZEM kết nối thành công!");
    printStatus();
  } else {
    Serial.println("[PowerMon] ✗ PZEM không phản hồi! Kiểm tra dây nối và nguồn AC 220V.");
  }
}

bool PowerMonitor::isConnected() {
  return !isnan(pzem.voltage());
}

bool PowerMonitor::readVoltage(float& voltage) {
  float v = pzem.voltage();
  if (isnan(v)) return false;
  voltage = v;
  return true;
}

bool PowerMonitor::readCurrent(float& current) {
  float i = pzem.current();
  if (isnan(i)) return false;
  current = i;
  return true;
}

bool PowerMonitor::readPower(float& power) {
  float p = pzem.power();
  if (isnan(p)) return false;
  power = p;
  return true;
}

bool PowerMonitor::readEnergy(float& energy) {
  float e = pzem.energy();
  if (isnan(e)) return false;
  energy = e;
  return true;
}

bool PowerMonitor::readFrequency(float& frequency) {
  float f = pzem.frequency();
  if (isnan(f)) return false;
  frequency = f;
  return true;
}

bool PowerMonitor::readPowerFactor(float& pf) {
  float p = pzem.pf();
  if (isnan(p)) return false;
  pf = p;
  return true;
}

bool PowerMonitor::readAll(float& v, float& i, float& p, float& e) {
  v = pzem.voltage();
  i = pzem.current();
  p = pzem.power();
  e = pzem.energy();
  return !isnan(v) && !isnan(p);
}

bool PowerMonitor::resetEnergy() {
  return pzem.resetEnergy();
}

void PowerMonitor::printStatus() {
  float v = pzem.voltage();
  float i = pzem.current();
  float p = pzem.power();
  float e = pzem.energy();
  float f = pzem.frequency();
  Serial.printf("[PZEM] %.1fV | %.2fA | %.1fW | %.3fkWh | %.1fHz\n",
    isnan(v)?0:v, isnan(i)?0:i, isnan(p)?0:p, isnan(e)?0:e, isnan(f)?0:f);
}