/*
	main.cpp

	Description:
		Main file of the tipe2026 project.
		Import the client, the server and run either 
			Classic mode : client mode, but with no networking
			Client mode : run simulations while sharing progress with a server
			Server mode : centralize all of the client's data and give tasks
		The choice is made after analysing the program input.
		
	Author:
		R. Benichou
		A. Spadone
*/

#include "server.hpp"
#include "client.hpp"
#include "filemanager.hpp"
#include <csignal>
#include <atomic>

// Logger pointer to use in crash handler
Logger* globalLogger = nullptr;;

/*
 *  Crash handler for the program, to catch SIGSERV and save logs.
 *  @param signal The signal received.
 */
void crashHandler(int signal) {
	if (globalLogger != nullptr) {
		globalLogger->logm("Segmentation fault detected. Saving logs before exiting.","CRASH");
		globalLogger->saveToDisk(globalLogger->lastPath,true);
	}
	std::exit(signal);
}

/*
 * Main function of the program, to start either the client, the server or run a classic simulation.
 * @param argc The number of arguments passed to the program.
 * @param argv The array of arguments passed to the program.
 * @return int 0 For a successfull execution.
 */
int main(int argc, char* argv[]) {
	// Read args
	int next_arg = 1;
	int mode = 0;
	std::string ip = "localhost";
	uint16_t port = 9002;
	std::string sbf_path = "sbf"; // Stand for Server Brain Files Path

	// Create the main logger
    Logger* logger = new Logger();
	globalLogger = logger;

	// Set up the crash handler
	std::signal(SIGSEGV, crashHandler);

	bool headless = false; // Allow to run without a window
	if (argc==1) mode = 0;
	else {
		while (next_arg != argc) {
			std::string arg = argv[next_arg];
			if (arg=="-c") {	// Client mode
				mode = 1;
				ip = argv[next_arg+1];
				str_to_uint16(argv[next_arg+2], &port);
				sbf_path = argv[next_arg+3];
				next_arg = next_arg+4;

			} else if (arg=="-s") { // Server mode
				mode = 2;
				ip = argv[next_arg+1];
				str_to_uint16(argv[next_arg+2], &port);
				sbf_path = argv[next_arg+3];
				next_arg = next_arg+4;

			} else if (arg=="-x") { // Headless mode
				headless = true;
				next_arg = next_arg+1;

			} else {
				logger->fatal("Unkown argument passed.");
			}
		}
	}
	
	// Load all the task we have to do
	SimTasker mainSimTasker("task.json");
	
	if (mode==0) {
		// Execute all the sims : 
		logger->logm("Starting Classic mode for "+std::to_string(mainSimTasker.len)+" simulations");
		for(int i=0;i<mainSimTasker.len;i+=1) {
			mainSimTasker.loadTask(i);
			simulate(mainSimTasker,logger,false,headless);
		}
	} else if (mode==1) {
		// Start client on ip and port
		logger->logm("Starting Client mode for "+std::to_string(mainSimTasker.len)+" simulations today.");
		SimpleClient cl("ws://"+ip+":"+std::to_string(port),sbf_path,logger);
		cl.run(&mainSimTasker,headless);
	} else if (mode==2) {
		// Start server on localhost and listening on port
		logger->logm("Server mode for "+std::to_string(mainSimTasker.len)+" simulations today.");
		LogicServer sv(sbf_path, logger);
		sv.run(port,&mainSimTasker);
	} 
	
	return 0;
}
