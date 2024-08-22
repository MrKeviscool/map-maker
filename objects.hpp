#include <vector>
#include <SFML/Graphics.hpp>

const int typesAmount = 7, WIDTH=1920, HEIGHT=1080;
inline int roundToSnap(float num);

enum class objType{
    floor,
    player,
    chaseEnemy,
    end,
    spikes,
    shootEnemy,
    chaseShootEnemy
};

const sf::Vector2f defaultSizes[] = {
    sf::Vector2f(300, 30), //floor
    sf::Vector2f(60, 90), //player
    sf::Vector2f(60, 90), //chaseEnemy
    sf::Vector2f(90, 90), //end
    sf::Vector2f(150, 30), //spikes
    sf::Vector2f(60, 90), //shootEnemy
    sf::Vector2f(60, 90), //chaseShootEnemy
};


const sf::Color colors[] = {
    sf::Color::White, //floor
    sf::Color::Blue, //player
    sf::Color::Red, //chaseEnemy
    sf::Color::Green, //end
    sf::Color::Magenta, //spikes
    sf::Color(255, 100, 0), //shootEnemy
    sf::Color::Yellow, //shootChaseEnemy
};

const std::vector<std::vector<objType>> bindings = {
    {objType::floor}, //1 key
    {objType::player}, //2
    {objType::chaseEnemy, objType::spikes, objType::shootEnemy, objType::chaseShootEnemy}, //3
    {objType::end}, //4
};

sf::Vector2f sizes[typesAmount];

class Object{
    public:
    sf::RectangleShape shape;
    const objType type;
    sf::Vector2f actualPos;
    Object();
    Object(sf::Vector2f pos, objType type) : type(type){
        shape = sf::RectangleShape(sf::Vector2f(roundToSnap(sizes[int(type)].x), roundToSnap(sizes[int(type)].y)));
        shape.setPosition(pos);
        shape.setFillColor(colors[int(type)]);
    }
    ~Object(){}
};

// struct Input{
//     int numDown = 0;
//     int timesPressed = 1;
//     bool mouseReleased = false;
//     sf::Vector2i screenPos = sf::Vector2i(0, 0);
// };

namespace Input{
    int numDown = 0;
    int timesPressed = 1;
    bool mouseReleased = false;
    sf::Vector2i screenPos = sf::Vector2i(0, 0);
}

struct Text{
    sf::Font font;
    bool loadedFont;
    // vector<sf::Text> texts = {sf::Text("Saved File...", font)};
    sf::Text savedText = sf::Text("File Saved...", font);
    sf::Text helpText = sf::Text("W, S, A, D - move\nI, K, J, L - re-size\n1-4 - select-type\nR - rotate floor\nF - save to file\nX - clear\nhint: there can be\nmultiple types on one key", font);
    Text(){
        if(!font.loadFromFile("font.ttf")){
            loadedFont = false;
            std::cout << "Error loading font.ttf, no text will be displayed\n";
        }
        else{
            loadedFont = true;
            savedText.setFillColor(sf::Color(255, 255, 255, 0));
            savedText.setPosition(0, 0);
            savedText.setCharacterSize(40);
            helpText.setCharacterSize(30);
            helpText.setPosition(5, HEIGHT-(helpText.getCharacterSize()*9));
        }
    }
};