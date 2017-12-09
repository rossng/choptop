#pragma once

#include <boost/lockfree/detail/atomic.hpp>
#include "WebSocketServer.h"
#include "Util.h"

class ChoptopServer : public WebSocketServer {
public:
    explicit ChoptopServer(int port, const std::string &certPath = "", const std::string &keyPath = "")
            : WebSocketServer(port, certPath, keyPath), connected_(false) {}

    ~ChoptopServer() override;

    void onConnect(int socketID) override;

    void onMessage(int socketID, const std::string &data) override;

    void onDisconnect(int socketID) override;

    void onError(int socketID, const std::string &message) override;

    std::atomic<bool> connected_;
};