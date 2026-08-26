#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
#include <Arduino.h>
#include <WiFi.h>
void initWiFi();
bool loadSavedWifi(String &ssid, String &password);
bool saveWifi(const String &ssid, const String &password);
void clearSavedWifi();
bool isWifiConfigured();
#endif