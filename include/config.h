#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>
// === WIFI НАЛАШТУВАННЯ ===
extern const char* ssid;
extern const char* password;
// mDNS налаштування
extern const char* mdns_hostname;
extern const char* mdns_instance;
extern WebServer server;
extern bool isAPMode;
// === LED МАТРИЦЯ ===
#define LED_PIN       8
#define NUM_LEDS      256
#define LED_COUNT     NUM_LEDS
#define TOUCH_PIN     4
#define MATRIX_WIDTH  16
#define MATRIX_HEIGHT 16

extern Adafruit_NeoPixel strip;

// === ЗОРЯНЕ НЕБО ===
#define STAR_COUNT    10
#define METEOR_CHANCE 0.05
#define STAR_FADE     20

struct Star {
  uint8_t x;
  uint8_t y;
  uint8_t brightness;
};

struct Meteor {
  int x;
  int y;
  bool active;
};

extern Star stars[STAR_COUNT];
extern Meteor meteor;

// === СПИСОК ЕФЕКТІВ та режимів===
enum Effect {
  NONE,
  RAINBOW,
  FIRE,
  FLASHING,
  STARFIELD,
  COLOR_WAVES,
  METEOR,
  RIPPLE,
  MATRIX_RAIN,
  AURORA,
  GALAXY,
  TUNNEL,
  SPIRAL,
  NEBULA,
  SPARKLES,
  GLITCH_BARS,
  PIXEL_NOISE,
  GLITCH_COLUMNS,
  PSYCHEDELIC_FLOW,
  CHAT,
  PIXEL_ART,
  OFF
};

extern Effect currentEffect;

// Змінні для збереження стану анімацій
extern uint32_t rainbowStep;
extern uint32_t fireFrame;
extern uint32_t flashingFrame;

#endif // CONFIG_H