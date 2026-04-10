#include <iostream>
#include <vector>

#include "../Players/Player.h"
#include "PlayerStrategies.h"
#include "../Cards/Cards.h"

using namespace std;

void testPlayerStrategies() {
    cout << "\n=== Player Strategies Test ===\n";

    Player* neutralPlayer = new Player("Neutral");
    Player* aggressivePlayer = new Player("Aggressive");
    Player* humanPlayer = new Player("Human");

    neutralPlayer->setStrategy(new NeutralPlayerStrategy());
    aggressivePlayer->setStrategy(new AggressivePlayerStrategy());
    humanPlayer->setStrategy(new HumanPlayerStrategy());

    Deck* deck = new Deck();
    vector<Player*> allPlayers = {neutralPlayer, aggressivePlayer, humanPlayer};

    cout << "\n-- Different Behavior --\n";
    neutralPlayer->issueOrder(deck, &allPlayers);
    aggressivePlayer->issueOrder(deck, &allPlayers);

    cout << "\n-- Dynamic Change --\n";
    neutralPlayer->setStrategy(new AggressivePlayerStrategy());
    neutralPlayer->issueOrder(deck, &allPlayers);

    cout << "\n-- Human vs Computer --\n";
    aggressivePlayer->issueOrder(deck, &allPlayers);
    humanPlayer->issueOrder(deck, &allPlayers);

    delete neutralPlayer;
    delete aggressivePlayer;
    delete humanPlayer;
    delete deck;
}

int main() {
    testPlayerStrategies();
    return 0;
}

