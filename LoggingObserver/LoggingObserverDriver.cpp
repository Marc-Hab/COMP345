
#include "LoggingObserver.h"
#include "../GameEngine/GameEngine.h"
#include "../Orders/Orders.h"
#include "../CommandProcessor/CommandProcessing.h"
#include "../Players/Player.h"
#include "../Maps/Map.h"
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

    // --- 1. GameEngine::transition() notifies observers ---
    std::cout << "--- GameEngine state transitions ---\n";
    GameEngine* engine = new GameEngine();
    engine->attach(logObs);

    engine->applyCommand("loadmap world.map");   // Start -> MapLoaded
    engine->applyCommand("validatemap");          // MapLoaded -> MapValidated
    engine->applyCommand("addplayer Alice");      // MapValidated -> PlayersAdded
    engine->applyCommand("addplayer Bob");        // PlayersAdded -> PlayersAdded
    engine->applyCommand("gamestart");            // PlayersAdded -> AssignReinforcement

    printLogFile("gamelog.txt");

    // Set up players and territories for orders demo
    Player* alice = new Player("Alice");
    Player* bob   = new Player("Bob");

    Territory* tAlice1 = new Territory("Normandy");
    Territory* tAlice2 = new Territory("Brittany");
    Territory* tBob    = new Territory("Paris");

    // Normandy and Paris are adjacent; Brittany and Normandy are adjacent
    tAlice1->addAdjacentTerritory(tBob);
    tBob->addAdjacentTerritory(tAlice1);
    tAlice1->addAdjacentTerritory(tAlice2);
    tAlice2->addAdjacentTerritory(tAlice1);

    tAlice1->setOwner(alice);  tAlice1->setArmyCount(20);
    tAlice2->setOwner(alice);  tAlice2->setArmyCount(5);
    tBob->setOwner(bob);       tBob->setArmyCount(10);

    alice->getTerritoriesOwned()->push_back(tAlice1);
    alice->getTerritoriesOwned()->push_back(tAlice2);
    bob->getTerritoriesOwned()->push_back(tBob);

    // --- 2. OrdersList::addOrder() notifies observers ---
    std::cout << "--- Adding orders to an OrdersList ---\n";
    OrdersList* oList = new OrdersList();
    oList->attach(logObs);

    oList->addOrder(new Deploy(alice, tAlice1, 5));
    oList->addOrder(new Advance(alice, tAlice1, tAlice2, 3));
    oList->addOrder(new Negotiate(alice, bob));

    printLogFile("gamelog.txt");

    // --- 3. Order::execute() notifies observers ---
    std::cout << "--- Executing orders ---\n";

    Bomb*     bomb     = new Bomb(alice, tBob);
    Blockade* blockade = new Blockade(alice, tAlice2);
    Airlift*  airlift  = new Airlift(alice, tAlice1, tAlice2, 4);

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

    // --- 4. CommandProcessor::saveCommand() notifies observers ---
    std::cout << "--- CommandProcessor saveCommand logging ---\n";
    CommandProcessor* cp = new CommandProcessor();
    cp->attach(logObs);

    // Attach logObs to each command so saveEffect() is also logged.
    Command* cmd1 = new Command("loadmap world.map");
    Command* cmd2 = new Command("validatemap");
    Command* cmd3 = new Command("addplayer Alice");
    cmd1->attach(logObs);
    cmd2->attach(logObs);
    cmd3->attach(logObs);

    // saveCommand pushes the command and calls notify(*this) on the processor.
    cp->saveCommand(cmd1);
    cp->saveCommand(cmd2);
    cp->saveCommand(cmd3);

    printLogFile("gamelog.txt");

    // --- 5. Command::saveEffect() notifies observers ---
    std::cout << "--- Command saveEffect logging ---\n";

    // Valid effect recorded on cmd1.
    cmd1->saveEffect("Map loaded successfully.");

    // Invalid command in wrong state: validate calls saveEffect with an error.
    cp->validate(cmd2, "Start");

    // Valid effect recorded on cmd3.
    cmd3->saveEffect("Player Alice added.");

    printLogFile("gamelog.txt");

    // cmd1/cmd2/cmd3 are owned by cp; deleting cp cleans them up.
    delete cp;
    delete alice;
    delete bob;
    delete tAlice1;
    delete tAlice2;
    delete tBob;
    delete logObs;

    std::cout << "Driver complete. Full log written to gamelog.txt.\n";
    return 0;
}
