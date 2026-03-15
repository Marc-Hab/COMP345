#include "GameEngine.h"
#include "../CommandProcessor/CommandProcessing.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {

    CommandProcessor* processor = nullptr;

    if (argc < 2 || strcmp(argv[1], "-console") == 0) {
        processor = new CommandProcessor();
    } else if (strcmp(argv[1], "-file") == 0) {
        if (argc < 3) {
            cout << "Usage: " << argv[0] << " -file <filename>" << endl;
            return 1;
        }
        processor = new FileCommandProcessorAdapter(argv[2]);
    } else {
        processor = new CommandProcessor();
    }

    // Verify file opened successfully
    FileCommandProcessorAdapter* fileProc =
        dynamic_cast<FileCommandProcessorAdapter*>(processor);
    if (fileProc && !fileProc->isOpen()) {
        cout << "ERROR: Could not open command file '" << argv[2] << "'" << endl;
        delete fileProc;
        return 1;
    }

    GameEngine* engine = new GameEngine(processor);

    // Run the startup phase - demonstrates all 4 requirements
    engine->startupPhase();

    delete engine;
    return 0;
}
