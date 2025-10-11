#ifndef REPRODUCTION_H
#define REPRODUCTION_H

#include <thread>
#include <mutex>
#include <vector>
#include "physics.h"
#include "creature.h"


void reproduce(std::vector<Brain*> agents, std::vector<float*> score_agent, int NB_BRAIN, float EVOLUTION, int bestKeep, int worstKill);
void reverse_sorting_brain(std::vector<Brain*>& tab, std::vector<float>& score_tab, int low, int high);

#endif // REPRODUCTION_H