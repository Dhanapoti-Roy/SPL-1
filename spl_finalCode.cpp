#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

const unsigned int WIDTH = 900;
const unsigned int HEIGHT = 900;
const unsigned int PADDING = 10.0f;
const unsigned int BOX_SIZE = (WIDTH / 12 - PADDING) * 1.0f;

vector<vector<int>> snakesarray = {
    {17, 7},
    {54, 34},
    {62, 19},
    {64, 60},
    {87, 24},
    {93, 70},
    {98, 79}};
vector<vector<int>> laddersarray = {
    {4, 14},
    {9, 31},
    {20, 38},
    {28, 84},
    {40, 59},
    {49, 68},
    {63, 81}};
class SnakesLink
{
public:
    int head;
    int tail;
    VertexArray line;
    SnakesLink(int head, int tail);
    void draw(RenderWindow &window)
    {
       
        VertexArray snake(LineStrip, 20);
        Vector2f start = {line[0].position.x - BOX_SIZE / 2.0f, line[0].position.y + BOX_SIZE / 2.0f};
        Vector2f end = {line[1].position.x - BOX_SIZE / 2.0f, line[1].position.y + BOX_SIZE / 2.0f};

        Vector2f diff = end - start;
        float length = sqrt(diff.x * diff.x + diff.y * diff.y);
        Vector2f normal(-diff.y / length, diff.x / length);

        for (int i = 0; i < 20; i++)
        {
            float t = i / 19.0f;
            float wave = sin(t * 6 * 3.14159f) * 20.0f;
            Vector2f pos = start + diff * t + normal * wave;
            snake[i].position = pos;
            snake[i].color = Color::Green;
        }
        window.draw(snake);
    }

private:
    ;
};

SnakesLink::SnakesLink(int head, int tail)
{
    this->head = head;
    this->tail = tail;
    this->line = VertexArray(Lines, 2);
    int headRow = (head - 1) / 10;
    int headCol = (head - 1) % 10;
    if (headRow % 2 == 1)
    {
        headCol = 9 - headCol;
    }

    int tailRow = (tail - 1) / 10;
    int tailCol = (tail - 1) % 10;
    if (tailRow % 2 == 1)
    {
        tailCol = 9 - tailCol;
    }
    this->line[0].position = Vector2f(10 + headCol * BOX_SIZE + PADDING * headCol + BOX_SIZE, 10 + (9 - headRow) * BOX_SIZE + PADDING * (9 - headRow));
    this->line[1].position = Vector2f(10 + tailCol * BOX_SIZE + PADDING * tailCol + BOX_SIZE, 10 + (9 - tailRow) * BOX_SIZE + PADDING * (9 - tailRow));

    this->line.append(Vertex(Vector2f(0, 0), Color::Green));
    this->line.append(Vertex(Vector2f(100, 100), Color::Green));
}

class LaddersLink
{
public:
    int head;
    int tail;
    VertexArray line;
    LaddersLink(int head, int tail);
    void draw(RenderWindow &window)
    {
        
        Vector2f start = {line[0].position.x - BOX_SIZE / 2.0f, line[0].position.y + BOX_SIZE / 2.0f};
        Vector2f end = {line[1].position.x - BOX_SIZE / 2.0f, line[1].position.y + BOX_SIZE / 2.0f};

        Vector2f diff = end - start;
        float length = sqrt(diff.x * diff.x + diff.y * diff.y);
        Vector2f dir = diff / length;
        Vector2f normal(-dir.y, dir.x);

        // Draw side lines
        VertexArray sides(Lines, 4);
        sides[0].position = start - normal * 10.0f;
        sides[1].position = end - normal * 10.0f;
        sides[2].position = start + normal * 10.0f;
        sides[3].position = end + normal * 10.0f;

        for (int i = 0; i < 4; i++)
        {
            sides[i].color = Color::Yellow;
        }

        window.draw(sides);

        
        const int numRungs = 5;
        VertexArray rungs(Lines, numRungs * 2);
        for (int i = 0; i < numRungs; i++)
        {
            float t = (i + 1.0f) / (numRungs + 1.0f);
            Vector2f pos = start + diff * t;
            rungs[i * 2].position = pos - normal * 10.f;
            rungs[i * 2 + 1].position = pos + normal * 10.f;
            rungs[i * 2].color = Color::Yellow;
            rungs[i * 2 + 1].color = Color::Yellow;
        }
        window.draw(rungs);
    }

private:
    ;
};

