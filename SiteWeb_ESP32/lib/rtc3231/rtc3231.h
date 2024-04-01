#ifndef rtc3231
#define rtc3231

#include <Arduino.h>
#include <Wire.h>

#define R_SECOND 0x00
#define R_MINUTE 0x01
#define R_HOUR 0x02
#define R_DAY 0x03
#define R_NUM 0x04
#define R_MONTH 0x05

class Rtc3231
{
public:
    Rtc3231();
    bool begin(uint8_t adress); //Initialisation de l'horloge
    void setTime(uint8_t hh, uint8_t mm, uint8_t ss);
    void setDate(uint8_t day, uint8_t num, uint8_t month);
    uint8_t getSecond(); // Accès aux secondes
    uint8_t getMinute(); // Accès aux minutes
    uint8_t getHour(); // Accès aux heures
    String getStringDay(); // Accès au jour (Lundi ... Dimanche)
    String getStringMonth(); // Accès au mois (Jan ... Dec)
    uint8_t getNumber(); // Accès au numéro du jour (0 ... 31)
private:
    void _set(uint8_t val, uint8_t registe); // Insère une valeur dans un registre
    uint8_t _get(uint8_t registre); // Recupère la valeur d'un registre
    uint8_t _BinToBcd(uint8_t val); // Converti du binaire en BCD
    uint8_t _BcdToBin(uint8_t val); // Converti du BCD en binaire
    uint8_t _adress;
    uint8_t _second, _minute, _hour;
    uint8_t _day, _num, _month;
    String _Sday, _Smonth;
};

#endif