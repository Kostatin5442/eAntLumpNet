#ifndef EFFECT_HANDLERS_H
#define EFFECT_HANDLERS_H
#include <Arduino.h>
// Прототипи функцій (Function prototypes)
//Перша функція
void handleRainbow();
//=============================================

void drawCharOnStrip(int16_t x, char c, uint32_t color);
void initStars();
void handleDraw();
void handleChat();



#endif EFFECT_HANDLERS_H