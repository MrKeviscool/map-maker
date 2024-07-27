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
const float MOVESPEED = (WIDTH/MAXFPS)/1.5, RESIZESPEED = 5, SNAPSIZE = 30;

void inputevents(sf::Event *event, bool freemem), display(), lockFrames(), placeObject(), moveScreen(), writeToFile(), resizeObj(), rotateNumsinVec2f(sf::Vector2f *vec), rotateFloor(), undo(), redo(), resetMap(), rmObj();
float roundToX(float num, float roundto);
sf::RectangleShape getCursorShape();

sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "map maker", sf::Style::Fullscreen);

sf::Font font;
int amountoftexts = 0;

bool drawText = false;
sf::Text *texts;
sf::Event event;
int activeobj = FLOOR;

vector<mapobj> objects;
vector<mapobj> redobuffer;
sf::Vector2f screenpos(0, 0);

bool rotatefloors = false;
bool playerplaced = false, endplaced=false;
float xmovesnapbuffer = 0, ymovesnapbuffer = 0;

int main(){

    if(font.loadFromFile("font.ttf")){
        drawText = true;
        const int charsize = 10;
        static sf::Text txt[] = {
            sf::Text("W, S, A, D - move", font),
            sf::Text("I, K, J, L - re-size", font),
            sf::Text("1-4 - select-type", font),
            sf::Text("R - rotate floor", font),
            sf::Text("F - save to file", font),
            sf::Text("X - clear", font),
        };
        const int upfrombot = 30;
        amountoftexts = sizeof(txt)/sizeof(sf::Text);
        for(int i = 0; i < amountoftexts; i++){
            txt[i].setPosition(1680, 1070-((amountoftexts-i)*upfrombot));
        }
        texts = txt;
    }
    else{
        cout << "cant load font file\n";
    }

    while(window.isOpen()){
        inputevents(&event, false);
        moveScreen();
        display();
        lockFrames();
    }
    // writeToFile();
    inputevents(nullptr, true);
    return 0;
}

void inputevents(sf::Event *event, bool freemem){
    
    static void (*frotate)() = []() -> void{rotatefloors = !rotatefloors;};

    static void (*changenemy)() = []() -> void{
        if(activeobj==ENEMEY){
            activeobj = SPIKES;
        }
        else if(activeobj == SPIKES){
            activeobj == ENEMEY;
        }
        else{
            activeobj = ENEMEY;
        }
    };
    
    static void (*funcptrs[])() = {frotate, undo, redo, resetMap, writeToFile, changenemy};
    static sf::Keyboard::Key keymap[] = {sf::Keyboard::Key::R, sf::Keyboard::Key::U, sf::Keyboard::Key::Y, sf::Keyboard::Key::X, sf::Keyboard::F, sf::Keyboard::Num3};
    static bool leftMouseDownLastFrame = false, rightMouseDownLastFrame = false;
    static bool *downlastframe = (bool *)calloc(sizeof(keymap) / sizeof(sf::Keyboard::Key), sizeof(bool));
    
    if(freemem){
        free(downlastframe);
        return;
    }

    while(window.pollEvent(*event)){
        if(event->type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
            return;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
            activeobj = FLOOR;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
            activeobj = PLAYER;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
            activeobj = END;  
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            leftMouseDownLastFrame = true;
        }
        else if(leftMouseDownLastFrame){
            leftMouseDownLastFrame = false;
            placeObject();
        }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
            rightMouseDownLastFrame = true;
        }
        else if(rightMouseDownLastFrame){
            rightMouseDownLastFrame = false;
            rmObj();
        }
    }

    for(int i = 0; i < sizeof(keymap) / sizeof(sf::Keyboard::Key); i++){
        if(sf::Keyboard::isKeyPressed(keymap[i])){
            downlastframe[i] = true;
        }
        else if(downlastframe[i]){
            downlastframe[i] = false;
            (*funcptrs[i])();
        }
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        xmovesnapbuffer += MOVESPEED;
        if(xmovesnapbuffer > SNAPSIZE){
            screenpos.x += roundToX(xmovesnapbuffer, SNAPSIZE);
            xmovesnapbuffer = 0;
        }
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        xmovesnapbuffer += MOVESPEED;
        if(xmovesnapbuffer > SNAPSIZE){
            screenpos.x -= roundToX(xmovesnapbuffer, SNAPSIZE);
            xmovesnapbuffer = 0;
        }

    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        ymovesnapbuffer += MOVESPEED;
        if(ymovesnapbuffer > SNAPSIZE){
            screenpos.y -= roundToX(ymovesnapbuffer, SNAPSIZE);
            ymovesnapbuffer = 0;
        }

    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        ymovesnapbuffer += MOVESPEED;
        if(ymovesnapbuffer > SNAPSIZE){
            screenpos.y += roundToX(ymovesnapbuffer, SNAPSIZE);
            ymovesnapbuffer = 0;
        }

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
    if(drawText){
        for(int i = 0; i < amountoftexts; i++){
            window.draw(texts[i]);
        }
    }
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
        case SPIKES:
            rectbuffer.setSize(spikesize);
            rectbuffer.setFillColor(sf::Color::Magenta);
            break;
        default:
            rectbuffer.setSize(sf::Vector2f(300, 30));
    };
    rectbuffer.setPosition(
        roundToX(sf::Mouse::getPosition().x, SNAPSIZE), 
        roundToX(sf::Mouse::getPosition().y, SNAPSIZE));
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
    objects.push_back(mapobj(shape, shape.getPosition() + sf::Vector2f(roundToX(screenpos.x, SNAPSIZE), roundToX(screenpos.y, SNAPSIZE)), activeobj));
    if(activeobj == PLAYER)
        playerplaced = true;
    else if(activeobj == END)
        endplaced = true;
}

