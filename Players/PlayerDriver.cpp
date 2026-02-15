// PlayerDriver.cpp
// Driver file to test the Player component functionality

#include "Player.h"
#include "../Maps/Map.h"
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"
#include <iostream>

using namespace std;

void testPlayerComponent() {
    cout << "========================================" << endl;
    cout << "    WARZONE PLAYER COMPONENT DRIVER" << endl;
    cout << "========================================" << endl << endl;
    
    // ===== PART 1: Create Players =====
    cout << "===== PART 1: Creating Players =====" << endl;
    
    Player* player1 = new Player("Alice");
    Player* player2 = new Player("Bob");
    
    cout << "Created two players:" << endl;
    cout << "  - " << player1->getName() << endl;
    cout << "  - " << player2->getName() << endl << endl;
    
    // ===== PART 2: Demonstrate Player Owns Territories =====
    cout << "===== PART 2: Player Stores List of Owned Territories =====" << endl;
    
    // Create some territories for testing
    Territory* canada = new Territory("Canada");
    Territory* usa = new Territory("USA");
    Territory* mexico = new Territory("Mexico");
    Territory* brazil = new Territory("Brazil");
    
    cout << "\nCreated territories: Canada, USA, Mexico, Brazil" << endl;
    
    // Assign territories to players
    cout << "\nAssigning territories to " << player1->getName() << "..." << endl;
    player1->getTerritoriesOwned()->push_back(canada);
    player1->getTerritoriesOwned()->push_back(usa);
    
    cout << "Assigning territories to " << player2->getName() << "..." << endl;
    player2->getTerritoriesOwned()->push_back(mexico);
    player2->getTerritoriesOwned()->push_back(brazil);
    
    // Display owned territories
    cout << "\n" << player1->getName() << " owns " 
         << player1->getTerritoriesOwned()->size() << " territories:" << endl;
    for (Territory* t : *(player1->getTerritoriesOwned())) {
        cout << "  - " << t->getName() << endl;
    }
    
    cout << "\n" << player2->getName() << " owns " 
         << player2->getTerritoriesOwned()->size() << " territories:" << endl;
    for (Territory* t : *(player2->getTerritoriesOwned())) {
        cout << "  - " << t->getName() << endl;
    }
    
    cout << "\n✓ VERIFIED: Players store list of owned territories" << endl << endl;
    
    // ===== PART 3: Demonstrate toDefend() Returns List of Territories =====
    cout << "===== PART 3: Demonstrate toDefend() Method =====" << endl;
    
    cout << "\nCalling toDefend() for " << player1->getName() << "..." << endl;
    vector<Territory*> defendList1 = player1->toDefend();
    
    cout << "toDefend() returned " << defendList1.size() << " territories:" << endl;
    for (Territory* t : defendList1) {
        cout << "  - " << t->getName() << endl;
    }
    
    cout << "\nCalling toDefend() for " << player2->getName() << "..." << endl;
    vector<Territory*> defendList2 = player2->toDefend();
    
    cout << "toDefend() returned " << defendList2.size() << " territories:" << endl;
    for (Territory* t : defendList2) {
        cout << "  - " << t->getName() << endl;
    }
    
    cout << "\n✓ VERIFIED: toDefend() can be called and returns list of territories" << endl << endl;
    
    // ===== PART 4: Demonstrate toAttack() Returns List of Territories =====
    cout << "===== PART 4: Demonstrate toAttack() Method =====" << endl;
    
    cout << "\nCalling toAttack() for " << player1->getName() << "..." << endl;
    vector<Territory*> attackList1 = player1->toAttack();
    
    cout << "toAttack() returned " << attackList1.size() << " territories" << endl;
    if (attackList1.empty()) {
        cout << "  (empty list - this is expected for now)" << endl;
    } else {
        for (Territory* t : attackList1) {
            cout << "  - " << t->getName() << endl;
        }
    }
    
    cout << "\nCalling toAttack() for " << player2->getName() << "..." << endl;
    vector<Territory*> attackList2 = player2->toAttack();
    
    cout << "toAttack() returned " << attackList2.size() << " territories" << endl;
    if (attackList2.empty()) {
        cout << "  (empty list - this is expected for now)" << endl;
    }
    
    cout << "\n✓ VERIFIED: toAttack() can be called and returns list of territories" << endl << endl;
    
    // ===== PART 5: Demonstrate Player Stores Hand of Cards =====
    cout << "===== PART 5: Player Stores Hand of Cards =====" << endl;
    
    // Create a deck for drawing cards
    Deck* deck = new Deck();
    
    cout << "\nAdding cards to " << player1->getName() << "'s hand..." << endl;
    deck->draw(player1->getHand());
    deck->draw(player1->getHand());
    deck->draw(player1->getHand());
    
    cout << "\n" << player1->getName() << "'s hand:" << endl;
    cout << *(player1->getHand()) << endl;
    
    cout << "\nAdding cards to " << player2->getName() << "'s hand..." << endl;
    deck->draw(player2->getHand());
    deck->draw(player2->getHand());
    
    cout << "\n" << player2->getName() << "'s hand:" << endl;
    cout << *(player2->getHand()) << endl;
    
    cout << "✓ VERIFIED: Players store hand of cards" << endl << endl;
    
    // ===== PART 6: Demonstrate issueOrder() Creates and Adds Orders =====
    cout << "===== PART 6: Demonstrate issueOrder() Method =====" << endl;
    
    cout << "\n" << player1->getName() << "'s orders before issuing:" << endl;
    cout << *(player1->getOrders()) << endl;
    
    cout << "\nCalling issueOrder() for " << player1->getName() << " (3 times)..." << endl;
    player1->issueOrder();
    player1->issueOrder();
    player1->issueOrder();
    
    cout << "\n" << player1->getName() << "'s orders after issuing:" << endl;
    cout << *(player1->getOrders()) << endl;
    
    if (player1->getOrders()->size() == 3) {
        cout << "✓ VERIFIED: issueOrder() creates orders and adds them to player's list" << endl;
    } else {
        cout << "✗ ERROR: Wrong number of orders" << endl;
    }
    
    cout << "\nCalling issueOrder() for " << player2->getName() << " (2 times)..." << endl;
    player2->issueOrder();
    player2->issueOrder();
    
    cout << "\n" << player2->getName() << "'s orders:" << endl;
    cout << *(player2->getOrders()) << endl << endl;
    
    // ===== PART 7: Demonstrate Stream Insertion Operator =====
    cout << "===== PART 7: Stream Insertion Operator =====" << endl;
    
    cout << "\nUsing stream insertion operator:" << endl;
    cout << *player1;
    cout << *player2;
    
    // Cleanup
    delete canada;
    delete usa;
    delete mexico;
    delete brazil;
    delete deck;
    delete player1;
    delete player2;
}

int main() {
    testPlayerComponent();
    return 0;
}