#ifndef PHYSICS_H
#define PHYSICS_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>

class Point {
public:
    Point(float x, float y, float mass);
    sf::Vector2f position;
    float mass;
    sf::Vector2f velocity;
    bool fixed;

    int id = -1;

    void setId(int id) {
        this->id = id;
    }
};

class Link {
public:
    Link(Point &a, Point &b, float restLength);
    Point* pointA;
    Point* pointB;
    float restLength;
};

class Spring {
public:
    Spring(Point& a, Point& b, float restLength, float springStrength);
    Point* pointA;
    Point* pointB;
    float restLength;
    float springStrength;
    
};

class PhysicsWorker {
public:
    PhysicsWorker();

    sf::Vector2f springForce(Point a, Point b, float springConstant, float restLength);
    void PBD(std::vector<Point>* objects, std::vector<Link> links, std::vector<Spring> springs, int numsubstep, float dt);

private:
    float distance(Point a, Point b);
};

#endif // PHYSICS_H