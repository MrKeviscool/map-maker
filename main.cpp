#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <SFML/Graphics.hpp>
#define STEADY_CLOCK std::chrono::steady_clock

#define FLOOR 0
#define PLAYER 1
#define ENEMEY 2
#define END 3

using namespace std;

const int MAXFPS = 60, WIDTH = 1920, HEIGHT = 1080;
const float MOVESPEED = WIDTH/MAXFPS;

void inputevents(sf::Event *event), display(), lockFrames(), placeObject(), moveScreen(), writeToFile();
float roundToX(float num, float roundto);
sf::RectangleShape getCursorShape();

sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "map maker", sf::Style::Fullscreen);

sf::Event event;
int activeobj = FLOOR;

vector<sf::RectangleShape> objects;
vector<sf::Vector2f> actualpos;
sf::Vector2f screenpos(0, 0);

bool rotatefloors = false;
bool playerplaced = false, endplaced=false;
float xsnap = 30, ysnap = 30;

int main(){
    while(window.isOpen()){
        inputevents(&event);
        moveScreen();
        display();
        lockFrames();
    }
    writeToFile();
    return 0;
}

void inputevents(sf::Event *event){
    static bool mouseDownLastFrame = false, rotatelastframe = false;
    while(window.pollEvent(*event)){
        if(event->type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
            return;
        }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            mouseDownLastFrame = true;
        }
        if(mouseDownLastFrame && !sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            mouseDownLastFrame = false;
            placeObject();
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
            rotatelastframe = true;
        }
        if(rotatelastframe && !sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
            rotatelastframe = false;
            rotatefloors = !rotatefloors;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
            activeobj = FLOOR;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
            activeobj = PLAYER;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
            activeobj = ENEMEY;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
            activeobj = END;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        screenpos.x += MOVESPEED;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        screenpos.x -= MOVESPEED;
    }
}

void display(){
    sf::RectangleShape cursorshape = getCursorShape();
    window.clear();
    for(vector<sf::RectangleShape>::iterator objitr = objects.begin(); objitr < objects.end(); ++objitr){
        window.draw(*objitr);
    }
    window.draw(cursorshape);
    window.display();
}


sf::RectangleShape getCursorShape(){
    sf::RectangleShape rectbuffer;
    switch(activeobj){
        case FLOOR:
            if(rotatefloors){
                rectbuffer.setSize(sf::Vector2f(30, 300));
                break;
            }
            rectbuffer.setSize(sf::Vector2f(300, 30));
            break;
        case PLAYER:
            if(playerplaced){
                activeobj = FLOOR;
                break;
            }
            rectbuffer.setSize(sf::Vector2f(50, 70));
            rectbuffer.setFillColor(sf::Color::Blue);
            break;
        case ENEMEY:
            rectbuffer.setSize(sf::Vector2f(50, 70));
            rectbuffer.setFillColor(sf::Color::Red);
            break;
        case END:
            if(endplaced){
                activeobj = FLOOR;
                break;
            }
            rectbuffer.setSize(sf::Vector2f(100, 100));
            rectbuffer.setFillColor(sf::Color::Green);
            break;
        default:
            rectbuffer.setSize(sf::Vector2f(300, 30));
    };
    rectbuffer.setPosition(
        roundToX(sf::Mouse::getPosition().x, xsnap), 
        roundToX(sf::Mouse::getPosition().y, ysnap));
    return rectbuffer;
}

void lockFrames(){
    const int MAXMICRO = (1000/(float)MAXFPS)*1000;
    static STEADY_CLOCK::time_point tp;
    static bool doneframe = false;
    if(doneframe = false){
        tp = STEADY_CLOCK::now();
        doneframe = true;
    }
    else{
        unsigned int ft = chrono::duration<float, micro>(STEADY_CLOCK::now() - tp).count();
        if(ft < MAXMICRO){
            this_thread::sleep_for(chrono::microseconds(MAXMICRO - ft));
        }
        tp = STEADY_CLOCK::now();
        doneframe = false;
        // cout << "ft: " << ft << endl;
    }
}

void placeObject(){
    objects.push_back(getCursorShape());
    actualpos.push_back(objects.back().getPosition() + screenpos);
    if(activeobj == PLAYER)
        playerplaced = true;
    else if(activeobj == END)
        endplaced = true;
}

float roundToX(float num, float roundto){
    if(num <= 0){
        return num;
    }
    return round(num/roundto)*roundto;
}

void writeToFile(){
    for(int i = 0; i < objects.size(); i++){
        cout << "index " << i << ": ";
        cout << actualpos[i].x << " " << actualpos[i].y << " ";
        cout << objects[i].getSize().x << " " << objects[i].getSize().y << " ";
        cout << endl;
    }
}

void moveScreen(){
    for(int i = 0; i < objects.size(); i++){
        objects[i].setPosition(actualpos[i] - screenpos);
    }
}