#include "GameEngine.h"
#include <algorithm>
#include <random>

// Constructor initializes map and deck
GameEngine::GameEngine() {
    map = nullptr;
    deck = new Deck();
}

// Destructor cleans allocated memory
GameEngine::~GameEngine() {

    delete deck;

    // delete all dynamically allocated players
    for (Player* p : players) {
        delete p;
    }
}

// Handles the command loop for the startup phase
void GameEngine::startupPhase() {

    std::string command;

    while (true) {

        std::cout << "Enter command: ";
        std::cin >> command;

        if (command == "loadmap") {

            std::string filename;
            std::cin >> filename;

            loadMap(filename);
        }

        else if (command == "validatemap") {

            validateMap();
        }

        else if (command == "addplayer") {

            std::string name;
            std::cin >> name;

            addPlayer(name);
        }

        else if (command == "gamestart") {

            gameStart();
            break;
        }

        else {

            std::cout << "Invalid command\n";
        }
    }
}

// Loads a map file (placeholder until MapLoader is integrated)
void GameEngine::loadMap(std::string filename) {

    std::cout << "Loading map: " << filename << std::endl;

    // temporary map creation until real loader is used
    map = new Map();
}

// Validates the current map
void GameEngine::validateMap() {

    if (map && map->validate())
        std::cout << "Map is valid\n";
    else
        std::cout << "Map is not valid\n";
}

// Adds a player to the game (2–6 players allowed)
void GameEngine::addPlayer(std::string name) {

    if (players.size() >= 6) {
        std::cout << "Maximum 6 players allowed\n";
        return;
    }

    Player* p = new Player(name);
    players.push_back(p);

    std::cout << "Player added: " << name << std::endl;
}

// Starts the game after setup
void GameEngine::gameStart() {

    if (players.size() < 2) {

        std::cout << "Need at least 2 players\n";
        return;
    }

    // Placeholder for territory distribution
    std::cout << "Distributing territories to players...\n";

    // Randomize the order of players
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(players.begin(), players.end(), g);

    std::cout << "Player order:\n";

    for (Player* p : players)
        std::cout << p->getName() << std::endl;

    // Give each player 50 initial armies
    for (Player* p : players)
        p->setReinforcementPool(50);

    // Each player draws 2 cards
    for (Player* p : players) {

        p->getHand()->addCard(deck->draw());
        p->getHand()->addCard(deck->draw());
    }

    std::cout << "Game started\n";
}
