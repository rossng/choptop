#include <boost/thread/thread.hpp>
#include <csignal>
#include <thread>
#include <iostream>
#include <atomic>
#include "CLI11.hpp"
#include "hx711.h"
#include "LoadCellReader.h"
#include "WeightProcessor.h"
#include "PositionProcessor.h"
#include <map>
#include <memory>
#include <boost/lockfree/spsc_queue.hpp>
#include <mutex>
#include "ChoptopServer.h"
#include <boost/format.hpp>

using namespace std;

#ifdef _WIN32
#define NULLFILE "nul"
#else
#define NULLFILE "/dev/null"
#endif

vector<thread> threads;
atomic<bool> executing(true);
mutex wiring_pi_mutex;

map<int, shared_ptr<LoadCellReader>> load_cell_readers;
shared_ptr<PositionProcessor> position_processor;
shared_ptr<WeightProcessor> load_cells_processor;

shared_ptr<ChoptopServer> choptop_server;

struct HX711Settings {
    uint8_t clk;
    uint8_t data;
    float scale;
};

map<int, HX711Settings> sensor_settings = {
        {0, {6,  5,  466.1f}},
        {1, {8,  7,  476.2f}},
        {2, {10, 9,  476.8f}},
        {3, {21, 20, 467.09f}}
};

void gracefulShutdown(int s) {
    executing = false;

    for (auto &load_cell_reader : load_cell_readers) {
        load_cell_reader.second->stopProducing();
        load_cell_reader.second->stopConsuming();
    }
    position_processor->stopThread();
    load_cells_processor->stopThread();

    if (choptop_server != nullptr) {
        choptop_server->stopServer();
    }

    exit(1);
}

shared_ptr<HX711> makeHX711(uint8_t clk, uint8_t data, float scale, mutex &wiring_pi_mutex) {
    auto sensor = make_shared<HX711>(clk, data, 0, wiring_pi_mutex);
    sensor->tare(30);
    sensor->setScale(scale);
    return sensor;
}

void startSensors(vector<int> enable_sensors, string log_sensors, string log_xy, string log_weight, string log_diff) {
    this_thread::sleep_for(500ms);

    std::for_each(enable_sensors.begin(), enable_sensors.end(), [&](int id) {
        cout << id;
        auto hx711 = makeHX711(sensor_settings[id].clk, sensor_settings[id].data, sensor_settings[id].scale,
                               wiring_pi_mutex);
        if (log_sensors.empty()) {
            load_cell_readers[id] = make_shared<LoadCellReader>(hx711, NULLFILE);
        } else {
            load_cell_readers[id] = make_shared<LoadCellReader>(hx711, log_sensors + to_string(id) + ".txt");
        }
    });

    cout << endl;

    if (enable_sensors.size() == 4) {
        position_processor = make_shared<PositionProcessor>(
                load_cell_readers[0]->raw_output_,
                load_cell_readers[1]->raw_output_,
                load_cell_readers[2]->raw_output_,
                load_cell_readers[3]->raw_output_,
                log_xy.empty() ? NULLFILE : log_xy + ".txt");

        load_cells_processor = make_shared<WeightProcessor>(
                load_cell_readers[0]->raw_output_,
                load_cell_readers[1]->raw_output_,
                load_cell_readers[2]->raw_output_,
                load_cell_readers[3]->raw_output_,
                log_weight.empty() ? NULLFILE : log_weight + ".txt",
                log_diff.empty() ? NULLFILE : log_diff + ".txt");

        position_processor->startThread();
        load_cells_processor->startThread();
    }
    for (auto &load_cell_reader : load_cell_readers) {
        load_cell_reader.second->startProducing();
        load_cell_reader.second->startConsuming();
    }
}

