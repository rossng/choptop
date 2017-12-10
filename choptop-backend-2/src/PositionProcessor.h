#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include <fstream>

struct PositionProcessorOutput {
    float x;
    float y;
};

class PositionProcessor {
public:
    explicit PositionProcessor(boost::lockfree::spsc_queue<float> &top_left,
                               boost::lockfree::spsc_queue<float> &top_right,
                               boost::lockfree::spsc_queue<float> &bottom_right,
                               boost::lockfree::spsc_queue<float> &bottom_left,
                               std::string log_file);

    void startThread();
    void stopThread();

    virtual ~PositionProcessor();

    void printStatus();

    boost::lockfree::spsc_queue<std::pair<float, float>> output_;
    boost::lockfree::spsc_queue<PositionProcessorOutput> output_verbose_;
private:
    boost::lockfree::spsc_queue<float> &top_left_;
    boost::lockfree::spsc_queue<float> &top_right_;
    boost::lockfree::spsc_queue<float> &bottom_right_;
    boost::lockfree::spsc_queue<float> &bottom_left_;
    std::atomic<bool> running_;

    std::thread *thread_;

    std::ofstream log_file_;
    float top_left_avg_ = 0; // for exponential smoothing
    float top_right_avg_ = 0; // for exponential smoothing
    float bottom_right_avg_ = 0; // for exponential smoothing
    float bottom_left_avg_ = 0; // for exponential smoothing

    float w = 0.05; // smoothing rate

    void consume();

    float expAvg(float sample, float avg, float w);
};