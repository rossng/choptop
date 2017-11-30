#include <thread>
#include <csignal>
#include <iostream>
#include <atomic>
#include "CLI11.hpp"
#include "hx711.h"

using namespace std;

float sensor_inputs[] = {0.0f, 0.0f, 0.0f, 0.0f};
thread threads[4];
std::atomic<bool> executing(true);
std::mutex wiringPiMutex;

void gracefulShutdown(int s) {
    executing = false;

    for (auto &thread : threads) {
        thread.join();
    }
    exit(1);
}

void getReadings(uint8_t clk, uint8_t data, int index) {
    HX711 sensor(clk, data, 0, wiringPiMutex);
    sensor.tare();
    sensor.setScale(16000);
    while (executing) {
        sensor_inputs[index] = sensor.getUnits();
    }
}

float clamp(float n, float hi, float lo) {
    return std::min(std::max(n, lo), hi);
}

void printValues(vector<int> sensors, bool xpos, bool ypos) {
    threads[0] = thread(getReadings, 6, 5, 0);
    threads[1] = thread(getReadings, 8, 7, 1);
    threads[2] = thread(getReadings, 10, 9, 2);
    threads[3] = thread(getReadings, 21, 20, 3);

    //signal(SIGINT, gracefulShutdown);
    float weights[] = {0.0f, 0.0f, 0.0f, 0.0f};
    while (executing) {
        float total = 0;
        for (int i = 0; i < 4; i++) {
            weights[i] = sensor_inputs[i];
            total += weights[i];
        }

        float x = clamp((weights[0] + weights[1]) / total, 0.0, 1.0f);
        float y = clamp((weights[1] + weights[2]) / total, 0.0, 1.0f);

        for (const auto sensor : sensors) {
            printf("Sensor %d: %.2f\n", sensor, weights[sensor]);
        }

        if (xpos) {
            printf("X %.2f", x);
        }
        if (xpos && ypos) {
            printf(", ");
        }
        if (xpos) {
            printf("Y %.2f", y);
        }
        if (xpos || ypos) {
            printf("\n");
        }
    }
}

int main(int argc, char** argv) {
    CLI::App app{"Choptop - an interactive chopping board"};
    app.require_subcommand(1);

    auto print = app.add_subcommand("print", "Print a stream of values from Choptop");

    vector<int> print_sensors;
    bool print_xpos, print_ypos;
    print->add_option("--sensors", print_sensors, "Sensors to be printed");
    print->add_flag("--xpos", print_xpos, "Print x position");
    print->add_flag("--ypos", print_ypos, "Print y position");

    CLI11_PARSE(app, argc, argv);

    signal(SIGINT, gracefulShutdown);
    signal(SIGTERM, gracefulShutdown);
    signal(SIGKILL, gracefulShutdown);
    signal(SIGABRT, gracefulShutdown);

    if (app.got_subcommand("print")) {
        printf("Printing values\n");
        printValues(print_sensors, print_xpos, print_ypos);
    }
}