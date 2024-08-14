#include <iostream>
#include <vector>
#include <array>
#include <thread>
#include <chrono>
#include <cmath>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "objects.hpp"
#define STEADY_CLOCK std::chrono::steady_clock

using namespace std;

const int MAXFPS = 60, WIDTH=1920, HEIGHT=1080;

vector<Object> objects;

int numButtonDown = 0;
int timesPressed = 0;


void manageEvents(sf::Event &event, sf::RenderWindow &window){

    while(window.pollEvent(event)){
        if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
            return;
        }
        if(event.type == sf::Event::KeyReleased){
            int num = event.key.code - 27; //get the actual nunber
            if(num < 0 || num > 3) //if its not in range, move on
                continue;
                
            if(num != numButtonDown) //if its diffrent to what was currently pressed...
                timesPressed=0; //then set the amount of times it was previously pressed to 0
            timesPressed++; //add one because you clicked it once
            numButtonDown = num; //set the number you clicked to the num you clicked
            if(bindings[numButtonDown].size() < timesPressed)
                timesPressed = 1;
        }
        continue;
    }
}

Object getCursorObj(){
    return(Object((sf::Vector2f)sf::Mouse::getPosition(), bindings[numButtonDown][timesPressed-1]));
}

void lockFrames(){
    constexpr int MAXMICRO = (1000/(float)MAXFPS)*1000;
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

void display(sf::RenderWindow &window){
    window.clear();
    for(auto &obj : objects)
        window.draw(obj.shape);
    window.draw(getCursorObj().shape);
    window.display();
}

int main(){
    for(int i = 0; i < typesAmount; i++)
        sizes[i] = defaultSizes[i];
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "map maker"/*, sf::Style::Fullscreen*/);
    // objects.push_back(Object(sf::Vector2f(100, 100), objType::floor));
    while(window.isOpen()){
        sf::Event event;
        manageEvents(event, window);
        display(window);
        lockFrames();
    }

    return 0; 
}