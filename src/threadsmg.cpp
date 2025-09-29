#include "threadsmg.h"

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
        for (int i = 0; i < agents.size(); ++i) {
            Creature* agent = agents[i];

            // handle physics update for each agent
            std::vector<Point> *vertices = &agent->vertices;
            physics->PBD(vertices, agent->links, agent->muscles, 10, *dt);

            if(brainAcc <= accbrain) 
            {
                // handle brain update for each agent
                agent->brainUpdate(objectif, brains[i]);
                agent->update(*dt);
                accbrain = 0;

            }
        }
        accbrain += *dt;
        acc += *dt;
    }

    // Evaluation of the simulation results
    for (int i = 0; i < agents.size(); ++i) {
        Creature* agent = agents[i];
        float dx = objectif.x - agent->vertices[0].position.x;
        float dy = objectif.y - agent->vertices[0].position.y;
        (*scores)[i] = 1/(1+std::sqrt(dx * dx + dy * dy));
        // Here you can store or print the distance or any other evaluation metric
        // For example, you might want to log it or update a shared data structure
    }


    // Mark the thread as finished
    *state = 2; // 2 means finished
}