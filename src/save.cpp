#include "save.h"

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

SimDataStruct::SimDataStruct(std::string path, std::string name, int generation, int simu_time, int evolution, int brains_number,int agents_number,int train_sessions) {
    
    // This bloc has been made with no intention for WIN32. 
	char hostname[HOST_NAME_MAX];
	gethostname(hostname, HOST_NAME_MAX);
	std::string host_name = hostname;
	// Cry about it.

	this->name = generate_name(name);
	
	std::filesystem::path part1 = path;
	std::filesystem::path part2 = this->name;
	this->fullpath = part1 / part2;
	
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
    
    save();
}

void SimDataStruct::save() {
    // write pretty~~ JSON to another file :)
	std::ofstream o(fullpath / (name+".json"));
	o << std::setw(4) << data << std::endl;
}