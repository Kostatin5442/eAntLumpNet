#include "stats.h"
#include <time.h>
#include <vector>
#include <map>

// =================================================================
// Час
// =================================================================
void initTime() {
  configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  
  struct tm timeinfo;
  int retries = 0;
  while (!getLocalTime(&timeinfo) && retries < 10) {
    delay(500);
    retries++;
  }
  
  if (retries < 10) {
    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
    Serial.println("🕐 NTP час: " + String(timeStr));
  }
}

uint64_t getUnixTime() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    return mktime(&timeinfo);
  }
  return millis() / 1000;
}

String formatTimestamp(uint64_t ts) {
  time_t t = (time_t)ts;
  struct tm* tm_info = localtime(&t);
  char buf[32];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
  return String(buf);
}

String getCurrentMonthKey() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "1970_01";
  }
  char buf[16];
  sprintf(buf, "%04d_%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1);
  return String(buf);
}

// =================================================================
// Змінні стану (тільки в RAM!)
// =================================================================
static MonthlyStats currentStats;
static uint64_t sessionStart = 0;
static bool sessionActive = false;
static String currentEffectName = "OFF";
static uint64_t currentEffectStartTime = 0;
static std::vector<EffectRecord> currentSessionEffects;
static uint64_t lastAutoSave = 0;

// =================================================================
// Ініціалізація
// =================================================================
void initStats() {
  if (!LittleFS.begin(false)) {
    Serial.println("⚠️ LittleFS не вдалося змонтувати. Форматування...");
    LittleFS.format();
    if (!LittleFS.begin(false)) {
      Serial.println("❌ LittleFS не вдалося ініціалізувати!");
      return;
    }
  }
  Serial.println("✅ LittleFS ініціалізовано");
  
  if (!LittleFS.exists("/stats")) {
    LittleFS.mkdir("/stats");
  }
  
  String monthKey = getCurrentMonthKey();
  loadMonthlyStats(monthKey);
  
  // Нова сесія (без відновлення)
  startSession();
  
  cleanupOldStats();
}

// =================================================================
// Сесії
// =================================================================
void startSession() {
  if (sessionActive) return;
  
  uint64_t now = getUnixTime();
  sessionStart = now;
  sessionActive = true;
  currentEffectName = "OFF";
  currentEffectStartTime = now;
  currentSessionEffects.clear();
  
  Serial.println("▶️ Нова сесія розпочата");
}

void endSession() {
  if (!sessionActive) return;
  
  uint64_t now = getUnixTime();
  uint64_t sessionDuration = now - sessionStart;
  
  // Завершуємо поточний ефект
  finalizeCurrentEffect();
  
  // Створюємо запис сесії
  SessionRecord session;
  session.startTime = sessionStart;
  session.endTime = now;
  session.duration = sessionDuration;
  session.effects = currentSessionEffects;
  
  currentStats.sessions.push_back(session);
  currentStats.totalUptime += sessionDuration;
  
  // Зберігаємо
  saveMonthlyStats();
  
  // Скидаємо стан
  sessionActive = false;
  currentSessionEffects.clear();
  
  Serial.println("⏹️ Сесія завершена: " + String(sessionDuration) + 
                 " сек, " + String(session.effects.size()) + " ефектів");
}

// =================================================================
// Ефекти
// =================================================================
void finalizeCurrentEffect() {
  uint64_t now = getUnixTime();
  uint64_t duration = now - currentEffectStartTime;
  
  if (duration > 0 && duration < 86400) {
    EffectRecord rec;
    rec.name = currentEffectName;
    rec.startTime = currentEffectStartTime;
    rec.duration = duration;
    currentSessionEffects.push_back(rec);
    
    Serial.println("📝 Ефект: " + currentEffectName + " | " + 
                   String(duration) + " сек");
  }
}

void recordEffectSwitch(const String& newEffectName, bool forceSave) {
  if (!sessionActive) return;
  
  // Завершуємо попередній ефект
  finalizeCurrentEffect();
  
  // Починаємо новий
  currentEffectName = newEffectName;
  currentEffectStartTime = getUnixTime();
  
  // Примусове збереження
  if (forceSave) {
    saveMonthlyStats();
  }
}

// =================================================================
// Автозбереження (раз на 5 хвилин)
// =================================================================
void autoSaveStats() {
  if (!sessionActive) return;
  
  uint64_t now = getUnixTime();
  if (now - lastAutoSave < 300) return; // 5 хвилин
  
  lastAutoSave = now;
  
  // Просто зберігаємо файл (saveMonthlyStats сам додасть поточну сесію)
  saveMonthlyStats();
  
  Serial.println("💾 Автозбереження");
}
// =================================================================
// Місячна статистика
// =================================================================
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
  deserializeJson(doc, file);
  file.close();
  
  currentStats.totalUptime = doc["total_uptime"].as<uint64_t>();
  
  JsonArray sessions = doc["sessions"];
  for (JsonObject s : sessions) {
    SessionRecord session;
    session.startTime = s["start"].as<uint64_t>();
    session.endTime = s["end"].as<uint64_t>();
    session.duration = s["duration"].as<uint64_t>();
    
    JsonArray effects = s["effects"];
    for (JsonObject e : effects) {
      EffectRecord rec;
      rec.name = e["name"].as<String>();
      rec.startTime = e["start"].as<uint64_t>();
      rec.duration = e["duration"].as<uint64_t>();
      session.effects.push_back(rec);
    }
    
    currentStats.sessions.push_back(session);
  }
  
  Serial.println("📊 Завантажено " + String(currentStats.sessions.size()) + 
                 " сесій за " + month);
}