LaddersLink::LaddersLink(int head, int tail)
{
    this->head = head;
    this->tail = tail;
    this->line = VertexArray(Lines, 2);

    int headRow = (head - 1) / 10;
    int headCol = (head - 1) % 10;
    if (headRow % 2 == 1)
    {
        headCol = 9 - headCol;
    }

    int tailRow = (tail - 1) / 10;
    int tailCol = (tail - 1) % 10;
    if (tailRow % 2 == 1)
    {
        tailCol = 9 - tailCol;
    }

    this->line[0].position = Vector2f(10 + headCol * BOX_SIZE + PADDING * headCol + BOX_SIZE, 10 + (9 - headRow) * BOX_SIZE + PADDING * (9 - headRow));
    this->line[1].position = Vector2f(10 + tailCol * BOX_SIZE + PADDING * tailCol + BOX_SIZE, 10 + (9 - tailRow) * BOX_SIZE + PADDING * (9 - tailRow));

    this->line.append(Vertex(Vector2f(0, 0), Color::Cyan));
    this->line.append(Vertex(Vector2f(100, 100), Color::Cyan));
}
class Player
{
private:
    
    CircleShape guti = CircleShape(20.0f);
    Text nameText = Text();
    int playerNumber = 0;
    bool isPlaying = false;
    string name;

public:
    int position = 0;
    bool isExit()
    {
        return isPlaying;
    }
    Player(String name, Color color, Font &font, int pcount);
    void draw(RenderWindow &window)
    {
        window.draw(guti);
        window.draw(nameText);
    }
    string getName()
    {
        return name;
    }
    void exit()
    {
        isPlaying = true;
    }
    void updateGuti()
    {
        int row = (position - 1) / 10;
        int col = (position - 1) % 10;
        if (row % 2 == 1)
        {
            col = 9 - col;
        }
        guti.setPosition(10 + col * BOX_SIZE + PADDING * col + BOX_SIZE / 2 - guti.getRadius(), 10 + (9 - row) * BOX_SIZE + PADDING * (9 - row) + BOX_SIZE / 2 - guti.getRadius());
    }

    ~Player();
};

Player::Player(String name, Color color, Font &font, int pcount)
{
    this->guti = CircleShape(20.0f);
    this->guti.setOutlineThickness(2);
    this->guti.setOutlineColor(Color::White);
    this->guti.setFillColor(color);
    this->guti.setPosition(WIDTH + 30, HEIGHT / 3 + pcount * 50);

    this->name = name;
    this->nameText = Text(name, font, 24);
    this->nameText.setFillColor(color);
    this->nameText.setPosition(WIDTH + 30 - 8 * 12.0f, HEIGHT / 3 + pcount * 50);
    this->playerNumber = pcount;
    this->position = 0;
}

Player::~Player()
{
}
class Dice
{
private:
    RectangleShape dice;
    int currentFace;
    bool rolling;
    Clock clock;
    Time rollDuration;

public:
    Dice();
    void roll();
    int update();
    void draw(RenderWindow &window);
    bool isRolling() const;
    void drawFace(RenderWindow &window, int face);
};

Dice::Dice()
{
    dice = RectangleShape(Vector2f(100, 100));
    dice.setFillColor(Color::White);
    dice.setOutlineThickness(5);
    dice.setOutlineColor(Color::Black);
    dice.setPosition(WIDTH + 100 - dice.getLocalBounds().width - 20, 20); // Adjust position as needed
    currentFace = 1;
    rolling = false;
    rollDuration = seconds(0.5);
    clock = Clock();
}

void Dice::roll()
{
    rolling = true;
    clock.restart();
}

int Dice::update()
{
    if (rolling)
    {
        if (clock.getElapsedTime() < rollDuration)
        {
            currentFace = rand() % 6 + 1;
        }
        else
        {
            rolling = false;
        }
        return currentFace;
    }
    return -1;
}

