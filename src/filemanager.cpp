/*
	filemanager.cpp

	Description:
		This file is responsible for storing and loading json data into files,
		serialising data for the websocket, logging, convertion, and os stuff.
		This is so much more that a filemanager, this is the project backbone.


	Author:
		R. Benichou
		A. Spadone
*/

#include "filemanager.hpp"

//// Utility functions

/*
 * Standardized logging function for the project
 * @param message The message to print
 * @param level The level of the message, by default INFO
 */
void logm(const std::string& message, const std::string& level) {
	std::cout << "[" << level << "] " << message << std::endl;
}

/*
 * Standardized error function for the project
 * @param message The message to print will exiting
 */
void error(const std::string& message) {
	logm(message, "FATAL");
	assert(false);
}

/*
 * Remove useless zero when converting float to string
 * @param value The float to convert and trim zeros
 * @return A string with the zeros removed
 */
std::string remove_zero_ts(float value) {
	std::ostringstream oss;
	oss << std::setprecision(8) << std::noshowpoint << value;
	return oss.str();
}

/*
 * Generate a name for a simulation with the current date
 * @param name The start of the name that will be generated
 * @return A concacenated string with name and date
 */
std::string generate_name(const std::string& name) {
	std::time_t now = std::time(nullptr); // Get current time
	std::tm local_time{};
	localtime_r(&now, &local_time);	// Not for Windows
	// Format time as YYYYMMDD_HHMMSS
	std::ostringstream oss;
	oss << std::put_time(&local_time, "%Y%m%d_%H%M%S");

	return name + "_" + oss.str();
}

/*
 * Make the average of a vector of float
 * @param v The vector to average
 * @return The average of v
 */
float average(std::vector<float> v){
	if(v.empty()){
		return 0;
	}
	auto const count = static_cast<float>(v.size());
	return std::reduce(v.begin(), v.end()) / count;
}

/*
 * Get the host name of the machine
 * This bloc has been made with no intention for Windows.
 * @return The host name
 */
std::string getHostName() {
	char hostname[HOST_NAME_MAX];
	gethostname(hostname, HOST_NAME_MAX);
	return hostname;
}

/*
 * Serialise a float vector to a json string
 * @param v The vector to convert
 * @return The json string
 */
std::string vectf_to_jsonstring(std::vector<float> v) {
	nlohmann::json j(v);
	return j.dump();
}

/*
 * Serialise a string vector to a json string
 * @param v The vector to convert
 * @return The json string
 */
std::string vects_to_jsonstring(std::vector<std::string> v) {
	nlohmann::json j(v);
	return j.dump();
}

/*
 * Convert a json string to float vector
 * @param s The json to convert
 * @return The float vector
 */
std::vector<float> jsonstring_to_vectf(std::string s) {
	nlohmann::json j = nlohmann::json::parse(s);
	return j;
}

/*
 * Convert a json string to string vector
 * @param s The json to convert
 * @return The string vector
 */
std::vector<std::string> jsonstring_to_vects(std::string s) {
	nlohmann::json j = nlohmann::json::parse(s);
	return j;
}

/*
 * Convert a string to a uint16
 * @param str The pointer to the string to convert
 * @param res The pointer where to store the result
 * @return A bool of the operation success
 */
bool str_to_uint16(const char *str, uint16_t *res) {
	char *end;
	errno = 0;
	long val = strtol(str, &end, 10);
	if (errno || end == str || *end != '\0' || val < 0 || val >= 0x10000) {
		return false;
	}
	*res = (uint16_t)val;
	return true;
}

//// SimDataStruct

/*
 * Constructor for the SimDataStruct class
 * @param path The path as a std::string of where to store the simulation data
 * @param name The name of the simulation
 * @param generation The current generation
 * @param sgeneration The current sub simulation
 * @param simu_time The time for one generation
 * @param evolution The current evolution
 * @param brain_acc The number of step skipped until brain are ticked
 * @param brains_number The total number of brains
 * @param agents_number The total number of agents
 * @param train_sessions The number of sessions where the AI was trained
 * @param empty If empty is true then skip this function at start
 */
SimDataStruct::SimDataStruct(std::string path, std::string name, int generation, int sgeneration, int simu_time, int evolution, float brain_acc, int brains_number, int agents_number, int train_sessions, bool empty) {
	if (empty) return;
	std::string host_name = getHostName();

	this->name = generate_name(name);

	folder = path;
	std::filesystem::path part2 = this->name;
	this->fullpath = folder / part2;

	if (!std::filesystem::exists(fullpath))
	{
		 std::filesystem::create_directory(fullpath);
	}

	data = {
		{"name", name},
		{"host_name", host_name},
		{"generation", generation},
		{"Sgeneration", sgeneration},
		{"simu_time", simu_time},
		{"evolution", evolution},
		{"brain_acc", brain_acc},
		{"brains-number", brains_number},
		{"agents-number", agents_number},
		{"total_trained_time", 0},
		{"train_sessions",train_sessions},
		{"nb_clients",0}
	};

}

