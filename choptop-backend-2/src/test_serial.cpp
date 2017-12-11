#include <iostream>
#include <thread>
#include <SensorReader.h>
#include "rs232.h"

using namespace std;

int main() {
    int num_serial_devices = comEnumerate();
    cout << num_serial_devices << " serial devices" << endl;
    for (int i = 0; i < num_serial_devices; i++) {
        cout << "Device " << i << ": " << comGetPortName(i) << endl;
    }

    SensorReader sr("/dev/ttyACM1");
    sr.tare();
    sr.startReading();

    while (true) {
        sr.sensor_data_.consume_all([](auto sd) {
            printf("%f %f %f %f\n", sd.top_left, sd.top_right, sd.bottom_right, sd.bottom_left);
        });
    }

    sr.stopReading();
}