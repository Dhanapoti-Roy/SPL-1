#include <iostream>
#include <list>
#include <random>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>

using namespace std;

string username;
int sock;
vector<string> available_clients;

void handle_server_message(const string& message) {
    cout << "Debug: Processing message: " << message << endl;
    
    if (message.substr(0, 11) == "CLIENT_LIST|") {
        available_clients.clear();
        string clients_str = message.substr(11);
        cout << "Debug: Client string: " << clients_str << endl;
        
        if (!clients_str.empty()) {
            size_t start = 0;
            size_t end = clients_str.find(',');
            while (start < clients_str.length()) {
                string client;
                if (end == string::npos) {
                    client = clients_str.substr(start);
                    start = clients_str.length();
                } else {
                    client = clients_str.substr(start, end - start);
                    start = end + 1;
                    end = clients_str.find(',', start);
                }
                
                if (!client.empty() && client != username) {
                    cout << "Debug: Adding client: " << client << endl;
                    available_clients.push_back(client);
                }
            }
        }
        cout << "Debug: Final client list size: " << available_clients.size() << endl;
    }
    // Add handling for game invites if needed
}

void read_from_server(int socket_fd) {
    char buffer[1024] = {0};
    while (true) {
        int valread = read(socket_fd, buffer, sizeof(buffer) - 1);
        if (valread <= 0) {
            cout << "Disconnected from server" << endl;
            exit(1);
        }
        buffer[valread] = '\0';  // Ensure null termination
        handle_server_message(string(buffer));
        memset(buffer, 0, sizeof(buffer));
    }
}

void send_to_server(int socket_fd, const string& message) {
    int sent = send(socket_fd, message.c_str(), message.length(), 0);
    if (sent < 0) {
        cerr << "Failed to send message: " << message << endl;
    } else {
        cout << "Sent to server: " << message << endl;
    }
}

class Player {
public:
    int position;
    string name;
    Player(string name) : name(name), position(0) {}
    int getPosition() { return position; }
};

class SnakesladdersLink {
public:
    int square, endSquare;
    SnakesladdersLink *next, *previous;
    SnakesladdersLink(int square, int endSquare) : square(square), endSquare(endSquare), next(nullptr), previous(nullptr) {}
    void displaySnakeLink() { cout << square << " " << endSquare << endl; }
};

class Board {
private:
    SnakesladdersLink *first, *last;
public:
    Board() : first(nullptr), last(nullptr) {}
    void insertSquare(int square, int endSquare);
    SnakesladdersLink* findSquare(int squareId);
    void displaybord();
    void rules();
    int snakesarray[7][2] = {{30, 7}, {47, 15}, {56, 19}, {73, 51}, {82, 42}, {92, 75}, {98, 55}};
    int laddersarray[6][2] = {{4, 25}, {21, 39}, {29, 74}, {43, 76}, {63, 80}, {71, 89}};
};

void Board::insertSquare(int square, int endSquare) {
    SnakesladdersLink* newSquare = new SnakesladdersLink(square, endSquare);
    if (!first) {
        first = last = newSquare;
    } else {
        last->next = newSquare;
        newSquare->previous = last;
        last = newSquare;
    }
}

SnakesladdersLink* Board::findSquare(int squareId) {
    SnakesladdersLink* findlink = first;
    while (findlink) {
        if (findlink->square == squareId) return findlink;
        findlink = findlink->next;
    }
    return nullptr;
}

void Board::displaybord() {
    cout << "\n---------------------------------------------------\n";
    cout << "                 Snakes and Ladders                \n";
    cout << "---------------------------------------------------\n\n";
    cout << "+----+----+----+----+----+----+----+----+----+----+\n";
    for (int i = 100; i > 0; i -= 10) {
        cout << "|";
        for (int j = 0; j < 10; j++) {
            int num = (i - j > 90) ? i - j : i - 9 + j;
            cout << (num < 10 ? "  " : " ") << num << "|";
        }
        cout << "\n+----+----+----+----+----+----+----+----+----+----+\n";
    }
    cout << "\nInstructions:\n1. Players take turns rolling the dice.\n2. Move your piece according to the number rolled.\n3. Ladders take you up, snakes bring you down.\n4. First to 100 wins!\n";
}

void Board::rules() {
    cout << "\n---------------------------------------------------\n";
    cout << "            Snakes and Ladders Game Rules          \n";
    cout << "---------------------------------------------------\n\n";
    cout << "Snakes:\n";
    for (int i = 0; i < 7; i++) cout << "   Snake " << i+1 << ": " << snakesarray[i][0] << " -> " << snakesarray[i][1] << "\n";
    cout << "\nLadders:\n";
    for (int i = 0; i < 6; i++) cout << "   Ladder " << i+1 << ": " << laddersarray[i][0] << " -> " << laddersarray[i][1] << "\n";
    cout << "\nGood luck!\n";
}

