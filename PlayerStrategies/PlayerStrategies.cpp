#include "PlayerStrategies.h"
#include "../Maps/Map.h"
#include "../Players/Player.h"
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"

#include <algorithm>

using namespace std;


//---- PlayerStrategy -----------------------

/*
* Stream insertion operator for strategies.
* Delegates to the polymorphic print method.  
*/
std::ostream& operator<<(std::ostream& os, const PlayerStrategy& ps){
    return ps.print(os);
}


//------------------------------------------


//---- HumanPlayerStrategy -----------------------

/*
* Attack Strategy: 
* Return all adjacent enemy territories sorted by army count in ASC order
*/
std::vector<Territory*> HumanPlayerStrategy::toAttack(const Player* player){
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
std::vector<Territory*> HumanPlayerStrategy::toDefend(const Player* player){
    vector<Territory*> defend = *(player->getTerritoriesOwned());
    
    sort(defend.begin(), defend.end(), [](Territory* a, Territory* b) {
        return a->getArmyCount() < b->getArmyCount();
    });
    
    return defend;
};

// ---------------------------------------------------------------------------
// Issues orders in this manner:
//   1. Deploy all armies in reinforcement pool.
//   2. Advance to defend (move armies between adjacent owned territories).
//   3. Advance to attack an enemy territory.
//   4. Play one card from hand.
// Returns true if an order was issued, false when the player's turn is done.
// ---------------------------------------------------------------------------
bool HumanPlayerStrategy::issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers){
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
};

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