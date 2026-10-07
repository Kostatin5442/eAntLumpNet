#ifndef STATS_H
#define STATS_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <vector>  // ДОДАНО
#include <map>     // ДОДАНО

// Структура запису ефекту
struct EffectRecord {
  String name;
  uint32_t startTime;  // Unix timestamp
  uint32_t duration;   // секунди
};

// Структура сесії
struct SessionRecord {
  uint32_t startTime;
  uint32_t endTime;
  uint32_t duration;
  std::vector<EffectRecord> effects;
};

// Структура місячної статистики
struct MonthlyStats {
  String month;  // "2024_10"
  std::vector<SessionRecord> sessions;
  uint32_t totalUptime;
};

// Оголошення функцій (щоб компілятор знав про них заздалегідь)
void initStats();
void startSession();
void endSession();
void savePendingEffects();
bool loadPendingEffects();
void clearPendingEffects();
void resetStats();

void recordEffectSwitch(const String& effectName, bool forceSave = false); // ВИПРАВЛЕНО: додано 2-й аргумент
String getCurrentMonthKey();
String getStatsJSON(const String& month);
String exportCSV(const String& month);
void cleanupOldStats();
uint32_t getTotalUptime();
uint32_t getSessionDuration();
void autoSaveStats();
// Внутрішні функції, які мають бути видимі
void loadMonthlyStats(const String& month);
void saveMonthlyStats();
String formatTimestamp(uint32_t ts);

// NTP 
void initNTP();
uint32_t getUnixTime();

// Getter-функції для відладки
bool isSessionActive();
uint32_t getPendingEffectsCount();
String getStatsCurrentEffectName();
uint32_t getCurrentTotalUptime();
uint32_t getCurrentSessionsCount();

#endif