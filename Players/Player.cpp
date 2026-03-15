#include "Player.h"
#include "../Maps/Map.h"
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"
#include <algorithm>

using namespace std;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
Player::Player(string playerName) {
    name = new string(playerName);
    territoriesOwned = new vector<Territory*>();
    hand = new Hand();
    ordersList = new OrdersList();
    reinforcementPool = new int(0);
    ordersIssuedThisTurn = new int(0);
    negotiatedWith = new vector<Player*>();
    conqueredThisTurn = new bool(false);
}

// ---------------------------------------------------------------------------
// Copy constructor
// ---------------------------------------------------------------------------
Player::Player(const Player& other) {
    name = new string(*other.name);
    territoriesOwned = new vector<Territory*>(*other.territoriesOwned);
    hand = new Hand(*other.hand);
    ordersList = new OrdersList(*other.ordersList);
    reinforcementPool = new int(*other.reinforcementPool);
    ordersIssuedThisTurn = new int(*other.ordersIssuedThisTurn);
    negotiatedWith = new vector<Player*>(*other.negotiatedWith);
    conqueredThisTurn = new bool(*other.conqueredThisTurn);
}

// ---------------------------------------------------------------------------
// Assignment operator
// ---------------------------------------------------------------------------
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        delete name;
        delete territoriesOwned;
        delete hand;
        delete ordersList;
        delete reinforcementPool;
        delete ordersIssuedThisTurn;
        delete negotiatedWith;
        delete conqueredThisTurn;

        name = new string(*other.name);
        territoriesOwned = new vector<Territory*>(*other.territoriesOwned);
        hand = new Hand(*other.hand);
        ordersList = new OrdersList(*other.ordersList);
        reinforcementPool = new int(*other.reinforcementPool);
        ordersIssuedThisTurn = new int(*other.ordersIssuedThisTurn);
        negotiatedWith = new vector<Player*>(*other.negotiatedWith);
        conqueredThisTurn = new bool(*other.conqueredThisTurn);
    }
    return *this;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
Player::~Player() {
    delete name;
    delete territoriesOwned;
    delete hand;
    delete ordersList;
    delete reinforcementPool;
    delete ordersIssuedThisTurn;
    delete negotiatedWith;
    delete conqueredThisTurn;
}

// ---------------------------------------------------------------------------
// Returns all owned territories sorted by army count in ascending order
// ---------------------------------------------------------------------------
vector<Territory*> Player::toDefend() const {
    vector<Territory*> defend = *territoriesOwned;
    sort(defend.begin(), defend.end(), [](Territory* a, Territory* b) {
        return a->getArmyCount() < b->getArmyCount();
    });
    return defend;
}

