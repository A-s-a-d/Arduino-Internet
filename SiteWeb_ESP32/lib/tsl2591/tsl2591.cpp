#include <tsl2591.h>

TSL2591::TSL2591()
{
    _adress = 0;
    _gain = 0;
    _time = 0;
}

bool TSL2591::begin(uint8_t i2cAdress)
{
    _adress = i2cAdress;
    return Wire.begin();
}

void TSL2591::config(uint8_t gain, uint8_t time)
{
    _gain = gain;
    _time = time;

    uint8_t data = (_gain << 4) | _time;

    Wire.beginTransmission(_adress);
    Wire.write(CMD_DATA | CONTROL); // On réalise un | (ou) avec le registre pour indiquer au registre de commande son mode de fonctionnement avec l'adresse du registre que l'on veut accéder
    Wire.write(data);
    Wire.endTransmission();
}

void TSL2591::enable()
{
    uint8_t EN_data = 0x13; // AIen bit, Power et AEN bit

    Wire.beginTransmission(_adress);
    Wire.write(CMD_DATA | ENABLE);
    Wire.write(EN_data);
    Wire.endTransmission();
}

void TSL2591::disable()
{
    uint8_t EN_data = 0;

    Wire.beginTransmission(ENABLE);
    Wire.write(CMD_DATA | ENABLE);
    Wire.write(EN_data);
    Wire.endTransmission();
}

uint8_t TSL2591::getId()
{
    Wire.beginTransmission(_adress);
    Wire.write(CMD_DATA | ID_REG);
    Wire.endTransmission();

    Wire.requestFrom(_adress, byte(1));
    uint8_t verif = Wire.read();
    Wire.endTransmission();

    return verif;
}

float TSL2591::calculateLux()
{
    enable();
    uint32_t data = _getAllDatas();
    uint16_t gain;

    switch (_gain)
    {
    case 0:
        gain = 1;
        break;
    case 1:
        gain = 25;
        break;
    case 2:
        gain = 428;
        break;
    default:
        gain = 9876;
        break;
    }

    uint16_t allCH0 = data >> 16;
    uint16_t allCH1 = data;

    if (allCH0 == 0xFFFF || allCH1 == 0xFFFF)
        return -1;

    disable();
    float cpl = ((_time * 100 + 100) * gain) / 408.0;                // Calcul issu du TD
    float lux = ((allCH0 - allCH1) * (1 - (allCH1 / allCH0))) / cpl; // Calcul issu du TD
    return lux;
}

uint32_t TSL2591::_getAllDatas() // Voir si on ne peut pas faire la partie data et décalage en même tps que le Wire.read()
{

    delay((_time * 100 + 120)); // Delai pour respecter les délais de la datasheet + 20 ms par sécurité

    uint32_t data;

    Wire.beginTransmission(_adress);
    Wire.write(CMD_DATA | CH0_L);
    Wire.endTransmission();

    Wire.requestFrom(_adress, byte(4));
    uint8_t LOW_ch0 = Wire.read();
    uint8_t HIGH_ch0 = Wire.read();
    uint8_t LOW_ch1 = Wire.read();
    uint8_t HIGH_ch1 = Wire.read();
    Wire.endTransmission();

    data = HIGH_ch0;
    data = (data << 8) | LOW_ch0;
    data = (data << 8) | HIGH_ch1;
    data = (data << 8) | LOW_ch1;
    return data;
}
