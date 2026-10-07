#include "stats.h"
#include <time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <vector>
#include <map>

// Додай цю змінну на початку файлу
static uint32_t lastAutoSave = 0;

// ===== NTP =====
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200, 60000); // UTC+2 (Київ)





void initNTP() {
  timeClient.begin();
  timeClient.update();
  Serial.println("🕐 NTP час: " + timeClient.getFormattedTime());
}

uint32_t getUnixTime() {
  timeClient.update();
  return timeClient.getEpochTime();
}

String formatTimestamp(uint32_t ts) {
  time_t t = ts;
  struct tm* tm_info = localtime(&t);
  char buf[32];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
  return String(buf);
}

// ===== Змінні стану =====
static MonthlyStats currentStats;
static uint32_t sessionStart = 0;
static uint32_t lastEffectSwitch = 0;
static String currentEffectName = "OFF";
static bool sessionActive = false;
static std::vector<EffectRecord> pendingEffects; // Перенесено вгору

// ===== Ініціалізація =====
void initStats() {
  if (!LittleFS.begin(true)) {
    Serial.println("❌ Помилка LittleFS!");
    return;
  }
  Serial.println("✅ LittleFS ініціалізовано");
  
  if (!LittleFS.exists("/stats")) {
    LittleFS.mkdir("/stats");
  }
  
  String monthKey = getCurrentMonthKey();
  loadMonthlyStats(monthKey);
  
  // ✅ Спочатку скидаємо sessionStart
  sessionStart = 0;
  sessionActive = false;
  
  // ✅ Намагаємось відновити pending
  if (loadPendingEffects()) {
    sessionActive = true;
    Serial.println("▶️ Відновлено активну сесію з " + String(pendingEffects.size()) + " ефектів");
  } else {
    // Нова сесія
    startSession();
  }
  
  cleanupOldStats();
}
String getCurrentMonthKey() {
  timeClient.update();
  time_t now = timeClient.getEpochTime();
  struct tm* timeinfo = localtime(&now);
  char buf[16];
  sprintf(buf, "%04d_%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1);
  return String(buf);
}

// ===== Завантаження/Збереження =====
void loadMonthlyStats(const String& month) {
  currentStats.month = month;
  currentStats.totalUptime = 0;
  currentStats.sessions.clear();
  
  String path = "/stats/" + month + ".json";
  if (!LittleFS.exists(path)) {
    Serial.println("📅 Новий місяць: " + month);
    return;
  }
  
  File file = LittleFS.open(path, "r");
  if (!file) return;
  
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();
  
  if (error) {
    Serial.println("❌ Помилка читання JSON: " + String(error.c_str()));
    return;
  }
  
  currentStats.totalUptime = doc["total_uptime"] | 0;
  
  JsonArray sessions = doc["sessions"];
  for (JsonObject s : sessions) {
    SessionRecord session;
    session.startTime = s["start"];
    session.endTime = s["end"];
    session.duration = s["duration"];
    
    JsonArray effects = s["effects"];
    for (JsonObject e : effects) {
      EffectRecord rec;
      rec.name = e["name"].as<String>();
      rec.startTime = e["start"];
      rec.duration = e["duration"];
      session.effects.push_back(rec);
    }
    
    currentStats.sessions.push_back(session);
  }
  
  Serial.println("📊 Завантажено " + String(currentStats.sessions.size()) + " сесій за " + month);
}

void saveMonthlyStats() {
  String path = "/stats/" + currentStats.month + ".json";
  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.println("❌ Не вдалося зберегти статистику!");
    return;
  }
  
  JsonDocument doc;
  doc["month"] = currentStats.month;
  doc["total_uptime"] = currentStats.totalUptime;
  
  JsonArray sessions = doc["sessions"].to<JsonArray>();
  for (const auto& s : currentStats.sessions) {
    JsonObject sess = sessions.add<JsonObject>();
    sess["start"] = s.startTime;
    sess["end"] = s.endTime;
    sess["duration"] = s.duration;
    
    JsonArray effects = sess["effects"].to<JsonArray>();
    for (const auto& e : s.effects) {
      JsonObject eff = effects.add<JsonObject>();
      eff["name"] = e.name;
      eff["start"] = e.startTime;
      eff["duration"] = e.duration;
    }
  }
  
  serializeJson(doc, file);
  file.close();
  Serial.println("💾 Збережено статистику за " + currentStats.month);
  loadMonthlyStats(currentStats.month);
}

