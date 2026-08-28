#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H
#include "Arduino.h"
#include "config.h"
#include <Font6x8.h>  // ✅ ОБОВ'ЯЗКОВО: бо використовується нижче

int xyToIndex(int x, int y);
void drawCharOnStrip(int16_t x, char c, uint32_t color);
void drawFrame();


#endif