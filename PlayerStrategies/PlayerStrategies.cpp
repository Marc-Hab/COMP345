#include "PlayerStrategies.h"
#include "../Maps/Map.h"
#include "../Players/Player.h"
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <cstdlib>

using namespace std;

//---- Helpers -----------------------

/*
* Reads an integer from cin.
* Re-prompts if invalid input.
*/
static int readChoice(int min, int max) {
    int choice;
    while (true) {
        cin >> choice;
        if (cin.fail() || choice < min || choice > max) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Enter a number between " << min << " and " << max << ": ";
        } else {
            cin.ignore(1000, '\n');
            return choice;
        }
    }
}

//------------------------------------------


//---- PlayerStrategy -----------------------

/*
* Stream insertion operator for strategies.
* Delegates to the polymorphic print method.  
*/
ostream& operator<<(ostream& os, const PlayerStrategy& ps){
    return ps.print(os);
}


//------------------------------------------


//---- HumanPlayerStrategy -----------------------

/*
* Attack Strategy: 
* Return all adjacent enemy territories sorted by army count in ASC order
*/
vector<Territory*> HumanPlayerStrategy::toAttack(const Player* player){
    vector<Territory*> attackTargets;
    
    for (Territory* t : *(player->getTerritoriesOwned())) {
        for (Territory* adj : *t->getAdjacentTerritories()) {
            
            if (adj->getOwner() != player) { // if the player does not own the territory
                
                bool alreadyListed = false;
                
                for (Territory* target : attackTargets) { // check if territory already in attack targets
                    if (target == adj) { alreadyListed = true; break; }
                }
                
                if (!alreadyListed) {
                    attackTargets.push_back(adj);
                }
            }
        }
    }
    sort(attackTargets.begin(), attackTargets.end(), [](Territory* a, Territory* b) {
        return a->getArmyCount() < b->getArmyCount();
    });
    return attackTargets;
};


/*
* Defend Strategy: 
* Return all owned territories sorted by army count in ASC order
*/
vector<Territory*> HumanPlayerStrategy::toDefend(const Player* player){
    vector<Territory*> defend = *(player->getTerritoriesOwned());
    
    sort(defend.begin(), defend.end(), [](Territory* a, Territory* b) {
        return a->getArmyCount() < b->getArmyCount();
    });
    
    return defend;
};


/*
* Order Issuing Strategy: 
* 1: Deploy all reinforcements before doing anything else.
* 2: Choose to play a card or advance until done.
*/
bool HumanPlayerStrategy::issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) {
    
    string playerName = player->getName();

    vector<Territory*> tOwned = toDefend(player); // sorted weakest first

    // ── Phase 1: Deploy Reinforcements Until None Left ───────────────────────────────────────
    
    if (player->getReinforcementPool() > 0) {

        // Store the number of reinforcements to be deployed in each territory 
        // (not actually deployed until execution phase)
        map<Territory*, int> pendingArmies;

        while (player->getReinforcementPool() > 0) {
            
            cout << "\n[" << playerName << "] Reinforcements remaining: " << player->getReinforcementPool() << "\n";
            
            // List player's territories
            cout << "Your territories:\n";
            
            for (int i = 0; i < (int)tOwned.size(); i++) {
                
                cout << "  " << (i + 1) << ". " << tOwned[i]->getName() << " (" << tOwned[i]->getArmyCount() << " armies, " 
                     << pendingArmies[tOwned[i]] << " to be deployed" << ")\n";
            }
            
            // Quit game option
            cout << "  0. Quit game\n";
            
            cout << "Choose territory to deploy to (1-" << tOwned.size() << "): ";
            int tChoice = readChoice(0, (int)tOwned.size());
            
            if (tChoice == 0) {
                cout << "\nQuitting game...\n";
                exit(0);
            }


            Territory* target = tOwned[tChoice - 1];

            cout << "How many armies to deploy? (1-" << player->getReinforcementPool() << "): ";
            int armies = readChoice(1, player->getReinforcementPool());

            cout << "\n" << playerName << " orders " << armies << " armies to deploy to " << target->getName() << ".\n";
            
            player->getOrders()->addOrder(new Deploy(player, target, armies));
            player->setReinforcementPool(player->getReinforcementPool() - armies);
            pendingArmies[target] += armies;
        }

        cout << "[" << playerName << "] All reinforcements deployed.\n";
        return true;
    }

    // ── Phase 2: Play Card or Advance Until Done ─────────────────────────────────────────────────
    
    vector<Card*>* cards = player->getHand()->getCards();
    
    
    cout << "\n[" << playerName << "] Choose an action:\n"
         << "  1. Done issuing orders\n"
         << "  2. Advance armies\n"
         << "  3. Play a card (" << cards->size() << " in hand)\n"
         << "  0. Quit game\n"
         << "Choice: ";

    int choice = readChoice(0, 3);

    if (choice == 0) {
        cout << "\nQuitting game...\n";
        exit(0);
    }
    if (choice == 1) {
        cout << "\n" << playerName << " is done issuing orders.\n";
        return false;
    }


    // ── Advance ──────────────────────────────────────────────────────────────
    if (choice == 2) {

        // Territories the player can advance from (army count > 0)
        vector<Territory*> advanceFrom;
        copy_if(tOwned.begin(), tOwned.end(), back_inserter(advanceFrom), [](Territory* t){return t->getArmyCount() > 0;});

        if (advanceFrom.empty()){
            cout << "\n" << "No armies available to advance.\n";
            return true;
        }

        cout << "\nChoose source territory:\n";
        
        for (int i = 0; i < (int)advanceFrom.size(); i++) {

            cout << "  " << (i + 1) << ". " << advanceFrom[i]->getName() << " (" << advanceFrom[i]->getArmyCount() << " armies)\n";
        }
        
        cout << "Source (1-" << advanceFrom.size() << "): ";
        int tChoice = readChoice(1, (int)advanceFrom.size());
        

        Territory* source = advanceFrom[tChoice - 1];

        vector<Territory*> adjacent = *source->getAdjacentTerritories();


        cout << "\nChoose target territory:\n";
        for (int i = 0; i < (int)adjacent.size(); i++) {
            
            string ownerName = adjacent[i]->getOwner() ? adjacent[i]->getOwner()->getName() : "neutral";
            
            cout << "  " << (i + 1) << ". " << adjacent[i]->getName()
                 << " (" << adjacent[i]->getArmyCount() << " armies, owner: " << ownerName << ")\n";
        }
        
        cout << "Target (1-" << adjacent.size() << "): ";
        Territory* target = adjacent[readChoice(1, (int)adjacent.size()) - 1];


        cout << "How many armies to advance? (1-" << source->getArmyCount() << "): ";
        int armies = readChoice(1, source->getArmyCount());

        cout << "\n" << playerName << " orders " << armies << " armies to advance: "
             << source->getName() << " -> " << target->getName() << ".\n";
        player->getOrders()->addOrder(new Advance(player, source, target, armies));
        return true;
    }


    // ── Play card ────────────────────────────────────────────────────────────
    if (choice == 3) {
        if (cards->empty()) {
            cout << "No cards in hand.\n";
            return true;
        }
        cout << "\nYour cards:\n";
        for (int i = 0; i < (int)cards->size(); i++) {
            cout << "  " << (i + 1) << ". " << *(*cards)[i] << "\n";
        }
        cout << "Choose card to play (1-" << cards->size() << "): ";
        Card* card = (*cards)[readChoice(1, (int)cards->size()) - 1];
        cout << "\n" << playerName << " plays " << *card << ".\n";
        card->play(deck, player->getHand(), allPlayers);
        return true;
    }

    return false;
}

