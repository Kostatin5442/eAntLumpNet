#include <Arduino.h>
#include "webhandlers.h"
#include "index_html.h"
#include "wifi_manager.h"
// ==========================================
// 🔍 Читання збережених WiFi даних
/*==========================================
bool loadSavedWifi(String &ssid, String &password) {
  wifiPrefs.begin("wifi", true);
  bool configured = wifiPrefs.getBool("configured", false);
  if (configured) {
    ssid = wifiPrefs.getString("ssid", "");
    password = wifiPrefs.getString("pass", "");
  }
  wifiPrefs.end();
  return configured;
}
*/
// 📡 Сторінка налаштування
// ==========================================
void handleSetupPage() {
  server.send_P(200, "text/html", SETUP_PAGE_HTML);
}
// ==========================================
// 📡 Сканування WiFi мереж
// ==========================================
void handleScanNetworks() {
  Serial.println("🔍 Сканування WiFi мереж...");
  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";
    String ssid = WiFi.SSID(i);
    // Екрануємо спецсимволи для JSON
    ssid.replace("\"", "\\\"");
    json += "{";
    json += "\"ssid\":\"" + ssid + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
    json += "\"lock\":" + String(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "false" : "true");
    json += "}";
  }
  json += "]";
  server.send(200, "application/json", json);
  WiFi.scanDelete();
}
// ==========================================
// 💾 Збереження WiFi налаштувань
// ==========================================
void handleSaveWifi() {
  if (!server.hasArg("ssid")) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"SSID missing\"}");
    return;
  }
  
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  
  Serial.println("💾 Збереження WiFi: " + ssid);
  
  wifiPrefs.begin("wifi", false);
  wifiPrefs.putString("ssid", ssid);
  wifiPrefs.putString("pass", password);
  wifiPrefs.putBool("configured", true);
  wifiPrefs.end();
  
  server.send(200, "application/json", "{\"success\":true}");
  
  // Перезавантаження через 2 секунди (даємо час відправити відповідь)
  delay(2000);
  ESP.restart();
}
// ✅ НОВИЙ: Обробник для отримання статусу WiFi
// ==========================================
// 📡 API: Повертає статус Wi-Fi у форматі JSON
// ==========================================
void handleWifiStatusApi() {
  String state = (WiFi.status() == WL_CONNECTED) ? "Підключено" : "Не підключено";
  
  String mode = "Невідомо";
  if (WiFi.getMode() == WIFI_AP) mode = "Точка доступу (AP)";
  else if (WiFi.getMode() == WIFI_STA) mode = "Клієнт (STA)";
  else if (WiFi.getMode() == WIFI_AP_STA) mode = "AP + STA";

  // Визначаємо IP залежно від режиму
  String ip = (WiFi.getMode() == WIFI_AP) ? WiFi.softAPIP().toString() : WiFi.localIP().toString();

  String json = "{";
  json += "\"state\":\"" + state + "\",";
  json += "\"ssid\":\"" + String(WiFi.SSID()) + "\",";
  json += "\"ip\":\"" + ip + "\",";
  json += "\"mac\":\"" + WiFi.macAddress() + "\",";
  json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  json += "\"mode\":\"" + mode + "\"";
  json += "}";

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "application/json", json);
}
// ==========================================
// 🌐 HTML: Віддає красиву сторінку
// ==========================================
void handleWifiPage() {
  server.send_P(200, "text/html", WIFI_PAGE_HTML);
}
// ==========================================
// 🎛️ Обробник веб-панелі керування
// ==========================================
void handleControlPage() {
  server.send_P(200, "text/html", CONTROL_PAGE_HTML);
}

// ==========================================
// 🗑️ Скидання WiFi налаштувань (викликає clearSavedWifi з wifi_manager)
// ==========================================
void handleResetWifi() {
  Serial.println("🗑️ Скидання WiFi налаштувань...");
  clearSavedWifi();  // ✅ Використовуємо функцію з wifi_manager.h
  
  server.send(200, "application/json", "{\"success\":true}");
  
  delay(1500);
  ESP.restart();
}

