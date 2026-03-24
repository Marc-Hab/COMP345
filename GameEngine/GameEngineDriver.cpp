#include "GameEngine.h"
#include "../CommandProcessor/CommandProcessing.h"
#include "../Maps/Map.h"
#include "../Players/Player.h"
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;


// ---------------------------------------------------------------------------
// Main driver
// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) {

    // -----------------------------------------------------------------------
    // Setup: startup phase via file commands or interactive console
    // -----------------------------------------------------------------------
    CommandProcessor* processor = nullptr;

    if (argc >= 3 && strcmp(argv[1], "-file") == 0) {
        FileCommandProcessorAdapter* fileProcessor = new FileCommandProcessorAdapter(argv[2]);

        if (!fileProcessor || !fileProcessor->isOpen()){
            delete fileProcessor;
            return 1;
        }

        processor = fileProcessor;
    }
    else if (argc >= 2 && strcmp(argv[1], "-console") == 0 ) { 
        processor = new CommandProcessor();
    }
    else {
        cout << "ERROR: Invalid command." << endl;
        cout << "Usage: " << argv[0] << " [-console | -file <filename>]" << endl;
        return 2;
    }

    GameEngine* engine = new GameEngine(processor);

    engine->run();

    delete engine;
    return 0;
}
