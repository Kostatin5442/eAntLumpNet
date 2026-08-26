#include "wifi_manager.h"
#include <Preferences.h>
#include <ESPmDNS.h>

//Pre
Preferences wifiPrefs;
bool isAPMode = false;
const char* mdns_hostname = "ledpanel";
const char* mdns_instance = "ESP32 LED Panel";


void initWiFi() {
    String savedSsid = "";
    String savedPassword = "";
    bool hasSavedWifi = loadSavedWifi(savedSsid, savedPassword);
    //Режим клієнта для підключення
    WiFi.mode(WIFI_STA);

    if (hasSavedWifi && savedSsid.length() > 0) {
        Serial.print("Підключення до: ");
        Serial.println(savedSsid);
        WiFi.begin(savedSsid.c_str(), savedPassword.c_str());
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            isAPMode = false;
            Serial.print("WiFi IP: ");
            Serial.println(WiFi.localIP());
            
            // mDNS ініціалізація
            if (MDNS.begin(mdns_hostname)) {
                MDNS.addService("http", "tcp", 80);
                MDNS.setInstanceName(mdns_instance);
                Serial.print("mDNS: http://");
                Serial.print(mdns_hostname);
                Serial.println(".local");
            }
        } else {
            Serial.println("WiFi не підключено. Перехід в AP режим.");
            WiFi.disconnect(true);
            delay(100);
            isAPMode = true;
            WiFi.mode(WIFI_AP);
            WiFi.softAP("ESP32_Setup");
            Serial.print("AP IP: ");
            Serial.println(WiFi.softAPIP());
        }
    } else {
        Serial.println("WiFi даних немає. Перехід в AP режим.");
        isAPMode = true;
        WiFi.mode(WIFI_AP);
        WiFi.softAP("ESP32_Setup");
        Serial.print("AP IP: ");
        Serial.println(WiFi.softAPIP());
    }
}

bool loadSavedWifi(String &ssid, String &password) {
//Відкрити простір імен "wifi" тільки читання true
//Спочатку wifiPrefs.begin("wifi", true)
//wifiPrefs.begin() - якщо успіх повернемо 1. Інверсія перетворить на 0 
//і умова if не виконається.
//Далі буде bool configured = wifiPrefs.getBool("configured", false);
//wifiPrefs.begin() - якщо невдало повернемо 0. Інверсія дає 1 і умова 
// if виконується - return false;
    if (!wifiPrefs.begin("wifi", true)){
//Якщо простір імен пошкоджений або не ініціалізований  повертаємо 0
        return false;
//Завершення програми. Читати Пам'ять далі не можливо
    } //тут перевірили цілістність пам'яті

//Читаємо configured. 
//Вікдриваємо простір імен false - тільки для читання.
    bool configured = wifiPrefs.getBool("configured", false);
//Якщо налаштування є тоді інверсія невиконає код if
//Далі буде читати ключі доступу ssid і password
    if (!configured) {
        wifiPrefs.end();
        return false;
//Завершення програми. Читати Пам'ять далі сенсу немає Ключі досутпу відстуні         
    }
//Якщо configured - налаштований - інверсія читаємо Ключі
    ssid = wifiPrefs.getString("ssid", "");
    password = wifiPrefs.getString("pass", "");
    wifiPrefs.end();//Завершуємо роботу з пам'ятью 
    return ssid.length()>0;//Якщо ssid пустий повертаємо false. Якщо є повертаємо ІСТИНА.
}

bool saveWifi(const String &ssid, const String &password) {
    // Відкриваємо простір імен "wifi" для запису (false)
    if (!wifiPrefs.begin("wifi", false)) {
        return false;
    }
    wifiPrefs.putBool("configured", true);
    wifiPrefs.putString("ssid", ssid);
    wifiPrefs.putString("pass", password);
    wifiPrefs.end();
    return true;
}

void clearSavedWifi() {
    if (wifiPrefs.begin("wifi", false)) {
        wifiPrefs.clear();
        wifiPrefs.end();
    }
}

bool isWifiConfigured() {
    bool configured = false;
    if (wifiPrefs.begin("wifi", true)) {
        configured = wifiPrefs.getBool("configured", false);
        wifiPrefs.end();
    }
    return configured;
}

