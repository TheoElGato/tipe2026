/*
	threadsmg.cpp

	Description:
		This file handle the task that are launch as thread.
		This allow an easyer gestion of the different thread.

	Author:
		R. Benichou
		A. Spadone
*/

#include "threadsmg.hpp"
#include <iostream>

/*
 * Calculate the distance between two vector
 * @param a Vector2 object
 * @param b same
 * @return the distance
 */
float distance(sf::Vector2f a, sf::Vector2f b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return std::sqrt(dx * dx + dy * dy);
}

/*
 * Manage a given task. Wich is givent by the agent, brain, start, objectives, time.
 * the brainAcc and dt define the quality
 * And update the scores and state
 * @param physics the physicwotker that the thread can use
 * @param agents vector of all the agent the task use
 * @param start the position of the start
 * @param objectives a vector of all the objectives
 * @param brains the brain associated to the agents
 * @param state the state of the thread (0 = not started; 1: running; 2: finished; 3: collected)
 * @param scores the score the agent got at the end of the task
 * @param dt the pressition of the physics simulation
 * @param time the simulated time
 * @param brainAcc the frequency of the activation of the brain
 */
void handleThread(PhysicsWorker* physics, std::vector<Creature *> agents, sf::Vector2f start,
				  std::vector<sf::Vector2f> objectives, std::vector<Brain*> brains, int* state,
				  std::vector<float>* scores, float* dt, float time, float brainAcc) {

	// Run the physics simulation for the specified time duration
	float acc = 0.0f;
	float accbrain = 0.0f;

	std::vector<int> obj;
	int nb_goal = objectives.size();

	// Put all the agent at the start
	for (int i = 0; i < agents.size(); ++i) {
		agents[i]->moveTo(start.x, start.y);
		obj.push_back(0);
	}
	// for the duration given run the simulation
	while (acc < time) {
		// Physics update for all agents
		for (int i = 0; i < agents.size(); ++i) {
			Creature* agent = agents[i];
			std::vector<Point> *vertices = &agent->vertices;
			physics->PBD(vertices, agent->links, agent->muscles, 10, *dt);
			agents[i]->update();
		}

		// Brain update for all agents (when timer triggers)
		if (accbrain > brainAcc) {
			for (int i = 0; i < agents.size(); ++i) {
				agents[i]->brainUpdate(objectives[obj[i]], brains[i]);

				// Give agent a new objective if they reached the objective
				if(distance(agents[i]->vertices[0].position, objectives[obj[i]]) < 20.0f) {
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
		float dst = distance(agent->vertices[0].position, objectives[obj[i]]);
		float scr = 1/(1+dst);
		(*scores)[i] = scr;
	}

	obj.clear();

	// Mark the thread as finished
	*state = 2; // 2 means finished
}
