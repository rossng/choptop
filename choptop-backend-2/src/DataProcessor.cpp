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
        sensor_data_(sensor_data), weight_(1024), press_events_(64), position_(1024), sensor_data_despiked_(1024),
        running_(false) {
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
            sd.top_right = hampel(sd.top_right, top_right_window1, top_right_window2, top_right_idx);
            sd.bottom_right = hampel(sd.bottom_right, bottom_right_window1, bottom_right_window2, bottom_right_idx);
            sd.bottom_left = hampel(sd.bottom_left, bottom_left_window1, bottom_left_window2, bottom_left_idx);

            sensor_data_despiked_.push({sd.top_left, sd.top_right, sd.bottom_right, sd.bottom_left});

            // Exponential average to smooth
            top_left_smooth_ = sd.top_left;//= expAvg(sd.top_left, top_left_smooth_, w);
            top_right_smooth_ = sd.top_right;//= expAvg(sd.top_right, top_right_smooth_, w);
            bottom_right_smooth_ = sd.bottom_right;//= expAvg(sd.bottom_right, bottom_right_smooth_, w);
            bottom_left_smooth_ = sd.bottom_left;//= expAvg(sd.bottom_left, bottom_left_smooth_, w);
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

            detectPress(weight, x, y);

            weight_.push(weight);
            position_.push(make_pair(x, y));
        }
    }
}

float DataProcessor::expAvg(float sample, float avg, float w) {
    return w * sample + (1.f - w) * avg;
}

void DataProcessor::detectPress(float sample, float x, float y) {
    float diff = sample - weight_slow_;

    bool send_press_start = false;
    bool send_press_success = false;
    bool send_press_cancel = false;
    auto time_since_press = chrono::duration_cast<chrono::milliseconds>(
            chrono::steady_clock::now() - press_started_).count();

    cout << setprecision(3) << time_since_press << ": " << diff << " (" << x << "," << y << ")" << endl;

    if (press_stage_ == PressStage::PRESS_CANCELLED && diff >= press_threshold_ && previous_diff_ < press_threshold_) {
        cout << "Tentative up edge" << endl;
        press_started_ = chrono::steady_clock::now();
        press_tentative_ = true;
    } else if (press_stage_ == PressStage::PRESS_CANCELLED && diff >= press_threshold_ &&
               time_since_press > press_debounce_time_ && press_tentative_) {
        cout << "Up edge started: " << time_since_press << endl;
        send_press_start = true;
        press_stage_ = PressStage::PRESS_STARTED;
    } else if (press_stage_ == PressStage::PRESS_CANCELLED && diff < (press_threshold_/2.0) &&
               time_since_press > press_debounce_time_ && press_tentative_) {
        cout << "Cancelled tentative up edge: " << time_since_press << endl;
        press_tentative_ = false;
    } else if (press_stage_ == PressStage::PRESS_STARTED && diff <= release_threshold_) {
        cout << "Down edge started" << endl;
        if (time_since_press < press_minimum_time_) {
            cout << "Down edge too soon, cancel: " << time_since_press << endl;
            press_stage_ = PressStage::PRESS_CANCELLED;
            send_press_cancel = true;
        } else {
            press_stage_ = PressStage::PRESS_SUCCESS;
            send_press_success = true;
        }
    } else if (press_stage_ == PressStage::PRESS_STARTED && time_since_press > press_timeout_millis_) {
        cout << "Press cancelled" << endl;
        press_stage_ = PressStage::PRESS_CANCELLED;
        send_press_cancel = true;
    }

    if (send_press_start) {
        cout << "Press start" << endl;
        if (x < 0.2 && y > 0.3 && y < 0.7)
            press_location_ = PressLocation::LEFT;
        else if (x > 0.3 && x < 0.7 && y > 0.8)
            press_location_ = PressLocation::TOP;
        else if (x > 0.8 && y < 0.7 && y > 0.3)
            press_location_ = PressLocation::RIGHT;
        else if (x < 0.7 && x > 0.3 && y < 0.2)
            press_location_ = PressLocation::BOTTOM;
        press_events_.push({x, y, press_location_, PressStage::PRESS_STARTED});
    }

    if (send_press_success) {
        cout << "Press success" << endl;
        press_events_.push({x, y, press_location_, PressStage::PRESS_SUCCESS});
        press_stage_ = PressStage::PRESS_CANCELLED;
    }

    if (send_press_cancel) {
        cout << "Press cancelled" << endl;
        press_events_.push({x, y, press_location_, PressStage::PRESS_CANCELLED});
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