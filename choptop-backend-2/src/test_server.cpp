#include <ChoptopServer.h>

using namespace std;

int main() {
    ChoptopServer srv(8765);

    srv.startServer();

    while (true) {
        srv.sendMessage("test");
        this_thread::sleep_for(1s);
    }

    srv.stopServer();
}