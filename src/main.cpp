#include <iostream>

#include "client.h"
#include "filemanager.h"

int main(int argc, char* argv[]) {
    // Read args
    int mode = 0;
    std::string ip = "localhost";
    std::string sbf_path = "sbf"; // Stand for Server Brain File Path
    if (argc==1) mode = 0;
    else {
        if (argc<4) return 1;
        std::string tmp = argv[1];
        if (tmp=="-c") {
            mode = 1;
            ip = argv[2];
            sbf_path = argv[3];
        } else if (tmp=="-s") {
            mode = 2;
            ip = argv[2];
            sbf_path = argv[3];
        } else {
            return 1;
        }
    }
    
    // Load all the task we have to do
    SimTasker mainSimTasker("task.json");
    
    if (mode==0) {
        // Execute all the sims : 
        logm("Classic mode for "+std::to_string(mainSimTasker.len)+" simulations");
        for(int i=0;i<mainSimTasker.len;i+=1) {
            mainSimTasker.loadTask(i);
            simulate(mainSimTasker);
        }
    } else if (mode==1) {
        logm("Client mode");
    } else if (mode==2) {
        logm("Server mode for "+std::to_string(mainSimTasker.len)+" simulations today.");
    } 
    
    return 0;
}