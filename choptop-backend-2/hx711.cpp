#include <wiringPi.h>
#include <wiringShift.h>
#include <thread>
#include <wiringSerial.h>
#include <iostream>
#include "hx711.h"

HX711::HX711(char dout, char pd_sck, char gain) {
	begin(dout, pd_sck, gain);
}

HX711::HX711() {
}

HX711::~HX711() {
}

void HX711::begin(char dout, char pd_sck, char gain) {
	PD_SCK = pd_sck;
	DOUT = dout;

	pinMode(PD_SCK, OUTPUT);
	pinMode(DOUT, INPUT);

	set_gain(gain);
}

bool HX711::is_ready() {
	return digitalRead(DOUT) == LOW;
}

void HX711::set_gain(char gain) {
	switch (gain) {
		case 128:		// channel A, gain factor 128
			GAIN = 1;
			break;
		case 64:		// channel A, gain factor 64
			GAIN = 3;
			break;
		case 32:		// channel B, gain factor 32
			GAIN = 2;
			break;
	}

	digitalWrite(PD_SCK, LOW);
	read();
}

long HX711::read() {
	// wait for the chip to become ready
	while (!is_ready()) {
		// Will do nothing on Arduino but prevent resets of ESP8266 (Watchdog Issue)
		std::this_thread::yield();
	}

	unsigned long value = 0;
	uint8_t data[3] = { 0 };
	uint8_t filler = 0x00;

	// pulse the clock pin 24 times to read the data
	data[2] = shiftIn(DOUT, PD_SCK, MSBFIRST);
	data[1] = shiftIn(DOUT, PD_SCK, MSBFIRST);
	data[0] = shiftIn(DOUT, PD_SCK, MSBFIRST);

	// set the channel and the gain factor for the next reading using the clock pin
	for (unsigned int i = 0; i < GAIN; i++) {
		digitalWrite(PD_SCK, HIGH);
		digitalWrite(PD_SCK, LOW);
	}

	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (data[2] & 0x80) {
		filler = 0xFF;
	} else {
		filler = 0x00;
	}

	// Construct a 32-bit signed integer
	value = ( static_cast<unsigned long>(filler) << 24
			| static_cast<unsigned long>(data[2]) << 16
			| static_cast<unsigned long>(data[1]) << 8
			| static_cast<unsigned long>(data[0]) );

	return static_cast<long>(value);
}

long HX711::read_average(char times) {
	long sum = 0;
	for (char i = 0; i < times; i++) {
		sum += read();
		std::this_thread::yield();
	}
	return sum / times;
}

double HX711::get_value(char times) {
	return read_average(times) - OFFSET;
}

float HX711::get_units(char times) {
	return get_value(times) / SCALE;
}

void HX711::tare(char times) {
	double sum = read_average(times);
	set_offset(sum);
}

void HX711::set_scale(float scale) {
	SCALE = scale;
}

float HX711::get_scale() {
	return SCALE;
}

void HX711::set_offset(long offset) {
	OFFSET = offset;
}

long HX711::get_offset() {
	return OFFSET;
}

void HX711::power_down() {
	digitalWrite(PD_SCK, LOW);
	digitalWrite(PD_SCK, HIGH);
}

void HX711::power_up() {
	digitalWrite(PD_SCK, LOW);
}

int main(){
    wiringPiSetup();    
    HX711 scale(5, 6);  
    scale.tare();

    while(true){
        scale.power_up();
        std::cout << scale.get_units(10) << std::endl;
        scale.power_down();
    }
    return 0;
}