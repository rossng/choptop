#include <stdio.h>
#include <inttypes.h>
#include <wiringPi.h>
#include <algorithm>
#include <thread>
#include <iostream>
#include <signal.h>
#include <vector>
#include "hx711.h"
using std::thread;
float sensor_inputs[] = {0f, 0f, 0f, 0f};
std::vector<thread> threads;

HX711::HX711(uint8_t clockPin, uint8_t dataPin, uint8_t skipSetup) :
	mGainBits(1),
	mScale(1.0f),
	mOffset(0),
	mClockPin(clockPin),
	mDataPin(dataPin)
{
	this->initialize(skipSetup);
}

void HX711::initialize(uint8_t skipSetup){
	if((!skipSetup) && wiringPiSetupGpio() == -1){
		printf("initialization failed");
	}
	pinMode(mClockPin, OUTPUT);
	pinMode(mDataPin, INPUT);
}

bool HX711::isReady(){
	return digitalRead(mDataPin) == LOW;
}

void HX711::setGain(uint8_t gain){
	switch(gain){
		case GAIN_128:
			this->mGainBits = 1;
		break;
		case GAIN_64:
			this->mGainBits = 3;
		break;
		case GAIN_32:
			this->mGainBits = 2;
		break;
		default:
			//invalid gain, ignore
		break;
	}

	digitalWrite(mClockPin, LOW);
	read();
}

int32_t HX711::read() {
	// wait for the chip to become ready
	while (!this->isReady());

	int32_t data = 0;
	// pulse the clock pin 24 times to read the data
	for(uint8_t i = 24; i--;){
		digitalWrite(mClockPin, HIGH);

		digitalRead(mDataPin);
		data |= (digitalRead(mDataPin) << i);

		digitalWrite(mClockPin, LOW);
	}

	// set the channel and the gain factor for the next reading using the clock pin
	for (int i = 0; i < mGainBits; i++) {
		digitalWrite(mClockPin, HIGH);
		digitalWrite(mClockPin, LOW);
	}

	if(data & 0x800000){
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
	digitalWrite(mClockPin, LOW);
	digitalWrite(mClockPin, HIGH);
}

void HX711::powerUp() {
	digitalWrite(mClockPin, LOW);
}

int32_t HX711::getOffset(){
	return this->mOffset;
}

float HX711::getScale(){
	return this->mScale;
}

void graceful_shutdown(sig_t s){
    for(auto t : threads){
        t.join();
    }
    exit(1);
}

void getReadings(int clk, int data, int index){
    HX711 sensor(clk, data, 0);
    sensor.tare();
    sensor.setScale(16000);
    while(true){
        sensor_inputs[index] = sensor.getUnits();
    }
}

int main(){
    thread sensor_a(getReadings, 6, 5, 0);     
    thread sensor_b(getReadings, 8, 7, 1);
	thread sensor_c(getReadings, 10, 9, 2);
    thread sensor_d(getReadings, 21, 20, 3);
    threads = {sensor_a, sensor_b, sensor_c, sensor_d};
    signal(SIGINT, graceful_shutdown)
    float weights[] = {0f, 0f, 0f, 0f};
	while(true){
        float total = 0;
        for(int i=0; i<4; i++){
            weights[i] = sensor_inputs[i];
            total += weights[i];
        }
        float x = std::clamp((weights[0] + weights[1]) / total, 0f, 1f);
        float y = std::clamp((weights[1] + weights[2]) / total, 0f, 1f);
        std::cout << x << "," << y << std::endl;
	}
}

