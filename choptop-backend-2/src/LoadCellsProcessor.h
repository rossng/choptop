#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include <fstream>

class LoadCellsProcessor {

public:
    explicit LoadCellsProcessor(boost::lockfree::spsc_queue<float> &top_left,
                                boost::lockfree::spsc_queue<float> &top_right,
                                boost::lockfree::spsc_queue<float> &bottom_right,
                                boost::lockfree::spsc_queue<float> &bottom_left,
                                std::string log_file);

    void startThread();

    void stopThread();

    virtual ~LoadCellsProcessor();

    void printStatus();

    boost::lockfree::spsc_queue<float> output_;
private:
    boost::lockfree::spsc_queue<float> &top_left_;
    boost::lockfree::spsc_queue<float> &top_right_;
    boost::lockfree::spsc_queue<float> &bottom_right_;
    boost::lockfree::spsc_queue<float> &bottom_left_;
    std::atomic<bool> running_;

    std::thread *thread_;

    std::ofstream log_file_;
    float top_left_total_ = 0;
    float top_right_total_ = 0;

    float bottom_right_total_ = 0;
    float bottom_left_total_ = 0;

    float w = 0.1; // smoothing rate

    void consume();

    float expAvg(float sample, float avg, float w);
};
