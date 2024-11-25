#ifndef MAX31856_TEMPERATURE_SENSOR_H
#define MAX31856_TEMPERATURE_SENSOR_H
// Refer this : https://github.com/adafruit/Adafruit_MAX31865 - RTD-to-Digital Converter
// Refer this: https://github.com/adafruit/Adafruit_MAX31856 - Thermocouple
// Refer for SPI: https://randomnerdtutorials.com/esp32-spi-communication-arduino/

#include <Adafruit_SPIDevice.h>
// #include <Adafruit_MAX31865.h> // RTD-to-Digital Converter
#include <Adafruit_MAX31856.h>

class DataCollection
{
public:
  // Constructor
  DataCollection(uint8_t csPin, uint8_t dReadyPin);

  // Initialize the sensor
  bool begin();

  // Set the thermocouple type
  void setThermocoupleType(max31856_thermocoupletype_t type);

  // Read temperature
  float readTemperature();

  // Get last error message
  String getLastError();

private:
  uint8_t _csPin;
  uint8_t _dReadyPin;
  Adafruit_MAX31856 _max31856;
  String _lastError;
};

#endif // MAX31856_TEMPERATURE_SENSOR_H
