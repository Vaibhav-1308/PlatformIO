
#include <Arduino.h>
// #include <Adafruit_SPIDevice.h>
// add import for freeRTOS compatibility
#include <FreeRTOS.h>

#define LED_PIN 48
uint8_t led1 = 48;
uint8_t led2 = 15;

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/Org_01.h>

#include <Adafruit_SSD1306.h>
#define SSD1306_NO_SPLASH
#include "data_collection.h"
void findSPI();
int mapTemperatureToValue(float temperature);
void blikLED(void *parameter);

// Define your CS and DREADY pins
#define CS_PIN 10
#define DREADY_PIN 14

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// Thermometer bitmap
const uint8_t thermometer_icon[] = {0x00, 0x7c, 0x00, 0x00, 0xfe, 0x00, 0x00, 0xee, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x01, 0xc7, 0x00, 0x03, 0x83, 0x80, 0x03, 0x01, 0x80, 0x03, 0x01, 0x80, 0x03, 0x01, 0x80, 0x03, 0x83, 0x80, 0x01, 0xc7, 0x00, 0x01, 0xff, 0x00, 0x00, 0x7c, 0x00};
const uint8_t battery_charging_icon[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x33, 0xe0, 0xfe, 0x73, 0xf0, 0xc0, 0x60, 0x30, 0xc0, 0xe0, 0x30, 0xc1, 0xc0, 0x30, 0xc1, 0x80, 0x33, 0xc3, 0xfc, 0x33, 0xc3, 0xfc, 0x33, 0xc0, 0x18, 0x33, 0xc0, 0x38, 0x30, 0xc0, 0x70, 0x30, 0xc0, 0x60, 0x70, 0xfc, 0xe7, 0xf0, 0x7c, 0xc7, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
DataCollection tempSensor(CS_PIN, DREADY_PIN);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
void displayText(String text);
void animateLogo(void);

// FreeRTOS task
TaskHandle_t Task1;

void blikLED(void *parameter)
{
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  uint8_t *led = (uint8_t *)parameter;
  Serial.println(*led);
  while (1)
  {
    digitalWrite(*led, HIGH);
    delay(1000);
    digitalWrite(*led, LOW);
    delay(1000);
  }
}

void setup()
{

  Serial.begin(115200);
  Serial.println("Welcome to PyroVU");
  findSPI();

  // Initialize the sensor
  if (!tempSensor.begin())
  {
    Serial.println(tempSensor.getLastError());
    while (1)
      ;
  }
  tempSensor.setThermocoupleType(MAX31856_TCTYPE_K);
  Serial.println(tempSensor.readTemperature());

  // maxthermo.setConversionMode(MAX31856_CONTINUOUS);
  Wire.begin(15, 16);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
               // Clear the buffer
  display.clearDisplay();
  // animateLogo();

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  xTaskCreatePinnedToCore(
      blikLED,
      "Task1",
      2048,
      &led1,
      2,
      &Task1,
      1);
}

void loop()
{

  float temperature = tempSensor.readTemperature();
  if (isnan(temperature))
  {
    Serial.println(tempSensor.getLastError());
  }
  else
  {
    Serial.print(">Temperature: ");
    Serial.println(temperature);
    int value = mapTemperatureToValue(temperature);
    Serial.print(">Value: ");
    Serial.println(value);
    displayText(": " + String(temperature) + " C");
  }

  delay(2000);
}

void findSPI() // pass by ref
{

  delay(2000);
  Serial.println("Default SPI pins for this board:");
  Serial.print("SS :");
  Serial.println(SS);
  Serial.print("MOSI : ");
  Serial.println(MOSI);
  Serial.print("bSCK :");
  Serial.println(SCK);
  Serial.print("MISO : ");
  Serial.println(MISO);
}

int mapTemperatureToValue(float temperature)
{
  // Ensure temperature is within the expected range
  if (temperature < 27.0)
    temperature = 27.0;
  if (temperature > 34.0)
    temperature = 34.0;

  // Map the temperature to a value from 10 to 100
  int value = (int)((temperature - 27.0) / 7.0 * 90.0) + 10;

  // Round to the nearest multiple of 10
  value = (value + 5) / 10 * 10;

  return value;
}

// void drawLogo(void) {
//   display.clearDisplay();

//   display.drawBitmap(
//     (display.width()  - LOGO_WIDTH ) / 2,
//     (display.height() - LOGO_HEIGHT) / 2,
//     logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
//   display.display();
//   delay(1000);
// }

void displayText(String text)
{
  // https://rickkas7.github.io/DisplayGenerator/index.html
  display.clearDisplay();
  // display.fillRect(0, 0, 25, 12, 1);
  display.setCursor(33, 5);
  display.setFont(&Org_01);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.println("PyroVU");
  display.fillRect(103, 0, 25, 12, 1);

  // display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);         // Draw 'inverse' text
  display.drawBitmap(0, 0, battery_charging_icon, 24, 24, 1); // Draw thermometer bitmap

  display.setFont(&FreeSans9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.drawBitmap(10, 20, thermometer_icon, 24, 24, 1); // Draw thermometer bitmap

  display.setTextSize(1);    // Normal 1:1 pixel scale
  display.setCursor(31, 38); // Start at center
  display.println(F(text.c_str()));

  display.setCursor(20, 56);
  display.setTextSize(1);
  display.setFont(NULL);
  display.println("TYPE: K");
  display.fillRect(0, 62, 128, 10, 1);
  // display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  // display.println(3.141592);

  display.display();
}

void animateLogo(void)
{
  display.clearDisplay();

  display.setFont(&Org_01);

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("PyroVU"));
  display.display(); // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(1000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(1000);
  display.stopscroll();
  delay(1000);
}