#include <ChoptopServer.h>
#include <thread>
#include <ChoptopServerWrapper.h>

int main() {
    ChoptopServerWrapper server(8080);

    server.startServer();

    this_thread::sleep_for(5s);

    server.sendMessage("hello");

    while (true);

    return 0;
}