#ifndef ds1621
#define ds1621

#include <Arduino.h>
#include <Wire.h>

#define CONFIG 0x03
#define R_CONFIG 0xAC
#define R_CONVERSION_START 0xEE
#define R_CONVERSION_STOP 0x22
#define R_TEMPERATURE 0xAA

class Ds1621
{
public:
    Ds1621();
    bool begin(uint8_t adresse);
    void setTemperature(float temperature);
    float getTemperature();

private:
    void _startConversion();
    void _stopConversion();
    uint8_t _adresse;
    float _temperature;
};

#endif