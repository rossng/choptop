#include "DataProcessor.h"

#include <thread>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <ratio>
#include <vector>
#include <numeric>
#include <cmath>

using namespace std;

DataProcessor::DataProcessor(boost::lockfree::spsc_queue<SensorData> &sensor_data) :
        sensor_data_(sensor_data), weight_(1024), press_events_(64), position_(1024), sensor_data_despiked_(1024) {

}

DataProcessor::~DataProcessor() {
    running_ = false;
    delete thread_;
}


void DataProcessor::startThread() {
    if (running_) return;
    running_ = true;
    thread_ = new thread(&DataProcessor::consume, this);
}

void DataProcessor::stopThread() {
    if (!running_) return;
    running_ = false;
    thread_->join();
}


void DataProcessor::consume() {
    static int step = 0;

    // Persistent state for Hampel filter
    std::vector<float> top_left_window1(7);
    std::vector<float> top_left_window2(7);
    std::vector<float> top_right_window1(7);
    std::vector<float> top_right_window2(7);
    std::vector<float> bottom_left_window1(7);
    std::vector<float> bottom_left_window2(7);
    std::vector<float> bottom_right_window1(7);
    std::vector<float> bottom_right_window2(7);
    int top_left_idx, top_right_idx, bottom_right_idx, bottom_left_idx;

    while (running_) {
        bool updated = false;

        sensor_data_.consume_all([&](SensorData sd) {
            // Hampel filter to remove spikes
            sd.top_left = hampel(sd.top_left, top_left_window1, top_left_window2, top_left_idx);
            sd.top_right = hampel(sd.top_left, top_right_window1, top_right_window2, top_right_idx);
            sd.bottom_right = hampel(sd.bottom_right, bottom_right_window1, bottom_right_window2, bottom_right_idx);
            sd.bottom_left = hampel(sd.bottom_left, bottom_left_window1, bottom_left_window2, bottom_left_idx);

            sensor_data_despiked_.push({sd.top_left, sd.top_right, sd.bottom_right, sd.bottom_left});

            // Exponential average to smooth
            top_left_smooth_ = expAvg(sd.top_left, top_left_smooth_, w);
            top_right_smooth_ = expAvg(sd.top_right, top_right_smooth_, w);
            bottom_right_smooth_ = expAvg(sd.bottom_right, bottom_right_smooth_, w);
            bottom_left_smooth_ = expAvg(sd.bottom_left, bottom_left_smooth_, w);
            updated = true;
        });

        if (updated) {
            float weight = top_left_smooth_ + top_right_smooth_ + bottom_left_smooth_ + bottom_right_smooth_;
            weight_slow_ = expAvg(weight, weight_slow_, lag_weight);
            float y_top = min(max((top_left_smooth_ + top_right_smooth_) / weight, 0.f), 1.0f);
            float x_right = min(max((top_right_smooth_ + bottom_right_smooth_) / weight, 0.f), 1.0f);
            float y_bottom = 1.0f - min(max((bottom_left_smooth_ + bottom_right_smooth_) / weight, 0.f), 1.0f);
            float x_left = 1.0f - min(max((top_left_smooth_ + bottom_left_smooth_) / weight, 0.f), 1.0f);
            auto y = (y_top + y_bottom) / 2.0f;
            auto x = (x_left + x_right) / 2.0f;
            edgeDetect(weight, edge_threshold_);

            if (is_pressed_) {
                press_start_time_ = std::chrono::high_resolution_clock::now();
                if (x < 0.1 && y > 0.3 && y < 0.7) press_events_.push({x, y, PressLocation::LEFT});
                else if (x > 0.3 && x < 0.7 && y > 0.9) press_events_.push({x, y, PressLocation::TOP});
                else if (x > 0.9 && y < 0.7 && y > 0.3) press_events_.push({x, y, PressLocation::RIGHT});
                else if (x < 0.7 && x > 0.3 && y < 0.1) press_events_.push({x, y, PressLocation::BOTTOM});
                is_pressed_ = false;
            }

            weight_.push(weight);

            position_.push(make_pair(x, y));
        }
        this_thread::sleep_for(50ms);
    }
}

float DataProcessor::expAvg(float sample, float avg, float w) {
    return w * sample + (1.f - w) * avg;
}

void DataProcessor::edgeDetect(float sample, float threshold) {
    float diff = sample - weight_slow_;
    auto now = chrono::high_resolution_clock::now();
    auto time_since_last_press = chrono::duration_cast<chrono::duration<double>>(now - press_start_time_);
    if (diff >= threshold && previous_diff_ < threshold && time_since_last_press.count() > 0.01f) {
        is_pressed_ = true;
        release_threshold = weight_slow_;
    } else if (sample < weight_slow_) {
        is_pressed_ = false;
    }
    previous_diff_ = diff;
}


float DataProcessor::hampel(float reading, std::vector<float> &window1, std::vector<float> &window2, int &current_idx) {
    window1[current_idx] = reading;
    current_idx = (current_idx + 1) % 7;

    // Compute median
    window2 = window1;
    std::sort(window2.begin(), window2.end());
    float median = window2[3];
    float mean = std::accumulate(window1.begin(), window1.end(), 0.f) / 7;

    // Computer stddev
    std::transform(window1.begin(), window1.end(), window2.begin(), [mean](float x) { return x - mean; });
    float sq_sum = std::inner_product(window2.begin(), window2.end(), window2.begin(), 0.f);
    float stdev = std::sqrt(sq_sum / 7);

    return std::abs(reading - median) > (2 * stdev) ? median : reading;
}