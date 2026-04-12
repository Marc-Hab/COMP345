#include "PlayerStrategies.h"
#include "../Players/Player.h"
#include "../Maps/Map.h"
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"

#include <iostream>
#include <vector>

using namespace std;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void printSeparator(const string& title) {
    cout << "\n============================================================\n";
    cout << "  " << title << "\n";
    cout << "============================================================\n";
}

// Prints the orders list of a player 
static void printOrders(Player* p) {
    cout << "\nOrders queued for " << p->getName() << " (" << p->getOrders()->size() << " total):\n";
    if (p->getOrders()->size() == 0) {
        cout << "  (none)\n";
    } else {
        for (int i = 0; i < p->getOrders()->size(); i++) {
            cout << "  " << (i + 1) << ". " << *p->getOrders()->orderAt(i) << "\n";
        }
    }
}

// Run one full order-issuing turn for a player and print the result
static void runTurn(Player* player, Deck* deck, const vector<Player*>* allPlayers) {
    player->resetOrderIssuingState();
    int calls = 0;
    while (player->issueOrder(deck, allPlayers)) {
        calls++;
        if (calls > 20) break; // safety guard
    }
    printOrders(player);
    // Clear orders so each demo starts fresh
    while (player->getOrders()->size() > 0)
        player->getOrders()->remove(0);
}

// ---------------------------------------------------------------------------
// Main driver
// ---------------------------------------------------------------------------

