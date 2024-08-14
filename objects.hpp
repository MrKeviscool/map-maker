#include <SFML/Graphics.hpp>
#include <array>

enum class objType{
    floor,
    player,
    chaseEnemy,
    end,
    spikes,
    shootEnemy,
    chaseShootEnemy
};

const int typesAmount = 7;


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


sf::Vector2f sizes[typesAmount];

class Object{
    public:
    sf::RectangleShape shape;
    const objType type;
    Object(sf::Vector2f pos, objType type) : type(type){
        shape = sf::RectangleShape(sizes[int(type)]);
        shape.setPosition(pos);
        shape.setFillColor(colors[int(type)]);
    }

};

// class Object{
//     public:
//     sf::RectangleShape shape;
//     Object(){};
//     Object(const sf::Vector2f &size, const sf::Vector2f &pos, const sf::Color &col){
//         shape = sf::RectangleShape(size);
//         shape.setPosition(pos);
//         shape.setFillColor(col);
//     };
// };

// class Floor : public Object{
//     private:
//     static const sf::Vector2f defaultSize;
//     static const sf::Color defaultColor;
//     public:
//     static const objType type;
//     Floor(const sf::Vector2f pos) : Object(defaultSize, pos, defaultColor){};
// };

// const sf::Vector2f Floor::defaultSize(10, 10);
// const sf::Color Floor::defaultColor(sf::Color::White);
// const objType Floor::type = objType::floor;