#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <vector>
#include <string>
#include <iostream>

#include "Map.h"
#include "Player.h"
#include "Cards.h"

// GameEngine controls the game startup phase
class GameEngine {

private:

    Map* map;                         // pointer to the game map
    std::vector<Player*> players;     // list of players
    Deck* deck;                       // deck of cards

public:

    // constructor / destructor
    GameEngine();
    ~GameEngine();

    // handles the startup command loop
    void startupPhase();

    // startup commands
    void loadMap(std::string filename);
    void validateMap();
    void addPlayer(std::string name);
    void gameStart();
};

#endif
