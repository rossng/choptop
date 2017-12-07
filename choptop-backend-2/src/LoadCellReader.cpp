#include "LoadCellReader.h"

#include <utility>
#include <chrono>
#include <iostream>

using namespace std;

LoadCellReader::LoadCellReader(shared_ptr<HX711> hx711, string log_file) :
        hx711_(std::move(hx711)), raw_output_(1024), load_cell_data_(1024),
        producer_thread_(nullptr), consumer_thread_(nullptr), producing_(false), consuming_(false),
        log_file_(log_file) {
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

void LoadCellReader::produce() {
    while (producing_) {
        load_cell_data_.push(hx711_->getUnits());
        this_thread::sleep_for(50ms);
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
            raw_output_.push(f);
            log_file_ << f << std::endl;
        });
        this_thread::sleep_for(50ms);
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
