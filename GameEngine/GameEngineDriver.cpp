#include "GameEngine.h"
#include "../CommandProcessor/CommandProcessing.h"
#include "../Maps/Map.h"
#include "../Players/Player.h"
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

// ---------------------------------------------------------------------------
// Helper: print territory ownership summary
// ---------------------------------------------------------------------------
static void printOwnership(GameEngine* engine) {
    cout << "\n--- Territory Ownership Summary ---" << endl;
    for (Player* p : *engine->getPlayers()) {
        cout << "  " << p->getName()
             << ": " << p->getTerritoriesOwned()->size() << " territories"
             << ", pool: " << p->getReinforcementPool() << endl;
    }
}

// ---------------------------------------------------------------------------
// Main driver — demonstrates all 6 Part-3 requirements
// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) {

    // -----------------------------------------------------------------------
    // Setup: startup phase via file commands or interactive console
    // -----------------------------------------------------------------------
    CommandProcessor* processor = nullptr;

    if (argc >= 3 && strcmp(argv[1], "-file") == 0) {
        processor = new FileCommandProcessorAdapter(argv[2]);
    } else if (argc >= 2 && strcmp(argv[1], "-console") == 0) {
        processor = new CommandProcessor();
    } else {
        // Default: use bundled demo command file
        processor = new FileCommandProcessorAdapter("demo_commands.txt");
    }

    FileCommandProcessorAdapter* fileProc =
        dynamic_cast<FileCommandProcessorAdapter*>(processor);
    if (fileProc && !fileProc->isOpen()) {
        cout << "ERROR: Could not open command file." << endl;
        cout << "Usage: " << argv[0] << " [-console | -file <filename>]" << endl;
        delete fileProc;
        return 1;
    }

    GameEngine* engine = new GameEngine(processor);

    // Run startup phase (loadmap -> validatemap -> addplayer(s) -> gamestart)
    engine->startupPhase();

    if (engine->getState() != GameState::AssignReinforcement) {
        cout << "Startup did not complete. Exiting." << endl;
        delete engine;
        return 1;
    }

    // -----------------------------------------------------------------------
    // DEMO 1: Reinforcement Phase — correct army counts, different cases
    // -----------------------------------------------------------------------
    cout << "\n\n##################################################" << endl;
    cout << "# DEMO 1: REINFORCEMENT PHASE                   #" << endl;
    cout << "# Shows: correct armies per territory count     #" << endl;
    cout << "# and continent control bonuses (min 3)         #" << endl;
    cout << "##################################################" << endl;
    printOwnership(engine);
    engine->reinforcementPhase();

    // -----------------------------------------------------------------------
    // DEMO 2 & 3 & 4: Issue Orders Phase
    //   2 — deploy-only while reinforcement pool > 0
    //   3 — advance orders to defend and attack after deploying
    //   4 — card played after advances
    // -----------------------------------------------------------------------
    cout << "\n\n##################################################" << endl;
    cout << "# DEMO 2-4: ISSUE ORDERS PHASE                  #" << endl;
    cout << "# 2: Deploy-only while armies remain in pool    #" << endl;
    cout << "# 3: Advance (defend then attack) after deploy  #" << endl;
    cout << "# 4: Card played from hand                      #" << endl;
    cout << "##################################################" << endl;
    engine->issueOrdersPhase();

    // Show each player's order list
    cout << "\n--- Orders queued ---" << endl;
    for (Player* p : *engine->getPlayers()) {
        cout << *p;
    }

    // Execute the orders we just issued
    engine->executeOrdersPhase();
    printOwnership(engine);

    // -----------------------------------------------------------------------
    // DEMO 5: Player elimination — player with no territories is removed
    // -----------------------------------------------------------------------
    cout << "\n\n##################################################" << endl;
    cout << "# DEMO 5: PLAYER ELIMINATION                    #" << endl;
    cout << "# A player who controls no territories is       #" << endl;
    cout << "# removed from the game.                        #" << endl;
    cout << "##################################################" << endl;

    // Force the last player (in play order) to lose all territories
    vector<Player*>* players = engine->getPlayers();
    if (players->size() >= 2) {
        Player* loser    = players->back();
        Player* receiver = players->front();

        cout << "\nForcing " << loser->getName()
             << " to lose all territories to " << receiver->getName() << "..." << endl;

        for (Territory* t : *loser->getTerritoriesOwned()) {
            t->setOwner(receiver);
            receiver->getTerritoriesOwned()->push_back(t);
        }
        loser->getTerritoriesOwned()->clear();
        cout << loser->getName() << " now has 0 territories." << endl;
    }

    // Run one full round — the loop will detect and eliminate the territory-less player
    cout << "\nRunning one game round to trigger elimination check..." << endl;
    engine->reinforcementPhase();
    engine->issueOrdersPhase();
    engine->executeOrdersPhase();

    // Manually trigger elimination (same logic as inside mainGameLoop)
    {
        auto it = players->begin();
        while (it != players->end()) {
            if ((*it)->getTerritoriesOwned()->empty()) {
                cout << "\n*** " << (*it)->getName()
                     << " controls no territories and is eliminated! ***" << endl;
                delete *it;
                it = players->erase(it);
            } else {
                ++it;
            }
        }
    }
    printOwnership(engine);

    // -----------------------------------------------------------------------
    // DEMO 6: Win condition — game ends when one player owns all territories
    // -----------------------------------------------------------------------
    cout << "\n\n##################################################" << endl;
    cout << "# DEMO 6: WIN CONDITION                         #" << endl;
    cout << "# Game ends when one player controls all        #" << endl;
    cout << "# territories.                                  #" << endl;
    cout << "##################################################" << endl;

    if (players->size() >= 2) {
        // Give all territories to the first player
        Player* winner = players->front();
        cout << "\nGiving all territories to " << winner->getName()
             << " to trigger win condition..." << endl;

        for (size_t i = 1; i < players->size(); ++i) {
            Player* loser = (*players)[i];
            for (Territory* t : *loser->getTerritoriesOwned()) {
                t->setOwner(winner);
                winner->getTerritoriesOwned()->push_back(t);
            }
            loser->getTerritoriesOwned()->clear();
        }
    }

    // One reinforcement phase to show the winner's army count, then mainGameLoop
    // detects the win.
    engine->reinforcementPhase();

    // Check win: if one player owns everything, announce winner
    int totalTerr = static_cast<int>(engine->getMap()->getTerritories()->size());
    for (Player* p : *engine->getPlayers()) {
        if (static_cast<int>(p->getTerritoriesOwned()->size()) == totalTerr) {
            cout << "\n========================================" << endl;
            cout << "  WINNER: " << p->getName()
                 << " controls all " << totalTerr << " territories!" << endl;
            cout << "========================================" << endl;
            break;
        }
    }

    delete engine;

    // -----------------------------------------------------------------------
    // DEMO 7: Run the entire game loop with the console
    // -----------------------------------------------------------------------

    CommandProcessor* processor2 = new CommandProcessor();
    GameEngine* engine2 = new GameEngine(processor2);

    (*engine2).run();

    delete engine2;
    return 0;
}
