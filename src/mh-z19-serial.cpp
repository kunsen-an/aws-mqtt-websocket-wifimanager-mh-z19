#include <ArduinoLog.h>
#include <ArduinoJson.h>

#define MINIMUM_UPDATE_INTERVAL 5000

#define DEVICE_NAME "MH-z19"

#if defined(ARDUINO_MH_ET_LIVE_ESP32MINIKIT)  // MH-ET Live ESP32 Minikit
#define D3  17
#define D4  16

#define MH_Z19_RX   D3  // data sending pin
#define MH_Z19_TX   D4  // data receiving pin

HardwareSerial co2Serial(1); 
#else // WeMos D1 mini
#define D3  0
#define D4  2

#include <SoftwareSerial.h>

#define MH_Z19_RX D4
#define MH_Z19_TX D3

SoftwareSerial co2Serial(MH_Z19_TX, MH_Z19_RX); // define MH-Z19 (receiving pin, sending pin)
#endif

#define MAX_DATA_ERRORS 10  // max of errors, reset after them
#define STARTUP_DELAY 3000  // start up delay (ms)
#define RESET_DELAY   2000  // delay before reset (ms)

extern void setupDeviceName(char deviceNameBuffer[], const char* deviceName);

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void setResetFunc(void(* func) (void)) {
  resetFunc = func;
}

byte checkSum(byte data[]) {
  byte value = 0;
  for ( int i=1; i < 9 ; i++ ) {
    value += data[i];
  }
  value = ~value + 1;
  return value;
}

int co2temperature;
int co2status;

int readCO2()
{
  // command to ask for data
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  byte response[9]; // for answer

  // discard read buffer (skip buffered input)
  while(co2Serial.available() > 0) 
  {
        byte incomingData = co2Serial.read();
        Log.notice("discard:0x%x\n", incomingData);
  }

  co2Serial.write(cmd, sizeof(cmd)); //request PPM CO2

  // read response
  int responseSize;
  responseSize = co2Serial.readBytes(response, sizeof(response));

  if (response[0] != 0xFF)
  {
    Log.notice("wrong response:size=%d:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x: checkSum=0x%x\n", 
      responseSize,
      response[0],response[1],response[2],response[3],response[4],response[5],response[6],response[7],response[8],
      checkSum(response));

    return -1;
  }

  if (response[1] != 0x86)
  {
    Log.notice("wrong command from co2 sensor!: 0x%x\n", response[1]);

    return -1;
  }

  int responseHigh = (int) response[2];
  int responseLow = (int) response[3];
  int ppm = (256 * responseHigh) + responseLow;

//Arduino IDE library for operating the MH-Z19 CO2 sensor in ESP-WROOM-02/32(esp8266/ESP32) or Arduino
// https://github.com/nara256/mhz19_uart
  co2temperature = (int) response[4]-40;
  co2status = response[5];

  Log.notice("ppm=%d, temp=%d, status=%d\n", ppm, co2temperature, co2status);

  return ppm;
}

void setupMHZ19(char deviceNameBuffer[]) {
  Log.notice("+setupMHZ19\n");

  // set device name
  setupDeviceName(deviceNameBuffer, DEVICE_NAME);

#if defined(ARDUINO_MH_ET_LIVE_ESP32MINIKIT)  // MH-ET Live ESP32 Minikit
  co2Serial.begin(9600, SERIAL_8N1, MH_Z19_TX, MH_Z19_RX); // start MH-Z19
#else
  co2Serial.begin(9600); // start MH-Z19
#endif

  Log.notice("Waiting for sensors to be initalized\n");
  delay(STARTUP_DELAY);

  Log.notice("-setupMHZ19\n");
}


long previousMillis = 0;
int errorCount = 0;

int updateMHZ19(JsonObject& jsonObject)
{
  long currentMillis = millis();
  if (currentMillis - previousMillis < MINIMUM_UPDATE_INTERVAL)
    return -1;
  previousMillis = currentMillis;

  if (errorCount > MAX_DATA_ERRORS)
  {
    Log.error("MH-z19 Too many errors. call reset function\n");
    delay(RESET_DELAY);
    if ( resetFunc ) {
      resetFunc();
    }
  }

  int ppm = readCO2();
  Log.notice("PPM = %d\n", ppm);

  bool dataError = false;
  if (ppm < 100 || ppm > 6000)
  {
    Log.warning("PPM not valid: %d\n", ppm);
    dataError = true;
  }
 
  if (dataError)
  {
    errorCount++;

    Log.warning("Skipping: error count=%d\n", errorCount);
    return errorCount;
  }

  jsonObject["CO2"]   = ppm;
  jsonObject["Temperature"]   = co2temperature;

  errorCount = 0;
  return 0;
}