#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>

class PositionProcessor {
public:
    explicit PositionProcessor(boost::lockfree::spsc_queue<float> &top_left,
                               boost::lockfree::spsc_queue<float> &top_right,
                               boost::lockfree::spsc_queue<float> &bottom_right,
                               boost::lockfree::spsc_queue<float> &bottom_left);

    void startThread();

    virtual ~PositionProcessor();

private:
    boost::lockfree::spsc_queue<float> &top_left_;
    boost::lockfree::spsc_queue<float> &top_right_;
    boost::lockfree::spsc_queue<float> &bottom_right_;
    boost::lockfree::spsc_queue<float> &bottom_left_;
    std::atomic<bool> running_;
    std::thread *thread_;

    float top_left_avg_ = 0; // for exponential smoothing
    float top_right_avg_ = 0; // for exponential smoothing
    float bottom_right_avg_ = 0; // for exponential smoothing
    float bottom_left_avg_ = 0; // for exponential smoothing
    float w = 0.25; // smoothing rate

    void consume();

    void stopThread();

    float expAvg(float sample, float avg, float w);
};