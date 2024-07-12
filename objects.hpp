#include <SFML/Graphics.hpp>

#define FLOOR 0
#define PLAYER 1
#define ENEMEY 2
#define END 3


class mapobj{
    public:
        sf::RectangleShape shape;
        sf::Vector2f actualpos;
        int type;
        mapobj(sf::RectangleShape shape, sf::Vector2f actualpos, int type) : shape(shape), actualpos(actualpos), type(type){}
};

sf::Vector2f floorsize(300, 30);
sf::Vector2f playersize(50, 70);
sf::Vector2f enemeysize(50, 70);
sf::Vector2f endsize(100, 100);