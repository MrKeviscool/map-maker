#include <SFML/Graphics.hpp>

/*
#define FLOOR 0
#define PLAYER 1
#define ENEMEY 2
#define END 3
*/

class mapobj{
    public:
        sf::RectangleShape shape;
        sf::Vector2f actualpos;
        int type;
        mapobj(sf::RectangleShape shape, sf::Vector2f actualpos, int type) : shape(shape), actualpos(actualpos), type(type){}
};