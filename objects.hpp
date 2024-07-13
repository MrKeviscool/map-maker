#include <SFML/Graphics.hpp>

#define FLOOR 0
#define PLAYER 1
#define ENEMEY 2
#define END 3

const sf::Vector2f defaultfloorsize(300, 30), defaultplayersize(50, 70), defaultenemeysize(50, 70), defaultendsize(100, 100);

class mapobj{
    public:
        sf::RectangleShape shape;
        sf::Vector2f actualpos;
        int type;
        mapobj(sf::RectangleShape shape, sf::Vector2f actualpos, int type) : shape(shape), actualpos(actualpos), type(type){}
};

sf::Vector2f floorsize(defaultfloorsize);
sf::Vector2f playersize(defaultplayersize);
sf::Vector2f enemeysize(defaultenemeysize);
sf::Vector2f endsize(defaultendsize);