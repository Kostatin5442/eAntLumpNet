#ifndef WEBHANDLERS_H
#define WEBHANDLERS_H
#include <Adafruit_NeoPixel.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Preferences.h>

//
// Оголошення зовнішніх змінних
extern Adafruit_NeoPixel strip;

extern WebServer server;
extern Preferences wifiPrefs;
extern bool isAPMode;

//Роути для налаштауваня модуля на з'єднання
//bool loadSavedWifi(String &ssid, String &password);
void handleSetupPage();
void handleWifiStatusApi();
void handleDashboard();
void handleSaveWifi();
void handleWifiPage();
void handleControlPage();
void handleScanNetworks();
void handleResetWifi();

// Нові API для розширеного Dashboard
void handleSetColor();
void handleSetSpeed();
void handleReboot();
void handleSystemInfoApi();


#endif