float roundToX(float num, float roundto){
    return round(num/roundto)*roundto;
}

void writeToFile(){
    bool writeNewLine = true;
    fstream file("maps", ios::in);
    if(!file.is_open()){
        writeNewLine = false;
    }
    file.close();
    file = fstream("maps", ios::app);
    if(writeNewLine){
        file << endl;
    }
    file << objects.size() << endl;
    for(int i = 0; i < objects.size(); i++){
        file << objects[i].type << " ";
        file << objects[i].actualpos.x << " " << objects[i].actualpos.y << " ";
        file << objects[i].shape.getSize().x << " " << objects[i].shape.getSize().y << " ";
        if(i < objects.size()-1)
            file << endl;
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
            break;
        case END:
            obj = &endsize;
            break;
        case SPIKES:
            obj = &spikesize;
            break;
        default:
            obj = &floorsize;
    };
    movebuff += RESIZESPEED;
    if(movebuff < SNAPSIZE){
        return;
    }
    if(obj == &floorsize && rotatefloors){
        rotateNumsinVec2f(obj);
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
    if(obj == &floorsize && rotatefloors){
        rotateNumsinVec2f(obj);
    }
}

void rotateNumsinVec2f(sf::Vector2f *vec){
    float tmp = vec->x;
    vec->x = vec->y;
    vec->y = tmp;
}

void undo(){
    if(objects.size() <= 0){
        return;
    }
    if(objects.back().type == PLAYER){
        playerplaced = false;
    }
    if(objects.back().type == END){
        endplaced = false;
    }
    redobuffer.push_back(objects.back());
    objects.pop_back();
}
void redo(){
    if(redobuffer.size() <= 0){
        return;
    }
    if(redobuffer.back().type == PLAYER){
        if(playerplaced){
            redobuffer.pop_back();
            return;
        }
        playerplaced = true;
        activeobj = FLOOR;
    }
    if(redobuffer.back().type == END){
        if(endplaced){
            redobuffer.pop_back();
            return;
        }
        endplaced = true;
        activeobj = FLOOR;
    }
    
    objects.push_back(redobuffer.back());
    redobuffer.pop_back();
}

void resetMap(){
    objects.clear();
    redobuffer.clear();
    rotatefloors = false;
    playerplaced = false;
    endplaced = false;
    floorsize = defaultfloorsize;
    playersize = defaultplayersize;
    enemeysize = defaultenemeysize;
    endsize = defaultendsize;
    spikesize = defaultspikesize;
    screenpos = sf::Vector2f(0, 0);
}

void rmObj(){
    sf::Vector2f mouseshape = getCursorShape().getPosition();
    for(int i = 0; i < objects.size(); i++){
        if(objects[i].shape.getPosition() == mouseshape){
            if(objects[i].type == PLAYER){
                playerplaced = false;
            }
            redobuffer.push_back(objects[i]);
            objects.erase(objects.begin() + i);
        }
    }
}