void Dice::draw(RenderWindow &window)
{
    window.draw(dice);
    drawFace(window, currentFace);
}

bool Dice::isRolling() const
{
    return rolling;
}

void Dice::drawFace(RenderWindow &window, int face)
{
    CircleShape dot(10);
    dot.setFillColor(Color::Black);

    switch (face)
    {
    case 1:
        dot.setPosition(dice.getPosition().x + 45 - dot.getLocalBounds().width / 2, dice.getPosition().y + 45 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        break;
    case 2:
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 20, dice.getPosition().y + 20 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 70, dice.getPosition().y + 70 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        break;
    case 3:
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 20, dice.getPosition().y + 20 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 45, dice.getPosition().y + 45 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 70, dice.getPosition().y + 70 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        break;
    case 4:
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 20, dice.getPosition().y + 20 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 70, dice.getPosition().y + 20 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 20, dice.getPosition().y + 70 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 70, dice.getPosition().y + 70 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        break;
    case 5:
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 20, dice.getPosition().y + 20 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 70, dice.getPosition().y + 20 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 45, dice.getPosition().y + 45 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 20, dice.getPosition().y + 70 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 70, dice.getPosition().y + 70 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        break;
    case 6:
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 20, dice.getPosition().y + 20 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 70, dice.getPosition().y + 20 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 20, dice.getPosition().y + 45 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 70, dice.getPosition().y + 45 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 20, dice.getPosition().y + 70 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        dot.setPosition(dice.getPosition().x - dot.getLocalBounds().width / 2 + 70, dice.getPosition().y + 70 - dot.getLocalBounds().height / 2);
        window.draw(dot);
        break;
    }
}

template <class T>
void drawLines(vector<T> lines, RenderWindow &window)
{
    for (int i = 0; i < lines.size(); i++)
    {
        lines[i].draw(window);
    }
}
int main(int argc, char const *argv[])
{
    srand(static_cast<unsigned int>(time(0))); // Initialize random seed

    Dice dice = Dice();
    vector<SnakesLink> sLinks;
    vector<LaddersLink> lLinks;
    RectangleShape transitionBox = RectangleShape({BOX_SIZE / 2, 5.0f});
    transitionBox.setFillColor(Color::Transparent);
    transitionBox.setOutlineThickness(2);
    transitionBox.setOutlineColor(Color(234, 231, 152, 222));
    bool isp1 = true;

    Font font;
    if (!font.loadFromFile("./myFont.ttf"))
    {
        cout << "Failed to load font" << endl;
        return -1;
    }

    for (int i = 0; i < snakesarray.size(); i++)
    {
        sLinks.push_back(SnakesLink(snakesarray[i][0], snakesarray[i][1]));
    }
    for (int i = 0; i < laddersarray.size(); i++)
    {
        lLinks.push_back(LaddersLink(laddersarray[i][0], laddersarray[i][1]));
    }
    string name1, name2;
    name1 = "Player 1";
    name2 = "Player 2";

    sf::Uint32 hexValue = 0x1F93FFFF; // color collected from Facebook theme

    Player p1 = Player(name1, Color::Red, font, 1);      // player1 object
    Player p2 = Player(name2, Color(hexValue), font, 2); // player2 object

    vector<vector<RectangleShape>> boxs(10, vector<RectangleShape>(10, RectangleShape({BOX_SIZE, BOX_SIZE})));
    vector<vector<Text>> texts;
    texts.resize(10);

    
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            int row = i;
            int col = j;
            if (i % 2 == 1)
            {
                col = 9 - col;
            }
            int number = col + 1 + 10 * row;
            Text text(to_string(number), font, 48);
            text.setFillColor(Color::White);
            text.setPosition(PADDING * 2 + j * BOX_SIZE + PADDING * j, PADDING * 2 + 9 * BOX_SIZE + 9 * PADDING - (10 + i * BOX_SIZE + PADDING * i));
            texts[i].push_back(text);

            boxs[i][j].setPosition(10 + j * BOX_SIZE + PADDING * j, 10 + i * BOX_SIZE + PADDING * i);
            boxs[i][j].setFillColor(Color::Transparent);
            boxs[i][j].setOutlineThickness(2);
            boxs[i][j].setOutlineColor(Color(255, 255, 255, 50));
        }
    }

    RenderWindow window(VideoMode(WIDTH + 100, HEIGHT - 50), "Snake lader", Style::Close);

    while (window.isOpen())
    {
        Event event;
        auto x = dice.update();
        if (!dice.isRolling() && x != -1)
        {
            cout << (isp1 ? "1 => " : "2 => ") << p1.position << ", " << p2.position << endl;
            if (x != 6)
            {
                if (isp1 && p1.isExit())
                {

                    p1.position += (p1.position == 0 ? 1 : x);
                    for (int i = 0; i < sLinks.size(); i++)
                    {
                        if (p1.position == sLinks[i].head)
                        {
                            p1.position = sLinks[i].tail;
                            break;
                        }
                    }
                    for (int i = 0; i < lLinks.size(); i++)
                    {
                        if (p1.position == lLinks[i].head)
                        {
                            p1.position = lLinks[i].tail;
                            break;
                        }
                    }
                    if (p1.position >= 100)
                    {
                        cout << p1.getName() << " wins" << endl;
                        window.close();
                    }
                }
                else if (!isp1 && p2.isExit())
                {
                    p2.position += (p2.position == 0 ? 1 : x);

                    for (int i = 0; i < sLinks.size(); i++)
                    {
                        if (p2.position == sLinks[i].head)
                        {
                            p2.position = sLinks[i].tail;
                            break;
                        }
                    }
                    for (int i = 0; i < lLinks.size(); i++)
                    {
                        if (p2.position == lLinks[i].head)
                        {
                            p2.position = lLinks[i].tail;
                            break;
                        }
                    }
                    if (p2.position >= 100)
                    {
                        cout << p2.getName() << " wins" << endl;
                        window.close();
                    }
                }

                isp1 = !isp1;
            }
            else
            {
                if (isp1)
                {
                    p1.exit();
                    p1.position += (p1.position == 0 ? 1 : x);
                    for (int i = 0; i < sLinks.size(); i++)
                    {
                        if (p1.position == sLinks[i].head)
                        {
                            p1.position = sLinks[i].tail;
                            break;
                        }
                    }
                    for (int i = 0; i < lLinks.size(); i++)
                    {
                        if (p1.position == lLinks[i].head)
                        {
                            p1.position = lLinks[i].tail;
                            break;
                        }
                    }
                    if (p1.position >= 100)
                    {
                        cout << p1.getName() << " wins" << endl;
                        window.close();
                    }
                }
                else
                {
                    p2.exit();
                    p2.position += (p2.position == 0 ? 1 : x);

                    for (int i = 0; i < sLinks.size(); i++)
                    {
                        if (p2.position == sLinks[i].head)
                        {
                            p2.position = sLinks[i].tail;
                            break;
                        }
                    }
                    for (int i = 0; i < lLinks.size(); i++)
                    {
                        if (p2.position == lLinks[i].head)
                        {
                            p2.position = lLinks[i].tail;
                            break;
                        }
                    }
                    if (p2.position >= 100)
                    {
                        cout << p2.getName() << " wins" << endl;
                        window.close();
                    }
                }
            }
        }
        while (window.pollEvent(event))
        {
            if (event.key.code == Keyboard::Space)
            {
                dice.roll();

                
            }
            if (event.type == Event::Closed || event.key.code == Keyboard::Escape)
            {
                window.close();
            }
        }
        window.clear(Color({10, 10, 10, 250}));

        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                window.draw(boxs[i][j]);
                window.draw(texts[i][j]);
            }
        }
        if (p1.isExit())
            p1.updateGuti();
        p1.draw(window);

        if (p2.isExit())
            p2.updateGuti();
        p2.draw(window);

        if (isp1)
        {
            transitionBox.setPosition(WIDTH - 14 * 8, HEIGHT / 3 + 60);
        }
        else
        {
            transitionBox.setPosition(WIDTH - 14 * 8, HEIGHT / 3 + 120);
        }
        window.draw(transitionBox);
        drawLines(sLinks, window);
        drawLines(lLinks, window);
        dice.draw(window);

        window.display();
    }

    return 0;
}
