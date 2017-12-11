#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>

struct SensorData {
    float top_left;
    float top_right;
    float bottom_right;
    float bottom_left;
};

class SensorReader {
public:
    explicit SensorReader(std::string device);
    explicit SensorReader(int port);

    static void printSerialDevices();

    void tare();
    void startReading();
    void stopReading();

    void read();

    boost::lockfree::spsc_queue<SensorData> sensor_data_;

private:
    std::string readLine();

    std::string device_;
    int port_;

    std::atomic<bool> running_;
    std::thread *thread_;
public:
    virtual ~SensorReader();
};