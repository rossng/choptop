#include "WebsocketServer.h"

using namespace std;

int main() {
    auto srv = make_shared<WebsocketServer>();
    srv->start();
}