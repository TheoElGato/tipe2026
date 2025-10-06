#ifndef REPRODUCTION_H
#define REPRODUCTION_H

#include <thread>
#include <mutex>
#include <vector>
#include "physics.h"
#include "creature.h"


void reproduce(std::vector<Creature>& agents, std::vector<float>& score_agent, int NB_BRAIN, float EVOLUTION);



#endif // REPRODUCTION_H