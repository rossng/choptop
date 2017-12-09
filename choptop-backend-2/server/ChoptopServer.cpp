#include <iostream>
#include "ChoptopServer.h"

using namespace std;

void ChoptopServer::onConnect(int socketID) {
    cout << "WebSocket connected!" << endl;
}

void ChoptopServer::onMessage(int socketID, const string &data) {
    cout << "Message: " << data << endl;
}

void ChoptopServer::onDisconnect(int socketID) {

}

void ChoptopServer::onError(int socketID, const string &message) {

}

ChoptopServer::~ChoptopServer() {
}
