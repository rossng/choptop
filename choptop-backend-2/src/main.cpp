#include <boost/thread/thread.hpp>
#include <csignal>
#include <thread>
#include <iostream>
#include <atomic>
#include "CLI11.hpp"
#include "hx711.h"
#include "LoadCellReader.h"
#include "LoadCellsProcessor.h"
#include "PositionProcessor.h"
#include <map>
#include <memory>
#include <boost/lockfree/spsc_queue.hpp>

using namespace std;

vector<thread> threads;
atomic<bool> executing(true);
mutex wiring_pi_mutex;

map<int, shared_ptr<LoadCellReader>> load_cell_readers;
shared_ptr<PositionProcessor> position_processor;
shared_ptr<LoadCellsProcessor> load_cell_processor;

struct HX711Settings {
    uint8_t clk;
    uint8_t data;
    float scale;
};

map<int, HX711Settings> sensor_settings = {
        {0, {6,  5,  443.7}},
        {1, {8,  7,  453.3}},
        {2, {10, 9,  422.2}},
        {3, {21, 20, 411.3}}
};

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

    std::for_each(enable_sensors.begin(), enable_sensors.end(), [&](int id) {
        auto hx711 = makeHX711(sensor_settings[id].clk, sensor_settings[id].data, sensor_settings[id].scale,
                               wiring_pi_mutex);
        if (log_sensors.empty()) {
            load_cell_readers[id] = make_shared<LoadCellReader>(hx711);
        } else {
            load_cell_readers[id] = make_shared<LoadCellReader>(hx711, log_sensors + to_string(id) + ".txt");
        }
    });

    if (enable_sensors.size() == 4) {
        position_processor = make_shared<PositionProcessor>(load_cell_readers[0]->raw_output_,
                                                            load_cell_readers[1]->raw_output_,
                                                            load_cell_readers[2]->raw_output_,
                                                            load_cell_readers[3]->raw_output_);
        load_cell_processor = make_shared<LoadCellsProcessor>(load_cell_readers[0]->raw_output_,
                                                              load_cell_readers[1]->raw_output_,
                                                              load_cell_readers[2]->raw_output_,
                                                              load_cell_readers[3]->raw_output_);
        position_processor->startThread();
        load_cell_processor->startThread();
    }
    for (auto &load_cell_reader : load_cell_readers) {
        load_cell_reader.second->startProducing();
        load_cell_reader.second->startConsuming();
    }
}

void printValues(const vector<int> &print_sensors, bool print_weight, bool print_xy) {
    while (executing) {
        for (auto sensor : print_sensors) {
            load_cell_readers[sensor]->raw_output_.consume_all([sensor](float f) {
                printf("Sensor %d: %fg\n", sensor, f);
            });
        }

        if (print_xy) {
            position_processor->output_.consume_all([](auto p) {
                printf("XY: %.2f %.2f\n", p.first, p.second);
            });
        }
    }
}

int main(int argc, char **argv) {
    CLI::App app{"Choptop - an interactive chopping board"};
    app.require_subcommand(1);

    vector<int> enable_sensors;
    string log_sensors;
    app.add_option("--enable-sensors", enable_sensors, "Sensors to be enabled");
    app.add_option("--log-sensors", log_sensors, "File prefix to log sensor data to");

    auto print = app.add_subcommand("print", "Print a stream of values from Choptop");

    vector<int> print_sensors;
    bool print_total_weight, print_xy;
    print->add_option("--sensors", print_sensors, "Sensors to be printed");
    print->add_flag("--total-weight", print_total_weight, "Print total weight");
    print->add_flag("--xy", print_xy, "Print x position");

    CLI11_PARSE(app, argc, argv);

    signal(SIGINT, gracefulShutdown);
    signal(SIGTERM, gracefulShutdown);
    //signal(SIGKILL, gracefulShutdown);
    signal(SIGABRT, gracefulShutdown);

    startSensors(enable_sensors, log_sensors);

    if (app.got_subcommand("print")) {
        printf("Printing values\n");
        printValues(print_sensors, print_total_weight, print_xy);
    }
}