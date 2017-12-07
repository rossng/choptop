#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>

class LoadCellsProcessor {

public:
    explicit LoadCellsProcessor(boost::lockfree::spsc_queue<float> &top_left,
                                boost::lockfree::spsc_queue<float> &top_right,
                                boost::lockfree::spsc_queue<float> &bottom_right,
                                boost::lockfree::spsc_queue<float> &bottom_left);

    void startThread();

    void stopThread();

    virtual ~LoadCellsProcessor();
private:
    boost::lockfree::spsc_queue<float> &top_left_;
    boost::lockfree::spsc_queue<float> &top_right_;
    boost::lockfree::spsc_queue<float> &bottom_right_;
    boost::lockfree::spsc_queue<float> &bottom_left_;
    std::atomic<bool> running_;

    std::thread *thread_;
    float top_left_total_ = 0;
    float top_right_total_ = 0;
    float bottom_right_total_ = 0;

    float bottom_left_total_ = 0;

    void consume();

};
