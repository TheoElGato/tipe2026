#include <iostream>
#include <chrono>

#include "server.h"
#include "client.h"
#include "filemanager.h"

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

int main(int argc, char* argv[]) {
    // Read args
    int mode = 0;
    std::string ip = "localhost";
    uint16_t port = 9002;
    std::string sbf_path = "sbf"; // Stand for Server Brain File Path
    if (argc==1) mode = 0;
    else {
        if (argc<5) return 1;
        std::string tmp = argv[1];
        if (tmp=="-c") {
            mode = 1;
            ip = argv[2];
            str_to_uint16(argv[3], &port);
            sbf_path = argv[4];
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
        logm("Starting Classic mode for "+std::to_string(mainSimTasker.len)+" simulations");
        for(int i=0;i<mainSimTasker.len;i+=1) {
            mainSimTasker.loadTask(i);
            simulate(mainSimTasker);
        }
    } else if (mode==1) {
        logm("Starting Client mode for "+std::to_string(mainSimTasker.len)+" simulations today.");
        SimpleClient cl("ws://"+ip+":"+std::to_string(port),sbf_path);
        cl.run(&mainSimTasker);
    } else if (mode==2) {
        logm("Server mode for "+std::to_string(mainSimTasker.len)+" simulations today.");
        LogicServer sv;
        sv.run(port,&mainSimTasker);
    } 
    
    return 0;
}