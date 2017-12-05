#pragma once


#include <memory>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/optional.hpp>
#include <thread>
#include "hx711.h"


class LoadCellReader {
public:
    explicit LoadCellReader(std::shared_ptr<HX711> hx711);

    void startProducing();
    void stopProducing();

    void startConsuming();
    void stopConsuming();

    virtual ~LoadCellReader();

    boost::lockfree::spsc_queue<float> raw_output_;
    boost::lockfree::spsc_queue<float> smoothed_output_;
private:
    void produce();
    void consume();

    std::shared_ptr<HX711> hx711_;
    boost::lockfree::spsc_queue<float> load_cell_data_;
    std::thread *producer_thread_;
    std::thread *consumer_thread_;
    std::atomic<bool> producing_;
    std::atomic<bool> consuming_;
};