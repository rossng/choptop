#pragma once

#include <thread>
#include <functional>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <mutex>
#include <set>
#include <boost/lockfree/spsc_queue.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;

class ChoptopServer {
public:
    explicit ChoptopServer(uint16_t port);

    void startServer();

    void stopServer();

    void sendMessage(std::string text);

    void run();

    void onOpen(connection_hdl hdl);

    void onClose(connection_hdl hdl);

private:
    void send();

    typedef std::set<connection_hdl,std::owner_less<connection_hdl>> con_list;
    server server_;
    con_list connections_;
    std::mutex mutex_;
    boost::lockfree::spsc_queue<std::string> commands_;
    std::thread *server_thread_;
    std::thread *sender_thread_;
    std::atomic<bool> running_;
    uint16_t port_;
};