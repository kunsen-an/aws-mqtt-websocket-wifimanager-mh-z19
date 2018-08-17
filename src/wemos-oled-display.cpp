#include <SPI.h>
#include <Wire.h>

#include <ArduinoLog.h>

//#define USE_ADAFRUIT_LIBRARIES  1
#define USE_SPARKFUN_LIBRARIES 1

#define OLED_SETUP_MESSAGE "OLED setup"

#define MAX_OLED_MESSAGE_SIZE 128

#if defined(USE_ADAFRUIT_LIBRARIES)
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#ifdef _Adafruit_SSD1306_H_
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 255 // No RESET for MH-ET Live Minikit for ESP32

#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#endif // _Adafruit_SSD1306_H_

Adafruit_SSD1306 oled(OLED_RESET);
#elif defined(USE_SPARKFUN_LIBRARIES)

// OLED
#include <SFE_MicroOLED.h> // Include the SFE_MicroOLED library
#define PIN_RESET 255      // MH-ET Live Minikit for ESP32
#define DC_JUMPER 0        // I2C Addres: 0 - 0x3C, 1 - 0x3D

MicroOLED oled(PIN_RESET, DC_JUMPER); // Example I2C declaration
#endif                     // USE_SPARKFUN_LIBRARIES

void clearOLED()
{
#if defined(USE_ADAFRUIT_LIBRARIES)
  oled.clearDisplay();
#elif defined(USE_SPARKFUN_LIBRARIES)
  oled.clear(PAGE);
#endif                  // USE_SPARKFUN_LIBRARIES
  oled.setCursor(0, 0); // Set the text cursor to the upper-left of the screen.
}

void displayOLED(const char *format, ...)
{
  char buf[MAX_OLED_MESSAGE_SIZE];

  va_list va;
  va_start(va, format);
  vsprintf(buf, format, va);
  va_end(va);

  clearOLED();
  oled.println(buf);
  oled.display(); // Draw to the screen
}

void setupOLED(int size)
{
  // OLED
  oled.begin();

#if defined(USE_ADAFRUIT_LIBRARIES)
  oled.clearDisplay();
  oled.setTextSize(size);
  oled.setTextColor(WHITE);
#elif defined(USE_SPARKFUN_LIBRARIES)
  // clear(ALL) will clear out the OLED's graphic memory.
  // clear(PAGE) will clear the Arduino's display buffer.
  oled.clear(ALL); // Clear the display's memory (gets rid of artifacts)

  //oled.setFontType(0);  // Set the text to small (10 columns, 6 rows worth of characters).
  oled.setFontType(1); // Set the text to medium (6 columns, 3 rows worth of characters).
  // oled.setFontType(2);  // Set the text to medium/7-segment (5 columns, 3 rows worth of characters).
  // oled.setFontType(3);  // Set the text to large (5 columns, 1 row worth of characters).
#endif // USE_SPARKFUN_LIBRARIES

  displayOLED(OLED_SETUP_MESSAGE);
}
