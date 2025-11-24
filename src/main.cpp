#include "server.h"
#include "client.h"
#include "filemanager.h"

int main(int argc, char* argv[]) {
    // Read args
    int next_arg = 1;
    int mode = 0;
    std::string ip = "localhost";
    uint16_t port = 9002;
    std::string sbf_path = "sbf"; // Stand for Server Brain File Path
    bool headless = false;
    if (argc==1) mode = 0;
    else {
        while (next_arg != argc) {
            std::string arg = argv[next_arg];
            if (arg=="-c") {
                mode = 1;
                ip = argv[next_arg+1];
                str_to_uint16(argv[next_arg+2], &port);
                sbf_path = argv[next_arg+3];
                next_arg = next_arg+4;
            } else if (arg=="-s") {
                mode = 2;
                ip = argv[next_arg+1];
                str_to_uint16(argv[next_arg+2], &port);
                sbf_path = argv[next_arg+3];
                next_arg = next_arg+4;
            } else if (arg=="-x") {
                headless = true;
                next_arg = next_arg+1;
            } else {
                assert(false  && "Unkown argument passed.");
            }
        
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
        // Start client on ip and port
        logm("Starting Client mode for "+std::to_string(mainSimTasker.len)+" simulations today.");
        SimpleClient cl("ws://"+ip+":"+std::to_string(port),sbf_path);
        cl.run(&mainSimTasker,headless);
    } else if (mode==2) {
        // Start server on localhost and listening on port
        logm("Server mode for "+std::to_string(mainSimTasker.len)+" simulations today.");
        LogicServer sv(sbf_path);
        sv.run(port,&mainSimTasker);
    } 
    
    return 0;
}