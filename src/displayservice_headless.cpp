/*
	displayservice_headless.cpp

	Description:
		This is a dummy file of displayservice.cpp 
		It is used to declare functions that actually do nothing and are 
		only here to be able to build the project 
		
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
	/*
	 * Initialize the DisplayServiceSFML class
	 * Because this is headless, it will do nothing
	 * @param headless If true run in headless mode
	 * @param runningptr The pointer to the running state bool of the simulation
	 * @param clean_exitptr The pointer to the clean_exitptr to request a clean exit
	 * @param agentPartitionsSize The size of one agent partition
	 * @param mc If true : run in client mode, else in classic mode
	 */
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
	
	/*
	 * Render a frame to the window
	 * Because this is headless, it will do nothing
	 * @param groups_avail The pointer to a vector of int representing which groups are available (see client.cpp)
	 * @param agentPartitions The pointer to the vector of the partitioning of agents 
	 * @param fps A float of the current Frame Per Second
	 * @param agent_size The number of agents in total
	 * @param generation The current generation number
	 * @param sous_sim_started The number of sous_sim_started
	 * @param acu Used for the time for a generation 
	 * @param simu_time The time for one generation
	 * @param evolution The current simulation parameter evolution
	 * @param start A vector2f for the spawnpoint
	 * @param goals A vector of vector2f of goals
	 * @param mc If true : run in client mode, else in classic mode
	 */
	void render(std::vector<int>* groups_avail,
				std::vector<std::vector<Creature*>>* agentPartitions,
				float fps, int agent_size, int generation, int sous_sim_started, float acu,
				float simu_time, float evolution, sf::Vector2f start, const std::vector<sf::Vector2f>& goals, bool mc) override {
		// Nothing to do here
	}
	
	/*
	 * Draw a creature to the screen
	 * Because this is headless, it will do nothing
	 * @param creature The pointer to the creature to draw
	 */
	void draw_creature(Creature* creature) override {
	
	}

private:
	bool* runningptr = nullptr;
	bool* clean_exitptr = nullptr;
	bool runningptr_local_owned = false;
};

/*
 * Create a unique instance of DisplayServiceSFML class
 * @return A unique ptr of a instance of DisplayServiceSFML class
 */
std::unique_ptr<DisplayService> CreateDisplayService() {
	return std::make_unique<DisplayServiceHeadless>();
}