void saveMonthlyStats() {
  String path = "/stats/" + currentStats.month + ".json";
  File file = LittleFS.open(path, "w");
  if (!file) return;
  
  JsonDocument doc;
  doc["month"] = currentStats.month;
  doc["total_uptime"] = (uint64_t)currentStats.totalUptime;
  
  JsonArray sessions = doc["sessions"].to<JsonArray>();
  
  // Зберігаємо всі завершені сесії
  for (const auto& s : currentStats.sessions) {
    JsonObject sess = sessions.add<JsonObject>();
    sess["start"] = (uint64_t)s.startTime;
    sess["end"] = (uint64_t)s.endTime;
    sess["duration"] = (uint64_t)s.duration;
    
    JsonArray effects = sess["effects"].to<JsonArray>();
    for (const auto& e : s.effects) {
      JsonObject eff = effects.add<JsonObject>();
      eff["name"] = e.name;
      eff["start"] = (uint64_t)e.startTime;
      eff["duration"] = (uint64_t)e.duration;
    }
  }
  
  // ДОДАЄМО ПОТОЧНУ СЕСІЮ (незавершену)
  if (sessionActive) {
    uint64_t now = getUnixTime();
    uint64_t sessionDuration = now - sessionStart;
    
    // Завершуємо поточний ефект тимчасово
    finalizeCurrentEffect();
    
    JsonObject sess = sessions.add<JsonObject>();
    sess["start"] = (uint64_t)sessionStart;
    sess["end"] = (uint64_t)now;
    sess["duration"] = (uint64_t)sessionDuration;
    sess["incomplete"] = true;  // Мітка що сесія не завершена
    
    JsonArray effects = sess["effects"].to<JsonArray>();
    for (const auto& e : currentSessionEffects) {
      JsonObject eff = effects.add<JsonObject>();
      eff["name"] = e.name;
      eff["start"] = (uint64_t)e.startTime;
      eff["duration"] = (uint64_t)e.duration;
    }
  }
  
  serializeJson(doc, file);
  file.close();
}
// =================================================================
// API
// =================================================================
String getStatsJSON(const String& month) {
  MonthlyStats tempStats;
  MonthlyStats* stats;
  
  if (month == currentStats.month) {
    stats = &currentStats;
  } else {
    tempStats.month = month;
    tempStats.totalUptime = 0;
    
    String path = "/stats/" + month + ".json";
    if (LittleFS.exists(path)) {
      File file = LittleFS.open(path, "r");
      if (file) {
        JsonDocument doc;
        deserializeJson(doc, file);
        file.close();
        
        tempStats.totalUptime = doc["total_uptime"].as<uint64_t>();
        JsonArray sessions = doc["sessions"];
        for (JsonObject s : sessions) {
          SessionRecord session;
          session.startTime = s["start"].as<uint64_t>();
          session.endTime = s["end"].as<uint64_t>();
          session.duration = s["duration"].as<uint64_t>();
          
          JsonArray effects = s["effects"];
          for (JsonObject e : effects) {
            EffectRecord rec;
            rec.name = e["name"].as<String>();
            rec.startTime = e["start"].as<uint64_t>();
            rec.duration = e["duration"].as<uint64_t>();
            session.effects.push_back(rec);
          }
          tempStats.sessions.push_back(session);
        }
      }
    }
    stats = &tempStats;
  }
  
  JsonDocument doc;
  doc["month"] = stats->month;
  doc["total_uptime"] = (uint64_t)stats->totalUptime;
  doc["sessions_count"] = (int)stats->sessions.size();
  
  JsonArray sessions = doc["sessions"].to<JsonArray>();
  for (const auto& s : stats->sessions) {
    JsonObject sess = sessions.add<JsonObject>();
    sess["start"] = (uint64_t)s.startTime;
    sess["end"] = (uint64_t)s.endTime;
    sess["duration"] = (uint64_t)s.duration;
    
    JsonArray effects = sess["effects"].to<JsonArray>();
    for (const auto& e : s.effects) {
      JsonObject eff = effects.add<JsonObject>();
      eff["name"] = e.name;
      eff["start"] = (uint64_t)e.startTime;
      eff["duration"] = (uint64_t)e.duration;
    }
  }
  
  std::map<String, uint64_t> effectTotals;
  for (const auto& s : stats->sessions) {
    for (const auto& e : s.effects) {
      effectTotals[e.name] += e.duration;
    }
  }
  
  JsonObject effects = doc["effects_summary"].to<JsonObject>();
  for (const auto& pair : effectTotals) {
    effects[pair.first] = (uint64_t)pair.second;
  }
  
  String output;
  serializeJson(doc, output);
  return output;
}

