#include <boost/asio.hpp>
#include <iostream>
#include <thread>

using namespace boost::asio;
using namespace std;

void read_from_server(ip::tcp::socket& socket) {
    try {
        while (true) {
            char data[512];
            size_t length = socket.read_some(buffer(data));
            if (length > 0) {
                cout << "Server: " << string(data, length) << endl;
                // Process received data and update game state
            }
        }
    } catch (std::exception& e) {
        cerr << "Exception in thread: " << e.what() << endl;
    }
}

int main() {
    try {
        io_service ioService;
        ip::tcp::resolver resolver(ioService);
        ip::tcp::resolver::query query("192.168.0.105", "8080"); // Replace with server IP and port
        ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        ip::tcp::socket socket(ioService);
        connect(socket, endpoint_iterator);

        cout << "Connected to server!" << endl;

        thread(read_from_server, ref(socket)).detach();

        while (true) {
            string message;
            getline(cin, message);
            write(socket, buffer(message));
        }
    } catch (std::exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}
