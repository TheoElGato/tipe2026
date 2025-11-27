#include "physics.hpp"

Point::Point(float x, float y, float mass)
{
    this->position = {x, y};
    this->mass = mass;
    this->velocity = {0.0f, 0.0f};
    this->fixed = false;

}

Link::Link(int a, int b, float restLength)
{
    this->pointA = a;
    this->pointB = b;
    this->restLength = restLength;
}

Spring::Spring(int a, int b, float restLength, float springStrength)
{
    this->pointA = a;
    this->pointB = b;
    this->restLength = restLength;
    this->springStrength = springStrength;
}

PhysicsWorker::PhysicsWorker()
{

}

float PhysicsWorker::distance(Point a, Point b)
{
    float dx = b.position.x - a.position.x;
    float dy = b.position.y - a.position.y;
    return sqrt(dx * dx + dy * dy);
}

void PhysicsWorker::PBD(std::vector<Point>* objects, std::vector<Link> links, std::vector<Spring> springs, int numsubstep, float dt)
{
    float h = dt / numsubstep;
    int n = objects->size();
    std::vector<sf::Vector2f> xprev(n);
    std::vector<sf::Vector2f> fext(n);

    for (int k = 0; k < numsubstep; k++) {
        
        for (int i = 0; i < n; i++) {
            xprev[i] = (*objects)[i].position;
            fext[i] = {0, 0};
        }

        // Manage springs here
        for (const auto& spring : springs) {
            
            sf::Vector2f force = springForce((*objects)[spring.pointA], (*objects)[spring.pointB], spring.springStrength, spring.restLength);

            // Trouver les indexes des points
            int idxA = spring.pointA;
            int idxB = spring.pointB;

            if (idxA != -1) fext[idxA] += force;
            if (idxB != -1) fext[idxB] -= force;
        }

        

        // Save previous positions
        // Compute new positions from speed and Euler method
        for (int i = 0; i < n; i++) {
            (*objects)[i].velocity += h * fext[i] / (*objects)[i].mass;
            if (!(*objects)[i].fixed) {
                (*objects)[i].velocity.x *= 0.98;
                (*objects)[i].velocity.y *= 0.98;
            } else {
                (*objects)[i].velocity.x *= 0.12; 
                (*objects)[i].velocity.y *= 0.12; 
            }

            (*objects)[i].position += (*objects)[i].velocity * h;

        }
        
        // SolvePosition
        for (const auto& link : links) {
            Point* a = &(*objects)[link.pointA];
            Point* b = &(*objects)[link.pointB];
            float l = link.restLength;


            sf::Vector2f delta = a->position - b->position;
            float delta_len = sqrt(delta.x * delta.x + delta.y * delta.y);
            float diff = (delta_len - l) / delta_len;

            // If points are fixed, we don't move them
            if (a->fixed && b->fixed) {
                // @TheoElGato, is this a normal ?
                a->position -= delta * diff * 0.5f;
                b->position += delta * diff * 0.5f;
            } else if (!a->fixed && b->fixed) {
                a->position -= delta * diff;
            } else if (a->fixed && !b->fixed) {
                b->position += delta * diff;
            } else if (!a->fixed && !b->fixed) {
                a->position -= delta * diff * 0.5f;
                b->position += delta * diff * 0.5f;
            }
        }

        // Update speeds with old pos and new ones
        for (int i = 0; i < n; i++) {
            (*objects)[i].velocity = ((*objects)[i].position - xprev[i]) / h;
        }

    }

}

sf::Vector2f PhysicsWorker::springForce(Point a, Point b, float springConstant, float restLength)
{
    // Calculates the force of a spring between two points
    // and returns the force vector from a to b (or from b to a)
        
    // /!\ WARNING: if there are performance issues, these checks can be removed.

    float l = distance(a, b);
    sf::Vector2f n;
    if (l != 0) {
        n = (b.position - a.position) / l;
    } else {
        n = {0, 0};
    }
    // If the points are in the same place, there is no normal vector

    if (l - restLength == 0) {
        // The spring is at its unloaded length, so there is no force
        // You cannot divide by 0
        return {0, 0};
    }

    return springConstant * (l - restLength) * n;
}