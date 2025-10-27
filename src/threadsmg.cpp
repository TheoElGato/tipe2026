#include "threadsmg.h"
#include <iostream>

void handleThread(PhysicsWorker* physics, std::vector<Creature *> agents, sf::Vector2f start, 
                  sf::Vector2f objectif, std::vector<Brain*> brains, int* state, 
                  std::vector<float>* scores, float* dt, float time,float brainAcc)
{
    
    // Run the physics simulation for the specified time duration
    float acc = 0.0f;
    float accbrain = 0.0f;

    for (int i = 0; i < agents.size(); ++i) {
        agents[i]->moveTo(start.x, start.y);
    }

    while (acc < time) {
        // Physics update for all agents
        for (int i = 0; i < agents.size(); ++i) {
            Creature* agent = agents[i];
            std::vector<Point> *vertices = &agent->vertices;
            physics->PBD(vertices, agent->links, agent->muscles, 10, *dt);
        }

        // Brain update for all agents (when timer triggers)
        // 10 Hours wasted here,
        // because only agent0 had time to update...
        if (accbrain > brainAcc) {
            for (int i = 0; i < agents.size(); ++i) {
                agents[i]->brainUpdate(objectif, brains[i]);
                agents[i]->update(*dt);
            }
            accbrain = 0;
        }

        accbrain += *dt;
        acc += *dt;
    }

    // Evaluation of the simulation results
    for (int i = 0; i < agents.size(); ++i) {
        Creature* agent = agents[i];
        float dx = objectif.x - agent->vertices[0].position.x;
        float dy = objectif.y - agent->vertices[0].position.y;
        float dst = std::sqrt(dx * dx + dy * dy);
        float scr = 1/(1+dst);
        (*scores)[i] = scr;
    }


    // Mark the thread as finished
    *state = 2; // 2 means finished
}