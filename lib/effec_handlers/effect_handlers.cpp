#include "effect_handlers.h"
#include "config.h"
//Зовнішні зміні

// HTTP HANDLERS
void handleRainbow() {
  currentEffect = RAINBOW;
  rainbowStep = 0;
  server.send(200, "text/plain", "Rainbow started");
}