//========================================================
// ==========================================
// 🎨 Встановлення статичного кольору
// ==========================================
void handleSetColor() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Missing value param");
    return;
  }
  String hex = server.arg("value");
  Serial.println("🎨 Колір: #" + hex);
  
  // Тут викликаємо функцію встановлення кольору на матриці
  // (потрібно додати в main.cpp або окремий модуль)
  extern void setStaticColor(const String &hex);
  setStaticColor(hex);
  
  server.send(200, "text/plain", "OK");
}

// ==========================================
// ⚡ Встановлення швидкості ефектів
// ==========================================
void handleSetSpeed() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Missing value param");
    return;
  }
  int speed = server.arg("value").toInt();
  speed = constrain(speed, 10, 200);
  Serial.println("⚡ Швидкість: " + String(speed));
  
  extern void setEffectSpeed(int speed);
  setEffectSpeed(speed);
  
  server.send(200, "text/plain", "OK");
}

// ==========================================
// 🔄 Перезавантаження пристрою
// ==========================================
void handleReboot() {
  server.send(200, "application/json", "{\"success\":true}");
  delay(500);
  ESP.restart();
}

// ==========================================
// 💻 API: Повна системна інформація (JSON)
// ==========================================
void handleSystemInfoApi() {
  // WiFi дані
  String state = (WiFi.status() == WL_CONNECTED) ? "Підключено" : "Не підключено";
  String mode = "Невідомо";
  if (WiFi.getMode() == WIFI_AP) mode = "Точка доступу (AP)";
  else if (WiFi.getMode() == WIFI_STA) mode = "Клієнт (STA)";
  else if (WiFi.getMode() == WIFI_AP_STA) mode = "AP + STA";
  String ip = (WiFi.getMode() == WIFI_AP) ? WiFi.softAPIP().toString() : WiFi.localIP().toString();

  // Uptime
  unsigned long uptimeMs = millis();
  unsigned long sec = uptimeMs / 1000;
  unsigned long min = sec / 60;
  unsigned long hr = min / 60;
  unsigned long days = hr / 24;
  String uptime = String(days) + "д " + String(hr % 24) + "г " + String(min % 60) + "хв";

  // Heap & CPU
  uint32_t freeHeap = ESP.getFreeHeap() / 1024;
  uint32_t cpuFreq = ESP.getCpuFreqMHz();
  
  // Chip model
  String chipModel = "ESP32";
  #if CONFIG_IDF_TARGET_ESP32C3
    chipModel = "ESP32-C3";
  #elif CONFIG_IDF_TARGET_ESP32S2
    chipModel = "ESP32-S2";
  #elif CONFIG_IDF_TARGET_ESP32S3
    chipModel = "ESP32-S3";
  #endif

  // Поточний ефект (потрібно оголосити як extern)
  extern String getCurrentEffectName();
  String effectName = getCurrentEffectName();

  String json = "{";
  json += "\"wifi\":{";
  json += "\"state\":\"" + state + "\",";
  json += "\"ssid\":\"" + String(WiFi.SSID()) + "\",";
  json += "\"ip\":\"" + ip + "\",";
  json += "\"mac\":\"" + WiFi.macAddress() + "\",";
  json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  json += "\"mode\":\"" + mode + "\"";
  json += "},";
  json += "\"system\":{";
  json += "\"uptime\":\"" + uptime + "\",";
  json += "\"heap\":" + String(freeHeap) + ",";
  json += "\"cpu\":" + String(cpuFreq) + ",";
  json += "\"chip\":\"" + chipModel + "\"";
  json += "},";
  json += "\"currentEffect\":\"" + effectName + "\"";
  json += "}";

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.send(200, "application/json", json);
}