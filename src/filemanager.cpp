/*
    filemanager.cpp

    Description:
        this file is responsible for storing and loading json data into files
        Serialising data for the websocket, logging, convertion, and os stuff.
        This is so much more that a filemanager, this is the project backbone.

        
    Author:
        R. Benichou
        A. Spadone
*/

#include "filemanager.hpp"

void logm(const std::string& message, const std::string& level) {   
    // Standardized logging function
    std::cout << "[" << level << "] " << message << std::endl;
}

void error(const std::string& message) {
    // Standardized error function
    logm(message, "ERROR");
    assert(false);
}

std::string remove_zero_ts(float value) {
    std::ostringstream oss;
    oss << std::setprecision(8) << std::noshowpoint << value;
    return oss.str();
}

std::string generate_name(const std::string& name) {
    std::time_t now = std::time(nullptr); // Get current time
    std::tm local_time{};
    localtime_r(&now, &local_time);    // Not for Windows
    // Format time as YYYYMMDD_HHMMSS
    std::ostringstream oss;
    oss << std::put_time(&local_time, "%Y%m%d_%H%M%S");

    return name + "_" + oss.str();
}

float average(std::vector<float> v){
    if(v.empty()){
        return 0;
    }

    auto const count = static_cast<float>(v.size());
    return std::reduce(v.begin(), v.end()) / count;
}

std::string getHostName() {
    // This bloc has been made with no intention for Windows. 
	char hostname[HOST_NAME_MAX];
	gethostname(hostname, HOST_NAME_MAX);
	// Cry about it.
	return hostname;
}

std::string vectf_to_jsonstring(std::vector<float> v) {
    nlohmann::json j(v);
    return j.dump();
}

std::string vects_to_jsonstring(std::vector<std::string> v) {
    nlohmann::json j(v);
    return j.dump();
}


std::vector<float> jsonstring_to_vectf(std::string s) {
    nlohmann::json j = nlohmann::json::parse(s);
    return j;
}

std::vector<std::string> jsonstring_to_vects(std::string s) {
    nlohmann::json j = nlohmann::json::parse(s);
    return j;
}

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

SimDataStruct::SimDataStruct(std::string path, std::string name, int generation, int sgeneration, int simu_time, int evolution, float brain_acc, int brains_number,int agents_number,int train_sessions,bool empty) {
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

void SimDataStruct::save() {
    // write a really pretty~~ JSON file :)
	std::ofstream o(fullpath / (name+".json"));
	o << std::setw(4) << data << std::endl;
	o.close();
	
	// write a cool CSV for the stats :
	std::ofstream ss(fullpath / (name+".csv"));
    auto writer = make_csv_writer(ss); // Fix auto I don't like this
    
    
    writer << std::vector<std::string>({"Generation","Agent0Score", "Agent1Score", "Agent2Score", "Agent3Score", 
                         "Agent4Score", "Agent5Score", "Agent6Score", "Agent7Score", "Agent8Score", 
                         "Agent9Score", "Mean", "Median", "BestAgentScore", "TimeForOneGen","TimeWaitingClients","TimeForProcessing","LocalTime"});
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

std::string SimDataStruct::getFullPath() {
    std::string fp = this->fullpath;
    return fp+"/";
}


// SimTasker

SimTasker::SimTasker(std::string tastPath) {
    // Read the JSON file from tastPath (tastPath ??? why not taskPath)
    std::ifstream f(tastPath);
    allData = nlohmann::json::parse(f);
    
    len = allData.size();
    
}

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
    this->sous_sim = data["SOUS_SIM"];
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

// Packets for the websocket

Packet::Packet(std::string c,std::string a1,std::string a2,std::string a3)
{
    cmd = c;
    arg1 = a1;
    arg2 = a2;
    arg3 = a3;
    update();
}

Packet::Packet(std::string loads)
{
    data = nlohmann::json::parse(loads);
    cmd = data["cmd"];
    arg1 = data["arg1"];
    arg2 = data["arg2"];
    arg3 = data["arg3"];
    update(); // Not needed I think, but why not.
}

std::string Packet::get_string()
{
    return data.dump();
}

void Packet::update()
{
    data = {
        {"cmd", cmd},
        {"arg1", arg1},
        {"arg2", arg2},
        {"arg3", arg3},
    };
}