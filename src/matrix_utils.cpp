#include "matrix_utils.h"
//================= del==============
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