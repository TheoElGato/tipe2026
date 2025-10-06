#ifndef SAVE_H
#define SAVE_H

#include <nlohmann/json.hpp>
#include <fstream>
#include <unistd.h>
#include <limits.h>


class SimDataSave {
public:
    SimDataSave();


   
private:
	nlohmann::json data;

};


#endif // SAVE_H