#include "LoadCellsProcessor.h"

#include <thread>
#include <iostream>

using namespace std;

LoadCellsProcessor::LoadCellsProcessor(boost::lockfree::spsc_queue<float> &top_left,
                                      boost::lockfree::spsc_queue<float> &top_right,
                                      boost::lockfree::spsc_queue<float> &bottom_right,
                                      boost::lockfree::spsc_queue<float> &bottom_left) : top_left_(top_left),
                                                                                         top_right_(top_right),
                                                                                         bottom_right_(bottom_right),
                                                                                         bottom_left_(bottom_left) {

}

void LoadCellsProcessor::startThread() {
    if (running_) return;
    running_ = true;
    thread_ = new thread(&LoadCellsProcessor::consume, this);
}

void LoadCellsProcessor::stopThread() {
    if (!running_) return;
    running_ = false;
    thread_->join();
}

void LoadCellsProcessor::consume() {
    while (running_){
        top_left_.consume_one([&](float f) {
            top_left_total_ = f;
        });
        top_right_.consume_one([&](float f) {
            top_right_total_ = f;
        });
        bottom_left_.consume_one([&](float f) {
            bottom_left_total_ = f;
        });
        bottom_right_.consume_one([&](float f) {
            bottom_right_total_ = f;
        });

        float total = top_left_total_ + top_right_total_ + bottom_right_total_ + bottom_left_total_;

        cout << "total 'weight': " << total << endl;
    }
}

LoadCellsProcessor::~LoadCellsProcessor() {
    running_ = false;
    delete thread_;
}