#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <nlohmann/json.hpp>
#include <fstream>
#include <unistd.h>
#include <limits.h>
#include <iostream>
#include "csv.hpp"

using namespace csv;

class SimTasker {
   public:
      SimTasker(std::string tastPath);
      void loadTask(int id);
      
      
      std::string sim_name;
      std::string device;
      int threads;
      
      bool load_from_file;
      std::string load_name;
      
      // SIM //
      int sim_time;
      float evolution;
      int nb_brain;
      int nb_agent;
      int nb_hidden_layer;
      int sous_sim;
      int best_keep;
      int selection_pol;
      
      // AUTOSAVE //
      bool autosave;
      int autosave_freq;
      
      // GOAL //
      int maxdist;
      int mindist;
      int nb_goal;
      
      // START POS//
      int startx;
      int starty;
   
   private:
      nlohmann::json allData;
   
};

class SimDataStruct {
public:
   nlohmann::json data;
   SimDataStruct(std::string path, std::string name, int generation, int simu_time, int evolution, int brains_number,int agents_number,int train_sessions);
   void save();
   void loadFromFile(std::string load_name);
   std::string getFullPath();
   
   void addStatRow(float generation, float agent0score, float agent1score, float agent2score, 
                   float agent3score, float agent4score, float agent5score, float agent6score,
                   float agent7score, float agent8score, float agent9score,
                   float bestAgentScore, float timeForOneGen);
   
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
   std::vector<float> bestAgentScoreV;
   std::vector<float> timeForOneGenV;
   
private:
   std::string name;
   std::filesystem::path fullpath;
   std::filesystem::path folder;
	

};


#endif // FILEMANAGER_H