/*
    displayservice_headless.cpp

    Description:
        This is a dummy file of displayservice.cpp? It is used to declare functions
        that actually do nothing here only to be able to run the program 
        
    Author:
        R. Benichou
        A. Spadone
*/

#include "displayservice.hpp"
#include <memory>
#include <iostream>

class DisplayServiceHeadless : public DisplayService {
public:
    DisplayServiceHeadless() = default;
    ~DisplayServiceHeadless() override = default;

    void init(bool headless, bool* runningptr, bool* clean_exitptr, int agentPartitionsSize, bool mc) override {
        // headless mode: do nothing, but we can set runningptr to true
        if (runningptr_local_owned) {
            // nothing
        }
        this->runningptr = runningptr;
        this->clean_exitptr = clean_exitptr;
        if (runningptr) *runningptr = true;

        logm("DisplayServiceHeadless initialized (no graphics).");
    }

    void render(std::vector<int>* groups_avail,
                std::vector<std::vector<Creature*>>* agentPartitions,
                float fps, int agent_size, int generation, int sous_sim_started, float acu,
                float simu_time, float evolution, sf::Vector2f start, const std::vector<sf::Vector2f>& goals, bool mc) override
    {
         // Nothing to do here :)
    }
    
    void draw_creature(Creature* creature) override {
    
    }

private:
    bool* runningptr = nullptr;
    bool* clean_exitptr = nullptr;
    bool runningptr_local_owned = false;
};

std::unique_ptr<DisplayService> CreateDisplayService() {
    return std::make_unique<DisplayServiceHeadless>();
}