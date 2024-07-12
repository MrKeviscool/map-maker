#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "objects.hpp"
#define STEADY_CLOCK std::chrono::steady_clock

using namespace std;

const int MAXFPS = 60, WIDTH = 1920, HEIGHT = 1080;
const float MOVESPEED = (WIDTH/MAXFPS)/1.5, RESIZESPEED = 2, SNAPSIZE = 10;

void inputevents(sf::Event *event), display(), lockFrames(), placeObject(), moveScreen(), writeToFile(), resizeObj();
float roundToX(float num, float roundto);
sf::RectangleShape getCursorShape();

sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "map maker", sf::Style::Fullscreen);

sf::Event event;
int activeobj = FLOOR;
 
vector<mapobj> objects;
sf::Vector2f screenpos(0, 0);

bool rotatefloors = false;
bool playerplaced = false, endplaced=false;
float xsnap = 30, ysnap = 30;
int mapsamount = 1;

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
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        screenpos.x -= MOVESPEED;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        screenpos.y -= MOVESPEED;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        screenpos.y += MOVESPEED;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::J) || 
    sf::Keyboard::isKeyPressed(sf::Keyboard::K) ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::L) ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::I)){
        resizeObj();
    }
}

void display(){
    sf::RectangleShape cursorshape = getCursorShape();
    window.clear();
    for(int i = 0; i < objects.size(); i++){
        window.draw(objects[i].shape);
    }
    window.draw(cursorshape);
    window.display();
}


sf::RectangleShape getCursorShape(){
    sf::RectangleShape rectbuffer;
    switch(activeobj){
        case FLOOR:
            if(rotatefloors){
                rectbuffer.setSize(sf::Vector2f(floorsize.y, floorsize.x));
                break;
            }
            rectbuffer.setSize(floorsize);
            break;
        case PLAYER:
            if(playerplaced){
                activeobj = FLOOR;
                break;
            }
            rectbuffer.setSize(playersize);
            rectbuffer.setFillColor(sf::Color::Blue);
            break;
        case ENEMEY:
            rectbuffer.setSize(enemeysize);
            rectbuffer.setFillColor(sf::Color::Red);
            break;
        case END:
            if(endplaced){
                activeobj = FLOOR;
                break;
            }
            rectbuffer.setSize(endsize);
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
    }
}

void placeObject(){
    sf::RectangleShape shape = getCursorShape();
    objects.push_back(mapobj(shape, shape.getPosition() + screenpos, activeobj));
    if(activeobj == PLAYER)
        playerplaced = true;
    else if(activeobj == END)
        endplaced = true;
}

float roundToX(float num, float roundto){
    // if(num <= 0){
    //     return num;
    // }
    return round(num/roundto)*roundto;
}

void writeToFile(){
    ofstream file("maps");
    file << mapsamount;
    file << "/";

    for(int i = 0; i < objects.size(); i++){
        cout << "index " << i << ": ";
        cout << objects[i].actualpos.x << " " << objects[i].actualpos.y << " ";
        cout << objects[i].shape.getSize().x << " " << objects[i].shape.getSize().y << " ";
        cout << endl;
    }
}

void moveScreen(){
    for(int i = 0; i < objects.size(); i++){
        objects[i].shape.setPosition(objects[i].actualpos - screenpos);
    }
}

void resizeObj(){
    static float movebuff = 0;
    char buttclicked;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::J))
        buttclicked = 'j';
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::L))
        buttclicked = 'l';
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::K))
        buttclicked = 'k';
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::I))
        buttclicked = 'i';
    sf::Vector2f *obj;
    switch(activeobj){
        case FLOOR:
            obj = &floorsize;
            break;
        case PLAYER:
            obj = &playersize;
            break;
        case ENEMEY:
            obj = &enemeysize;
        case END:
            obj = &endsize;
        default:
            obj = &floorsize;
    };
    movebuff += RESIZESPEED;
    if(movebuff < SNAPSIZE){
        return;
    }
    switch(buttclicked){
        case 'j':
            if(obj->x <= 1){
                obj->x = 1;
                break;
            }
            obj->x -= SNAPSIZE;
            break;
        case 'i':
            if(obj->y <= 1){
                obj->y = 1;
                break;
            }
            obj->y -= SNAPSIZE;
            break;
        case 'l':
            obj->x += SNAPSIZE;
            break;
        case 'k':
            obj->y += SNAPSIZE;
        break;
    };
    movebuff = 0;
}