// ===== Сесії =====
void startSession() {
  if (sessionActive) return;
  sessionStart = getUnixTime();
  lastEffectSwitch = sessionStart;
  sessionActive = true;
  Serial.println("▶️ Сесія розпочата");
}

void endSession() {
  if (!sessionActive) return;
  
  uint32_t now = getUnixTime();
  uint32_t duration = now - sessionStart;
  
  // ✅ ЗАХИСТ: якщо sessionStart = 0, duration буде гігантським
  if (sessionStart == 0 || duration > 86400) {
    Serial.println("⚠️ sessionStart=0 або duration>доба, скидаємо duration");
    duration = 0;
  }
  
  recordEffectSwitch(currentEffectName, false);
  
  SessionRecord session;
  session.startTime = sessionStart;
  session.endTime = now;
  session.duration = duration;
  session.effects = pendingEffects;
  
  currentStats.sessions.push_back(session);
  currentStats.totalUptime += duration;
  
  sessionActive = false;
  pendingEffects.clear();
  clearPendingEffects();
  
  saveMonthlyStats();
  Serial.println("⏹️ Сесія завершена: " + String(duration) + " сек");
}
// ===== Ефекти =====
void recordEffectSwitch(const String& effectName, bool forceSave) {
  uint32_t now = getUnixTime();
  uint32_t duration = now - lastEffectSwitch;
  
  if (duration > 0 && duration < 86400) {
    EffectRecord rec;
    rec.name = currentEffectName;
    rec.startTime = lastEffectSwitch;
    rec.duration = duration;
    pendingEffects.push_back(rec);

    // ✅ Зберігаємо в файл після кожного перемикання
    savePendingEffects();

  }
  
  currentEffectName = effectName;
  lastEffectSwitch = now;
  
  if (forceSave) {
    saveMonthlyStats();
  }
}

// ===== API =====
String getStatsJSON(const String& month) {
  MonthlyStats tempStats;
  MonthlyStats* stats = &currentStats;
  
  if (month != currentStats.month) {
    loadMonthlyStats(month);
    tempStats = currentStats;
    stats = &tempStats;
    loadMonthlyStats(currentStats.month);
  }
  
  JsonDocument doc;
  doc["month"] = stats->month;
  doc["total_uptime"] = stats->totalUptime;
  doc["sessions_count"] = stats->sessions.size();
  
  std::map<String, uint32_t> effectTotals;
  for (const auto& s : stats->sessions) {
    for (const auto& e : s.effects) {
      effectTotals[e.name] += e.duration;
    }
  }
  
  JsonObject effects = doc["effects_summary"].to<JsonObject>();
  for (const auto& pair : effectTotals) {
    effects[pair.first] = pair.second;
  }
  
  String output;
  serializeJson(doc, output);
  return output;
}

String exportCSV(const String& month) {
  MonthlyStats tempStats;
  MonthlyStats* stats = &currentStats;
  
  if (month != currentStats.month) {
    loadMonthlyStats(month);
    tempStats = currentStats;
    stats = &tempStats;
    loadMonthlyStats(currentStats.month);
  }
  
  String csv = "Session Start,Session End,Session Duration (min),Effect Name,Effect Start,Effect Duration (min)\n";
  
  for (const auto& s : stats->sessions) {
    for (const auto& e : s.effects) {
      csv += formatTimestamp(s.startTime) + ",";
      csv += formatTimestamp(s.endTime) + ",";
      csv += String(s.duration / 60.0, 1) + ",";
      csv += e.name + ",";
      csv += formatTimestamp(e.startTime) + ",";
      csv += String(e.duration / 60.0, 1) + "\n";
    }
  }
  
  return csv;
}

