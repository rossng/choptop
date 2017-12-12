#include "ChoptopServer.h"

using namespace std;

void ChoptopServer::send() {
    while (running_) {
        this_thread::sleep_for(100ms);

        std::lock_guard<std::mutex> lock(mutex);
        commands_.consume_all([&](string command) {
            for (auto &con : connections_) {
                server_.send(con, command, websocketpp::frame::opcode::text);
            }
        });
    }
}

void ChoptopServer::startServer() {
    running_ = true;
    sender_thread_ = new thread(&ChoptopServer::send, this);
    server_thread_ = new thread(&ChoptopServer::run, this);
}

void ChoptopServer::stopServer() {
    running_ = false;
    if (server_.is_listening()) {
        server_.stop_listening();
    }
    for (auto &conn : connections_) {
        websocketpp::lib::error_code ec;
        server_.close(conn, websocketpp::close::status::going_away, "", ec);
    }
    if (!server_.stopped()) {
        server_.stop();
    }
    if (server_thread_ != nullptr) {
        if (server_thread_->joinable()) {
            server_thread_->join();
        }
    }
    if (sender_thread_ != nullptr) {
        if (sender_thread_->joinable()) {
            sender_thread_->join();
        }
    }
}

void ChoptopServer::onOpen(connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(mutex_);
    connections_.insert(hdl);
}

void ChoptopServer::onClose(connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(mutex_);
    connections_.erase(hdl);
}

ChoptopServer::ChoptopServer(uint16_t port) : commands_(100), server_thread_(nullptr), sender_thread_(nullptr),
                                         running_(false), server_(), port_(port) {
    server_.init_asio();
    server_.set_open_handler(std::bind(&ChoptopServer::onOpen, this, std::placeholders::_1));
    server_.set_close_handler(std::bind(&ChoptopServer::onClose, this, std::placeholders::_1));
    server_.set_access_channels(websocketpp::log::alevel::none);
    server_.set_error_channels(websocketpp::log::elevel::none);
}

void ChoptopServer::sendMessage(std::string text) {
    commands_.push(text);
}

void ChoptopServer::run() {
    server_.listen(port_);
    server_.start_accept();
    server_.run();
}
