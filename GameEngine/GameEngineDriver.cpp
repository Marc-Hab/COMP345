#include "GameEngine.h"
#include "../CommandProcessor/CommandProcessing.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {

    CommandProcessor* processor = nullptr;
    
    //Check command line arguments to determine which command processor to use
    if (argc < 2) {
        // Default to console
        processor = new CommandProcessor();
    }
    else if (strcmp(argv[1], "-console") == 0) {
        processor = new CommandProcessor();
    }
    else if (strcmp(argv[1], "-file") == 0) {
        
        if (argc < 3) {
            cout << "ERROR: -file requires a filename" << endl;
            cout << "Usage: " << argv[0] << " -file <filename>" << endl;
            return 1;
        }

        processor = new FileCommandProcessorAdapter(argv[2]);
    }
    else {
        processor = new CommandProcessor();
    }

    // Check if file opened successfully
    FileCommandProcessorAdapter* fileProcessor = dynamic_cast<FileCommandProcessorAdapter*>(processor);
    if (fileProcessor && !fileProcessor->isOpen()) {
        cout << "ERROR: Could not open file '" << argv[2] << "'" << endl;
        cout << "Please check that the file exists and is readable." << endl;
        delete fileProcessor;
        return 1;
    }
    
    GameEngine* engine = new GameEngine(processor);
    engine->run();
    
    delete engine;
    
    return 0;
}
