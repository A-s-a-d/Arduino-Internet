#include <ds1621.h>

Ds1621::Ds1621()
{
  _adresse = 0;
  _temperature = 0.0f;
}

bool Ds1621::begin(uint8_t adresse)
{
  _adresse = adresse;
  bool res = Wire.begin();
  Wire.beginTransmission(_adresse);
  Wire.write(R_CONFIG);
  Wire.write(CONFIG);
  Wire.endTransmission();
  return res;
}

void Ds1621::_startConversion()
{
  Wire.beginTransmission(_adresse);
  Wire.write(R_CONVERSION_START);
  Wire.endTransmission();
}

void Ds1621::_stopConversion()
{
  Wire.beginTransmission(_adresse);
  Wire.write(R_CONVERSION_STOP);
  Wire.endTransmission();
}

void Ds1621::setTemperature(float temperature)
{
  _temperature = temperature;
}

float Ds1621::getTemperature()
{
  uint8_t msb, lsb;

  _startConversion();

  Wire.beginTransmission(_adresse);
  Wire.write(R_TEMPERATURE);
  Wire.endTransmission();

  Wire.requestFrom(_adresse, byte(2));
  msb = Wire.read();
  lsb = Wire.read();
  Wire.endTransmission();

  _stopConversion();

  if ((msb & 0x80) > 0)
    _temperature = -(msb ^ 0xFF) + 1;
  else
    _temperature = msb;

  if ((lsb & 0x80) > 0)
    _temperature += 0.5;
  else
    _temperature *= 1;

  return _temperature;
}