int main() {
    cout << "============================================================\n";
    cout << "           PLAYER STRATEGIES DRIVER\n";
    cout << "============================================================\n";

    // ── Shared resources ──────────────────────────────────────────────────
    Deck deck;
    vector<Player*> allPlayers;

    // ── Territory setup ───────────────────────────────────────────────────
    // Layout (chain with a side branch):
    //
    //   [Enemy1] -- [Home1] -- [Home2] -- [Enemy2]
    //                               \-- [Home3]
    //
    // Reused across demos by resetting ownership and army counts.

    Territory home1("Home-Alpha");   // strong owned territory
    Territory home2("Home-Beta");    // weaker owned territory
    Territory home3("Home-Gamma");   // weakest owned territory
    Territory enemy1("Enemy-West");  // enemy adjacent to Home1
    Territory enemy2("Enemy-East");  // enemy adjacent to Home2

    // Bidirectional adjacency
    home1.addAdjacentTerritory(&enemy1);
    enemy1.addAdjacentTerritory(&home1);

    home1.addAdjacentTerritory(&home2);
    home2.addAdjacentTerritory(&home1);

    home2.addAdjacentTerritory(&enemy2);
    enemy2.addAdjacentTerritory(&home2);

    home2.addAdjacentTerritory(&home3);
    home3.addAdjacentTerritory(&home2);

    // Placeholder enemy player that owns enemy territories
    Player enemy("EnemyPlayer");
    allPlayers.push_back(&enemy);
    enemy1.setOwner(&enemy); enemy1.setArmyCount(3);
    enemy2.setOwner(&enemy); enemy2.setArmyCount(2);
    enemy.getTerritoriesOwned()->push_back(&enemy1);
    enemy.getTerritoriesOwned()->push_back(&enemy2);

    // ── Helper lambda: reset territory ownership for the demo player ───────
    auto assignTerritories = [&](Player* p) {
        // Clear previous ownership
        home1.setOwner(p); home1.setArmyCount(8);
        home2.setOwner(p); home2.setArmyCount(3);
        home3.setOwner(p); home3.setArmyCount(1);
        p->getTerritoriesOwned()->clear();
        p->getTerritoriesOwned()->push_back(&home1);
        p->getTerritoriesOwned()->push_back(&home2);
        p->getTerritoriesOwned()->push_back(&home3);
    };

    // Give the enemy player a card
    deck.draw(enemy.getHand());

    // =========================================================================
    // DEMO 1 — Aggressive Strategy
    // =========================================================================
    printSeparator("DEMO 1: Aggressive Player Strategy");
    cout << "Behavior: deploys all reinforcements to its STRONGEST territory,\n"
         << "then advances that territory's armies to the weakest adjacent enemy.\n";

    Player aggressive("AggressiveBot");
    aggressive.setStrategy(new AggressivePlayerStrategy());
    allPlayers.push_back(&aggressive);
    assignTerritories(&aggressive);
    aggressive.setReinforcementPool(6);
    deck.draw(aggressive.getHand()); // give a card to play

    cout << "\nTerritories: Home-Alpha(8), Home-Beta(3), Home-Gamma(1)\n"
         << "Reinforcements: 6\n"
         << "Enemy territories adjacent: Enemy-West(3 adj Home-Alpha), Enemy-East(2 adj Home-Beta)\n";

    runTurn(&aggressive, &deck, &allPlayers);

    // =========================================================================
    // DEMO 2 — Benevolent Strategy
    // =========================================================================
    printSeparator("DEMO 2: Benevolent Player Strategy");
    cout << "Behavior: deploys all reinforcements to its WEAKEST territory,\n"
         << "then advances armies from strongest to an adjacent weaker own territory.\n"
         << "NEVER attacks enemy territories.\n";

    Player benevolent("BenevolentBot");
    benevolent.setStrategy(new BenevolentPlayerStrategy());
    allPlayers.push_back(&benevolent);
    assignTerritories(&benevolent);
    benevolent.setReinforcementPool(6);
    deck.draw(benevolent.getHand());

    cout << "\nTerritories: Home-Alpha(8), Home-Beta(3), Home-Gamma(1)\n"
         << "Reinforcements: 6\n";

    runTurn(&benevolent, &deck, &allPlayers);

    // =========================================================================
    // DEMO 3 — Neutral Strategy
    // =========================================================================
    printSeparator("DEMO 3: Neutral Player Strategy");
    cout << "Behavior: issues NO orders, regardless of reinforcements or card hand.\n"
         << "If attacked, will switch to Aggressive (see Demo 5).\n";

    Player neutral("NeutralBot");
    neutral.setStrategy(new NeutralPlayerStrategy());
    allPlayers.push_back(&neutral);
    assignTerritories(&neutral);
    neutral.setReinforcementPool(6);
    deck.draw(neutral.getHand());

    cout << "\nTerritories: Home-Alpha(8), Home-Beta(3), Home-Gamma(1)\n"
         << "Reinforcements: 6\n";

    runTurn(&neutral, &deck, &allPlayers);

    // =========================================================================
    // DEMO 4 — Cheater Strategy
    // =========================================================================
    printSeparator("DEMO 4: Cheater Player Strategy");
    cout << "Behavior: automatically conquers ALL adjacent enemy territories once per turn\n"
         << "(no battle — ownership is transferred directly).\n";

    Player cheater("CheaterBot");
    cheater.setStrategy(new CheaterPlayerStrategy());
    allPlayers.push_back(&cheater);
    assignTerritories(&cheater);
    cheater.setReinforcementPool(6);

    // Give enemy territories back to enemy so cheater can conquer them
    enemy1.setOwner(&enemy); enemy1.setArmyCount(3);
    enemy2.setOwner(&enemy); enemy2.setArmyCount(2);
    enemy.getTerritoriesOwned()->clear();
    enemy.getTerritoriesOwned()->push_back(&enemy1);
    enemy.getTerritoriesOwned()->push_back(&enemy2);

    cout << "\nTerritories: Home-Alpha(8), Home-Beta(3), Home-Gamma(1)\n"
         << "Enemy territories adjacent: Enemy-West(3), Enemy-East(2)\n"
         << "Reinforcements: 6\n";

    cheater.resetOrderIssuingState();
    int calls = 0;
    while (cheater.issueOrder(&deck, &allPlayers)) {
        if (++calls > 20) break;
    }
    cout << "\nAfter cheater's turn — territory ownership:\n";
    cout << "  CheaterBot owns: ";
    for (Territory* t : *cheater.getTerritoriesOwned())
        cout << t->getName() << "(" << t->getArmyCount() << ") ";
    cout << "\n";
    cout << "  EnemyPlayer owns: ";
    for (Territory* t : *enemy.getTerritoriesOwned())
        cout << t->getName() << " ";
    if (enemy.getTerritoriesOwned()->empty()) cout << "(none — all conquered)";
    cout << "\n";

    // Restore enemy territories for remaining demos
    enemy1.setOwner(&enemy); enemy1.setArmyCount(3);
    enemy2.setOwner(&enemy); enemy2.setArmyCount(2);
    enemy.getTerritoriesOwned()->clear();
    enemy.getTerritoriesOwned()->push_back(&enemy1);
    enemy.getTerritoriesOwned()->push_back(&enemy2);

    // =========================================================================
    // DEMO 5 — Dynamic Strategy Change: Neutral → Aggressive when attacked
    // =========================================================================
    printSeparator("DEMO 5: Dynamic Strategy Change (Neutral → Aggressive)");
    cout << "Demonstrates requirement (2): a player's strategy can be changed at runtime.\n"
         << "Scenario: NeutralBot sits idle. An enemy captures one of its territories.\n"
         << "          The game engine detects the attack and switches NeutralBot to Aggressive.\n"
         << "          Next turn, NeutralBot attacks aggressively.\n";

    Player switcher("SwitchBot");
    switcher.setStrategy(new NeutralPlayerStrategy());
    allPlayers.push_back(&switcher);
    assignTerritories(&switcher);
    switcher.setReinforcementPool(4);

    cout << "\n--- Turn 1 (Neutral) ---\n";
    cout << "Strategy before attack: NeutralPlayerStrategy\n";
    runTurn(&switcher, &deck, &allPlayers);

    cout << "\n--- Simulating attack: enemy captures Home-Gamma from SwitchBot ---\n";
    // Simulate a successful capture (as Advance::execute would do)
    switcher.getTerritoriesOwned()->erase(
        remove(switcher.getTerritoriesOwned()->begin(),
               switcher.getTerritoriesOwned()->end(),
               &home3),
        switcher.getTerritoriesOwned()->end());
    home3.setOwner(&enemy);
    enemy.getTerritoriesOwned()->push_back(&home3);
    switcher.isAttacked();

    cout << "\n--- Turn 2 (now Aggressive) ---\n";
    assignTerritories(&switcher); // restore for second turn
    switcher.setReinforcementPool(4);
    deck.draw(switcher.getHand());
    runTurn(&switcher, &deck, &allPlayers);

    // =========================================================================
    // DEMO 6 — Same Player, Multiple Strategies in Sequence
    // =========================================================================
    printSeparator("DEMO 6: Strategy Pattern — Same Player, Different Strategies");
    cout << "Demonstrates requirement (1): different strategies assigned to the same player\n"
         << "produce different behavior.\n";

    Player shapeshifter("ShiftBot");
    allPlayers.push_back(&shapeshifter);

    const string strategyNames[] = {
        "AggressivePlayerStrategy",
        "BenevolentPlayerStrategy",
        "NeutralPlayerStrategy"
    };
    PlayerStrategy* strategies[] = {
        new AggressivePlayerStrategy(),
        new BenevolentPlayerStrategy(),
        new NeutralPlayerStrategy()
    };

    for (int i = 0; i < 3; i++) {
        cout << "\n--- ShiftBot with " << strategyNames[i] << " ---\n";
        shapeshifter.setStrategy(strategies[i]);
        assignTerritories(&shapeshifter);
        shapeshifter.setReinforcementPool(5);
        runTurn(&shapeshifter, &deck, &allPlayers);
    }

    // =========================================================================
    // DEMO 7 — Human Strategy (requires user input)
    // =========================================================================
    printSeparator("DEMO 7: Human Player Strategy (interactive)");
    cout << "Demonstrates requirement (3): human player makes decisions via user interaction.\n"
         << "You will be prompted to deploy armies and then choose further actions.\n";

    Player human("HumanPlayer");
    // HumanPlayerStrategy is the default strategy in Player constructor
    allPlayers.push_back(&human);

    // Set up fresh territories for human demo
    Territory humanBase("Human-Citadel");
    Territory humanFrontier("Human-Frontier");
    Territory humanEnemy("Rival-Outpost");

    humanBase.addAdjacentTerritory(&humanFrontier);
    humanFrontier.addAdjacentTerritory(&humanBase);
    humanFrontier.addAdjacentTerritory(&humanEnemy);
    humanEnemy.addAdjacentTerritory(&humanFrontier);

    humanBase.setOwner(&human);      humanBase.setArmyCount(5);
    humanFrontier.setOwner(&human);  humanFrontier.setArmyCount(2);
    humanEnemy.setOwner(&enemy);     humanEnemy.setArmyCount(3);

    human.getTerritoriesOwned()->push_back(&humanBase);
    human.getTerritoriesOwned()->push_back(&humanFrontier);
    enemy.getTerritoriesOwned()->push_back(&humanEnemy);

    human.setReinforcementPool(5);
    deck.draw(human.getHand());
    deck.draw(human.getHand());

    cout << "\nYour territories: Human-Citadel(5 armies), Human-Frontier(2 armies)\n"
         << "Enemy adjacent to Frontier: Rival-Outpost(3 armies)\n"
         << "Reinforcements: 5 | Cards in hand: 2\n\n";

    runTurn(&human, &deck, &allPlayers);

    // ── Cleanup ───────────────────────────────────────────────────────────────
    cout << "\n============================================================\n";
    cout << "  PLAYER STRATEGIES DRIVER COMPLETE\n";
    cout << "============================================================\n";

    return 0;
}
