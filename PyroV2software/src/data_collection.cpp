#include "data_collection.h"
// Use software SPI: CS, DI, DO, CLK
// Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(10, 11, 12, 13);
// use hardware SPI, just pass in the CS pin
// Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(10);

// Constructor
DataCollection::DataCollection(uint8_t csPin, uint8_t dReadyPin)
    : _csPin(csPin), _dReadyPin(dReadyPin), _max31856(csPin) {}

// Initialize the sensor
bool DataCollection::begin()
{
  if (!_max31856.begin())
  {
    _lastError = "Failed to initialize MAX31856 sensor";
    // while (1)
    //   delay(10);
    return false;
  }

  // RTD begin
  return true;
}

// Set the thermocouple type
void DataCollection::setThermocoupleType(max31856_thermocoupletype_t type)
{
  _max31856.setThermocoupleType(type);
}

// Read temperature
float DataCollection::readTemperature()
{
  float temp;
  _max31856.triggerOneShot();
  delay(200);
  if (_max31856.conversionComplete())
  {
    temp = _max31856.readThermocoupleTemperature();
  }
  else
  {
    _lastError = "Conversion not complete!";
  }

  if (isnan(temp))
  {
    _lastError = "Failed to read temperature";
  }
  return temp;
}

// Get last error message
String DataCollection::getLastError()
{
  return _lastError;
}
