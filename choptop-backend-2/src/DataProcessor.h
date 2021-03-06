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

enum class PressStage {
    PRESS_STARTED, PRESS_SUCCESS, PRESS_CANCELLED
};

struct PressEvent {
    float x;
    float y;
    PressLocation location;
    PressStage stage;
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

    float w = 0.6; // smoothing rate
    float lag_weight = 0.8;
    float weight_slow_ = 0;

    // Press detection
    int stage = 0;
    std::chrono::time_point<std::chrono::steady_clock> time_of_last_trigger_ = std::chrono::steady_clock::now();
    float stage_1_x_ = 0;
    float stage_1_y_ = 0;
    float stage_2_x_ = 0;
    float stage_2_y_ = 0;
    int stage_2_num_readings_ = 0;
    int stage_3_num_readings_ = 0;
    PressLocation stage_2_location_ = PressLocation::TOP;

    void consume();

    float expAvg(float sample, float avg, float w);

    void detectPress(float sample, float x, float y);

    float hampel(float reading, std::vector<float> &window1, std::vector<float> &window2, int &current_idx);
};