/*
* Clones the strategy
*/
PlayerStrategy* HumanPlayerStrategy::clone() const {
    return new HumanPlayerStrategy(*this);
}

/*
* Stream insertion operator
*/
std::ostream& HumanPlayerStrategy::print(std::ostream& os) const {
    return os << "HumanPlayerStrategy";
};


//-------------------------------------------------------------------


//---- BenevolentPlayerStrategy -----------------------

vector<Territory*> BenevolentPlayerStrategy::toAttack(const Player*){
    //TODO: implement this
    return {};
}

vector<Territory*> BenevolentPlayerStrategy::toDefend(const Player*){
    //TODO: implement this
    return {};
}

bool BenevolentPlayerStrategy::issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) {
    //TODO: implement this
    return false;
}

/*
* Clones the strategy
*/
PlayerStrategy* BenevolentPlayerStrategy::clone() const {
    return new BenevolentPlayerStrategy(*this);
}

/*
* Stream insertion operator
*/
std::ostream& BenevolentPlayerStrategy::print(std::ostream& os) const {
    return os << "BenevolentPlayerStrategy";
};


//-------------------------------------------------------------------


//---- AgressivePlayerStrategy -----------------------

vector<Territory*> AggressivePlayerStrategy::toAttack(const Player*){
    //TODO: implement this
    return {};
}

vector<Territory*> AggressivePlayerStrategy::toDefend(const Player*){
    //TODO: implement this
    return {};
}

bool AggressivePlayerStrategy::issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) {
    //TODO: implement this
    return false;
}

/*
* Clones the strategy
*/
PlayerStrategy* AggressivePlayerStrategy::clone() const {
    return new AggressivePlayerStrategy(*this);
}

/*
* Stream insertion operator
*/
std::ostream& AggressivePlayerStrategy::print(std::ostream& os) const {
    return os << "AggressivePlayerStrategy";
};


//-------------------------------------------------------------------


//---- NeutralPlayerStrategy -----------------------

vector<Territory*> NeutralPlayerStrategy::toAttack(const Player*){
    return {};
}

vector<Territory*> NeutralPlayerStrategy::toDefend(const Player*){
    return *(player->getTerritoriesOwned());
}

bool NeutralPlayerStrategy::issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) {
    cout << player->getName() << " (Neutral) does not issue any orders.\n";
    return false;
}

/*
* Clones the strategy
*/
PlayerStrategy* NeutralPlayerStrategy::clone() const {
    return new NeutralPlayerStrategy(*this);
}

