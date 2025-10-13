#include "reproduction.h"


std::vector<Brain>* reproduce(std::vector<Brain>& agents, std::vector<float>& score_agent, int NB_BRAIN, float EVOLUTION, int bestKeep, int selectionPol){

    // Fool proof
    if (agents.size() != NB_BRAIN || score_agent.size() != NB_BRAIN) {
        std::cerr << "Error: agents and score_agent vectors must have size NB_BRAIN." << std::endl;
        return;
    }
    if (bestKeep > 100 || selectionPol > 100 || bestKeep < 0 || selectionPol < 0) {
        std::cerr << "Error: bestKeep and selectionPol must be purcentage" << std::endl;
        return;
    }


    // Creation of the new brain vector
    std::vector<Brain> new_agents;

    // Calculate number of best and worst agents to keep/kill
    int num_best_to_keep = (bestKeep * NB_BRAIN) / 100;
    int num_selctionable = (selectionPol * NB_BRAIN) / 100;

    // Sort agents by score
    reverse_sorting_brain(agents, score_agent, 0, NB_BRAIN - 1);

    // Keep best agent
    for(int i = 0; i < num_best_to_keep; i+=1){
        new_agents.emplace_back(agents[i]);
    }

    // Normalise score
    float total = 0;
    for(int i = 0; i <= num_selctionable; i+=1){
        total += score_agent[i];
    }
    for(int i = 0; i <= num_selctionable; i+=1){
        score_agent[i] /= total;
    }

    // Generation of the new agent for the next gen
    for(int i = num_best_to_keep; i < NB_BRAIN; i+=1){
        float ran = ((float) rand())/ RAND_MAX;
        float temp = score_agent[0];
        int j = 0;

        while(temp < ran){
            j += 1;
            temp += score_agent[j];
        }

        new_agents.emplace_back(agents[j]);
        new_agents.back().mutate(EVOLUTION);

    }

    while (!agents.empty())
    {
        agents.pop_back();
    }

    return &new_agents;
}


void reverse_sorting_brain(std::vector<Brain>& tab, std::vector<float>& score_tab, int low, int high){

    if (low >= high) {
        return;
    }

    float pivot = score_tab[high];
    int i = low;
    float sc_temp;
    Brain* b_temp;

    for (int j = low; j < high; j+=1){
        if(score_tab[j] >= pivot){

            // Swap scores
            sc_temp = score_tab[i];
            score_tab[i] = score_tab[j];
            score_tab[j] = sc_temp;

            // Swap brains
            b_temp = &tab[i];
            tab[i] = tab[j];
            tab[j] = *b_temp;

            i += 1;
        }

    }

    // Place pivot in the correct position
    sc_temp = score_tab[i];
    score_tab[i] = score_tab[high];
    score_tab[high] = sc_temp;

    b_temp = &tab[i];
    tab[i] = tab[high];
    tab[high] = *b_temp;

    reverse_sorting_brain(tab, score_tab, low, i-1);
    reverse_sorting_brain(tab, score_tab, i + 1, high);
}



