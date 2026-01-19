/*
    threadsmg.hpp

    Description:
        Header file for threadsmg.cpp

    Author:
        R. Benichou
        A. Spadone
*/

#ifndef THREADSMG_H
#define THREADSMG_H

#include <thread>
#include <mutex>
#include <vector>
#include "physics.hpp"
#include "creature.hpp"

float distance(sf::Vector2f a, sf::Vector2f b);
void handleThread(PhysicsWorker* physics, std::vector<Creature *> agents, sf::Vector2f start,
                  std::vector<sf::Vector2f> objectives, std::vector<Brain*> brains, int* state,
                  std::vector<float>* scores, float* dt, float time,float brainAcc);

#endif // THREADSMG_H
