#include <SFML/Graphics.hpp>

#define FLOOR 0
#define PLAYER 1
#define ENEMEY 2
#define END 3
#define SPIKES 4

const sf::Vector2f defaultfloorsize(300, 30), defaultplayersize(60, 90), defaultenemeysize(60, 90), defaultendsize(90, 90), defaultspikesize(150, 30);

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
sf::Vector2f spikesize(defaultspikesize);