#include <Arduino.h>
#include "webhandlers.h"
#include "index_html.h"
//Роути для налаштауваня модуля на з'єднання
// ==========================================
// 🔍 Читання збережених WiFi даних
// ==========================================
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
// ==========================================
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
// 🎨 Сторінка Pixel Art Studio
// ==========================================
void handlePixelArtPage() {
  server.send_P(200, "text/html", PIXEL_ART_HTML);
  delay(100);
}
// ==========================================
//  Отримання даних Pixel Art
// ==========================================
void handlePixelArtData() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No data");
    return;
  }
  
  String json = server.arg("plain");
  Serial.println("🎨 Отримано Pixel Art дані: " + json);

  // Тут можна розпарсити JSON і встановити кольори на матриці
  // Для простоти - просто підтверджуємо отримання
  server.send(200, "text/plain", "OK");
}
//=================================================================