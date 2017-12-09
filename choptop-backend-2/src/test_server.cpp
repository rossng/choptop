#include <ChoptopServer.h>
#include <thread>
#include <ChoptopServerWrapper.h>

int main() {
    ChoptopServerWrapper server(8765);

    server.startServer();

    this_thread::sleep_for(1s);

    server.waitUntilConnected();

    while (true) {
        server.sendMessage("{event:'downPressed'}");
        this_thread::sleep_for(5s);
    }

    return 0;
}