/*
відповідає виключно за підключення до мережі та/або збереження завантаження 
налатшуваньчерез Preferences а такоєж ініціалізацію mDNS
*/

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
#include <Arduino.h>

void initWiFi();
bool loadSavedWifi(String &ssid, String &password);
bool saveWifi(const String &ssid, const String &password);
void clearSavedWifi();
bool isWifiConfigured();


#endif