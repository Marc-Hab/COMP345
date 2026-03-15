// OrdersDriver.cpp
// Demonstrates all Part-4 order execution requirements:
//  (1) Each order is validated before execution.
//  (2) Territory ownership transfers when an Advance attack succeeds.
//  (3) One card is given to a player who conquers at least one territory per turn.
//  (4) Negotiate prevents attacks between the two involved players.
//  (5) Blockade transfers ownership to the Neutral player.
//  (6) All six order types can be issued and executed.

#include "Orders.h"
#include "../Maps/Map.h"
#include "../Players/Player.h"
#include "../Cards/Cards.h"
#include <iostream>

using namespace std;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static void printBanner(const string& title) {
    cout << "\n##################################################" << endl;
    cout << "# " << title << endl;
    cout << "##################################################" << endl;
}

static void printState(const vector<Territory*>& territories, const vector<Player*>& players) {
    cout << "\n--- Territory State ---" << endl;
    for (Territory* t : territories) {
        cout << "  " << t->getName()
             << ": owner=" << (t->getOwner() ? t->getOwner()->getName() : "none")
             << ", armies=" << t->getArmyCount() << endl;
    }
    cout << "--- Player State ---" << endl;
    for (Player* p : players) {
        cout << "  " << p->getName()
             << ": " << p->getTerritoriesOwned()->size() << " territories"
             << ", pool=" << p->getReinforcementPool()
             << ", cards=" << p->getHand()->getCards()->size() << endl;
    }
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------
int main() {
    cout << "==========================================" << endl;
    cout << "   WARZONE ORDERS EXECUTION DRIVER" << endl;
    cout << "==========================================" << endl;

    // -----------------------------------------------------------------------
    // Build map: North <-> East <-> South; West <-> South
    //   P1 owns: North (5), South (8), West (10)
    //   P2 owns: East (3)
    // -----------------------------------------------------------------------
    Territory north("North");
    Territory east("East");
    Territory south("South");
    Territory west("West");

    // Set adjacency (bidirectional)
    north.addAdjacentTerritory(&east);
    east.addAdjacentTerritory(&north);
    east.addAdjacentTerritory(&south);
    south.addAdjacentTerritory(&east);
    south.addAdjacentTerritory(&west);
    west.addAdjacentTerritory(&south);

    Player p1("Player1");
    Player p2("Player2");

    // Assign ownership and army counts
    north.setOwner(&p1);  north.setArmyCount(5);
    south.setOwner(&p1);  south.setArmyCount(8);
    west.setOwner(&p1);   west.setArmyCount(10);
    east.setOwner(&p2);   east.setArmyCount(3);

    p1.getTerritoriesOwned()->push_back(&north);
    p1.getTerritoriesOwned()->push_back(&south);
    p1.getTerritoriesOwned()->push_back(&west);
    p2.getTerritoriesOwned()->push_back(&east);

    p1.setReinforcementPool(10);
    p2.setReinforcementPool(5);

    // Give each player a card of each type for demo purposes
    Deck deck;
    deck.draw(p1.getHand()); // draw cards into hands
    deck.draw(p1.getHand());
    deck.draw(p2.getHand());
    deck.draw(p2.getHand());
    p1.getHand()->setPlayer(&p1);
    p2.getHand()->setPlayer(&p2);

    vector<Territory*> allTerritories = {&north, &east, &south, &west};
    vector<Player*> allPlayers = {&p1, &p2};

    cout << "\nInitial state:";
    printState(allTerritories, allPlayers);

    // -----------------------------------------------------------------------
    // DEMO 1: DEPLOY ORDER (valid and invalid cases)
    // -----------------------------------------------------------------------
    printBanner("DEMO 1: DEPLOY ORDER");

    cout << "\n[Valid] P1 deploys 10 armies to North (owns it, pool = 10):" << endl;
    Deploy validDeploy(&p1, &north, 10);
    validDeploy.execute();  // should add 10 armies to North
    p1.setReinforcementPool(0);

    cout << "\n[Invalid] P1 tries to deploy to East (P2 owns it):" << endl;
    Deploy invalidDeploy(&p1, &east, 5);
    invalidDeploy.execute();  // should print INVALID

    cout << "\n[Valid] P2 deploys 5 armies to East:" << endl;
    Deploy p2Deploy(&p2, &east, 5);
    p2Deploy.execute();
    p2.setReinforcementPool(0);

    printState(allTerritories, allPlayers);

    // -----------------------------------------------------------------------
    // DEMO 2: ADVANCE — FRIENDLY MOVE (defend)
    // -----------------------------------------------------------------------
    printBanner("DEMO 2: ADVANCE ORDER — FRIENDLY MOVE");

    cout << "\n[Valid] P1 moves 4 armies from South to West (both P1, adjacent):" << endl;
    Advance defendMove(&p1, &south, &west, 4);
    defendMove.execute();

    cout << "\n[Invalid] P1 tries to advance from East (P2 owns it):" << endl;
    Advance invalidSource(&p1, &east, &north, 2);
    invalidSource.execute();  // should print INVALID

    cout << "\n[Invalid] P1 tries to advance from North to West (not adjacent):" << endl;
    Advance notAdjacent(&p1, &north, &west, 2);
    notAdjacent.execute();  // should print INVALID

    printState(allTerritories, allPlayers);

    // -----------------------------------------------------------------------
    // DEMO 3 & DEMO 2 (ownership transfer): ADVANCE — ATTACK
    // -----------------------------------------------------------------------
    printBanner("DEMO 3: ADVANCE ORDER — ATTACK (ownership transfer if successful)");

    // Run multiple attacks until P1 captures East (or show what happens either way)
    // Give North enough armies to ensure a likely victory for demo clarity
    north.setArmyCount(20);
    east.setArmyCount(2);  // fewer defenders to make capture likely

    cout << "\n[Valid] P1 attacks East from North (North=" << north.getArmyCount()
         << " armies vs East=" << east.getArmyCount() << " armies):" << endl;
    // Run up to 3 attempts to show at least one capture for the demo
    bool captured = false;
    for (int attempt = 1; attempt <= 3 && !captured; attempt++) {
        north.setArmyCount(20);
        east.setArmyCount(2);
        if (east.getOwner() != &p1) {
            east.setOwner(&p2);
            if (find(p2.getTerritoriesOwned()->begin(), p2.getTerritoriesOwned()->end(), &east)
                    == p2.getTerritoriesOwned()->end()) {
                p2.getTerritoriesOwned()->push_back(&east);
            }
        }
        p1.resetOrderIssuingState(); // clear conquered flag between attempts
        cout << "\n  Attempt " << attempt << ":" << endl;
        Advance attack(&p1, &north, &east, 15);
        attack.execute();
        if (east.getOwner() == &p1) {
            captured = true;
            cout << "  >> P1 captured East! Requirement (2) demonstrated." << endl;
        }
    }
    if (!captured) {
        cout << "  >> East was not captured across 3 attempts (probability-based)." << endl;
    }

    printState(allTerritories, allPlayers);

    // -----------------------------------------------------------------------
    // DEMO 3 (continued): CARD REWARD for conquest
    // -----------------------------------------------------------------------
    printBanner("DEMO 3 (continued): CARD REWARD FOR CONQUEST");

    cout << "\nChecking conquest flag after battle:" << endl;
    if (p1.getConqueredThisTurn()) {
        cout << "  P1 conquered a territory this turn — awarding one card:" << endl;
        size_t cardsBefore = p1.getHand()->getCards()->size();
        if (!deck.getCards()->empty()) {
            deck.draw(p1.getHand());
        }
        cout << "  P1 cards: " << cardsBefore << " -> " << p1.getHand()->getCards()->size() << endl;
        cout << "  (Only one card awarded per turn, regardless of conquests count.)" << endl;
        p1.setConqueredThisTurn(false);
    } else {
        cout << "  P1 did not conquer a territory this turn (random combat) — no card." << endl;
    }

    // -----------------------------------------------------------------------
    // DEMO 4: NEGOTIATE ORDER prevents attacks
    // -----------------------------------------------------------------------
    printBanner("DEMO 4: NEGOTIATE ORDER — PREVENTS ATTACKS");

    // Reset state for clean demo
    if (east.getOwner() != &p2) {
        if (east.getOwner() == &p1) {
            auto& p1List = *p1.getTerritoriesOwned();
            p1List.erase(remove(p1List.begin(), p1List.end(), &east), p1List.end());
        }
        east.setOwner(&p2);
        if (find(p2.getTerritoriesOwned()->begin(), p2.getTerritoriesOwned()->end(), &east)
                == p2.getTerritoriesOwned()->end()) {
            p2.getTerritoriesOwned()->push_back(&east);
        }
    }
    north.setArmyCount(15);
    east.setArmyCount(3);
    p1.resetOrderIssuingState();
    p2.resetOrderIssuingState();

    cout << "\n[Valid] P1 negotiates with P2:" << endl;
    Negotiate truce(&p1, &p2);
    truce.execute();

    cout << "\n[Now Invalid] P1 tries to attack East from North (truce is active):" << endl;
    Advance blockedAttack(&p1, &north, &east, 10);
    blockedAttack.execute();  // should print INVALID — truce prevents attack

    cout << "\n[Invalid] P2 tries to attack North from East (truce blocks both sides):" << endl;
    Advance blockedReverse(&p2, &east, &north, 2);
    blockedReverse.execute();  // should print INVALID

    cout << "\n[Invalid] Negotiate with yourself:" << endl;
    Negotiate selfNegotiate(&p1, &p1);
    selfNegotiate.execute();  // should print INVALID

    // -----------------------------------------------------------------------
    // DEMO 5: BLOCKADE ORDER transfers ownership to Neutral
    // -----------------------------------------------------------------------
    printBanner("DEMO 5: BLOCKADE ORDER — TRANSFERS TO NEUTRAL");

    cout << "\nBefore blockade: West owner=" << west.getOwner()->getName()
         << ", armies=" << west.getArmyCount() << endl;

    cout << "\n[Valid] P1 blockades West (P1 owns it):" << endl;
    Blockade blockade(&p1, &west);
    blockade.execute();

    cout << "\nAfter blockade: West owner=" << west.getOwner()->getName()
         << ", armies=" << west.getArmyCount()
         << " (doubled and transferred to Neutral)" << endl;

    cout << "\n[Invalid] P1 tries to blockade East (P2 owns it):" << endl;
    Blockade invalidBlockade(&p1, &east);
    invalidBlockade.execute();  // should print INVALID

    printState(allTerritories, allPlayers);

    // -----------------------------------------------------------------------
    // DEMO 6: BOMB and AIRLIFT orders
    // -----------------------------------------------------------------------
    printBanner("DEMO 6: BOMB ORDER");
    
    p1.resetOrderIssuingState();
    p2.resetOrderIssuingState();
    
    east.setArmyCount(10);
    cout << "\nBefore bomb: East armies=" << east.getArmyCount() << endl;
    cout << "\n[Valid] P1 bombs East from North (adjacent enemy territory):" << endl;
    Bomb bomb(&p1, &east);
    bomb.execute();
    cout << "After bomb: East armies=" << east.getArmyCount()
         << " (halved)" << endl;

    cout << "\n[Invalid] P1 tries to bomb North (own territory):" << endl;
    Bomb selfBomb(&p1, &north);
    selfBomb.execute();  // should print INVALID

    // Bomb from non-adjacent territory
    cout << "\n[Invalid] P2 tries to bomb West (not adjacent to any P2 territory):" << endl;
    Bomb notAdjBomb(&p2, &west);
    notAdjBomb.execute();  // should print INVALID — East adj South adj West, but East not adj West

    printBanner("DEMO 6 (cont.): AIRLIFT ORDER");

    south.setArmyCount(12);
    north.setArmyCount(3);
    cout << "\nBefore airlift: South armies=" << south.getArmyCount()
         << ", North armies=" << north.getArmyCount()
         << " (not adjacent)" << endl;

    cout << "\n[Valid] P1 airlifts 6 armies from South to North (non-adjacent, both P1):" << endl;
    Airlift airlift(&p1, &south, &north, 6);
    airlift.execute();
    cout << "After airlift: South=" << south.getArmyCount()
         << ", North=" << north.getArmyCount() << endl;

    cout << "\n[Invalid] P1 tries to airlift to East (P2 owns it):" << endl;
    Airlift invalidAirlift(&p1, &south, &east, 3);
    invalidAirlift.execute();  // should print INVALID

    // -----------------------------------------------------------------------
    // DEMO 6 (cont.): ORDERLIST — move, remove, copy, executeAll
    // -----------------------------------------------------------------------
    printBanner("DEMO 6 (cont.): ORDERSLIST — MOVE, REMOVE, COPY");

    // Restore east to P2 for clean copy demo
    if (east.getOwner() != &p2) {
        east.setOwner(&p2);
    }
    east.setArmyCount(4);

    OrdersList olist;
    olist.addOrder(new Deploy(&p1, &north, 3));
    olist.addOrder(new Advance(&p1, &south, &east, 5));
    olist.addOrder(new Bomb(&p1, &east));
    olist.addOrder(new Airlift(&p1, &south, &north, 2));
    olist.addOrder(new Negotiate(&p1, &p2));
    cout << "\nInitial OrdersList:\n" << olist;

    cout << "\nMoving order at index 4 (Negotiate) to index 0:" << endl;
    olist.move(4, 0);
    cout << olist;

    cout << "Removing order at index 2:" << endl;
    olist.remove(2);
    cout << olist;

    cout << "Deep copy of OrdersList:" << endl;
    OrdersList copy(olist);
    cout << copy;

    cout << "\nExecuting all orders in the list:" << endl;
    p1.resetOrderIssuingState();
    p2.resetOrderIssuingState();
    north.setArmyCount(5);
    east.setArmyCount(4);
    east.setOwner(&p2);
    south.setArmyCount(10);
    olist.executeAll();

    cout << "\n==========================================" << endl;
    cout << "   ALL DEMO REQUIREMENTS SHOWN" << endl;
    cout << "==========================================" << endl;
    return 0;
}
