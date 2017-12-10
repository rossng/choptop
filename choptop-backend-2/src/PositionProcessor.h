#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include <fstream>
#include <chrono>
enum class PressEvent {TOP, BOTTOM, LEFT, RIGHT};

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
    boost::lockfree::spsc_queue<PressEvent> press_events_;    
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

    float w = 0.4; // smoothing rate
    float lag_weight = 0.1;
    float previous_diff = 0;
    const float edge_threshold = 300;
    float release_threshold;
    bool isPressed = false;
    float total_slow_ = 0;
    std::chrono::high_resolution_clock::time_point press_start_time_;
    void consume();

    float expAvg(float sample, float avg, float w);

    void edgeDetect(float sample, float threshold);
    
};