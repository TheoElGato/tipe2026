#ifndef THREADSMG_H
#define THREADSMG_H

#include <thread>
#include <mutex>
#include <vector>
#include "physics.h"
#include "creature.h"

void handleThread(PhysicsWorker &physics, std::vector<Creature *> agents, std::vector<float> objectives, float dt, float time)

#endif // THREADSMG_H