// ===== Очищення старих даних =====
void cleanupOldStats() {
  timeClient.update();
  time_t now = timeClient.getEpochTime();
  struct tm* timeinfo = localtime(&now);
  int currentYear = timeinfo->tm_year + 1900;
  int currentMonth = timeinfo->tm_mon + 1;
  
  File dir = LittleFS.open("/stats", "r");
  if (!dir) return;
  
  File file = dir.openNextFile();
  while (file) {
    String name = file.name();
    if (name.endsWith(".json")) {
      int year = name.substring(1, 5).toInt();
      int month = name.substring(6, 8).toInt();
      
      int monthsDiff = (currentYear - year) * 12 + (currentMonth - month);
      if (monthsDiff > 24) {
        String path = "/stats/" + name;
        LittleFS.remove(path);
        Serial.println("🗑️ Видалено старий файл: " + name);
      }
    }
    file = dir.openNextFile();
  }
}

uint32_t getTotalUptime() { return currentStats.totalUptime; }
uint32_t getSessionDuration() { return sessionActive ? (getUnixTime() - sessionStart) : 0; }

// Додай цю функцію для автозбереження статистики
void autoSaveStats() {
  uint32_t now = getUnixTime();
  if (now - lastAutoSave > 300) { // Кожні 5 хвилин (300 секунд)
    if (sessionActive && !pendingEffects.empty()) {
      saveMonthlyStats();
      lastAutoSave = now;
      Serial.println("💾 Автозбереження статистики");
    }
  }
}

// ===== Getter-функції для відладки =====
bool isSessionActive() {
  return sessionActive;
}

uint32_t getPendingEffectsCount() {
  return pendingEffects.size();
}

// ПЕРЕЙМЕНОВАНО, щоб не конфліктувати з main.cpp
String getStatsCurrentEffectName() {
  return currentEffectName;
}

uint32_t getCurrentTotalUptime() {
  return currentStats.totalUptime;
}

uint32_t getCurrentSessionsCount() {
  return currentStats.sessions.size();
}


// Збереження pendingEffects у файл
void savePendingEffects() {
  File file = LittleFS.open("/stats/pending.json", "w");
  if (!file) return;
  
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();
  
  for (const auto& e : pendingEffects) {
    JsonObject obj = arr.add<JsonObject>();
    obj["name"] = e.name;
    obj["start"] = e.startTime;
    obj["duration"] = e.duration;
  }
  
  // Зберігаємо також час старту сесії
  doc["sessionStart"] = sessionStart;
  
  serializeJson(doc, file);
  file.close();
}

// Завантаження pendingEffects з файлу
bool loadPendingEffects() {
  File file = LittleFS.open("/stats/pending.json", "r");
  if (!file) return false;
  
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();
  
  if (error) {
    Serial.println("❌ Помилка читання pending.json: " + String(error.c_str()));
    return false;
  }
  
  pendingEffects.clear();
  
  JsonArray arr = doc.as<JsonArray>();
  for (JsonObject obj : arr) {
    EffectRecord rec;
    rec.name = obj["name"].as<String>();
    rec.startTime = obj["start"].as<uint32_t>();
    rec.duration = obj["duration"].as<uint32_t>();
    pendingEffects.push_back(rec);
  }
  
  // ✅ Відновлюємо sessionStart з останнього ефекту
  if (!pendingEffects.empty()) {
    sessionStart = pendingEffects[0].startTime;
    lastEffectSwitch = pendingEffects.back().startTime + pendingEffects.back().duration;
  }
  
  Serial.println("✅ Відновлено " + String(pendingEffects.size()) + " ефектів, sessionStart=" + String(sessionStart));
  return true;
}
// Видалення pending файлу
void clearPendingEffects() {
  LittleFS.remove("/stats/pending.json");
}

void resetStats() {
  currentStats.totalUptime = 0;
  currentStats.sessions.clear();
  pendingEffects.clear();
  sessionActive = false;
  sessionStart = 0;
  
  // Видалити всі файли статистики
  File dir = LittleFS.open("/stats", "r");
  if (dir) {
    File file = dir.openNextFile();
    while (file) {
      String name = file.name();
      String path = "/stats/" + name;
      LittleFS.remove(path);
      file = dir.openNextFile();
    }
  }
  
  saveMonthlyStats();
  Serial.println("🗑️ Статистику повністю скинуто");
}