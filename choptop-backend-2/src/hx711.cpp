//#define IS_RPI
#ifdef IS_RPI

#include <cstdio>
#include <cinttypes>
#include <wiringPi.h>
#include <algorithm>
#include <thread>
#include <iostream>
#include <csignal>
#include <vector>
#include "hx711.h"

HX711::HX711(uint8_t clockPin, uint8_t dataPin, uint8_t skipSetup, std::mutex &wiringPiMutex) :
        gainBits_(1),
        scale_(1.0f),
        offset_(0),
        clockPin_(clockPin),
        dataPin_(dataPin),
        wiringPiMutex_(wiringPiMutex) {
    this->initialize(skipSetup);
}

void HX711::initialize(uint8_t skipSetup) {
    wiringPiMutex_.lock();
    if ((!skipSetup) && wiringPiSetupGpio() == -1) {
        printf("initialization failed");
    }
    pinMode(clockPin_, OUTPUT);
    pinMode(dataPin_, INPUT);
    wiringPiMutex_.unlock();
}

bool HX711::isReady() {
    return digitalRead(dataPin_) == LOW;
}

void HX711::setGain(uint8_t gain) {
    switch (gain) {
        case GAIN_128:
            this->gainBits_ = 1;
            break;
        case GAIN_64:
            this->gainBits_ = 3;
            break;
        case GAIN_32:
            this->gainBits_ = 2;
            break;
        default:
            //invalid gain, ignore
            break;
    }

    digitalWrite(clockPin_, LOW);
    read();
}

int32_t HX711::read() {
    // wait for the chip to become ready
    while (!this->isReady());

    int32_t data = 0;
    // pulse the clock pin 24 times to read the data
    for (uint8_t i = 24; i--;) {
        digitalWrite(clockPin_, HIGH);

        digitalRead(dataPin_);
        data |= (digitalRead(dataPin_) << i);

        digitalWrite(clockPin_, LOW);
    }

    // set the channel and the gain factor for the next reading using the clock pin
    for (int i = 0; i < gainBits_; i++) {
        digitalWrite(clockPin_, HIGH);
        digitalWrite(clockPin_, LOW);
    }

    // Sign-extend 24-bit integer to 32 bits
    if (data & 0x800000) {
        data |= (long) ~0xffffff;
    }

    return data;
}

int32_t HX711::readAverage(uint8_t times) {
    int64_t sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += read();
    }
    return sum / times;
}

int32_t HX711::readMedian(uint8_t times) {
    std::vector<int32_t> readings(times);
    for (uint8_t i = 0; i < times; i++) {
        readings.push_back(read());
    }
    std::sort(readings.begin(), readings.end());
    return readings[times/2];
}

int32_t HX711::getRawValue(uint8_t times) {
    return readAverage(times) - offset_;
}

float HX711::getUnits(uint8_t times) {
    return getRawValue(times) / scale_;
}

void HX711::tare(uint8_t times) {
    uint64_t sum = readMedian(times);
    setOffset(sum);
}

void HX711::setScale(float scale) {
    scale_ = scale;
}

void HX711::setOffset(int32_t offset) {
    offset_ = offset;
}

void HX711::powerDown() {
    digitalWrite(clockPin_, LOW);
    digitalWrite(clockPin_, HIGH);
}

void HX711::powerUp() {
    digitalWrite(clockPin_, LOW);
}

int32_t HX711::getOffset() {
    return this->offset_;
}

float HX711::getScale() {
    return this->scale_;
}

#endif