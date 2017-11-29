#ifndef IS_RPI

#include <cstdio>
#include <cinttypes>
#include <algorithm>
#include <thread>
#include <iostream>
#include <csignal>
#include <vector>
#include "hx711.h"

HX711::HX711(uint8_t clockPin, uint8_t dataPin, uint8_t skipSetup) :
        mGainBits(1),
        mScale(1.0f),
        mOffset(0),
        mClockPin(clockPin),
        mDataPin(dataPin),
        gen(0),
        dis(0, 16777215) {
    this->initialize(skipSetup);
}

void HX711::initialize(uint8_t skipSetup) {
}

bool HX711::isReady() {
    return true;
}

void HX711::setGain(uint8_t gain) {
}

int32_t HX711::read() {
    return dis(gen);
}

int32_t HX711::readAverage(uint8_t times) {
    int64_t sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += read();
    }
    return sum / times;
}

int32_t HX711::getRawValue(uint8_t times) {
    return readAverage(times) - mOffset;
}

float HX711::getUnits(uint8_t times) {
    return getRawValue(times) / mScale;
}

void HX711::tare(uint8_t times) {
    uint64_t sum = readAverage(times);
    setOffset(sum);
}

void HX711::setScale(float scale) {
    mScale = scale;
}

void HX711::setOffset(int32_t offset) {
    mOffset = offset;
}

void HX711::powerDown() {
}

void HX711::powerUp() {
}

int32_t HX711::getOffset() {
    return this->mOffset;
}

float HX711::getScale() {
    return this->mScale;
}

#endif
