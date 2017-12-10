#include "LoadCellsProcessor.h"

#include <thread>
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace std;

LoadCellsProcessor::LoadCellsProcessor(boost::lockfree::spsc_queue<float> &top_left,
                                       boost::lockfree::spsc_queue<float> &top_right,
                                       boost::lockfree::spsc_queue<float> &bottom_right,
                                       boost::lockfree::spsc_queue<float> &bottom_left,
                                       string log_file,
                                       string log_diff_file) :
        top_left_(top_left), top_right_(top_right), bottom_right_(bottom_right), bottom_left_(bottom_left),
        output_(1024), log_file_(log_file), log_diff_file_(log_diff_file), press_events_(64), output_slow_(1024) {

}

void LoadCellsProcessor::startThread() {
    if (running_) return;
    running_ = true;
    thread_ = new thread(&LoadCellsProcessor::consume, this);
}

void LoadCellsProcessor::stopThread() {
    if (!running_) return;
    running_ = false;
    thread_->join();
}

float LoadCellsProcessor::expAvg(float sample, float avg, float w) {
    return w * sample + (1.f - w) * avg;
}

void LoadCellsProcessor::edgeDetect(float sample, float threshold) {
    float diff = sample - total_slow_;
    log_diff_file_ << std::fixed << std::setprecision(5) << diff << endl;

    if (diff >= threshold && previous_diff < threshold) {
        isPressed = true;
        cout << "pressed" << endl;

    } else if (isPressed &&
               ((diff <= (-threshold) && previous_diff > (-threshold)) || stop - start >= std::chrono::seconds(2))) {

        isPressed = false;
        timeStart = false;
        cout << "released" << endl;
    }
    previous_diff = diff;
}

void LoadCellsProcessor::consume() {

    static int step = 0;
    while (running_) {
        bool updated = false;
        stop = std::chrono::system_clock::now();

        top_left_.consume_all([&](float f) {
            top_left_total_ = expAvg(f, top_left_total_, w);
            updated = true;
        });
        top_right_.consume_all([&](float f) {
            top_right_total_ = expAvg(f, top_right_total_, w);
            updated = true;
        });
        bottom_left_.consume_all([&](float f) {
            bottom_left_total_ = expAvg(f, bottom_left_total_, w);
            updated = true;
        });
        bottom_right_.consume_all([&](float f) {
            bottom_right_total_ = expAvg(f, bottom_right_total_, w);
            updated = true;
        });

        if (updated) {
            float total = top_left_total_ + top_right_total_ + bottom_right_total_ + bottom_left_total_;
            total_slow_ = expAvg(total, total_slow_, lag_weight);
            float y = min(max((top_left_pos_ + top_right_pos_) / total, 0.f), 1.0f);
            float x = min(max((top_right_pos_ + bottom_right_pos_) / total, 0.f), 1.0f);

            edgeDetect(total, edge_threshold);

            if (isPressed) {
                if (!timeStart) {
                    start = std::chrono::system_clock::now();
                    timeStart = true;
                    cout << "XY: " << x << " : " << y << endl;
                    if(x < 0.5 && y > 0.5) press_events_.push(PressEvent::LEFT);
                    else if ( x > 0.5 && y > 0.5) press_events_.push(PressEvent::TOP);
                    else if ( x > 0.5 && y < 0.5) press_events_.push(PressEvent::RIGHT);
                    else if ( x < 0.5 && y < 0.5) press_events_.push(PressEvent::BOTTOM);
                }
            }

            if (step++ % 1 == 0) {
                output_.push(total);
                output_slow_.push(total_slow_);
                log_file_ << std::fixed << std::setprecision(5) << total << endl;
            }
        }

        this_thread::sleep_for(50ms);
    }
}

LoadCellsProcessor::~LoadCellsProcessor() {
    running_ = false;
    delete thread_;
}

void LoadCellsProcessor::printStatus() {
    cout << "LoadCellsProcessor: " << endl;
    cout << "output_: " << output_.read_available() << "/" << output_.write_available() << endl;
    cout << "top_left_: " << top_left_.read_available() << "/" << top_left_.write_available() << endl;
    cout << "top_right_: " << top_right_.read_available() << "/" << top_right_.write_available() << endl;
    cout << "bottom_right_: " << bottom_right_.read_available() << "/" << bottom_right_.write_available() << endl;
    cout << "bottom_left_: " << bottom_left_.read_available() << "/" << bottom_left_.write_available() << endl;
}