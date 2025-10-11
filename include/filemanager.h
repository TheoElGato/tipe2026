#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <nlohmann/json.hpp>
#include <fstream>
#include <unistd.h>
#include <limits.h>
#include <iostream>


class SimDataStruct {
public:
   nlohmann::json data;
   SimDataStruct(std::string path, std::string name, int generation, int simu_time, int evolution, int brains_number,int agents_number,int train_sessions);
   void save();
   void loadFromFile(std::string load_name);
   std::string getFullPath();


   
private:
   std::string name;
   std::filesystem::path fullpath;
   std::filesystem::path folder;
	

};


#endif // FILEMANAGER_H