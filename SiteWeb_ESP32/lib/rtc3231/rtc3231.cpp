#include <rtc3231.h>

const String TAB_DAY[] = {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"};
const String TAB_MONTH[] = {"Jan", "Fev", "Mar", "Avr", "Mai", "Juin", "Juil", "Aout", "Sept", "Oct", "Nov", "Dec"};

Rtc3231::Rtc3231()
{
    _adress = 0;
    _second = 0;
    _minute = 0;
    _hour = 0;
    _day = 0;
    _num = 0;
    _month = 0;
    _Sday = "";
    _Smonth = "";
}

bool Rtc3231::begin(uint8_t adress)
{
    _adress = adress;
    return Wire.begin();
}

void Rtc3231::setTime(uint8_t hh, uint8_t mm, uint8_t ss)
{
    _set(hh, R_HOUR);
    _set(mm, R_MINUTE);
    _set(ss, R_SECOND);
}

void Rtc3231::setDate(uint8_t day, uint8_t num, uint8_t month)
{
    _set(day, R_DAY);
    _set(num, R_NUM);
    _set(month, R_MONTH);
}

uint8_t Rtc3231::getSecond()
{
    _second = _get(R_SECOND);
    return _second;
}

uint8_t Rtc3231::getMinute()
{
    _minute = _get(R_MINUTE);
    return _minute;
}

uint8_t Rtc3231::getHour() // Il faut finir la fontion notamment en regardant ce qu'il faut faire pour les 4 premiers bits
{
    _hour = _get(R_HOUR);
    return _hour;
}

String Rtc3231::getStringDay()
{
    _day = _get(R_DAY);
    _Sday = TAB_DAY[_day];
    return _Sday;
}

uint8_t Rtc3231::getNumber()
{
    _num = _get(R_NUM);
    return _num;
}

String Rtc3231::getStringMonth()
{
    Wire.beginTransmission(_adress);
    Wire.write(R_MONTH);
    Wire.endTransmission();

    Wire.requestFrom(_adress, byte(1));
    _month = _BcdToBin(Wire.read() & 0x1F); // On fait un masque pour garder que les bits correspondant au mois et enlever le siècle
    Wire.endTransmission();

    _Smonth = TAB_MONTH[_month % 13];
    return _Smonth;
}

void Rtc3231::_set(uint8_t val, uint8_t registre)
{
    Wire.beginTransmission(_adress);
    Wire.write(registre);
    Wire.write(_BinToBcd(val));
    Wire.endTransmission();
}

uint8_t Rtc3231::_get(uint8_t registre)
{
    Wire.beginTransmission(_adress);
    Wire.write(registre);
    Wire.endTransmission();

    Wire.requestFrom(_adress, byte(1));
    uint8_t val = _BcdToBin(Wire.read());
    Wire.endTransmission();
    return val;
}

uint8_t Rtc3231::_BinToBcd(uint8_t val)
{
    return (val / 10) * 16 + val % 10;
}

uint8_t Rtc3231::_BcdToBin(uint8_t val)
{
    return (val / 16) * 10 + val % 16;
}