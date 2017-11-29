#include <thread>
#include <csignal>
#include <iostream>
#include "hx711.h"

using namespace std;

float sensor_inputs[] = {0.0f, 0.0f, 0.0f, 0.0f};
thread threads[4];

void gracefulShutdown(sig_t s) {
    for (int i = 0; i < 4; i++) {
        threads[i].join();
    }
    exit(1);
}

void getReadings(int clk, int data, int index) {
    HX711 sensor(clk, data, 0);
    sensor.tare();
    sensor.setScale(16000);
    while (true) {
        sensor_inputs[index] = sensor.getUnits();
    }
}

float clamp(float n, float hi, float lo) {
    return std::min(std::max(n, lo), hi);
}

int main() {
    threads[0] = thread(getReadings, 6, 5, 0);
    threads[1] = thread(getReadings, 8, 7, 1);
    threads[2] = thread(getReadings, 10, 9, 2);
    threads[3] = thread(getReadings, 21, 20, 3);

    //signal(SIGINT, gracefulShutdown);
    float weights[] = {0.0f, 0.0f, 0.0f, 0.0f};
    while (true) {
        float total = 0;
        for (int i = 0; i < 4; i++) {
            weights[i] = sensor_inputs[i];
            total += weights[i];
        }
        float x = clamp((weights[0] + weights[1]) / total, 0.0, 1.0f);
        float y = clamp((weights[1] + weights[2]) / total, 0.0, 1.0f);
        std::cout << x << "," << y << std::endl;
    }
}