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

void display(sf::RenderWindow &window){
    window.clear();
    for(auto &obj : objects)
        window.draw(obj.shape);
    window.display();
}

void manageEvents(sf::Event &event, sf::RenderWindow &window){
    if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
        window.close();
        return;
    }
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


int main(){
    for(int i = 0; i < typesAmount; i++)
        sizes[i] = defaultSizes[i];
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "map maker", sf::Style::Fullscreen);
    objects.push_back(Object(sf::Vector2f(100, 100), objType::floor));
    while(window.isOpen()){
        display(window);
        sf::Event event;
        manageEvents(event, window);
    }

    return 0;
}