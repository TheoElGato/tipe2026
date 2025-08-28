#include "physics.h"

Point::Point(float x, float y, float mass)
{
    this->position = {x, y};
    this->mass = mass;
    this->velocity = {0.0f, 0.0f};
    this->fixed = false;

}

Link::Link(Point &a, Point &b, float restLength)
{
    this->pointA = &a;
    this->pointB = &b;
    this->restLength = restLength;
}

Spring::Spring(Point &a, Point &b, float restLength, float springStrength)
{
    this->pointA = &a;
    this->pointB = &b;
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

    for (int i = 0; i < n; i++) {
        (*objects)[i].setId(i);
    }


    for (int k = 0; k < numsubstep; k++) {
        
        for (int i = 0; i < n; i++) {
            xprev[i] = (*objects)[i].position;
            fext[i] = {0, 0};
        }

        

        //gestion des ressorts possibles
        for (const auto& spring : springs) {
            
            sf::Vector2f force = springForce(*spring.pointA, *spring.pointB, spring.springStrength, spring.restLength);

            // Trouver les indexes des points
            int idxA = spring.pointA->id;
            int idxB = spring.pointB->id;

            if (idxA != -1) fext[idxA] += force;
            if (idxB != -1) fext[idxB] -= force;
        }

        

        //sauvegarde des positions précédentes
        //calculs des nouvelles positions en fonction de la vitesse à l'aide de la méthode d'Euler
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
        
        // gestion des liens solides (SolvePosition)
        for (const auto& link : links) {
            Point* a = link.pointA;
            Point* b = link.pointB;
            float l = link.restLength;


            sf::Vector2f delta = a->position - b->position;
            float delta_len = delta.length();
            float diff = (delta_len - l) / delta_len;

            // si les points sont fixes, on ne les bouge pas
            if (a->fixed && b->fixed) {
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


        // mise à jour des vitesses en fonction des positions précédentes et des nouvelles positions
        for (int i = 0; i < n; i++) {
            (*objects)[i].velocity = ((*objects)[i].position - xprev[i]) / h;
        }

    }

}


sf::Vector2f PhysicsWorker::springForce(Point a, Point b, float springConstant, float restLength)
{
    //calcule la force d'un ressort entre deux points
    //et renvoie le vecteur force de a vers b (ou - de b vers a)
    float l = distance(a, b);
    sf::Vector2f n;
    if (l != 0) {
        n = (b.position - a.position) / l;
    } else {
        n = {0, 0};
    }
    //si les points sont au même endroit, il n'existe pas de vecteur normal

    if (l - restLength == 0) {
        // le ressort est à sa longueur à vide, donc pas de force
        // on ne peut pas diviser par 0
        return {0, 0};
    }

    return springConstant * (l - restLength) * n;
}
