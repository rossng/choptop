#include "LoadCellsProcessor.h"

/*
while (executing) {
float total = 0;
for (auto &sensor : sensor_data) {
while (!sensor.second->read_available()) {}
float weight = sensor.second->pop();
total += weight;

printf("Sensor %d: %.2f\n", sensor.first, weight);
}

float x = clamp((weights[0] + weights[1]) / total, 0.0, 1.0f);
float y = clamp((weights[1] + weights[2]) / total, 0.0, 1.0f);

if (total_weight) {
printf("%.2fg\n", total);
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
}*/
