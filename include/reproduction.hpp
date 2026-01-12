/*
    reproduction.hpp

    Description:
        Header file for reproduction.cpp
        
    Author:
        R. Benichou
        A. Spadone
*/

#ifndef REPRODUCTION_H
#define REPRODUCTION_H

#include <thread>
#include <mutex>
#include <vector>
#include "physics.hpp"
#include "creature.hpp"


void reproduce(std::vector<Brain>* agents, std::vector<float> score_agent, int NB_BRAIN, float EVOLUTION, int bestKeep, int selectionPol);
void reverse_sorting_brain(std::vector<Brain>* tab, std::vector<float>* score_tab, int low, int high);

#endif // REPRODUCTION_H