#ifndef THREADSMG_H
#define THREADSMG_H

#include <thread>
#include <mutex>
#include <vector>
#include "physics.h"
#include "creature.h"

void handleThread(PhysicsWorker &physics, std::vector<Creature *> agents, sf::Vector2f start, sf::Vector2f objectif, std::vector<Brain*> brains,float* dt, float time,float brainAcc);

#endif // THREADSMG_H