String exportCSV(const String& month) {
  MonthlyStats tempStats;
  MonthlyStats* stats;
  
  if (month == currentStats.month) {
    stats = &currentStats;
  } else {
    tempStats.month = month;
    tempStats.totalUptime = 0;
    String path = "/stats/" + month + ".json";
    if (LittleFS.exists(path)) {
      File file = LittleFS.open(path, "r");
      if (file) {
        JsonDocument doc;
        deserializeJson(doc, file);
        file.close();
        tempStats.totalUptime = doc["total_uptime"].as<uint64_t>();
        JsonArray sessions = doc["sessions"];
        for (JsonObject s : sessions) {
          SessionRecord session;
          session.startTime = s["start"].as<uint64_t>();
          session.endTime = s["end"].as<uint64_t>();
          session.duration = s["duration"].as<uint64_t>();
          JsonArray effects = s["effects"];
          for (JsonObject e : effects) {
            EffectRecord rec;
            rec.name = e["name"].as<String>();
            rec.startTime = e["start"].as<uint64_t>();
            rec.duration = e["duration"].as<uint64_t>();
            session.effects.push_back(rec);
          }
          tempStats.sessions.push_back(session);
        }
      }
    }
    stats = &tempStats;
  }
  
  String csv = "Session Start,Session End,Session Duration (min),Effect Name,Effect Start,Effect Duration (min)\n";
  for (const auto& s : stats->sessions) {
    for (const auto& e : s.effects) {
      csv += formatTimestamp(s.startTime) + ",";
      csv += formatTimestamp(s.endTime) + ",";
      csv += String((double)s.duration / 60.0, 1) + ",";
      csv += e.name + ",";
      csv += formatTimestamp(e.startTime) + ",";
      csv += String((double)e.duration / 60.0, 1) + "\n";
    }
  }
  return csv;
}

// =================================================================
// Очищення
// =================================================================
void cleanupOldStats() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;
  
  int currentYear = timeinfo.tm_year + 1900;
  int currentMonth = timeinfo.tm_mon + 1;
  
  File dir = LittleFS.open("/stats", "r");
  if (!dir) return;
  
  std::vector<String> toDelete;
  
  File file = dir.openNextFile();
  while (file) {
    String fullPath = file.name();
    int lastSlash = fullPath.lastIndexOf('/');
    String fileName = (lastSlash >= 0) ? fullPath.substring(lastSlash + 1) : fullPath;
    
    if (fileName.endsWith(".json") && fileName.length() >= 11) {
      String yearStr = fileName.substring(0, 4);
      String monthStr = fileName.substring(5, 7);
      
      int year = yearStr.toInt();
      int month = monthStr.toInt();
      
      if (year >= 2020 && year <= 2100 && month >= 1 && month <= 12) {
        int monthsDiff = (currentYear - year) * 12 + (currentMonth - month);
        if (monthsDiff > 24) {
          toDelete.push_back(fullPath);
        }
      }
    }
    file = dir.openNextFile();
  }
  
  for (const auto& path : toDelete) {
    LittleFS.remove(path);
    Serial.println("🗑️ Видалено: " + path);
  }
}

void resetStats() {
  currentStats.totalUptime = 0;
  currentStats.sessions.clear();
  currentSessionEffects.clear();
  sessionActive = false;
  sessionStart = 0;
  currentEffectName = "OFF";
  currentEffectStartTime = 0;
  
  std::vector<String> toDelete;
  File dir = LittleFS.open("/stats", "r");
  if (dir) {
    File file = dir.openNextFile();
    while (file) {
      toDelete.push_back(String(file.name()));
      file = dir.openNextFile();
    }
  }
  
  for (const auto& path : toDelete) {
    LittleFS.remove(path);
  }
  
  saveMonthlyStats();
  Serial.println("🗑️ Статистику скинуто");
}

// =================================================================
// Getter-функції
// =================================================================
bool isSessionActive() { return sessionActive; }

uint64_t getSessionDuration() {
  if (!sessionActive || sessionStart == 0) return 0;
  return getUnixTime() - sessionStart;
}

uint64_t getPendingEffectsCount() { 
  return currentSessionEffects.size() + (sessionActive ? 1 : 0); 
}

String getStatsCurrentEffectName() { return currentEffectName; }

uint64_t getCurrentTotalUptime() { return currentStats.totalUptime; }

uint64_t getCurrentSessionsCount() { return currentStats.sessions.size(); }

uint64_t getCurrentEffectDuration() {
  if (!sessionActive) return 0;
  return getUnixTime() - currentEffectStartTime;
}