/*
* Stream insertion operator
*/
std::ostream& NeutralPlayerStrategy::print(std::ostream& os) const {
    return os << "NeutralPlayerStrategy";
};


//-------------------------------------------------------------------


//---- CheaterPlayerStrategy -----------------------

vector<Territory*> CheaterPlayerStrategy::toAttack(const Player*){
    //TODO: implement this
    return {};
}

vector<Territory*> CheaterPlayerStrategy::toDefend(const Player*){
    //TODO: implement this
    return {};
}

bool CheaterPlayerStrategy::issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) {
    //TODO: implement this
    return false;
}

/*
* Clones the strategy
*/
PlayerStrategy* CheaterPlayerStrategy::clone() const {
    return new CheaterPlayerStrategy(*this);
}

/*
* Stream insertion operator
*/
std::ostream& CheaterPlayerStrategy::print(std::ostream& os) const {
    return os << "CheaterPlayerStrategy";
};


//-------------------------------------------------------------------



































/*
*   Helper functions, will be deleted later 
*/

static vector<Territory*> toAttack(const Player* player){
    return {};
}

static vector<Territory*> toDefend(const Player* player){
    return {};
}

static bool defaultIssueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers){
    // 1. Deploy all armies in reinforcement pool.
    if (player->getReinforcementPool() > 0) {
        
        vector<Territory*> defend = toDefend(player);
        
        if (!defend.empty()) {
            Territory* target = defend[0];
            cout << player->getName() << " issues Deploy order: " << player->getReinforcementPool()
                 << " armies to " << target->getName() << endl;
            
            // Strategy: deploy all armies this turn to territory with lowest army count
            player->getOrders()->addOrder(new Deploy(player, target, player->getReinforcementPool()));
            player->setReinforcementPool(0);
        } 
        else {
            cout << player->getName() << " has no territories to deploy to." << endl;
            player->setReinforcementPool(0);
        }
        
        return true;
    }

    // 2. Advance to defend.
    if (player->getOrdersIssuedThisTurn() == 0) {
        player->incrementOrdersIssuedThisTurn();

        vector<Territory*> defend = toDefend(player);
        bool issued = false;
        
        // Iterate through terrorities by army count in descending order
        for (int i = (int)defend.size() - 1; i >= 1 && !issued; i--) {
            
            // Iterate through terrorities by army count in ascending order
            for (int j = 0; j < i && !issued; j++) {
                
                // Strategy: Find a strong territory adjacent to a weaker territory, then send half the reinforcements
                if (defend[i]->isAdjacentTo(defend[j]) && defend[i]->getArmyCount() > 1) {
                    
                    int moveArmies = defend[i]->getArmyCount() / 2;
                    cout << player->getName() << " issues Advance order (defend): "
                         << defend[i]->getName() << " -> " << defend[j]->getName()
                         << " (" << moveArmies << " armies)" << endl;
                    
                    player->getOrders()->addOrder(new Advance(player, defend[i], defend[j], moveArmies));
                    issued = true;
                }
            }
        }
        if (!issued) {
            cout <<  player->getName() << " skips defend-advance (no adjacent territories to reinforce)." << endl;
        }
        return true;
    }

    // 3. Advance to attack an enemy territory.
    if (player->getOrdersIssuedThisTurn() == 1) {
        player->incrementOrdersIssuedThisTurn();
        
        vector<Territory*> attack = toAttack(player);
        bool issued = false;
        
        if (!attack.empty()) {
            Territory* attackTarget = attack[0];
            
            // Find the owned territory with most armies that is adjacent to the attack target
            Territory* source = nullptr;
            int maxArmies = 0;
            for (Territory* t : *player->getTerritoriesOwned()) {
                if (t->isAdjacentTo(attackTarget) && t->getArmyCount() > maxArmies) {
                    source = t;
                    maxArmies = t->getArmyCount();
                }
            }
            
            // Strategy: Advance all your troops except one in the enemy territory
            if (source && source->getArmyCount() > 1) {
                int attackArmies = source->getArmyCount() - 1; // leave 1 army behind
                cout << player->getName() << " issues Advance order (attack): "
                     << source->getName() << " -> " << attackTarget->getName()
                     << " (" << attackArmies << " armies)" << endl;
                player->getOrders()->addOrder(new Advance(player, source, attackTarget, attackArmies));
                issued = true;
            }
        }
        if (!issued) {
            cout << player->getName() << " skips attack-advance (no valid source territory)." << endl;
        }
        return true;
    }

    //4. Play one card from hand.
    if (player->getOrdersIssuedThisTurn() == 2) {
        player->incrementOrdersIssuedThisTurn();
        
        vector<Card*>* cards = player->getHand()->getCards();
        
        // Strategy: Play the first card in the player's hand
        if (!cards->empty() && deck != nullptr) {
            Card* card = cards->front();
            cout << player->getName() << " plays card: " << *card << endl;
            card->play(deck, player->getHand(), allPlayers);
        } else {
            cout << player->getName() << " has no cards to play." << endl;
        }
        return true;
    }

    // End of issuing orders for this turn
    cout << player->getName() << " is done issuing orders." << endl;
    return false;
}


/*
*   END OF TO BE DELETED
*/
