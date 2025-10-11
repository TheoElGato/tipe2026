#include "reproduction.h"


void reproduce(std::vector<Brain*>& agents, std::vector<float>& score_agent, int NB_BRAIN, float EVOLUTION, int bestKeep, int worstKill){

    // Fool proof
    if (agents.size() != NB_BRAIN || score_agent.size() != NB_BRAIN) {
        std::cerr << "Error: agents and score_agent vectors must have size NB_BRAIN." << std::endl;
        return;
    }
    if (bestKeep > 100 || worstKill > 100 || bestKeep < 0 || worstKill < 0) {
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


void reverse_sorting_brain(std::vector<Brain*>* tab, std::vector<float>* score_tab, int low, int high){

    if (low >= high) {
        return;
    }

    float pivot = (*score_tab)[high];
    int i = low;
    float sc_temp;
    Brain* b_temp;

    for (int j = low; j < high; j+=1){
        if((*score_tab)[j] >= pivot){

            // Swap scores
            sc_temp = (*score_tab)[i];
            (*score_tab)[i] = (*score_tab)[j];
            (*score_tab)[j] = sc_temp;

            // Swap brains
            b_temp = (*tab)[i];
            (*tab)[i] = (*tab)[j];
            (*tab)[j] = b_temp;

            i += 1;
        }

    }
    
    // Place pivot in the correct position
    sc_temp = (*score_tab)[i];
    (*score_tab)[i] = (*score_tab)[high];
    (*score_tab)[high] = sc_temp;

    b_temp = (*tab)[i];
    (*tab)[i] = (*tab)[high];
    (*tab)[high] = b_temp;

    reverse_sorting_brain(tab, score_tab, low, i-1);
    reverse_sorting_brain(tab, score_tab, i + 1, high);
}



