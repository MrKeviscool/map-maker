#include <SFML/Graphics.hpp>
#include <array>

const int typesAmount = 7;
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
};

struct Input{
    int numDown = 0;
    int timesPressed = 0;
    bool mouseReleased = false;
    sf::Vector2i screenPos = sf::Vector2i(0, 0);
};