#ifndef PHYSICS_H
#define PHYSICS_H

#include <SFML/Graphics.hpp>

class Point {
public:
    Point(float x, float y, float mass);
    sf::Vector2f position;
    float mass;
};

#endif // PHYSICS_H