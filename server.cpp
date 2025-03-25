#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  // Add this header for inet_pton
#include <unistd.h>
#include <string.h>
#include <vector>
#include <thread>
#include <algorithm>
#include <map>

using namespace std;

struct Client {
    int socket;
    string username;
    bool in_game;
    bool available;  // Add availability flag
};

map<int, Client> client_list;
vector<int> clients;

void broadcast_client_list() {
    string client_list_msg = "CLIENT_LIST|";
    bool first = true;
    cout << "Number of clients in list: " << client_list.size() << endl;
    for (const auto& client : client_list) {
        cout << "Client: " << client.second.username << ", in_game: " << client.second.in_game << endl;
        if (client.second.available && !client.second.in_game) {  // Only show available players
            if (!first) {
                client_list_msg += ",";
            }
            client_list_msg += client.second.username;
            first = false;
        }
    }
    
    cout << "Broadcasting client list: " << client_list_msg << endl;
    
    for (const auto& client : client_list) {
        int sent = send(client.first, client_list_msg.c_str(), client_list_msg.length(), 0);
        if (sent < 0) {
            cerr << "Failed to send to client " << client.second.username << endl;
        } else {
            cout << "Sent to " << client.second.username << ": " << sent << " bytes" << endl;
        }
    }
}

void handle_client(int client_socket) {
    char buffer[1024] = {0};
    
    // Wait for username
    int valread = read(client_socket, buffer, 1024 - 1);  // Leave space for null terminator
    if (valread > 0) {
        buffer[valread] = '\0';  // Ensure null termination
        if (strncmp(buffer, "LOGIN|", 6) == 0) {
            string username = string(buffer + 6);
            client_list[client_socket] = {client_socket, username, false, false};  // Initialize as unavailable
            cout << "User logged in: " << username << endl;
            broadcast_client_list();
        }
    }

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        valread = read(client_socket, buffer, 1024 - 1);
        if (valread <= 0) {
            cout << "Client disconnected" << endl;
            break;
        }
        buffer[valread] = '\0';  // Ensure null termination

        string message(buffer);
        cout << "Received from client: " << message << endl;
        if (message == "LIST|") {
            broadcast_client_list();  // Client requested refresh of client list
            continue;
        }
        if (message == "AVAILABLE|") {
            client_list[client_socket].available = true;
            broadcast_client_list();
            continue;
        }
        if (message == "BUSY|") {
            client_list[client_socket].available = false;
            broadcast_client_list();
            continue;
        }
        if (message.substr(0, 5) == "GAME|") {
            // Handle game invites and state updates
            string target_username = message.substr(5, message.find('|', 5) - 5);
            for (const auto& client : client_list) {
                if (client.second.username == target_username) {
                    send(client.first, message.c_str(), message.length(), 0);
                    break;
                }
            }
        }

        // Broadcast message to all other clients
        for (int other_client : clients) {
            if (other_client != client_socket) {
                send(other_client, buffer, strlen(buffer), 0);
            }
        }
    }
    
    client_list.erase(client_socket);
    auto it = find(clients.begin(), clients.end(), client_socket);
    if (it != clients.end()) clients.erase(it);
    broadcast_client_list();
    close(client_socket);
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        cerr << "Socket creation failed" << endl;
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        cerr << "Setsockopt failed" << endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Listen on all available interfaces
    address.sin_port = htons(8080);  // Change port to 8080

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        cerr << "Bind failed" << endl;
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        cerr << "Listen failed" << endl;
        return -1;
    }

    cout << "Server is running on port 8080..." << endl;

    while (true) {
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            cerr << "Accept failed" << endl;
            continue;
        }

        cout << "New client connected" << endl;
        clients.push_back(new_socket);
        
        // Create new thread for client
        thread(handle_client, new_socket).detach();
    }

    close(server_fd);
    return 0;
}