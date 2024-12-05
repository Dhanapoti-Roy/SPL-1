#include <iostream>
using namespace std;

class Player {
public:
    int position;
    string name;
    Player(string name);
    int getPosition();
};

Player::Player(string name) {
    this->name = name;
    this->position = 0;
}

int Player::getPosition() {
    return position;
}

class SnakesladdersLink {
public:
    int square;
    int endSquare;
    SnakesladdersLink* next;
    SnakesladdersLink* previous;
    SnakesladdersLink(int square,int endSquare);
    void displaySnakeLink();
};

SnakesladdersLink::SnakesladdersLink(int square,int endSquare) {
    this->square = square;
    this->endSquare = endSquare;
    next = NULL;
    previous = NULL;
}
class Board {
private:
    SnakesladdersLink* first;
    SnakesladdersLink* last;
public:
    Board();
   
    SnakesladdersLink* findSquare(int Squareid);
    int snakesarray[7][2] = {
        {30, 7},
        {47, 15},
        {56, 19},
        {73, 51},
        {82, 42},
        {92, 75},
        {98, 55}
    };
    int laddersarray[6][2] = {
        {4, 25},
        {21, 39},
        {29, 74},
        {43, 76},
        {63, 80},
        {71, 89}
    };

};


