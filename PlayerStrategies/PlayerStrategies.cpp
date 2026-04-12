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

/*
* Attack Strategy: 
* Does not attack
*/
vector<Territory*> BenevolentPlayerStrategy::toAttack(const Player*){
    return {};
}

/*
* Defend Strategy: 
* Return all owned territories sorted by army count in ASC order
*/
vector<Territory*> BenevolentPlayerStrategy::toDefend(const Player* player){
    vector<Territory*> defend = *(player->getTerritoriesOwned());

    sort(defend.begin(), defend.end(), [](Territory* a, Territory* b) {
        return a->getArmyCount() < b->getArmyCount();
    });

    return defend;
}


/*
* Order Issuing Strategy: 
* 1: Deploy all reinforcements into weakest territory.
* 2: Advance half the armies of a territory to a weaker adjacent territory.
* 3: Play a card.
*/
bool BenevolentPlayerStrategy::issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) {
    
    string playerName = player->getName();
    vector<Territory*> defend = toDefend(player);

    // 1- Deploy reinforcements into weakest territory
    if (player->getReinforcementPool() > 0) {
        if (!defend.empty()) {
            Territory* target = defend[0];
            int armies = player->getReinforcementPool();

            cout << "\n" << playerName << " orders " << armies 
                 << " armies to deploy to " << target->getName() << ".\n";

            player->getOrders()->addOrder(new Deploy(player, target, armies));
            player->setReinforcementPool(0);
        }
        return true;
    }

    // Advance half the armies of a strongest territory to an adjacent weaker territory
    if (player->getOrdersIssuedThisTurn() == 0) {
        player->incrementOrdersIssuedThisTurn();

        bool issued = false;
        
        // Source territory
        for (int i = (int)defend.size() - 1; i >= 1 && !issued; i--) {
            // Target territory
            for (int j = 0; j < i && !issued; j++) {
                
                if (defend[i]->isAdjacentTo(defend[j]) && defend[i]->getArmyCount() > 1) {
                    
                    int moveArmies = defend[i]->getArmyCount() / 2;

                    cout << "\n" << playerName << " orders " << moveArmies << " armies to advance: "
                         << defend[i]->getName() << " -> " << defend[j]->getName() << ".\n";

                    player->getOrders()->addOrder(new Advance(player, defend[i], defend[j], moveArmies));
                    issued = true;
                }
            }
        }

        if (!issued) {
            cout << playerName << " skips defend-advance (no adjacent territories to reinforce)." << endl;
        }
        return true;
    }


    // 3: Play a card 
    if (player->getOrdersIssuedThisTurn() == 1) {
        player->incrementOrdersIssuedThisTurn();

        vector<Card*>* cards = player->getHand()->getCards();
        
        if (!cards->empty() && deck != nullptr) {
            
            Card* card = cards->front();

            cout << "\n" << playerName << " plays " << *card << ".\n";
            card->play(deck, player->getHand(), allPlayers);
        
        }
        else {
            cout << playerName << " has no cards to play." << endl;
        }
        return true;
    }

    cout << playerName << " is done issuing orders." << endl;
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

/*
* Attack Strategy: 
* Return all adjacent enemy territories sorted by army count in ASC order
*/
vector<Territory*> AggressivePlayerStrategy::toAttack(const Player* player){
    vector<Territory*> attackTargets;

    for (Territory* t : *(player->getTerritoriesOwned())) {
        for (Territory* adj : *(t->getAdjacentTerritories())) {
            if (adj->getOwner() != player) {
                bool alreadyListed = false;

                for (Territory* target : attackTargets) {
                    if (target == adj) {
                        alreadyListed = true;
                        break;
                    }
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
}


/*
* Defend Strategy: 
* Return all owned territories sorted by army count in ASC order
*/
vector<Territory*> AggressivePlayerStrategy::toDefend(const Player* player){
    vector<Territory*> defend = *(player->getTerritoriesOwned());

    sort(defend.begin(), defend.end(), [](Territory* a, Territory* b) {
        return a->getArmyCount() < b->getArmyCount();
    });

    return defend;
}


/*
* Order Issuing Strategy: 
* 1: Deploy all reinforcements into strongest territory.
* 2: Advance all armies (except 1) of strongest territory to weakest enemy adjacent territory.
* 3: Play a card.
*/
bool AggressivePlayerStrategy::issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) {
    
    string playerName = player->getName();
    vector<Territory*> defend = toDefend(player);

    // Deploy all reinforcements into strongest territory
    if (player->getReinforcementPool() > 0) {
        if (!defend.empty()) {
            Territory* target = defend.back();
            int armies = player->getReinforcementPool();

            cout << "\n" << playerName << " orders " << armies 
                 << " armies to deploy to " << target->getName() << ".\n";

            player->getOrders()->addOrder(new Deploy(player, target, armies));
            player->setReinforcementPool(0);
        }
        return true;
    }

    // Advance strongest territory into weakest territory
    if (player->getOrdersIssuedThisTurn() == 0) {
        player->incrementOrdersIssuedThisTurn();
        
        bool issued = false;

        if (!defend.empty()) {
            Territory* strongest = defend.back();
            vector<Territory*> attack = toAttack(player);

            for (Territory* target : attack) {
                if (strongest->isAdjacentTo(target) && strongest->getArmyCount() > 1) {
                    int attackArmies = strongest->getArmyCount() - 1;

                    cout << player->getName() << " issues Advance order (attack): "
                        << strongest->getName() << " -> " << target->getName()
                        << " (" << attackArmies << " armies)" << endl;

                    player->getOrders()->addOrder(new Advance(player, strongest, target, attackArmies));
                    issued = true;
                    break;
                }
            }
        }

        if (!issued){
            cout << playerName << " skips defend-advance (no adjacent territories to reinforce)." << endl;
        }
        return true;
    }   

    // Play a card
    if (player->getOrdersIssuedThisTurn() == 1) {
        player->incrementOrdersIssuedThisTurn();

        vector<Card*>* cards = player->getHand()->getCards();
        if (!cards->empty() && deck != nullptr) {
            
            Card* card = cards->front();
            
            cout << "\n" << playerName << " plays " << *card << ".\n";
            card->play(deck, player->getHand(), allPlayers);
        } else {
            cout << player->getName() << " has no cards to play." << endl;
        }
        return true;
    }

    cout << player->getName() << " is done issuing orders." << endl;
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

/*
* Attack Strategy:
* Never attacks.
*/
vector<Territory*> NeutralPlayerStrategy::toAttack(const Player*){
    return {};
}

/*
* Defend Strategy:
* Never defends.
*/
vector<Territory*> NeutralPlayerStrategy::toDefend(const Player*){
    return {};
}

/*
* Order Issuing Strategy:
* Issues no orders whatsoever. 
* If a player with this strategy is attacked, the strategy will be changed to Aggressive Player.
*/
bool NeutralPlayerStrategy::issueOrder(Player* player, Deck*, const std::vector<Player*>*) {
    cout << player->getName() << " (Neutral) takes no action this turn." << endl;
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

/*
* Attack Strategy: 
* Return all adjacent enemy territories sorted by army count in ASC order
*/
vector<Territory*> CheaterPlayerStrategy::toAttack(const Player* player){
    vector<Territory*> attackTargets;

    for (Territory* t : *(player->getTerritoriesOwned())) {
        for (Territory* adj : *(t->getAdjacentTerritories())) {
            if (adj->getOwner() != player) {
                bool alreadyListed = false;

                for (Territory* target : attackTargets) {
                    if (target == adj) {
                        alreadyListed = true;
                        break;
                    }
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
}


/*
* Defend Strategy: 
* Return all owned territories sorted by army count in ASC order
*/
vector<Territory*> CheaterPlayerStrategy::toDefend(const Player* player){
    vector<Territory*> defend = *(player->getTerritoriesOwned());

    sort(defend.begin(), defend.end(), [](Territory* a, Territory* b) {
        return a->getArmyCount() < b->getArmyCount();
    });

    return defend;
}


/*
* Order Issuing Strategy: 
* 1: Deploy all reinforcements into strongest territory.
* 2: Advance all armies (except 1) of strongest territory to weakest enemy adjacent territory.
* 3: Play a card.
*/
bool CheaterPlayerStrategy::issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) {
    
    // Unused parameters
    (void)deck;
    (void)allPlayers;

    string playerName = player->getName();
    vector<Territory*> defend = toDefend(player);


    // Deploy reinforcements into strongest territory
    if (player->getReinforcementPool() > 0) {
        if (!defend.empty()) {
            Territory* target = defend.back();
            int armies = player->getReinforcementPool();

            cout << "\n" << playerName << " orders " << armies 
                 << " armies to deploy to " << target->getName() << ".\n";

            player->getOrders()->addOrder(new Deploy(player, target, armies));
            player->setReinforcementPool(0);
        }
        return true;
    }

    // Conquer all adjacent territories
    if (player->getOrdersIssuedThisTurn() == 0) {
        player->incrementOrdersIssuedThisTurn();

        vector<Territory*> conquered = toAttack(player);

        if (conquered.empty()) {
            cout << playerName << " has no adjacent enemy territories to conquer." << endl;
            return false;
        }

        cout << playerName << " automatically conquers adjacent enemy territories:" << endl;

        for (Territory* target : conquered) {
            Player* previousOwner = target->getOwner();
            
            // Remove territory from previous owner's territories
            if (previousOwner != nullptr) {
                auto& prevList = *previousOwner->getTerritoriesOwned();
                prevList.erase(remove(prevList.begin(), prevList.end(), target), prevList.end());
                
                // Cheating is considered being attacked
                previousOwner->isAttacked();
            }

            target->setOwner(player);
            player->getTerritoriesOwned()->push_back(target);
            player->setConqueredThisTurn(true);

            cout << "  " << target->getName() << endl;
        }

        return true;
    }

    cout << player->getName() << " is done issuing orders." << endl;
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


