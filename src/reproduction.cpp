#include "reproduction.h"


void reproduce(std::vector<Brain*> agents, std::vector<float> score_agent, int NB_BRAIN, float EVOLUTION, int bestKeep, int worstKill){

    // Fool proof
    if (agents.size() != NB_BRAIN || score_agent.size() != NB_BRAIN) {
        std::cerr << "Error: agents and score_agent vectors must have size NB_BRAIN." << std::endl;
        return;
    }
    if (bestKeep > 100 || worstKill > 100) {
        std::cerr << "Error: bestKeep and worstKill must be purcentage" << std::endl;
        return;
    }


    // Creation of the new brain vector
    std::vector<Brain*> new_agents(NB_BRAIN, nullptr);

    // Calculate number of best and worst agents to keep/kill
    int num_best_to_keep = (bestKeep * NB_BRAIN) / 100;
    int num_worst_to_kill = (worstKill * NB_BRAIN) / 100;

    // Sort agents by score
    reverse_sorting_brain(agents, score_agent, 0, NB_BRAIN - 1);

    std::cout << "THIS IS NOT FINISHED YET" << std::endl;

}

void partition(std::vector<Brain*> tab, std::vector<float> score_tab, int low, int high, int* pivotIndex) {
    float pivot = score_tab[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (score_tab[j] >= pivot) {
            i++;
            std::swap(score_tab[i], score_tab[j]);
            std::swap(tab[i], tab[j]);
        }
    }
    std::swap(score_tab[i + 1], score_tab[high]);
    std::swap(tab[i + 1], tab[high]);
    *pivotIndex = i + 1;
}

void reverse_sorting_brain(std::vector<Brain*> tab, std::vector<float> score_tab, int low, int high){
    // Quick sort
    if (low < high) {
        int pivotIndex;
        partition(tab, score_tab, low, high, &pivotIndex);
        reverse_sorting_brain(tab, score_tab, low, pivotIndex - 1);
        reverse_sorting_brain(tab, score_tab, pivotIndex + 1, high);
    }
    
}




