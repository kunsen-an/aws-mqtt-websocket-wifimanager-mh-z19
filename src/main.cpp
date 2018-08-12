#include <Arduino.h>
#include <Stream.h>
#include <ArduinoLog.h> // https://github.com/thijse/Arduino-Log/
#include <ArduinoJson.h>

#define LOGLEVEL LOG_LEVEL_NOTICE
//#define LOGLEVEL LOG_LEVEL_WARNING

#define USE_HARD_RESET 1

#define OLED_DISPLAY_ARGS "CO2:\n %d\nTemp:%d", (int)jsonObject["CO2"], (int)jsonObject["Temperature"]

#define DELAY_INTERVAL (15 * 1000)
#define RETRY_INTERVAL (1 * 1000)

#define MAX_JSON_SIZE 512
#define MAX_DEVICE_NAME 128

#ifdef ARDUINO_ESP8266_WEMOS_D1MINI
#define STOP_WIFI 1
void nothing(...)
{
}

#define publishMessage(arg) nothing(arg)
#define publisJson(arg) nothing(arg)
#endif

#ifdef USE_HARD_RESET
// wiring is required to use hardReset()
extern void hardReset();
#endif // USE_HARD_RESET
extern void setupHardReset();

extern void stopWiFi();

#ifndef STOP_WIFI
extern void setupWiFi();
extern void setupMQTT();
extern void loopClient();
extern int loopMQTT();
extern int publishJson(JsonObject &jsonObject);
#endif // STOP_WIFI

extern void setupMHZ19(char deviceNameBuffer[]);
extern int updateMHZ19(JsonObject &jsonObject);

extern void setupOLED(int size);
extern void displayOLED(char *format, ...);

extern void setEventHandler(void (*handler)(JsonObject &objec));
extern void setResetFunc(void (*func)(void));

char  deviceNameBuffer[MAX_DEVICE_NAME];

void resetESP()
{
#ifdef ESP8266
  ESP.reset();
#endif // ESP8266
#ifdef ESP32
  ESP.restart();
#endif // ESP32
}
void softReset()
{
  StaticJsonBuffer<MAX_JSON_SIZE> errorJsonBuffer;
  JsonObject &errorJsonObject = errorJsonBuffer.createObject();

  Log.notice("softReset()\n");
  errorJsonObject["Reset"] = "soft";
  publishJson(errorJsonObject);
  resetESP();
}

void controlEventHandler(JsonObject &object)
{
  Log.notice(">controlEventHandler\n");

  const char *resetMethod = (const char *)object["Reset"];

  if (resetMethod != NULL)
  {
    if (strcmp(resetMethod, "hard") == 0)
    {
      Log.notice("hardReset\n");
#ifdef USE_HARD_RESET
      hardReset();
#endif // USE_HARD_RESET
    }
    else if (strcmp(resetMethod, "soft") == 0)
    {
      Log.notice("softReset\n");
      softReset();
    }
  }
  Log.notice("<controlEventHandler\n");
}

void setupDevices(char deviceNameBuffer[])
{
  setupMHZ19(deviceNameBuffer);
}

char* getDeviceName()
{
  return deviceNameBuffer;
}

void setup()
{
#ifdef STOP_WIFI
  stopWiFi();
#endif // STOP_WIFI

  Serial.begin(115200);
  Log.begin(LOGLEVEL, &Serial);
  delay(2000);

#ifdef USE_HARD_RESET
  setupHardReset();
#endif  // USE_HARD_RESET
  setupOLED(2);

  setupDevices(deviceNameBuffer);
  displayOLED("Device setup done.");

  setupWiFi();
  displayOLED("WiFi setup done.");
  
  setupMQTT();
  displayOLED("MQTT setup done.");

  setEventHandler(controlEventHandler);
  setResetFunc(softReset);

  delay(5000);
}

unsigned long lasttime;

void loop()
{
  unsigned long time = millis();

  if ((time - lasttime) < DELAY_INTERVAL)
  {
#ifndef STOP_WIFI
    loopClient();
#endif
    delay(1000);
    return;
  }
  else
  {
    lasttime = time;
  }
  Log.notice("freesize=%d\n", ESP.getFreeHeap());

  StaticJsonBuffer<MAX_JSON_SIZE> jsonBuffer;
  JsonObject &jsonObject = jsonBuffer.createObject();

  // update
  while (updateMHZ19(jsonObject) != 0)
  {
    delay(RETRY_INTERVAL);
  }
  displayOLED(OLED_DISPLAY_ARGS);

#ifndef STOP_WIFI
  // MQTT
  int rc;
  rc = loopMQTT();
  if (rc == 0)
  {
    publishJson(jsonObject);
  }
  else
  {
    displayOLED("MQTT error=%d", rc);
    Log.error("MQTT error=%d\n", rc);
  }
#endif // STOP_WIFI
}
