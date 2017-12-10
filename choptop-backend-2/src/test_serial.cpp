#include <iostream>
#include <thread>
#include "rs232.h"

using namespace std;

int main() {
    int num_serial_devices = comEnumerate();
    cout << num_serial_devices << " serial devices" << endl;
    for (int i = 0; i < num_serial_devices; i++) {
        cout << "Device " << i << ": " << comGetPortName(i) << endl;
    }

    char serial_char;

    if (comOpen(0, 38400)) {
        while (true) {
            if (comRead(0, &serial_char, 1) > 0) {
                printf("%c", serial_char);
            }
        }
    } else {
        cout << "Failed to open device 0" << endl;
    }

    comCloseAll();
}