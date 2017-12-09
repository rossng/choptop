#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include "ChoptopServer.h"

class ChoptopServerWrapper {
public:
    ChoptopServerWrapper(int port);

    void startServer();

    void stopServer();

    void sendMessage(std::string text);

private:
    void serve();

    ChoptopServer server_;
    boost::lockfree::spsc_queue<std::string> commands_;
    std::thread *server_thread_;
    std::atomic<bool> running_;
};