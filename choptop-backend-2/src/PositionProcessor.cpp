#include "PositionProcessor.h"

#include <thread>

#include <algorithm>
#include <iostream>

using namespace std;

PositionProcessor::PositionProcessor(boost::lockfree::spsc_queue<float> &top_left,
                                     boost::lockfree::spsc_queue<float> &top_right,
                                     boost::lockfree::spsc_queue<float> &bottom_right,
                                     boost::lockfree::spsc_queue<float> &bottom_left) : top_left_(top_left),
                                                                                        top_right_(top_right),
                                                                                        bottom_right_(bottom_right),
                                                                                        bottom_left_(bottom_left) {

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
    while (running_) {
        top_left_.consume_one([&](float f) {
            top_left_avg_ = expAvg(f, top_left_avg_, w);
        });
        top_right_.consume_one([&](float f) {
            top_right_avg_ = expAvg(f, top_right_avg_, w);
        });
        bottom_right_.consume_one([&](float f) {
            bottom_right_avg_ = expAvg(f, bottom_right_avg_, w);
        });
        bottom_left_.consume_one([&](float f) {
            bottom_left_avg_ = expAvg(f, bottom_left_avg_, w);
        });

        float total = top_left_avg_ + top_right_avg_ + bottom_left_avg_ + bottom_right_avg_;
        float x = min(max((top_left_avg_ + top_right_avg_) / total, 0.f), 1.0f);
        float y = min(max((top_right_avg_ + bottom_right_avg_) / total, 0.f), 1.0f);

        cout << "xy: " << x << "," << y << endl;
    }
}

PositionProcessor::~PositionProcessor() {
    running_ = false;
    delete thread_;
}

