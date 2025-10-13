#include "filemanager.h"


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
        {"train_sessions",train_sessions}
    };
    
}

void SimDataStruct::save() {
    // write a really pretty~~ JSON file :)
	std::ofstream o(fullpath / (name+".json"));
	o << std::setw(4) << data << std::endl;
	o.close();
}



void SimDataStruct::loadFromFile(std::string load_name) {
    // read the JSON file from the load_name sim 
    std::string filename = folder / load_name / (load_name+".json");
    
    
    std::ifstream f(filename);
    nlohmann::json data = nlohmann::json::parse(R"({"test": "aaa"})");

    
    
    std::string host_name = getHostName();
    //j["host_name"] = host_name;
    
    //data = j;
}

std::string SimDataStruct::getFullPath() {
    std::string fp = this->fullpath;
    return fp+"/";
}