/*
    threadsmg.cpp

    Description:
        DESCRIPTION

    Author:
        R. Benichou
        A. Spadone
*/

#include "threadsmg.hpp"
#include <iostream>

float distance(sf::Vector2f a, sf::Vector2f b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}


void handleThread(PhysicsWorker* physics, std::vector<Creature *> agents, sf::Vector2f start,
                  std::vector<sf::Vector2f> objectif, std::vector<Brain*> brains, int* state,
                  std::vector<float>* scores, float* dt, float time,float brainAcc)
{

    // Run the physics simulation for the specified time duration
    float acc = 0.0f;
    float accbrain = 0.0f;

    std::vector<int> obj;
    int nb_goal = objectif.size();

    for (int i = 0; i < agents.size(); ++i) {
        agents[i]->moveTo(start.x, start.y);
        obj.push_back(0);
    }

    while (acc < time) {
        // Physics update for all agents
        for (int i = 0; i < agents.size(); ++i) {
            Creature* agent = agents[i];
            std::vector<Point> *vertices = &agent->vertices;
            physics->PBD(vertices, agent->links, agent->muscles, 10, *dt);
            agents[i]->update();
        }

        // Brain update for all agents (when timer triggers)
        // 10 Hours wasted here,
        // because only agent0 had time to update...
        if (accbrain > brainAcc) {
            for (int i = 0; i < agents.size(); ++i) {
                agents[i]->brainUpdate(objectif[obj[i]], brains[i]);

                // Move agent if they reached the objective
                if(distance(agents[i]->vertices[0].position, objectif[obj[i]]) < 20.0f) {
                    (*scores)[i] += 1.0f;
                    if (obj[i] == nb_goal - 1) obj[i]=0;
                    else obj[i]+=1;
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
        float dst = distance(agent->vertices[0].position, objectif[obj[i]]);
        float scr = 1/(1+dst);
        (*scores)[i] = scr;
    }

    obj.clear();

    // Mark the thread as finished
    *state = 2; // 2 means finished
}
