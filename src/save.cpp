#include "save.h"

SimDataSave::SimDataSave() {

	char hostname[HOST_NAME_MAX];
	gethostname(hostname, HOST_NAME_MAX);
	
	

	std::string name = "";
	std::string host_name = hostname;
	int generation = 0;
	int simu_time = 0;
	int evolution = 0;
	int brains_number = 0;
	int agents_number = 0;
	int train_sessions = 0;
	
	data = {
        {"name", "name"},
        {"host_name", host_name},
        {"generation", generation},
        {"simu_time", simu_time},
        {"evolution", evolution},
        {"brains-number", brains_number},
        {"agents-number", agents_number},
        {"train_sessions",train_sessions}
    };
}