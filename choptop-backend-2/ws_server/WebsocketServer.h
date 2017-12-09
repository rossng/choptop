#pragma once

#include "server_ws.hpp"

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

class WebsocketServer {
public:
    WebsocketServer();

    void start();

    virtual ~WebsocketServer();

private:
    WsServer server_;
};