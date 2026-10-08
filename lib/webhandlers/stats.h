#ifndef STATS_H
#define STATS_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <vector>
#include <map>

struct EffectRecord {
  String name;
  uint64_t startTime;
  uint64_t duration;
};

struct SessionRecord {
  uint64_t startTime;
  uint64_t endTime;
  uint64_t duration;
  std::vector<EffectRecord> effects;
};

struct MonthlyStats {
  String month;
  std::vector<SessionRecord> sessions;
  uint64_t totalUptime;
};

// Ініціалізація
void initStats();
void initTime();
void startSession();
void endSession();

// Ефекти
void recordEffectSwitch(const String& effectName, bool forceSave = false);
void finalizeCurrentEffect();

// Автозбереження
void autoSaveStats();

// Отримання даних
String getCurrentMonthKey();
String getStatsJSON(const String& month);
String exportCSV(const String& month);

// Очищення
void cleanupOldStats();
void resetStats();

// Getter-функції
bool isSessionActive();
uint64_t getSessionDuration();
uint64_t getPendingEffectsCount();
String getStatsCurrentEffectName();
uint64_t getCurrentTotalUptime();
uint64_t getCurrentSessionsCount();
uint64_t getCurrentEffectDuration();

// Час
uint64_t getUnixTime();
String formatTimestamp(uint64_t ts);

// ⚠️ ДОДАЙ ЦЕ - внутрішні функції
void loadMonthlyStats(const String& month);
void saveMonthlyStats();
void savePendingEffects();
bool loadPendingEffects();
void clearPendingEffects();

#endif