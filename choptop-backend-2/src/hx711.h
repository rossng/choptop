#pragma once

#include <cinttypes>
#include <random>
#include <mutex>

//channel A, gain 128
#define GAIN_128 128

//channel A, gain 64
#define GAIN_64 64

//channel B, gain 32
#define GAIN_32 32

class HX711 {
private:
    uint8_t gainBits_;
    float scale_;
    int32_t offset_;

    uint8_t clockPin_;
    uint8_t dataPin_;
    std::mutex &wiringPiMutex_;

#ifndef IS_RPI
    std::mt19937 gen;
    std::uniform_int_distribution<> dis;
#endif

public:
    HX711(uint8_t clockPin, uint8_t dataPin, uint8_t skipSetup, std::mutex &wiringPiMutex);

    void initialize(uint8_t skipSetup);

    bool isReady();

    void setGain(uint8_t gain);

    int32_t read();

    int32_t readAverage(uint8_t times = 10);

    int32_t getRawValue(uint8_t times = 10);

    float getUnits(uint8_t times = 1);

    void tare(uint8_t times = 10);

    void setScale(float scale);

    void setOffset(int32_t offset);

    void powerDown();

    void powerUp();

    int32_t getOffset();

    float getScale();
};
