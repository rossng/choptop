#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include <fstream>
#include <atomic>
#include <chrono>

enum class PressEvent {TOP, BOTTOM, LEFT, RIGHT};

class LoadCellsProcessor {

public:
    explicit LoadCellsProcessor(boost::lockfree::spsc_queue<float> &top_left,
                                boost::lockfree::spsc_queue<float> &top_right,
                                boost::lockfree::spsc_queue<float> &bottom_right,
                                boost::lockfree::spsc_queue<float> &bottom_left,
                                std::string log_file,
                                std::string log_diff_file);

    void startThread();

    void stopThread();

    virtual ~LoadCellsProcessor();

    void printStatus();



    boost::lockfree::spsc_queue<float> output_;
    boost::lockfree::spsc_queue<PressEvent> press_events_;
private:
    boost::lockfree::spsc_queue<float> &top_left_;
    boost::lockfree::spsc_queue<float> &top_right_;
    boost::lockfree::spsc_queue<float> &bottom_right_;
    boost::lockfree::spsc_queue<float> &bottom_left_;
    std::atomic<bool> running_;

    std::thread *thread_;

    std::ofstream log_file_;
    std::ofstream log_diff_file_;
    float top_left_total_ = 0;
    float top_right_total_ = 0;

    float bottom_right_total_ = 0;
    float bottom_left_total_ = 0;

    float top_left_pos_ = 0;
    float top_right_pos_ = 0;

    float bottom_right_pos_ = 0;
    float bottom_left_pos_ = 0;

    float total_slow_ = 0;
    float w = 0.1; // smoothing rate
    float lag_weight = 0.0625;
    float previous_diff = 0;
    const float edge_threshold = 300;
    bool isPressed = false;

    std::chrono::system_clock::time_point start;
    bool timeStart = false;
    std::chrono::system_clock::time_point stop;

    void consume();

    float expAvg(float sample, float avg, float w);

    void edgeDetect(float sample, float threshold);
};
