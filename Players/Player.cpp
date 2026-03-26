#include "Player.h"
#include "../Maps/Map.h"
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"
#include "../PlayerStrategies/PlayerStrategies.h"
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
    strategy = new HumanPlayerStrategy();
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
    strategy = other.strategy->clone();
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
        delete strategy;

        name = new string(*other.name);
        territoriesOwned = new vector<Territory*>(*other.territoriesOwned);
        hand = new Hand(*other.hand);
        ordersList = new OrdersList(*other.ordersList);
        reinforcementPool = new int(*other.reinforcementPool);
        ordersIssuedThisTurn = new int(*other.ordersIssuedThisTurn);
        negotiatedWith = new vector<Player*>(*other.negotiatedWith);
        conqueredThisTurn = new bool(*other.conqueredThisTurn);
        strategy = other.strategy->clone();
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
    delete strategy;
}

// ---------------------------------------------------------------------------
// Calls the toDefend method in the PlayerStrategy
// ---------------------------------------------------------------------------
vector<Territory*> Player::toDefend() const {
    return strategy->toDefend(this);
}

// ---------------------------------------------------------------------------
// Calls the toAttack method in the PlayerStrategy
// ---------------------------------------------------------------------------
vector<Territory*> Player::toAttack() const {
    return strategy->toAttack(this);
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
// Calls the issueOrder method in the PlayerStrategy
// ---------------------------------------------------------------------------
bool Player::issueOrder(Deck* deck, const std::vector<Player*>* allPlayers) {
    return strategy->issueOrder(const_cast<Player*>(this), deck, allPlayers);
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

int Player::getOrdersIssuedThisTurn() const {
    return *ordersIssuedThisTurn;
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

/*
* Sets a new strategy for the player
*/
void Player::setStrategy(PlayerStrategy* strategy) {
    
    delete this->strategy;
    this->strategy = strategy;

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
