#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <Ws2812b_effects_fire.h>
#include <Ws2812b_effects_fire_2.h>
#include <Font6x8.h>
#include <math.h>
#include <Preferences.h>
#include <config.h>
#include "wifi_manager.h" //Функції для кеування wifi
#include "index_html.h"//Сторінки для WEB
#include "webhandlers.h"//функції керування через WEB
//Глобальні змінні оголошення для класів
WebServer server(80);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Прототипи функцій (Function prototypes)
int xyToIndex(int x, int y);
void drawCharOnStrip(int16_t x, char c, uint32_t color);
void initStars();
void handleDraw();
void handleChat();
// Прототипи ефектів
void starfieldStepUpdate();
void colorWavesStepUpdate();
void meteorRainStepUpdate();
void rippleStepUpdate();
void matrixRainStepUpdate();
void auroraStepUpdate();
void galaxyStepUpdate();
void tunnelStepUpdate();
void spiralGalaxyStepUpdate();
void nebulaStepUpdate();
void sparklesStepUpdate();
void turnOffStepUpdate();
void glitchBarsStepUpdate();
void pixelNoiseStepUpdate();
void glitchColumnsStepUpdate();
void psychedelicFlowStepUpdate();
Effect currentEffect = NONE;
// Налаштування чату
String currentMessage = "";
int16_t scrollX = MATRIX_WIDTH;
unsigned long lastUpdate = 0;
const unsigned long scrollDelay = 150;
void displayTextOnMatrix(String text) {
  currentMessage = text;
  scrollX = MATRIX_WIDTH;
}
void displayTextOnStrip(String text) {
  currentMessage = text;
  scrollX = 16;
}
void updateScrollingText() {
  if (currentMessage.length() == 0) return;
  unsigned long now = millis();
  if (now - lastUpdate < scrollDelay) return;
  lastUpdate = now;
  
  strip.clear();
  for (int i = 0; i < currentMessage.length(); i++) {
    char c = currentMessage[i];
    drawCharOnStrip(scrollX + i * 7, c, strip.Color(255, 255, 255));
  }
  strip.show();
  scrollX--;
  
  int totalWidth = currentMessage.length() * 7;
  if (scrollX < -totalWidth) {
    scrollX = 16;
  }
}
void drawCharOnStrip(int16_t x, char c, uint32_t color) {
  if (c < 32 || c > 127) return;
  const uint8_t* glyph = Font6x8 + (c - 32) * 6;
  for (int col = 0; col < 6; col++) {
    uint8_t column = pgm_read_byte(&glyph[col]);
    for (int row = 0; row < 8; row++) {
      if (column & (1 << row)) {
        int ledIndex = xyToIndex(x + col, row + 4);
        if (ledIndex >= 0 && ledIndex < LED_COUNT) {
          strip.setPixelColor(ledIndex, color);
        }
      }
    }
  }
}
//Обробник запиту на отримання статсу WiFi
void getAntStatusLux(){
//Поки порожня функція, яка може бути розширена для отримання статусу WiFi та інших параметрів.

}
Star stars[STAR_COUNT];
Meteor meteor = {0, 0, false};
uint32_t rainbowStep = 0;
uint32_t fireFrame = 0;
uint32_t flashingFrame = 0;
uint32_t hexToColor(String hex) {
  if (hex.length() != 6) return strip.Color(0, 0, 0);
  long number = strtol(hex.c_str(), nullptr, 16);
  uint8_t r = (number >> 16) & 0xFF;
  uint8_t g = (number >> 8) & 0xFF;
  uint8_t b = number & 0xFF;
  return strip.Color(r, g, b);
}
int xyToIndex(int x, int y) {
  if (x < 0 || x >= 16 || y < 0 || y >= 16) return -1;
  x = 15 - x;
  int index;
  if (y % 2 == 0) {
    index = y * 16 + x;
  } else {
    index = y * 16 + (15 - x);
  }
  return index;
}
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
void rainbowStepUpdate() {
  uint16_t i;
  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + rainbowStep) & 255));
  }
  strip.show();
  rainbowStep++;
  if (rainbowStep >= 256 * 5) rainbowStep = 0;
}
void fireStepUpdate() {
  if (fireFrame >= WS2812B_EFFECTS_FIRE_FRAMES) fireFrame = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t g = pgm_read_byte(&(Ws2812b_effects_fire[fireFrame][i * 3 + 0]));
    uint8_t r = pgm_read_byte(&(Ws2812b_effects_fire[fireFrame][i * 3 + 1]));
    uint8_t b = pgm_read_byte(&(Ws2812b_effects_fire[fireFrame][i * 3 + 2]));
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
  fireFrame++;
}
void flashingStepUpdate() {
  if (flashingFrame >= WS2812B_EFFECTS_FIRE_2_FRAMES) flashingFrame = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t g = pgm_read_byte(&(Ws2812b_effects_fire_2[flashingFrame][i * 3 + 0]));
    uint8_t r = pgm_read_byte(&(Ws2812b_effects_fire_2[flashingFrame][i * 3 + 1]));
    uint8_t b = pgm_read_byte(&(Ws2812b_effects_fire_2[flashingFrame][i * 3 + 2]));
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
  flashingFrame++;
}
// HTTP HANDLERS
void handleRainbow() {
  currentEffect = RAINBOW;
  rainbowStep = 0;
  server.send(200, "text/plain", "Rainbow started");
}
void handleFire() {
  currentEffect = FIRE;
  fireFrame = 0;
  server.send(200, "text/plain", "Fire started");
}
void handleFlashing() {
  currentEffect = FLASHING;
  flashingFrame = 0;
  server.send(200, "text/plain", "Flashing started");
}
void handleStarfield() {
  currentEffect = STARFIELD;
  initStars();
  server.send(200, "text/plain", "Starfield started");
}
//Оброники запитів будуть перенесені в HUB для розвантаження основного файлу main.cpp. Вони відповідають за запуск відповідних ефектів на LED-матриці через HTTP-запити.  
void handleColorWaves() {
  currentEffect = COLOR_WAVES;
  server.send(200, "text/plain", "Color Waves started");
}
// SETUP - 
void setup() {
  Serial.begin(115200);
  initWiFi();//Ініціалізація Wifi. Відкриття Центру налаштування
  // === Initialize LED strip ===
  strip.begin();
  strip.setBrightness(100);
  strip.clear();
  strip.show();
  for (int i = 0; i < 16; i++) {
    strip.setPixelColor(i, isAPMode ? strip.Color(255, 0, 0) : strip.Color(0, 255, 0));
  }
  strip.show();
  delay(1000);
  // === Register routes ===
  server.on("/effects_fire/state1", handleRainbow);
  server.on("/effects_fire/state2", handleFire);
  server.on("/effects_fire/state3", handleFlashing);
  server.on("/effects_fire/state4", handleStarfield);
  server.on("/effects_fire/state5", handleColorWaves);
  server.on("/effects_fire/state6", []() { currentEffect = METEOR; server.send(200, "text/plain", "Meteor Rain started"); });
  server.on("/effects_fire/state7", []() { currentEffect = RIPPLE; server.send(200, "text/plain", "Ripple started"); });
  server.on("/effects_fire/state8", []() { currentEffect = MATRIX_RAIN; server.send(200, "text/plain", "Matrix Rain started"); });
  server.on("/effects_fire/state9", []() { currentEffect = AURORA; server.send(200, "text/plain", "Aurora started"); });
  server.on("/effects_fire/state10", []() { currentEffect = GALAXY; server.send(200, "text/plain", "Galaxy started"); });
  server.on("/effects_fire/state11", []() { currentEffect = TUNNEL; server.send(200, "text/plain", "Tunnel started"); });
  server.on("/effects_fire/state12", []() { currentEffect = SPIRAL; server.send(200, "text/plain", "Spiral Galaxy started"); });
  server.on("/effects_fire/state13", []() { currentEffect = NEBULA; server.send(200, "text/plain", "Nebula started"); });
  server.on("/effects_fire/state14", []() { currentEffect = SPARKLES; server.send(200, "text/plain", "Sparkles started"); });
  server.on("/effects_fire/off", []() { currentEffect = OFF; server.send(200, "text/plain", "LEDs turned OFF"); });
  // Головна сторінка тепер - це солідна Dashboard-панель
   server.on("/", []() {
    if (isAPMode) {
      // Якщо в режимі AP - перенаправляємо на сторінку налаштування
      server.sendHeader("Location", "/setup");
      server.send(302, "text/plain", "Redirecting to setup...");
    } else {
      // Якщо підключено - показуємо Dashboard
      server.send_P(200, "text/html", DASHBOARD_HTML);
    }
  });
  
  server.on("/ws2812/glitch_bars", []() { currentEffect = GLITCH_BARS; server.send(200, "text/plain", "Glitch Bars started"); });
  server.on("/ws2812/pixel_noise", []() { currentEffect = PIXEL_NOISE; server.send(200, "text/plain", "Pixel Noise started"); });
  server.on("/ws2812/psychedelic_flow", []() { currentEffect = PSYCHEDELIC_FLOW; server.send(200, "text/plain", "Psychedelic Flow started"); });
  server.on("/ws2812/glitch_columns", []() { currentEffect = GLITCH_COLUMNS; server.send(200, "text/plain", "Glitch Columns started"); });
  
  server.on("/brightness", []() {
    if (server.hasArg("value")) {
      int brightness = server.arg("value").toInt();
      brightness = constrain(brightness, 0, 250);
      strip.setBrightness(brightness);
      strip.show();
      server.send(200, "text/plain", "Brightness set to " + String(brightness));
    } else {
      server.send(400, "text/plain", "Missing value param");
    }
  });
  
  server.on("/ws2812/next", []() {
    int next = (int)currentEffect + 1;
    if (next >= OFF) next = 1;
    currentEffect = (Effect)next;
    server.send(200, "text/plain", "Next effect");
  });
  
  server.on("/ws2812/prev", []() {
    int prev = (int)currentEffect - 1;
    if (prev < 1) prev = OFF - 1;
    currentEffect = (Effect)prev;
    server.send(200, "text/plain", "Prev effect");
  });
  server.on("/ws2812/draw", HTTP_GET, handleDraw);
  server.on("/chat", HTTP_GET, handleChat);
  // ✅ НОВИЙ: Обробник для перевірки статусу WiFi
  // Красива сторінка статусу Wi-Fi
  server.on("/wifi", HTTP_GET, handleWifiPage);
  // API для отримання даних сторінкою (без перезавантаження)
  server.on("/api/wifi", HTTP_GET, handleWifiStatusApi);
    // Веб-панель керування (безпечна для додатка)
  server.on("/control", HTTP_GET, handleControlPage);
    // === WiFi Setup Portal ===
  server.on("/setup", HTTP_GET, handleSetupPage);
  server.on("/api/scan", HTTP_GET, handleScanNetworks);
  server.on("/api/save", HTTP_POST, handleSaveWifi);
  server.on("/api/wifi/reset", HTTP_POST, handleResetWifi);
  server.begin();
}
// LOOP
void loop() {
  server.handleClient();

  switch (currentEffect) {
    case RAINBOW: rainbowStepUpdate(); break;
    case FIRE: fireStepUpdate(); break;
    case FLASHING: flashingStepUpdate(); break;
    case STARFIELD: starfieldStepUpdate(); break;
    case COLOR_WAVES: colorWavesStepUpdate(); break;
    case METEOR: meteorRainStepUpdate(); break;
    case RIPPLE: rippleStepUpdate(); break;
    case MATRIX_RAIN: matrixRainStepUpdate(); break;
    case AURORA: auroraStepUpdate(); break;
    case GALAXY: galaxyStepUpdate(); break;
    case TUNNEL: tunnelStepUpdate(); break;
    case SPIRAL: spiralGalaxyStepUpdate(); break;
    case NEBULA: nebulaStepUpdate(); break;
    case SPARKLES: sparklesStepUpdate(); break;
    case OFF: turnOffStepUpdate(); break;
    case GLITCH_BARS: glitchBarsStepUpdate(); break;
    case PIXEL_NOISE: pixelNoiseStepUpdate(); break;
    case GLITCH_COLUMNS: glitchColumnsStepUpdate(); break;
    case PSYCHEDELIC_FLOW: psychedelicFlowStepUpdate(); break;
    case CHAT: updateScrollingText(); break;
    default: rainbowStepUpdate(); break; //змінено для усіх Виключень з правил (запуск ефекту при увімкненні)
  }
  delay(WS2812B_EFFECTS_FIRE_FPS);
}
// Ініціалізація зірок
void initStars() {
  for (int i = 0; i < STAR_COUNT; i++) {
    stars[i].x = random(0, 16);
    stars[i].y = random(0, 16);
    stars[i].brightness = random(50, 255);
  }
}
//Функція беде перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану зоряного неба та метеорів на LED-матриці.  
void starfieldStepUpdate() {
  strip.clear();
  for (int i = 0; i < STAR_COUNT; i++) {
    if (stars[i].brightness > STAR_FADE) {
      stars[i].brightness -= STAR_FADE;
    } else {
      stars[i].brightness = random(100, 255);
      stars[i].x = random(0, 16);
      stars[i].y = random(0, 16);
    }
    strip.setPixelColor(stars[i].y * 16 + stars[i].x,
      stars[i].brightness,
      stars[i].brightness,
      stars[i].brightness);
  }
  
  if (!meteor.active && random(0.0, 1.0) < METEOR_CHANCE) {
    meteor.active = true;
    meteor.x = 0;
    meteor.y = random(0, 16);
  }
  
  if (meteor.active) {
    if (meteor.x < 16) {
      strip.setPixelColor(meteor.y * 16 + meteor.x, 255, 255, 255);
      meteor.x++;
    } else {
      meteor.active = false;
    }
  }
  strip.show();
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту кольорових хвиль на LED-матриці.  
void colorWavesStepUpdate() {
  static uint16_t waveStep = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t r = (sin((i + waveStep) * 0.1) * 127 + 128);
    uint8_t g = (sin((i + waveStep) * 0.13) * 127 + 128);
    uint8_t b = (sin((i + waveStep) * 0.17) * 127 + 128);
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
  waveStep++;
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту метеорного дощу на LED-матриці.
void meteorRainStepUpdate() {
  static int pos = 0;
  static int meteorSize = 4;
  static int decay = 64;
  static int speed = 1;
  
  for (int i = 0; i < NUM_LEDS; i++) {
    uint32_t c = strip.getPixelColor(i);
    uint8_t r = (c >> 16) & 0xFF;
    uint8_t g = (c >> 8) & 0xFF;
    uint8_t b = c & 0xFF;
    r = (r <= 10) ? 0 : (r - r * decay / 256);
    g = (g <= 10) ? 0 : (g - g * decay / 256);
    b = (b <= 10) ? 0 : (b - b * decay / 256);
    strip.setPixelColor(i, r, g, b);
  }
  
  for (int j = 0; j < meteorSize; j++) {
    int index = (pos - j + NUM_LEDS) % NUM_LEDS;
    strip.setPixelColor(index, strip.Color(255, 255 - j * 60, 100));
  }
  strip.show();
  pos += speed;
  if (pos >= NUM_LEDS) pos = 0;
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту хвиль на LED-матриці.
void rippleStepUpdate() {
  static int center = random(NUM_LEDS);
  static int step = -1;
  static uint32_t color;
  
  if (step == -1) {
    center = random(NUM_LEDS);
    color = strip.Color(random(100, 255), random(100, 255), random(100, 255));
    step = 0;
  }
  
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0);
  }
  
  for (int i = 0; i < NUM_LEDS; i++) {
    int dist = abs(i - center);
    if (dist == step) {
      strip.setPixelColor(i, color);
    }
  }
  strip.show();
  step++;
  if (step > 8) step = -1;
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту матричного дощу на LED-матриці.
void matrixRainStepUpdate() {
  static uint8_t drops[NUM_LEDS] = {0};
  for (int i = 0; i < NUM_LEDS; i++) {
    if (random(10) > 8) {
      drops[i] = 255;
    } else {
      if (drops[i] > 20) drops[i] -= 20; else drops[i] = 0;
    }
    strip.setPixelColor(i, 0, drops[i], 0);
  }
  strip.show();
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту північного сяйва на LED-матриці.
void auroraStepUpdate() {
  static uint16_t t = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    float x = (float)i / NUM_LEDS;
    uint8_t r = (sin(x * 3 + t * 0.02) * 127 + 128) / 3;
    uint8_t g = (sin(x * 4 + t * 0.015) * 127 + 128);
    uint8_t b = (sin(x * 5 + t * 0.01) * 127 + 128);
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
  t++;
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту галактики на LED-матриці.
void galaxyStepUpdate() {
  static uint16_t t = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    float x = (float)i / NUM_LEDS;
    uint8_t r = (sin(x * 7 + t * 0.02) * 127 + 128) / 2;
    uint8_t g = (sin(x * 3 + t * 0.015) * 127 + 128) / 3;
    uint8_t b = (sin(x * 5 + t * 0.01) * 127 + 128);
    if (random(1000) > 995) {
      r = g = b = 255;
    }
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
  t++;
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту тунелю на LED-матриці.
void tunnelStepUpdate() {
  static uint16_t t = 0;
  for (int y = 0; y < 16; y++) {
    for (int x = 0; x < 16; x++) {
      int i = y * 16 + x;
      float dx = x - 7.5;
      float dy = y - 7.5;
      float dist = sqrt(dx * dx + dy * dy);
      float angle = atan2(dy, dx) + t * 0.05;
      float wave = sin(dist * 2 - t * 0.1);
      uint8_t r = (sin(angle * 3) * 127 + 128) * (wave > 0 ? 1 : 0.3);
      uint8_t g = (sin(angle * 2 + 2) * 127 + 128) * (wave > 0 ? 1 : 0.3);
      uint8_t b = (sin(angle * 4 + 4) * 127 + 128) * (wave > 0 ? 1 : 0.3);
      float fade = min(1.0, dist / 8.0);
      r *= fade;
      g *= fade;
      b *= fade;
      strip.setPixelColor(i, r, g, b);
    }
  }
  strip.show();
  t++;
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту спіральної галактики на LED-матриці.
void spiralGalaxyStepUpdate() {
  static uint16_t t = 0;
  for (int y = 0; y < 16; y++) {
    for (int x = 0; x < 16; x++) {
      int i = y * 16 + x;
      float dx = x - 7.5;
      float dy = y - 7.5;
      float dist = sqrt(dx * dx + dy * dy);
      float angle = atan2(dy, dx) + t * 0.05;
      float spiral = sin(dist * 1.5 - angle * 3);
      uint8_t r = (sin(angle * 2 + t * 0.03) * 127 + 128) * (spiral > 0 ? 1 : 0.3);
      uint8_t g = (sin(angle * 3 + t * 0.02) * 127 + 128) * (spiral > 0 ? 1 : 0.3);
      uint8_t b = (sin(angle * 4 + t * 0.01) * 127 + 128);
      strip.setPixelColor(i, r, g, b);
    }
  }
  strip.show();
  t++;
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту туманності на LED-матриці.
void nebulaStepUpdate() {
  static uint16_t t = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    float x = (float)i / NUM_LEDS;
    uint8_t r = (sin(x * 3 + t * 0.02) * 127 + 128) / 2;
    uint8_t g = (sin(x * 5 + t * 0.015) * 127 + 128) / 4;
    uint8_t b = (sin(x * 7 + t * 0.01) * 127 + 128);
    if (random(1000) > 998) {
      r = g = b = 255;
    }
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
  t++;
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту іскр на LED-матриці.
void sparklesStepUpdate() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (random(100) > 95) {
      strip.setPixelColor(i, strip.Color(random(255), random(255), random(255)));
    } else {
      uint32_t c = strip.getPixelColor(i);
      uint8_t r = (c >> 16) & 0xFF;
      uint8_t g = (c >> 8) & 0xFF;
      uint8_t b = c & 0xFF;
      r = (r > 10) ? r - 10 : 0;
      g = (g > 10) ? g - 10 : 0;
      b = (b > 10) ? b - 10 : 0;
      strip.setPixelColor(i, r, g, b);
    }
  }
  strip.show();
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за вимкнення всіх світлодіодів на LED-матриці.
void turnOffStepUpdate() {
  strip.clear();
  strip.show();
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту "глітч барів" на LED-матриці.
void glitchBarsStepUpdate() {
  strip.clear();
  int barHeight = random(2, 5);
  int barStart = random(0, 16 - barHeight);
  uint32_t color = strip.Color(random(255), random(255), random(255));
  for (int y = barStart; y < barStart + barHeight; y++) {
    for (int x = 0; x < 16; x++) {
      int index = y * 16 + x;
      strip.setPixelColor(index, color);
    }
  }
  strip.show();
  delay(50);
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту "піксельного шуму" на LED-матриці.
void pixelNoiseStepUpdate() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (random(100) > 90) {
      strip.setPixelColor(i, strip.Color(random(255), random(255), random(255)));
    } else {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }
  strip.show();
  delay(80);
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту "глітч колонок" на LED-матриці.
void glitchColumnsStepUpdate() {
  strip.clear();
  int col = random(0, 16);
  uint32_t color = strip.Color(random(255), random(255), random(255));
  for (int y = 0; y < 16; y++) {
    int index = y * 16 + col;
    strip.setPixelColor(index, color);
  }
  strip.show();
  delay(60);
}
//Функція має бути перенесена в окремий модуль, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за оновлення стану ефекту "психоделічного потоку" на LED-матриці.
void psychedelicFlowStepUpdate() {
  static uint16_t t = 0;
  t++;
  for (int y = 0; y < 16; y++) {
    for (int x = 0; x < 16; x++) {
      int i = y * 16 + x;
      float fx = (float)x / 16.0;
      float fy = (float)y / 16.0;
      uint8_t r = (uint8_t)((sin(fx * 6.28 + t * 0.05) + cos(fy * 6.28 - t * 0.04)) * 127 + 128);
      uint8_t g = (uint8_t)((cos(fx * 7.85 - t * 0.03) + sin(fy * 7.85 + t * 0.06)) * 127 + 128);
      uint8_t b = (uint8_t)((sin(fx * 9.42 + t * 0.07) + cos(fy * 9.42 - t * 0.05)) * 127 + 128);
      strip.setPixelColor(i, r, g, b);
    }
  }
  strip.show();
  delay(30);
}
//Функція handleDraw буде перенесоно в окрему бібліотеку, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за обробку HTTP-запиту на малювання конкретного пікселя на LED-матриці з заданими координатами та кольором.
void handleDraw() {
  Serial.println("=== Отримано запит /draw ===");
  if (server.hasArg("x") && server.hasArg("y") && server.hasArg("color")) {
    int x = server.arg("x").toInt();
    int y = server.arg("y").toInt();
    String colorHex = server.arg("color");
    Serial.print("x="); Serial.println(x);
    Serial.print("y="); Serial.println(y);
    Serial.print("color="); Serial.println(colorHex);
    int index = xyToIndex(x, y);
    if (index >= 0) {
      uint32_t color = hexToColor(colorHex);
      strip.setPixelColor(index, color);
      strip.show();
      server.send(200, "text/plain", "OK");
      return;
    }
  }
  server.send(400, "text/plain", "Error");
}
//Разом із функцією handleDraw, функція handleChat також буде перенесена в окрему бібліотеку. Вона відповідає за обробку HTTP-запиту на відображення тексту на LED-стрічці, отриманого через параметр "text".
void handleChat() {
  if (server.hasArg("text")) {
    String text = server.arg("text");
    Serial.println("💬 Отримано текст: " + text);
    currentEffect = CHAT;
    displayTextOnStrip(text);
    server.send(200, "text/plain", "OK: " + text);
  } else {
    server.send(400, "text/plain", "Помилка: параметр 'text' відсутній");
  }
}
//Функція drawFrame буде перенесена в окрему бібліотеку, щоб уникнути перевантаження основного файлу main.cpp. Вона відповідає за малювання рамки на LED-матриці з використанням різних кольорів для кожного стовпця.
void drawFrame() {
  for (int x = 0; x < 16; x++) {
    int idx = xyToIndex(x, 0);
    if (idx >= 0) {
      if (x % 3 == 0) strip.setPixelColor(idx, strip.Color(0, 255, 0));
      else if (x % 3 == 1) strip.setPixelColor(idx, strip.Color(0, 0, 255));
      else strip.setPixelColor(idx, strip.Color(255, 255, 0));
    }
  }
  for (int x = 0; x < 16; x++) {
    int idx = xyToIndex(x, 15);
    if (idx >= 0) {
      if (x % 3 == 0) strip.setPixelColor(idx, strip.Color(0, 255, 0));
      else if (x % 3 == 1) strip.setPixelColor(idx, strip.Color(0, 0, 255));
      else strip.setPixelColor(idx, strip.Color(255, 255, 0));
    }
  }
}