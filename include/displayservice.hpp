#ifndef DISPLAYSERVICE_H
#define DISPLAYSERVICE_H


// DisplayService interface - NO SFML Graphics/Window includes here.
// displayservice.ccp and displayservice_headless.cpp will include this file.

#include <SFML/System/Vector2.hpp>
#include <vector>
#include <memory>
#include "filemanager.hpp"

class Creature; // forward

class DisplayService {
public:
    virtual ~DisplayService() = default;
    virtual void init(bool headless, bool* runningptr, bool* clean_exitptr, int agentPartitionsSize, bool mc) = 0;
    virtual void draw_creature(Creature* creature) = 0;
    virtual void render(std::vector<int>* groups_avail,
                        std::vector<std::vector<Creature*>>* agentPartitions,
                        float fps, int agent_size, int generation, int sous_sim_started, float acu,
                        float simu_time, float evolution, sf::Vector2f start, const std::vector<sf::Vector2f>& goals, bool mc) = 0;
};

// Each implementation file (graphical / headless) will provide its own CreateDisplayService()
// and CMake will ensure only the correct one is compiled into each executable.
std::unique_ptr<DisplayService> CreateDisplayService();

#endif // DISPLAYSERVICE_H