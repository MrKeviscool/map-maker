#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <SFML/Graphics.hpp>
#define STEADY_CLOCK std::chrono::steady_clock

#define FLOOR 0
#define LONGFLOOR 1
using namespace std;
const int MAXFPS = 60, WIDTH = 1920, HEIGHT = 1080;

void inputevents(sf::Event *event), display(), lockFrames(), placeObject(), writeToFile();
float roundToX(float num, float roundto);
sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "map maker", sf::Style::Fullscreen);

sf::Event event;
int currentpos = 0, activeobj = FLOOR;
vector<sf::RectangleShape> objects;
sf::RectangleShape getCursorShape();

float xsnap = 50, ysnap = 30;

int main(){
    while(window.isOpen()){
        inputevents(&event);
        display();
        lockFrames();


    }
    writeToFile();
    return 0;
}

void inputevents(sf::Event *event){
    static bool mouseDownLastFrame = false;
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
            rectbuffer.setSize(sf::Vector2f(300, 30));
            break;
        case LONGFLOOR:
            rectbuffer.setSize(sf::Vector2f(1200, 30));
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
        cout << objects[i].getPosition().x << " " << objects[i].getPosition().y << " ";
        cout << objects[i].getSize().x << " " << objects[i].getSize().y << " ";
        cout << endl;
    }
}