// ---------------------------------------------------------------------------
// Returns adjacent enemy territories sorted by army count in ascending order
// ---------------------------------------------------------------------------
vector<Territory*> Player::toAttack() const {
    
    vector<Territory*> attackTargets;
    
    for (Territory* t : *territoriesOwned) {
        for (Territory* adj : *t->getAdjacentTerritories()) {
            
            if (adj->getOwner() != this) { // if the player does not own the territory
                
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
}

// ---------------------------------------------------------------------------
// Called at the start of each issue-orders phase in main game loop
// ---------------------------------------------------------------------------
void Player::resetOrderIssuingState() {
    *ordersIssuedThisTurn = 0;
    negotiatedWith->clear();
    *conqueredThisTurn = false;
}

// ---------------------------------------------------------------------------
// Issues orders in this manner:
//   1. Deploy all armies in reinforcement pool.
//   2. Advance to defend (move armies between adjacent owned territories).
//   3. Advance to attack an enemy territory.
//   4. Play one card from hand.
// Returns true if an order was issued, false when the player's turn is done.
// ---------------------------------------------------------------------------
bool Player::issueOrder(Deck* deck, const std::vector<Player*>* allPlayers) {
    
    // 1. Deploy all armies in reinforcement pool.
    if (*reinforcementPool > 0) {
        
        vector<Territory*> defend = toDefend();
        
        if (!defend.empty()) {
            Territory* target = defend[0];
            cout << *name << " issues Deploy order: " << *reinforcementPool
                 << " armies to " << target->getName() << endl;
            
            // Strategy: deploy all armies this turn to territory with lowest army count
            ordersList->addOrder(new Deploy(this, target, *reinforcementPool));
            *reinforcementPool = 0;
        } 
        else {
            cout << *name << " has no territories to deploy to." << endl;
            *reinforcementPool = 0;
        }
        
        return true;
    }

    // 2. Advance to defend.
    if (*ordersIssuedThisTurn == 0) {
        (*ordersIssuedThisTurn)++;

        vector<Territory*> defend = toDefend();
        bool issued = false;
        
        // Iterate through terrorities by army count in descending order
        for (int i = (int)defend.size() - 1; i >= 1 && !issued; i--) {
            
            // Iterate through terrorities by army count in ascending order
            for (int j = 0; j < i && !issued; j++) {
                
                // Strategy: Find a strong territory adjacent to a weaker territory, then send half the reinforcements
                if (defend[i]->isAdjacentTo(defend[j]) && defend[i]->getArmyCount() > 1) {
                    
                    int moveArmies = defend[i]->getArmyCount() / 2;
                    cout << *name << " issues Advance order (defend): "
                         << defend[i]->getName() << " -> " << defend[j]->getName()
                         << " (" << moveArmies << " armies)" << endl;
                    
                    ordersList->addOrder(new Advance(this, defend[i], defend[j], moveArmies));
                    issued = true;
                }
            }
        }
        if (!issued) {
            cout << *name << " skips defend-advance (no adjacent territories to reinforce)." << endl;
        }
        return true;
    }

    // 3. Advance to attack an enemy territory.
    if (*ordersIssuedThisTurn == 1) {
        (*ordersIssuedThisTurn)++;
        
        vector<Territory*> attack = toAttack();
        bool issued = false;
        
        if (!attack.empty()) {
            Territory* attackTarget = attack[0];
            
            // Find the owned territory with most armies that is adjacent to the attack target
            Territory* source = nullptr;
            int maxArmies = 0;
            for (Territory* t : *territoriesOwned) {
                if (t->isAdjacentTo(attackTarget) && t->getArmyCount() > maxArmies) {
                    source = t;
                    maxArmies = t->getArmyCount();
                }
            }
            
            // Strategy: Advance all your troops except one in the enemy territory
            if (source && source->getArmyCount() > 1) {
                int attackArmies = source->getArmyCount() - 1; // leave 1 army behind
                cout << *name << " issues Advance order (attack): "
                     << source->getName() << " -> " << attackTarget->getName()
                     << " (" << attackArmies << " armies)" << endl;
                ordersList->addOrder(new Advance(this, source, attackTarget, attackArmies));
                issued = true;
            }
        }
        if (!issued) {
            cout << *name << " skips attack-advance (no valid source territory)." << endl;
        }
        return true;
    }

    //4. Play one card from hand.
    if (*ordersIssuedThisTurn == 2) {
        (*ordersIssuedThisTurn)++;
        
        vector<Card*>* cards = hand->getCards();
        
        // Strategy: Play the first card in the player's hand
        if (!cards->empty() && deck != nullptr) {
            Card* card = cards->front();
            cout << *name << " plays card: " << *card << endl;
            card->play(deck, hand, allPlayers);
        } else {
            cout << *name << " has no cards to play." << endl;
        }
        return true;
    }

    // End of issuing orders for this turn
    cout << *name << " is done issuing orders." << endl;
    return false;
}

// ---------------------------------------------------------------------------
// Getters / Setters
// ---------------------------------------------------------------------------
string Player::getName() const {
    return *name;
}

OrdersList* Player::getOrders() const {
    return ordersList;
}

vector<Territory*>* Player::getTerritoriesOwned() const {
    return territoriesOwned;
}

Hand* Player::getHand() const {
    return hand;
}

int Player::getReinforcementPool() const {
    return *reinforcementPool;
}

bool Player::getConqueredThisTurn() const {
    return *conqueredThisTurn;
}

void Player::setReinforcementPool(int count) {
    *reinforcementPool = count;
}

void Player::addReinforcements(int count) {
    *reinforcementPool += count;
}

void Player::setConqueredThisTurn(bool val) {
    *conqueredThisTurn = val;
}

void Player::addNegotiation(Player* other) {
    // Only add if not already present
    for (Player* p : *negotiatedWith) {
        if (p == other) return;
    }
    negotiatedWith->push_back(other);
}

bool Player::hasNegotiatedWith(Player* other) const {
    for (Player* p : *negotiatedWith) {
        if (p == other) return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Stream insertion operator
// ---------------------------------------------------------------------------
ostream& operator<<(ostream& out, const Player& p) {
    out << "========================================\n";
    out << "Player: " << *p.name << "\n";
    out << "Reinforcement Pool: " << *p.reinforcementPool << " armies\n";
    out << "========================================\n";

    out << "Territories Owned (" << p.territoriesOwned->size() << "):\n";
    if (p.territoriesOwned->empty()) {
        out << "  (none)\n";
    } else {
        for (size_t i = 0; i < p.territoriesOwned->size(); i++) {
            out << "  " << (i + 1) << ". "
                << (*p.territoriesOwned)[i]->getName()
                << " (" << (*p.territoriesOwned)[i]->getArmyCount() << " armies)\n";
        }
    }

    out << "\nOrders (" << p.ordersList->size() << "):\n";
    if (p.ordersList->size() == 0) {
        out << "  (none)\n";
    } else {
        for (size_t i = 0; i < (size_t)p.ordersList->size(); i++) {
            out << "  " << (i + 1) << ". "
                << *((*p.ordersList).orderAt(i)) << "\n";
        }
    }

    out << "========================================\n";
    return out;
}