/*
 * Add a line of stats in vectors for a generation, to later
 * push them into a CSV file
 * @param generation The current generation
 * @param agent0score The score for the generation for agent 0
 * @param agent1score The score for the generation for agent 1
 * @param agent2score The score for the generation for agent 2
 * @param agent3score The score for the generation for agent 3
 * @param agent4score The score for the generation for agent 4
 * @param agent5score The score for the generation for agent 5
 * @param agent6score The score for the generation for agent 6
 * @param agent7score The score for the generation for agent 7
 * @param agent8score The score for the generation for agent 8
 * @param agent9score The score for the generation for agent 9
 * @param mean The mean for the generation
 * @param median The median for the generation
 * @param bestAgentScore The score for the generation for the best agent
 * @param timeForOneGen The time to train this generation
 * @param timeWaitingClients The time use to wait all the clients
 * @param timeForProcessing The time use to process and save data
 * @param localTime The local time of the device
 * @param nb_clients The number of client connected as this data is save.
 */
void SimDataStruct::addStatRow(float generation, float agent0score, float agent1score, float agent2score,
				   float agent3score, float agent4score, float agent5score, float agent6score,
				   float agent7score, float agent8score, float agent9score, float mean, float median,
				   float bestAgentScore, float timeForOneGen, float timeWaitingClients,
				   float timeForProcessing, float localTime, int nb_clients) {
	generationV.push_back(generation);
	agent0scoreV.push_back(agent0score);
	agent1scoreV.push_back(agent1score);
	agent2scoreV.push_back(agent2score);
	agent3scoreV.push_back(agent3score);
	agent4scoreV.push_back(agent4score);
	agent5scoreV.push_back(agent5score);
	agent6scoreV.push_back(agent6score);
	agent7scoreV.push_back(agent7score);
	agent8scoreV.push_back(agent8score);
	agent9scoreV.push_back(agent9score);
	meanV.push_back(mean);
	medianV.push_back(median);
	bestAgentScoreV.push_back(bestAgentScore);
	timeForOneGenV.push_back(timeForOneGen);
	timeWaitingClientsV.push_back(timeWaitingClients);
	timeForProcessingV.push_back(timeForProcessing);
	localTimeV.push_back(localTime);
	data["nb_clients"] = nb_clients;
}

/*
 * Push data saved in vectors into a JSON and a CSV file
 */
void SimDataStruct::save() {
	// Write a really JSON file
	std::ofstream o(fullpath / (name+".json"));
	o << std::setw(4) << data << std::endl;
	o.close();

	// Write a CSV for the stats
	std::ofstream ss(fullpath / (name+".csv"));
	auto writer = make_csv_writer(ss);


	writer << std::vector<std::string>({"Generation","Agent0Score", "Agent1Score", "Agent2Score", "Agent3Score",
						 "Agent4Score", "Agent5Score", "Agent6Score", "Agent7Score", "Agent8Score",
						 "Agent9Score", "Mean", "Median", "BestAgentScore", "TimeForOneGen", "TimeWaitingClients", "TimeForProcessing", "LocalTime"});
	for(int i=0;i<this->generationV.size();i+=1) {
		writer << std::vector<float>({ generationV[i],
									   agent0scoreV[i],
									   agent1scoreV[i],
									   agent2scoreV[i],
									   agent3scoreV[i],
									   agent4scoreV[i],
									   agent5scoreV[i],
									   agent6scoreV[i],
									   agent7scoreV[i],
									   agent8scoreV[i],
									   agent9scoreV[i],
									   meanV[i],
									   medianV[i],
									   bestAgentScoreV[i],
									   timeForOneGenV[i],
									   timeWaitingClientsV[i],
									   timeForProcessingV[i],
									   localTimeV[i]});
	}


	o.close();
}

/*
 * Push data saved in vectors into a JSON and a CSV file
 */
