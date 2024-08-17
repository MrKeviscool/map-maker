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

const int MAXFPS = 60, WIDTH=1920, HEIGHT=1080, SNAPSIZE = 30;

vector<Object> objects;
vector<Object> undoBuffer;

Input input;

bool playerPLaced = false, endPlaced = false;

inline int roundToSnap(float num){
    return round(num/SNAPSIZE)*SNAPSIZE;
}

inline objType curObjType(){
    return bindings[input.numDown][input.timesPressed-1];
}

void undo(){
    if(objects.size() == 0)
        return;
    if(objects.back().type == objType::player)
        playerPLaced = false;
    else if(objects.back().type == objType::end)
        endPlaced = false;
    undoBuffer.push_back(objects.back());
    objects.pop_back();
}

void reset(){
    objects.clear();
    undoBuffer.clear();
    playerPLaced = false;
    endPlaced = false;
    input.numDown = 0;
    input.timesPressed = 1;
    input.screenPos = sf::Vector2i(0, 0);
    for(int i = 0; i < typesAmount; i++) //could use std::copy however this is fine
        sizes[i] = defaultSizes[i];
}

void redo(){
    if(undoBuffer.size() == 0)
        return;
    if(undoBuffer.back().type == objType::player && playerPLaced || undoBuffer.back().type == objType::end && endPlaced){
        undoBuffer.pop_back();
        return;
    }
    objects.push_back(undoBuffer.back());
    undoBuffer.pop_back();
}

void moveObjects(){
    const int MOVESPEED = 20;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        input.screenPos.x -= MOVESPEED;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        input.screenPos.x += MOVESPEED;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        input.screenPos.y -= MOVESPEED;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        input.screenPos.y += MOVESPEED;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        for(auto &obj : objects)
            obj.shape.setPosition(obj.actualPos - sf::Vector2f(roundToSnap(input.screenPos.x), roundToSnap(input.screenPos.y)));
}

void resizeObjects(){
    const float RESIZESPEED = 5;
    sf::Vector2f &curSize = sizes[int(curObjType())];
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::J) && curSize.x-RESIZESPEED > SNAPSIZE)
        curSize.x -= RESIZESPEED;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::L))
        curSize.x += RESIZESPEED;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::I) && curSize.y-RESIZESPEED > SNAPSIZE)
        curSize.y -= RESIZESPEED;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::K))
        curSize.y += RESIZESPEED;
}

void rmObject(){
    for(int i = 0; i < objects.size(); i++){
        if(sf::Vector2f(roundToSnap(sf::Mouse::getPosition().x), roundToSnap(sf::Mouse::getPosition().y)) == objects[i].shape.getPosition()){
            undoBuffer.push_back(objects[i]);
            objects.pop_back();
            return;
        }
    }
}

void rotateObjects(){
    sf::Vector2f &objsize = sizes[int(curObjType())];
    float temp = objsize.x;
    objsize.x = objsize.y;
    objsize.y = temp;
}

void manageEvents(sf::Event &event, sf::RenderWindow &window){

    while(window.pollEvent(event)){
        if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
            return;
        }
        if(event.type == sf::Event::KeyReleased){
            if(event.key.code == sf::Keyboard::R){
                rotateObjects();
                continue;
            }
            if(event.key.code == sf::Keyboard::U){
                undo();
                continue;
            }
            if(event.key.code == sf::Keyboard::Y){
                redo();
                continue;
            }
            if(event.key.code == sf::Keyboard::X){
                reset();
                continue;
            }
            int num = event.key.code - 27; //get the actual nunber
            if(num < 0 || num > 3) //if its not in range, move on
                continue;
                
            if(num != input.numDown) //if its diffrent to what was currently pressed...
                input.timesPressed=0; //then set the amount of times it was previously pressed to 0
            input.timesPressed++; //add one because you clicked it once
            input.numDown = num; //set the number you clicked to the num you clicked
            if(bindings[input.numDown].size() < input.timesPressed)
                input.timesPressed = 1;
            continue;
        }
        if(event.type == sf::Event::MouseButtonReleased){
            if(event.mouseButton.button == sf::Mouse::Left){
                input.mouseReleased = true;
                continue;
            }
            else if(event.mouseButton.button == sf::Mouse::Right){
                rmObject();
            }
        }
    }
    moveObjects();
    resizeObjects();
}

Object getCursorObj(const sf::RenderWindow &window){
    return(Object(sf::Vector2f(roundToSnap(sf::Mouse::getPosition(window).x), roundToSnap(sf::Mouse::getPosition(window).y)), curObjType()));
}

void placeObj(const sf::RenderWindow &window){
    Object cursorObj = getCursorObj(window);
    if(cursorObj.type == objType::player && playerPLaced|| cursorObj.type == objType::end && endPlaced){
        input.mouseReleased = false;
        return;
    }
    objects.push_back(cursorObj);
    cursorObj.actualPos.x = roundToSnap(objects.back().shape.getPosition().x +  (float)input.screenPos.x);
    cursorObj.actualPos.y = roundToSnap(objects.back().shape.getPosition().y +  (float)input.screenPos.y);
    input.mouseReleased = false;
    if(cursorObj.type == objType::player)
        playerPLaced = true;
    else if(cursorObj.type == objType::end)
        endPlaced = true;
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
    window.draw(getCursorObj(window).shape);
    window.display();
}

int main(){
    for(int i = 0; i < typesAmount; i++)
        sizes[i] = defaultSizes[i];
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "map maker");
    sf::Event event;
    while(window.isOpen()){
        manageEvents(event, window);
        display(window);
        lockFrames();
        if(input.mouseReleased)
            placeObj(window);
    }

    return 0; 
}