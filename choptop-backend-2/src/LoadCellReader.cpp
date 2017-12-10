#include "LoadCellReader.h"

#include <utility>
#include <chrono>
#include <iostream>
#include <numeric>
#include <wiringPi.h>

using namespace std;

LoadCellReader::LoadCellReader(shared_ptr<HX711> hx711, string log_file) :
        hx711_(std::move(hx711)), raw_output_(1024), load_cell_data_(1024),
        producer_thread_(nullptr), consumer_thread_(nullptr), producing_(false), consuming_(false),
        log_file_(log_file), window_(7, 0), window2_(7, 0), hampel_idx_(0) {
}

void LoadCellReader::startProducing() {
    if (producing_) return;
    producing_ = true;
    producer_thread_ = new thread(&LoadCellReader::produce, this);
}

void LoadCellReader::stopProducing() {
    if (!producing_) return;
    producing_ = false;
    producer_thread_->join();
}

float LoadCellReader::hampel(float reading) {
    window_[hampel_idx_] = reading;
    hampel_idx_ = (hampel_idx_ + 1) % 7;

    // Compute median
    window2_ = window_;
    std::sort(window2_.begin(), window2_.end());
    float median = window2_[3];
    float mean = std::accumulate(window_.begin(), window_.end(), 0.f) / 7;

    // Computer stddev
    std::transform(window_.begin(), window_.end(), window2_.begin(), [mean](float x) { return x - mean; });
    float sq_sum = std::inner_product(window2_.begin(), window2_.end(), window2_.begin(), 0.f);
    float stdev = std::sqrt(sq_sum/7);

    return std::abs(reading - median) > (2 * stdev) ? median : reading;
}

void LoadCellReader::produce() {
    while (producing_) {
        load_cell_data_.push(hx711_->getUnits());
        delay(1);
    }
}

void LoadCellReader::startConsuming() {
    if (consuming_) return;
    consuming_ = true;
    consumer_thread_ = new thread(&LoadCellReader::consume, this);
}

void LoadCellReader::stopConsuming() {
    if (!consuming_) return;
    consuming_ = false;
    consumer_thread_->join();
}

void LoadCellReader::consume() {
    while (consuming_) {
        load_cell_data_.consume_all([&](float f) {
            float despiked_value = hampel(f);
            raw_output_.push(despiked_value);
            log_file_ << despiked_value << std::endl;
        });
        delay(50);
    }
}

LoadCellReader::~LoadCellReader() {
    consuming_ = false;
    producing_ = false;

    delete consumer_thread_;
    delete producer_thread_;
}

void LoadCellReader::printStatus(int idx) {
    cout << "LoadCellReader " << idx << ": " << endl;
    cout << "load_cell_data_: " << load_cell_data_.read_available() << "/" << load_cell_data_.write_available() << endl;
    cout << "raw_output_: " << raw_output_.read_available() << "/" << raw_output_.write_available() << endl;
}
