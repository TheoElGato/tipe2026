#include "physics.h"

Point::Point(float x, float y, float mass)
{
    this->position = {x, y};
    this->mass = mass;
}