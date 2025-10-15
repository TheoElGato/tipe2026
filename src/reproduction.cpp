#include "reproduction.h"
#include <random>

void reproduce(std::vector<Brain>* brain_agents, std::vector<float> score_agent, 
               int NB_BRAIN, float EVOLUTION, int bestKeep, int selectionPol) {

    // Validation
    if (brain_agents->size() != NB_BRAIN || score_agent.size() != NB_BRAIN) {
        std::cerr << "Error: brain_agents and score_agent must have size NB_BRAIN.\n";
        exit(EXIT_FAILURE);
    }
    if (bestKeep < 0 || bestKeep > 100 || selectionPol < 0 || selectionPol > 100) {
        std::cerr << "Error: bestKeep and selectionPol must be percentages.\n";
        exit(EXIT_FAILURE);
    }

    // Sort by score descending
    reverse_sorting_brain(brain_agents, &score_agent, 0, NB_BRAIN - 1);

    // Compute how many to keep and to select
    int num_best_to_keep = (bestKeep * NB_BRAIN) / 100;
    int num_selectable = (selectionPol * NB_BRAIN) / 100;
    if (num_selectable == 0) num_selectable = 1; // Avoid divide by zero

    // Normalize scores
    float total = 0.f;
    for (int i = 0; i < num_selectable; ++i){
        total += score_agent[i];
    }

    if (total == 0.f) total = 1.f; // Prevent division by zero

    for (int i = 0; i < num_selectable; ++i){
        score_agent[i] /= total;
    }

    // Random number setup
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    // Generate new brains
    std::vector<Brain> new_agents;
    new_agents.reserve(NB_BRAIN - num_best_to_keep);

    for (int i = num_best_to_keep; i < NB_BRAIN; ++i) {
        float ran = dist(gen);  // [0,1[
        float cumul = score_agent[0];
        int j = 0;
        while (cumul < ran && j < num_selectable - 1) {
            j+=1;
            cumul += score_agent[j];
        }
        // Add mutated copy of selected brain
        new_agents.emplace_back((*brain_agents)[j].copy());
        new_agents.back().mutate(EVOLUTION);
    }

    // Replace worst brains
    for (int i = num_best_to_keep; i < NB_BRAIN; ++i){
        (*brain_agents)[i] = new_agents[i - num_best_to_keep];
    }
    // Cleanup
    new_agents.clear();
}



void reverse_sorting_brain(std::vector<Brain>* tab, std::vector<float>* score_tab, int low, int high){

    if (low >= high) {
        return;
    }

    float pivot = (*score_tab)[high];
    int i = low;
    float sc_temp;

    for (int j = low; j < high; j+=1){
        if((*score_tab)[j] >= pivot){

            // Swap scores
            sc_temp = (*score_tab)[i];
            (*score_tab)[i] = (*score_tab)[j];
            (*score_tab)[j] = sc_temp;

            // Swap brains
            Brain b_temp = (*tab)[i];
            (*tab)[i] = (*tab)[j];
            (*tab)[j] = b_temp;

            i += 1;
        }

    }

    // Place pivot in the correct position
    sc_temp = (*score_tab)[i];
    (*score_tab)[i] = (*score_tab)[high];
    (*score_tab)[high] = sc_temp;

    Brain b_temp = (*tab)[i];
    (*tab)[i] = (*tab)[high];
    (*tab)[high] = b_temp;

    reverse_sorting_brain(tab, score_tab, low, i-1);
    reverse_sorting_brain(tab, score_tab, i + 1, high);
}



