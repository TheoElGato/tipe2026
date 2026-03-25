/*
	filemanager.hpp

	Description:
		Header file for filemanager.cpp

	Author:
		R. Benichou
		A. Spadone
*/

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <fstream>
#include <unistd.h>
#include <limits.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include "csv.hpp"

using namespace csv;
using namespace nlohmann::literals;

class Logger {
  public:
	  void logm(const std::string& message, const std::string& level = "INFO", const std::string& thread = "Main");
	  void fatal(const std::string& message, const std::string& thread = "Main");
	  void saveToDisk(std::string filename, bool append = false);

	  std::string lastPath = "crash_no_name.log";

  private:
	  std::vector<std::string> cache;
};

class Packet {
   public:
	  Packet(std::string c,std::string a1,std::string a2,std::string a3);
	  Packet(std::string loads);
	  std::string get_string();
	  void update();
	  
	  std::string cmd = "";
	  std::string arg1 = "";
	  std::string arg2 = "";
	  std::string arg3 = "";
	  
   private:
	  nlohmann::json data;
};

class SimTasker {
   public:
	  SimTasker(std::string taskPath);
	  void loadTask(int id);

	  int len=0;
	  std::string sim_name;
	  std::string device;
	  int threads;
	  bool load_from_file;
	  std::string load_name;

	  // SIM
	  int sim_time;
	  float evolution;
	  float brain_acc;
	  int nb_brain;
	  int nb_agent;
	  int nb_hidden_layer;
	  int sub_sim;
	  int best_keep;
	  int selection_pol;

	  // AUTOSAVE
	  bool autosave;
	  int autosave_freq;

	  // GOALS
	  int maxdist;
	  int mindist;
	  int nb_goal;

	  // START POS
	  int startx;
	  int starty;

	  // OTHERS
	  bool use_evolution_curve;
	  float curve_a;
	  float curve_b;
	  float curve_c;
	  float curve_d;

	  bool is_infinite;
	  int time_allowed;
	  float kick_factor;

   private:
	  nlohmann::json allData;
};

class SimDataStruct {
public:
   SimDataStruct(std::string path, std::string name, int generation, int sgeneration, int simu_time, int evolution, float brain_acc, int brains_number,int agents_number,int train_sessions,bool empty=false);
   void save();
   void loadFromFile(std::string load_name);
   std::string getFullPath();
   void addStatRow(float generation, float agent0score, float agent1score, float agent2score,
				   float agent3score, float agent4score, float agent5score, float agent6score,
				   float agent7score, float agent8score, float agent9score, float mean, float median,
				   float bestAgentScore,float timeForOneGen, float timeWaitingClients,
				   float timeForProcessing, float localTime, int nb_clients, int active_clients);

   nlohmann::json data;
   std::vector<float> generationV;
   std::vector<float> agent0scoreV;
   std::vector<float> agent1scoreV;
   std::vector<float> agent2scoreV;
   std::vector<float> agent3scoreV;
   std::vector<float> agent4scoreV;
   std::vector<float> agent5scoreV;
   std::vector<float> agent6scoreV;
   std::vector<float> agent7scoreV;
   std::vector<float> agent8scoreV;
   std::vector<float> agent9scoreV;
   std::vector<float> meanV;
   std::vector<float> medianV;
   std::vector<float> bestAgentScoreV;
   std::vector<float> timeForOneGenV;
   std::vector<float> timeWaitingClientsV;
   std::vector<float> timeForProcessingV;
   std::vector<float> localTimeV;

private:
   std::string name;
   std::filesystem::path fullpath;
   std::filesystem::path folder;
};

std::string remove_zero_ts(float value);
std::string getHostName();
std::string vectf_to_jsonstring(std::vector<float> v);
std::string vects_to_jsonstring(std::vector<std::string> v);
std::vector<float> jsonstring_to_vectf(std::string s);
std::vector<std::string> jsonstring_to_vects(std::string s);
bool str_to_uint16(const char *str, uint16_t *res);
float average(std::vector<float> v);
std::string get_timestamp();

#endif // FILEMANAGER_H
