#ifndef tsl2591
#define tsl2591

#include <Arduino.h>
#include <Wire.h>

#define CMD_DATA 0xA0
#define CMD_TYPE 0x80
#define ENABLE 0x0
#define CONTROL 0x01
#define ID_REG 0x12
#define CH0_L 0x14
#define CH0_H 0x15
#define CH1_L 0x16
#define CH1_H 0x17

class TSL2591
{
public:
    TSL2591();
    bool begin(uint8_t i2cAdress);
    void config(uint8_t gain, uint8_t time);
    void enable();
    void disable();
    uint8_t getId();
    float calculateLux();
    enum AGain {GAIN_LOW = 0x0, GAIN_MED = 0x1, GAIN_HIGH = 0x2, GAIN_MAX = 0x3};
    enum ATime {ATIME_100 = 0x0, ATIME_200 = 0x1, ATIME_300 = 0x2, ATIME_400 = 0x3, ATIME_500 = 0x4, ATIME_600 = 0x5};
private:
    uint32_t _getAllDatas();
    uint8_t _adress;
    uint8_t _gain;
    uint8_t _time;
};

#endif