void printValues(const vector<int> &print_sensors, bool debug, bool print_weight, bool print_xy) {
    static int step = 0;

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

        if (print_weight) {
            load_cells_processor->output_.consume_all([](auto w) {
                printf("Weight: %.2fg\n", w);
            });
        }

        this_thread::sleep_for(50ms);

        if (debug) {
            if (step++ % 100 == 0) {
                load_cell_readers[0]->printStatus(0);
                load_cell_readers[1]->printStatus(1);
                load_cell_readers[2]->printStatus(2);
                load_cell_readers[3]->printStatus(3);
                position_processor->printStatus();
                load_cells_processor->printStatus();
            }
        }
    }
}


void startServer(uint16_t port) {
    choptop_server = make_shared<ChoptopServer>(port);
    choptop_server->startServer();
    auto lastSend = std::chrono::system_clock::now();

    while (executing) {
        position_processor->press_events_.consume_all([&](auto p) {
            switch (p) {
                case PressEvent::TOP:
                    choptop_server->sendMessage("{\"event\": \"upPressed\"}");
                    break;
                case PressEvent::BOTTOM:
                    choptop_server->sendMessage("{\"event\": \"downPressed\"}");
                    break;
                case PressEvent::RIGHT:
                    choptop_server->sendMessage("{\"event\": \"rightPressed\"}");
                    break;
                case PressEvent::LEFT:
                    choptop_server->sendMessage("{\"event\": \"leftPressed\"}");
                    break;
            }
        });

        load_cells_processor->output_.consume_all([&](auto p) {
            //if(std::chrono::system_clock::now() - lastSend > std::chrono::milliseconds(50)){
            lastSend = std::chrono::system_clock::now();
            std::stringstream stream;
            stream << "{\"event\": \"weightReading\", \"value\":" << std::fixed << std::setprecision(0) << p << "}";
            choptop_server->sendMessage(stream.str());
            //}
        });

        load_cells_processor->output_verbose_.consume_all([&](auto p) {
            //if(std::chrono::system_clock::now() - lastSend > std::chrono::milliseconds(50)){
            lastSend = std::chrono::system_clock::now();
            std::stringstream stream;
            stream << "{\"event\": \"WeightProcessor\""
                   << ", \"weight\":" << std::fixed << std::setprecision(0) << p.weight
                   << "}";
            choptop_server->sendMessage(stream.str());
            //}
        });
    }
}

int main(int argc, char **argv) {
    CLI::App app{"Choptop - an interactive chopping board"};
    app.require_subcommand(1);

    vector<int> enable_sensors;
    bool debug;
    string log_sensors, log_xy, log_weight, log_diff;
    app.add_option("--enable-sensors", enable_sensors, "Sensors to be enabled");
    app.add_option("--log-sensors", log_sensors, "File prefix to log sensor data to");
    app.add_option("--log-xy", log_xy, "File prefix to log xy data to");
    app.add_option("--log-weight", log_weight, "File prefix to log weight data to");
    app.add_option("--log-diff", log_diff, "File prefix to log weight change data to");
    app.add_flag("--debug", debug, "File prefix to log weight data to");

    auto print = app.add_subcommand("print", "Print a stream of values from Choptop");

    vector<int> print_sensors;
    bool print_weight, print_xy;
    print->add_option("--sensors", print_sensors, "Sensors to be printed");
    print->add_flag("--weight", print_weight, "Print total weight");
    print->add_flag("--xy", print_xy, "Print x position");

    auto serve = app.add_subcommand("serve", "Serve a stream of information over a WebSocket");

    uint16_t port = 8765;
    serve->add_option("--port", port, "Port to serve the WebSocket on");

    CLI11_PARSE(app, argc, argv);

    signal(SIGINT, gracefulShutdown);
    signal(SIGTERM, gracefulShutdown);
    //signal(SIGKILL, gracefulShutdown);
    signal(SIGABRT, gracefulShutdown);

    cout << "Start sensors" << endl;

    startSensors(enable_sensors, log_sensors, log_xy, log_weight, log_diff);

    if (app.got_subcommand("serve")) {
        cout << "Serve over WebSocket" << endl;
        startServer(port);
    } else if (app.got_subcommand("print")) {
        cout << "Print values" << endl;
        printValues(print_sensors, debug, print_weight, print_xy);
    }
}
