#include "ChoptopServerWrapper.h"

using namespace std;

void ChoptopServerWrapper::serve() {
    while (running_) {
        server_.wait(50);
        commands_.consume_all([&](string command) {
            server_.broadcast(command);
        });
    }
}

void ChoptopServerWrapper::startServer() {
    running_ = true;
    server_thread_ = new thread(&ChoptopServerWrapper::serve, this);
}

void ChoptopServerWrapper::stopServer() {
    running_ = false;
    server_thread_->join();
}

ChoptopServerWrapper::ChoptopServerWrapper(int port): commands_(100), server_thread_(nullptr), running_(false), server_(port) {}

void ChoptopServerWrapper::sendMessage(std::string text) {
    commands_.push(text);
}

void ChoptopServerWrapper::waitUntilConnected() {
    while (!server_.connected_);
}
