#include <boost/thread/thread.hpp>
#include <csignal>
#include <thread>
#include <iostream>
#include <atomic>
#include "CLI11.hpp"
#include "hx711.h"
#include "LoadCellReader.h"
#include <map>
#include <memory>
#include <boost/lockfree/spsc_queue.hpp>

using namespace std;

std::vector<thread> threads;
std::atomic<bool> executing(true);
mutex wiring_pi_mutex;

std::map<int, shared_ptr<LoadCellReader>> load_cell_readers;

void gracefulShutdown(int s) {
    executing = false;

    for (auto &load_cell_reader : load_cell_readers) {
        load_cell_reader.second->stopProducing();
        load_cell_reader.second->stopConsuming();
    }
    exit(1);
}

float clamp(float n, float hi, float lo) {
    return std::min(std::max(n, lo), hi);
}

shared_ptr<HX711> makeHX711(uint8_t clk, uint8_t data, float scale, mutex &wiring_pi_mutex) {
    auto sensor = make_shared<HX711>(clk, data, 0, wiring_pi_mutex);
    sensor->tare(100);
    sensor->setScale(scale);
    return sensor;
}

void startSensors(vector<int> enable_sensors, string log_sensors) {
    this_thread::sleep_for(500ms);
    if (std::find(enable_sensors.begin(), enable_sensors.end(), 0) != enable_sensors.end()) {
        load_cell_readers[0] = make_shared<LoadCellReader>(makeHX711(6, 5, 443.7, wiring_pi_mutex));
    }
    if (std::find(enable_sensors.begin(), enable_sensors.end(), 1) != enable_sensors.end()) {
        load_cell_readers[1] = make_shared<LoadCellReader>(makeHX711(8, 7, 453.3, wiring_pi_mutex));
    }
    if (std::find(enable_sensors.begin(), enable_sensors.end(), 2) != enable_sensors.end()) {
        load_cell_readers[2] = make_shared<LoadCellReader>(makeHX711(10, 9, 422.2, wiring_pi_mutex));
    }
    if (std::find(enable_sensors.begin(), enable_sensors.end(), 3) != enable_sensors.end()) {
        load_cell_readers[3] = make_shared<LoadCellReader>(makeHX711(21, 20, 411.3, wiring_pi_mutex));
    }

    for (auto &load_cell_reader : load_cell_readers) {
        load_cell_reader.second->startProducing();
        load_cell_reader.second->startConsuming();
    }
}

void printValues(const vector<int> &print_sensors, bool total_weight, bool xpos, bool ypos) {
    while (executing) {
        for (auto sensor : print_sensors) {
            load_cell_readers[sensor]->raw_output_.consume_one([sensor](float f) {
                printf("Sensor %d: %fg\n", sensor, f);
            });
        }
    }
}

int main(int argc, char** argv) {
    CLI::App app{"Choptop - an interactive chopping board"};
    app.require_subcommand(1);

    vector<int> enable_sensors;
    string log_sensors;
    app.add_option("--enable-sensors", enable_sensors, "Sensors to be enabled");
    app.add_option("--log-sensors", log_sensors, "File prefix to log sensor data to");

    auto print = app.add_subcommand("print", "Print a stream of values from Choptop");

    vector<int> print_sensors;
    bool print_total_weight, print_xpos, print_ypos;
    print->add_option("--sensors", print_sensors, "Sensors to be printed");
    print->add_flag("--total-weight", print_total_weight, "Print total weight");
    print->add_flag("--xpos", print_xpos, "Print x position");
    print->add_flag("--ypos", print_ypos, "Print y position");

    CLI11_PARSE(app, argc, argv);

    signal(SIGINT, gracefulShutdown);
    signal(SIGTERM, gracefulShutdown);
    //signal(SIGKILL, gracefulShutdown);
    signal(SIGABRT, gracefulShutdown);

    startSensors(enable_sensors, log_sensors);

    if (app.got_subcommand("print")) {
        printf("Printing values\n");
        printValues(print_sensors, print_total_weight, print_xpos, print_ypos);
    }
}