void SimDataStruct::loadFromFile(std::string load_name) {
	// Read the JSON file from the load_name sim
	std::string filename = folder / load_name / (load_name+".json");

	std::ifstream f(filename);
	nlohmann::json j = nlohmann::json::parse(f);

	std::string host_name = getHostName();
	j["host_name"] = host_name;

	data = j;
	// Load the STATS
	std::string filenameCsv = folder / load_name / (load_name+".csv");
	CSVReader reader(filenameCsv);
	for (CSVRow& row: reader) { // Input iterator
		generationV.push_back(row["Generation"].get<float>());
		agent0scoreV.push_back(row["Agent0Score"].get<float>());
		agent1scoreV.push_back(row["Agent1Score"].get<float>());
		agent2scoreV.push_back(row["Agent2Score"].get<float>());
		agent3scoreV.push_back(row["Agent3Score"].get<float>());
		agent4scoreV.push_back(row["Agent4Score"].get<float>());
		agent5scoreV.push_back(row["Agent5Score"].get<float>());
		agent6scoreV.push_back(row["Agent6Score"].get<float>());
		agent7scoreV.push_back(row["Agent7Score"].get<float>());
		agent8scoreV.push_back(row["Agent8Score"].get<float>());
		agent9scoreV.push_back(row["Agent9Score"].get<float>());
		meanV.push_back(row["Mean"].get<float>());
		medianV.push_back(row["Median"].get<float>());
		bestAgentScoreV.push_back(row["BestAgentScore"].get<float>());
		timeForOneGenV.push_back(row["TimeForOneGen"].get<float>());
		timeWaitingClientsV.push_back(row["TimeWaitingClients"].get<float>());
		timeForProcessingV.push_back(row["TimeForProcessing"].get<float>());
		localTimeV.push_back(row["LocalTime"].get<float>());
	}
}

/*
 * Get a the full path of the Sim Data Struct folder
 * @return A string of the path
 */
std::string SimDataStruct::getFullPath() {
	std::string fp = this->fullpath;
	return fp+"/";
}

//// SimTasker

/*
 * Constructor for the SimTasker class
 * @param taskPath The path as a std::string of the task list
 */
SimTasker::SimTasker(std::string taskPath) {
	// Read the JSON file from taskPath
	std::ifstream f(taskPath);
	allData = nlohmann::json::parse(f);

	len = allData.size();
}

/*
 * Load the task parameters into the object variables
 * @param id The id of the task to load from the json file
 */
void SimTasker::loadTask(int id) {
	nlohmann::json data = this->allData[id];
	this->sim_name = data["SIM_NAME"];
	this->device = data["DEVICE"];
	this->threads = data["THREADS"];
	this->load_from_file = data["LOAD_FROM_FILE"];
	this->load_name = data["LOAD_NAME"];
	this->sim_time = data["SIM_TIME"];
	this->evolution = data["EVOLUTION"];
	this->brain_acc = data["BRAIN_ACC"];
	this->nb_brain = data["NB_BRAIN"];
	this->nb_agent = nb_brain*threads;
	this->nb_hidden_layer = data["NB_HIDDEN_LAYER"];
	this->sub_sim = data["SOUS_SIM"];
	this->best_keep = data["BEST_KEEP"];
	this->selection_pol = data["SELECTION_POL"];
	this->autosave = data["AUTOSAVE"];
	this->autosave_freq = data["AUTOSAVE_FREQ"];
	this->maxdist = data["MAXDIST"];
	this->mindist = data["MINDIST"];
	this->nb_goal = data["NB_GOAL"];
	this->startx = data["STARTX"];
	this->starty = data["STARTY"];
	this->use_evolution_curve = data["USE_EVOLUTION_CURVE"];
	this->curve_a = data["CURVE_A"];
	this->curve_b = data["CURVE_B"];
	this->curve_c = data["CURVE_C"];
	this->curve_d = data["CURVE_D"];
	this->is_infinite = data["IS_INFINITE"];
	this->time_allowed = data["TIME_ALLOWED"];
}

//// Packet

/*
 * Constructor for the Packet class
 * Create a Packet object from scratch
 * @param c The command/packet identifier (see filemanager.cpp for a list)
 * @param a1 The first argument
 * @param a2 The second argument
 * @param a3 The third argument
 */
Packet::Packet(std::string c, std::string a1, std::string a2, std::string a3) {
	cmd = c;
	arg1 = a1;
	arg2 = a2;
	arg3 = a3;
	update();
}

/*
 * Constructor for the Packet class
 * Unpack a Packet object from a json string
 * @param loads The json string we parse and load into variables
 */
Packet::Packet(std::string loads) {
	data = nlohmann::json::parse(loads);
	cmd = data["cmd"];
	arg1 = data["arg1"];
	arg2 = data["arg2"];
	arg3 = data["arg3"];
	update();
}

/*
 * Create a json string to pack the object
 * @return A json string
 */
std::string Packet::get_string() {
	return data.dump();
}

/*
 * Update the json object data with the current variables
 */
void Packet::update() {
	data = {
		{"cmd", cmd},
		{"arg1", arg1},
		{"arg2", arg2},
		{"arg3", arg3},
	};
}
