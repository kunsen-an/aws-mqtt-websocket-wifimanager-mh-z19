#include <ArduinoLog.h>

#if defined(ARDUINO_MH_ET_LIVE_ESP32MINIKIT)
#define D0  26    // MH ET LIVE ESP32MiniKit
#else
#define D0  16    // DUMMY  (for WeMos D1 mini)
#endif

#define RESET_CONTROL_PIN   D0    // MH-ET Live Minikit for ESP32 and WeMos D1 mini
#define HARD_RESET_DELAY    1000  // delay before hard reset (ms)

void hardReset() {
  Log.notice("hardReset()\n");
  delay(HARD_RESET_DELAY);
  digitalWrite(RESET_CONTROL_PIN, LOW);
}

void setupHardReset() {
  Log.notice("setupHardReset()\n");
  digitalWrite(RESET_CONTROL_PIN, HIGH);
  pinMode(RESET_CONTROL_PIN, OUTPUT);
  digitalWrite(RESET_CONTROL_PIN, HIGH);
}
