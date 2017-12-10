#include "PositionProcessor.h"

#include <thread>

#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace std;

PositionProcessor::PositionProcessor(boost::lockfree::spsc_queue<float> &top_left,
                                     boost::lockfree::spsc_queue<float> &top_right,
                                     boost::lockfree::spsc_queue<float> &bottom_right,
                                     boost::lockfree::spsc_queue<float> &bottom_left,
                                     string log_file) :
        top_left_(top_left), top_right_(top_right), bottom_right_(bottom_right), bottom_left_(bottom_left),
        output_(1024), log_file_(log_file), output_verbose_(1024) {

}

void PositionProcessor::startThread() {
    if (running_) return;
    running_ = true;
    thread_ = new thread(&PositionProcessor::consume, this);
}

void PositionProcessor::stopThread() {
    if (!running_) return;
    running_ = false;
    thread_->join();
}

float PositionProcessor::expAvg(float sample, float avg, float w) {
    return w * sample + (1.f - w) * avg;
}

void PositionProcessor::consume() {
    static int step = 0;
    while (running_) {
        bool updated = false;

        top_left_.consume_all([&](float f) {
            top_left_avg_ = expAvg(f, top_left_avg_, w);
            updated = true;
        });
        top_right_.consume_all([&](float f) {
            top_right_avg_ = expAvg(f, top_right_avg_, w);
            updated = true;
        });
        bottom_right_.consume_all([&](float f) {
            bottom_right_avg_ = expAvg(f, bottom_right_avg_, w);
            updated = true;
        });
        bottom_left_.consume_all([&](float f) {
            bottom_left_avg_ = expAvg(f, bottom_left_avg_, w);
            updated = true;
        });

        if (updated) {
            float total = top_left_avg_ + top_right_avg_ + bottom_left_avg_ + bottom_right_avg_;
            float y = min(max((top_left_avg_ + top_right_avg_) / total, 0.f), 1.0f);
            float x = min(max((top_right_avg_ + bottom_right_avg_) / total, 0.f), 1.0f);

            if (step++ % 1 == 0) {
                output_.push(make_pair(x, y));
                output_verbose_.push({x, y});
                log_file_ << std::fixed << std::setprecision(5) << x << "," << y << endl;
            }
        }
        this_thread::sleep_for(50ms);
    }
}

PositionProcessor::~PositionProcessor() {
    running_ = false;
    delete thread_;
}

void PositionProcessor::printStatus() {
    cout << "PositionProcessor: " << endl;
    cout << "output_: " << output_.read_available() << "/" << output_.write_available() << endl;
    cout << "top_left_: " << top_left_.read_available() << "/" << top_left_.write_available() << endl;
    cout << "top_right_: " << top_right_.read_available() << "/" << top_right_.write_available() << endl;
    cout << "bottom_right_: " << bottom_right_.read_available() << "/" << bottom_right_.write_available() << endl;
    cout << "bottom_left_: " << bottom_left_.read_available() << "/" << bottom_left_.write_available() << endl;
}
