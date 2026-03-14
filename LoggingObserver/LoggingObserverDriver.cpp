
#include "LoggingObserver.h"
#include "../GameEngine/GameEngine.h"
#include "../Orders/Orders.h"
#include <iostream>
#include <fstream>

// Prints the current contents of the log file
static void printLogFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::cout << "\n===== " << filename << " =====\n";
    while (std::getline(file, line)) {
        std::cout << "  " << line << "\n";
    }
    std::cout << "========================\n\n";
}

int main() {
    // Clears the log file from any previous run.
    { std::ofstream clear("gamelog.txt"); }

    LogObserver* logObs = new LogObserver("gamelog.txt");

    std::cout << "============================================================\n";
    std::cout << "  Game Log Observer Driver\n";
    std::cout << "============================================================\n\n";

    std::cout << "--- GameEngine state transitions--- \n";
    GameEngine* engine = new GameEngine();
    engine->attach(logObs);

    engine->applyCommand("loadmap world.map");   // Start to  MapLoaded
    engine->applyCommand("validatemap");          // MapLoaded to MapValidated
    engine->applyCommand("addplayer Alice");      // MapValidated  to PlayersAdded
    engine->applyCommand("addplayer Bob");        // PlayersAdded to PlayersAdded
    engine->applyCommand("gamestart");            // PlayersAdded to AssignReinforcement

    printLogFile("gamelog.txt");

    std::cout << "--- Adding orders to an OrdersList ---\n";
    OrdersList* oList = new OrdersList();
    oList->attach(logObs);

    oList->addOrder(new Deploy());
    oList->addOrder(new Advance());
    oList->addOrder(new Negotiate());

    printLogFile("gamelog.txt");

    std::cout << "--- Executing orders ---\n";
    Bomb*     bomb     = new Bomb();
    Blockade* blockade = new Blockade();
    Airlift*  airlift  = new Airlift();

    bomb->attach(logObs);
    blockade->attach(logObs);
    airlift->attach(logObs);

    bomb->execute();
    blockade->execute();
    airlift->execute();

    printLogFile("gamelog.txt");

    delete bomb;
    delete blockade;
    delete airlift;
    delete oList;
    delete engine;
    delete logObs;

    std::cout << "Driver complete. Full log written to gamelog.txt.\n";
    return 0;
}
