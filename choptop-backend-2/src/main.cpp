#include <boost/thread/thread.hpp>
#include <csignal>
#include <thread>
#include <iostream>
#include <atomic>
#include "CLI11.hpp"
#include "DataProcessor.h"
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

shared_ptr<DataProcessor> data_processor;
shared_ptr<SensorReader> sensor_reader;

shared_ptr<ChoptopServer> choptop_server;

void gracefulShutdown(int s) {
    executing = false;

    cout << "Stopping SensorReader" << endl;
    if (sensor_reader != nullptr) {
        sensor_reader->stopReading();
    }
    cout << "Stopping DataProcessor" << endl;
    if (data_processor != nullptr) {
        data_processor->stopThread();
    }

    cout << "Stopping ChoptopServer" << endl;
    if (choptop_server != nullptr) {
        choptop_server->stopServer();
    }

    cout << "Goodbye!" << endl;
}

void startSensors(string device) {
    this_thread::sleep_for(500ms);

    sensor_reader = make_shared<SensorReader>(device);
    sensor_reader->tare();
    sensor_reader->startReading();

    data_processor = make_shared<DataProcessor>(sensor_reader->sensor_data_);
    data_processor->startThread();
}

void printValues(const vector<int> &print_sensors, bool print_weight, bool print_xy, bool print_presses) {
    static int step = 0;

    while (executing) {
        if (print_weight) {
            data_processor->weight_.consume_all([](float weight) {
                printf("Weight: %.2fg\n", weight);
            });
        }

        if (print_presses) {
            data_processor->press_events_.consume_all([](PressEvent e) {
                switch (e.location) {
                    case PressLocation::TOP:
                        printf("Pressed Top\n");
                        break;
                    case PressLocation::BOTTOM:
                        printf("Pressed Bottom\n");
                        break;
                    case PressLocation::LEFT:
                        printf("Pressed Left\n");
                        break;
                    case PressLocation::RIGHT:
                        printf("Pressed Right\n");
                        break;
                }
            });
        }

        if (print_xy) {
            data_processor->position_.consume_all([](auto p) {
                printf("XY: %.2f %.2f\n", p.first, p.second);
            });
        }

        data_processor->sensor_data_despiked_.consume_all([&](SensorData sd) {
            if (std::find(print_sensors.begin(), print_sensors.end(), 0) != print_sensors.end()) {
                printf("Sensor 0: %.2f\n", sd.top_left);
            }
            if (std::find(print_sensors.begin(), print_sensors.end(), 1) != print_sensors.end()) {
                printf("Sensor 1: %.2f\n", sd.top_right);
            }
            if (std::find(print_sensors.begin(), print_sensors.end(), 2) != print_sensors.end()) {
                printf("Sensor 2: %.2f\n", sd.bottom_right);
            }
            if (std::find(print_sensors.begin(), print_sensors.end(), 3) != print_sensors.end()) {
                printf("Sensor 3: %.2f\n", sd.bottom_left);
            }
        });

        this_thread::sleep_for(50ms);
    }
}


void startServer(uint16_t port) {
    choptop_server = make_shared<ChoptopServer>(port);
    choptop_server->startServer();
    auto lastSend = std::chrono::system_clock::now();

    while (executing) {
        data_processor->press_events_.consume_all([&](auto p) {
            switch (p.location) {
                case PressLocation::TOP:
                    choptop_server->sendMessage("{\"event\": \"upPressed\"}");
                    break;
                case PressLocation::BOTTOM:
                    choptop_server->sendMessage("{\"event\": \"downPressed\"}");
                    break;
                case PressLocation::RIGHT:
                    choptop_server->sendMessage("{\"event\": \"rightPressed\"}");
                    break;
                case PressLocation::LEFT:
                    choptop_server->sendMessage("{\"event\": \"leftPressed\"}");
                    break;
            }
        });

        data_processor->weight_.consume_all([&](auto p) {
            //if(std::chrono::system_clock::now() - lastSend > std::chrono::milliseconds(50)){
            lastSend = std::chrono::system_clock::now();
            std::stringstream stream;
            stream << "{\"event\": \"weightReading\", \"value\":" << std::fixed << std::setprecision(0) << p << "}";
            choptop_server->sendMessage(stream.str());
            //}
        });

        data_processor->position_.consume_all([&](auto p) {
            lastSend = std::chrono::system_clock::now();
            std::stringstream stream;
            stream << "{\"event\": \"position\""
                   << ", \"pos\": [" << std::fixed << std::setprecision(3) << p.first
                   << ", " << std::fixed << std::setprecision(3) << p.second
                   << "], \"scale\": 1 }";
            choptop_server->sendMessage(stream.str());
        });
    }
}

int main(int argc, char **argv) {
    CLI::App app{"Choptop - an interactive chopping board"};
    app.require_subcommand(1);

    string device = "ttyACM0";
    app.add_option("--device", device, "Serial device name");

    auto print = app.add_subcommand("print", "Print a stream of values from Choptop");

    vector<int> print_sensors;
    bool print_weight, print_xy, print_presses;
    print->add_option("--sensors", print_sensors, "Sensors to be printed");
    print->add_flag("--weight", print_weight, "Print total weight");
    print->add_flag("--xy", print_xy, "Print x,y position");
    print->add_flag("--presses", print_presses, "Print edge presses");

    auto serve = app.add_subcommand("serve", "Serve a stream of information over a WebSocket");

    uint16_t port = 8765;
    serve->add_option("--port", port, "Port to serve the WebSocket on");

    CLI11_PARSE(app, argc, argv);

    signal(SIGINT, gracefulShutdown);
    signal(SIGTERM, gracefulShutdown);
    //signal(SIGILL, gracefulShutdown);
    signal(SIGABRT, gracefulShutdown);

    cout << "Start sensors" << endl;

    startSensors(device);
    if (app.got_subcommand("serve")) {
        cout << "Serve over WebSocket" << endl;

        startServer(port);
    } else if (app.got_subcommand("print")) {
        cout << "Print values" << endl;
        printValues(print_sensors, print_weight, print_xy, print_presses);
    }
}