class Game {
private:
    string level, answer;
    int mode, position;
    Board& board;
    Player *player1, *player2;

public:
    Game(Board& b) : board(b), player1(nullptr), player2(nullptr), position(0) {}
    void play();
    void selectLevel();
    void selectMode();
    int rollDice();
    void movePlayer(Player* player, int diceResult);
};

void Game::selectLevel() {
    while (true) {
        cout << "\nSelect level: Hard (H), Medium (M), Easy (E): ";
        cin >> level;
        if (level == "H" || level == "h" || level == "M" || level == "m" || level == "E" || level == "e") break;
    }
}

void Game::selectMode() {
    while(true) {
        cout << "\nSelect mode:" << endl;
        //cout << "1 - Me vs Computer" << endl;
        cout << "2 - Multiplayer" << endl;
        cout << "3 - Toggle Availability" << endl;
        cin >> mode;

        if (mode == 1) {
            return;
        } else if (mode == 2) {
            while (true) {
                send_to_server(sock, "LIST|");
                this_thread::sleep_for(chrono::seconds(2));  // Increased wait time
                cout << "Debug: Available clients size: " << available_clients.size() << endl;
                cout << "\nAvailable players:\n";
                if (available_clients.empty()) {
                    cout << "No players available. Waiting...\n0 to refresh, -1 to go back: ";
                    int choice;
                    cin >> choice;
                    if (choice == 0) continue;
                    if (choice == -1) break;
                } else {
                    for (size_t i = 0; i < available_clients.size(); i++) {
                        cout << i + 1 << ". " << available_clients[i] << endl;
                    }
                    cout << "Select player (0 to refresh, -1 to go back): ";
                    int choice;
                    cin >> choice;
                    if (choice == 0) continue;
                    if (choice == -1) break;
                    if (choice > 0 && choice <= static_cast<int>(available_clients.size())) {
                        string invite = "GAME|" + available_clients[choice-1] + "|INVITE";
                        send_to_server(sock, invite);
                        return;
                    }
                }
            }
        } else if (mode == 3) {
            static bool is_available = false;
            is_available = !is_available;
            send_to_server(sock, is_available ? "AVAILABLE|" : "BUSY|");
            cout << "You are now " << (is_available ? "available" : "unavailable") << " for games." << endl;
            continue;
        }
    }
}

int Game::rollDice() {
    static mt19937 generator(static_cast<unsigned>(time(0)));
    uniform_int_distribution<int> distribution(1, 6);
    return distribution(generator);
}

void Game::movePlayer(Player* player, int diceResult) {
    // Simplified movement logic for brevity
    if (player) {
        player->position += diceResult;
        if (player->position > 100) player->position = 100 - (player->position - 100);
        cout << player->name << "'s position: " << player->position << endl;
    } else {
        position += diceResult;
        if (position > 100) position = 100 - (position - 100);
        cout << "Computer's position: " << position << endl;
    }
}

void Game::play() {
    selectLevel();
    selectMode();
    if (mode == 1) {
        cout << "Enter your name: ";
        string name;
        cin >> name;
        player1 = new Player(name);
        while (true) {
            cout << player1->name << "'s turn (R to roll): ";
            char input;
            cin >> input;
            if (input == 'R' || input == 'r') {
                int dice = rollDice();
                cout << "Rolled: " << dice << endl;
                movePlayer(player1, dice);
            }
            cout << "Computer's turn: ";
            int dice = rollDice();
            cout << "Rolled: " << dice << endl;
            movePlayer(nullptr, dice);
        }
    }
    // Add multiplayer logic if needed
}

int main() {
    cout << "Enter your username: ";
    cin >> username;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Socket creation error" << endl;
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);  // Change port to 8080
    
    // Connect to specific IP
    if (inet_pton(AF_INET, "192.168.0.105", &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address" << endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed" << endl;
        return -1;
    }

    cout << "Connected to server at 192.168.0.105:8080" << endl;

    send_to_server(sock, "LOGIN|" + username);
    thread(read_from_server, sock).detach();

    Board board;
    for (int i = 1; i <= 100; i++) {
        bool found = false;
        for (int j = 0; j < 7; j++) if (i == board.snakesarray[j][0]) { board.insertSquare(i, board.snakesarray[j][1]); found = true; break; }
        if (!found) for (int j = 0; j < 6; j++) if (i == board.laddersarray[j][0]) { board.insertSquare(i, board.laddersarray[j][1]); found = true; break; }
        if (!found) board.insertSquare(i, i);
    }

    Game game(board);
    board.displaybord();
    board.rules();
    game.play();

    close(sock);
    return 0;
}