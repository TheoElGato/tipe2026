#include "filemanager.h"

void logm(const std::string& message, const std::string& level) {   
    // Standardized logging function
    std::cout << "[" << level << "] " << message << std::endl;
}

std::string generate_name(const std::string& name) {
    std::time_t now = std::time(nullptr); // Get current time
    std::tm local_time{};
#ifdef _WIN32                            // Yes WIN32 support
    localtime_s(&local_time, &now);
#else
    localtime_r(&now, &local_time);
#endif

    // Format time as YYYYMMDD_HHMMSS
    std::ostringstream oss;
    oss << std::put_time(&local_time, "%Y%m%d_%H%M%S");

    return name + "_" + oss.str();
}

std::string getHostName() {
    // This bloc has been made with no intention for WIN32. 
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

SimDataStruct::SimDataStruct(std::string path, std::string name, int generation, int simu_time, int evolution, int brains_number,int agents_number,int train_sessions) {
    
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
        {"simu_time", simu_time},
        {"evolution", evolution},
        {"brains-number", brains_number},
        {"agents-number", agents_number},
        {"total_trained_time", 0},
        {"train_sessions",train_sessions}
    };
    
}

void SimDataStruct::addStatRow(float generation, float agent0score, float agent1score, float agent2score, 
                   float agent3score, float agent4score, float agent5score, float agent6score,
                   float agent7score, float agent8score, float agent9score, float mean,
                   float bestAgentScore, float timeForOneGen) {
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
    bestAgentScoreV.push_back(bestAgentScore);
    timeForOneGenV.push_back(timeForOneGen);
    

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
                         "Agent9Score", "Mean", "BestAgentScore", "TimeForOneGen"});
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
                                       bestAgentScoreV[i],
                                       timeForOneGenV[i]});
    }
    
    
	o.close();
}



void SimDataStruct::loadFromFile(std::string load_name) {
    // read the JSON file from the load_name sim 
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
        bestAgentScoreV.push_back(row["BestAgentScore"].get<float>());
        timeForOneGenV.push_back(row["TimeForOneGen"].get<float>());
    }
}

std::string SimDataStruct::getFullPath() {
    std::string fp = this->fullpath;
    return fp+"/";
}


// SimTasker

SimTasker::SimTasker(std::string tastPath) {
    // read the JSON file from the load_name sim 
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
    this->is_infinite = data["IS_INFINITE"];
    this->time_allowed = data["TIME_ALLOWED"];
}

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
    update();
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

