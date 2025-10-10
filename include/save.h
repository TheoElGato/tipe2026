#ifndef SAVE_H
#define SAVE_H

#include <nlohmann/json.hpp>
#include <fstream>
#include <unistd.h>
#include <limits.h>


class SimDataStruct {
public:
    SimDataStruct(std::string path, std::string name, int generation, int simu_time, int evolution, int brains_number,int agents_number,int train_sessions);
    void save();


   
private:
    std::string name;
    std::filesystem::path fullpath;
	nlohmann::json data;

};


#endif // SAVE_H