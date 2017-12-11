#include <iostream>
#include <utility>
#include <sstream>
#include "SensorReader.h"

#include "rs232.h"

using namespace std;

SensorReader::SensorReader(string device) :
        device_(std::move(device)), sensor_data_(1024), running_(false), thread_(nullptr) {
    int num_serial_devices = comEnumerate();
    port_ = -1;
    for (int i = 0; i < num_serial_devices; i++) {
        if (device_ == comGetInternalName(i)) {
            port_ = i;
        }
    }
    for (int i = 0; i < num_serial_devices; i++) {
        if (device_ == comGetPortName(i)) {
            port_ = i;
        }
    }
    if (port_ < 0) {
        throw "Did not find device";
    } else {
        comOpen(port_, 38400);
        comClose(port_);
        comOpen(port_, 38400);
    }
}

SensorReader::SensorReader(int port) :
        port_(port), sensor_data_(1024), running_(false), thread_(nullptr) {
    if (port_ >= comEnumerate()) {
        throw "Did not find device";
    }
    device_ = comGetInternalName(port_);
    comOpen(port_, 38400);
    comClose(port_);
    comOpen(port_, 38400);
}

void SensorReader::printSerialDevices() {
    int num_serial_devices = comEnumerate();
    cout << num_serial_devices << " serial devices" << endl;
    for (int i = 0; i < num_serial_devices; i++) {
        cout << "Device " << i << ": " << comGetPortName(i) << endl;
    }
}

SensorReader::~SensorReader() {
    comClose(port_);
}

std::string SensorReader::readLine() {
    std::string str;
    str.reserve(40);
    std::stringstream sstr(str);
    char c = '\0';
    while (c != '\n') {
        if (comRead(port_, &c, 1) > 0 && c != '\n' && c != '\r') {
            sstr << c;
        }
    }

    return sstr.str();
}

void SensorReader::startReading() {
    if (running_) return;
    running_ = true;
    thread_ = new thread(&SensorReader::read, this);
}

void SensorReader::stopReading() {
    if (!running_) return;
    running_ = false;
    thread_->join();
}

void SensorReader::read() {
    while (running_) {
        std::string line = readLine();
        while (line[0] != '0') {
            line = readLine();
            try {
                stof(line.substr(2));
            } catch (std::invalid_argument &e) {
                line = "";
            }
        }
        float top_left = stof(line.substr(2));

        while (line[0] != '1') {
            line = readLine();
            try {
                stof(line.substr(2));
            } catch (std::invalid_argument &e) {
                line = "";
            }
        }
        float top_right = stof(line.substr(2));

        while (line[0] != '2') {
            line = readLine();
            try {
                stof(line.substr(2));
            } catch (std::invalid_argument &e) {
                line = "";
            }
        }
        float bottom_right = stof(line.substr(2));

        while (line[0] != '3') {
            line = readLine();
            try {
                stof(line.substr(2));
            } catch (std::invalid_argument &e) {
                line = "";
            }
        }
        float bottom_left = stof(line.substr(2));

        sensor_data_.push({top_left, top_right, bottom_right, bottom_left});
    }
}

void SensorReader::tare() {
    std::string line;
    this_thread::sleep_for(2s);
    comWrite(port_, "t", 1);
}
