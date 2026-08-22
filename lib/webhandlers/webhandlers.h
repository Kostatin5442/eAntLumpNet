#ifndef WEBHANDLERS_H
#define WEBHANDLERS_H

#include <WebServer.h>
#include <WiFi.h>
#include <Preferences.h>

//
extern WebServer server;
extern Preferences wifiPrefs;
extern bool isAPMode;

//Роути для налаштауваня модуля на з'єднання
bool loadSavedWifi(String &ssid, String &password);
void handleSetupPage();
void handleWifiStatusApi();
void handleDashboard();
void handleSaveWifi();
void handleWifiPage();
void handleControlPage();
void handleScanNetworks();

#endif