#include "threadsmg.h"
#include <iostream>

float distance(sf::Vector2f a, sf::Vector2f b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}


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
                
                // Move agent if they reached the objective
                agents[i]->update(*dt);
                if(distance(agents[i]->vertices[0].position, objectif) < 20.0f) {
                    float angle = fmod(acc*5, 2 * M_PI);
                    float x = objectif.x + 300 * cos(angle);
                    float y = objectif.y + 300 * sin(angle);
                    agents[i]->moveTo(fmod(x, 1050) , fmod(y, 750));
                    (*scores)[i] += 1.0f;
                }
            }
            accbrain = 0;
        }

        accbrain += *dt;
        acc += *dt;
    }

    // Evaluation of the simulation results
    for (int i = 0; i < agents.size(); ++i) {
        Creature* agent = agents[i];
        float dst = distance(agent->vertices[0].position, objectif);
        float scr = 1/(1+dst);
        (*scores)[i] = scr;
    }


    // Mark the thread as finished
    *state = 2; // 2 means finished
}