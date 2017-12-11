#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include <fstream>
#include <chrono>
#include <vector>
#include "SensorReader.h"

enum class PressLocation {
    TOP, BOTTOM, LEFT, RIGHT
};

struct PressEvent {
    float x;
    float y;
    PressLocation location;
};

class DataProcessor {
public:
    explicit DataProcessor(boost::lockfree::spsc_queue<SensorData> &sensor_data);

    void startThread();

    void stopThread();

    virtual ~DataProcessor();

    boost::lockfree::spsc_queue<std::pair<float, float>> position_;
    boost::lockfree::spsc_queue<float> weight_;
    boost::lockfree::spsc_queue<PressEvent> press_events_;
    boost::lockfree::spsc_queue<SensorData> sensor_data_despiked_;
private:
    boost::lockfree::spsc_queue<SensorData> &sensor_data_;
    std::atomic<bool> running_;
    std::thread *thread_;

    float top_left_smooth_ = 0; // for exponential smoothing
    float top_right_smooth_ = 0; // for exponential smoothing
    float bottom_right_smooth_ = 0; // for exponential smoothing
    float bottom_left_smooth_ = 0; // for exponential smoothing

    float w = 0.4; // smoothing rate
    float lag_weight = 0.1;
    float previous_diff_ = 0;
    const float edge_threshold_ = 200;
    float release_threshold;
    bool is_pressed_ = false;
    float weight_slow_ = 0;
    std::chrono::high_resolution_clock::time_point press_start_time_;


    std::chrono::system_clock::time_point start;
    bool timeStart = false;
    std::chrono::system_clock::time_point stop;

    void consume();

    float expAvg(float sample, float avg, float w);

    void edgeDetect(float sample, float threshold);

    float hampel(float reading, std::vector<float> &window1, std::vector<float> &window2, int &current_idx);
};