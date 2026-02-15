#include "Player.h"
#include "../Maps/Map.h"
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"


using namespace std;


/*
 * Constructor
 */
Player::Player(string playerName) {
    name = new string(playerName);
    territoriesOwned = new vector<Territory*>();
    hand = new Hand();
    ordersList = new OrdersList();
}

/*
 * Copy constructor
 */
Player::Player(const Player& other) {
    name = new string(*other.name);
    territoriesOwned = new vector<Territory*>(*other.territoriesOwned);
    hand = new Hand(*other.hand);
    ordersList = new OrdersList(*other.ordersList);
}


/*
 * Assignment Operator
 */
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        delete name;
        delete territoriesOwned;
        delete hand;
        delete ordersList;

        name = new string(*other.name);
        territoriesOwned = new vector<Territory*>(*other.territoriesOwned);
        hand = new Hand(*other.hand);
        ordersList = new OrdersList(*other.ordersList);
    }
    return *this;
}

/*
 * Destructor
 */
Player::~Player() {
    delete name;
    delete territoriesOwned;
    delete hand;
    delete ordersList;
}

/*
 * Returns a list of territories to defend
 */
vector<Territory*> Player::toDefend() const {
    cout << *name << " deciding which territories to defend..." << endl;
    return *territoriesOwned; // arbitrary
}


/*
 * Returns a list of territories to attack
 */
vector<Territory*> Player::toAttack() const {
    cout << *name << " deciding which territories to attack..." << endl;
    return {}; // empty list, for now
}

/*
 * Creates a new Order (Bomb for now) and adds it to the list
 */
void Player::issueOrder() {
    cout << *name << " is issuing an order..." << endl;
    
    Order* newOrder = new Bomb();

    ordersList->addOrder(newOrder);
}


/*
 * Get Player Name
 */
string Player::getName() const {
    return *name;
}

/*
 * Get Player Orders
 */
OrdersList* Player::getOrders() const{
    return ordersList;
};

/*
 * Get Territories Owned
 */
vector<Territory*>* Player::getTerritoriesOwned() const {
    return territoriesOwned;
}

/*
 * Get Hand
 */
Hand* Player::getHand() const {
    return hand;
}

ostream& operator<<(ostream& out, const Player& p) {
    out << "========================================\n";
    out << "Player: " << *p.name << "\n";
    out << "========================================\n";
    
    // Display territories owned
    out << "Territories Owned (" << p.territoriesOwned->size() << "):\n";
    if (p.territoriesOwned->empty()) {
        out << "  (none)\n";
    } else {
        for (size_t i = 0; i < p.territoriesOwned->size(); i++) {
            out << "  " << (i + 1) << ". " 
                << (*p.territoriesOwned)[i]->getName() << "\n";
        }
    }
    
    // Display orders
    out << "\nOrders (" << p.ordersList->size() << "):\n";
    if (p.ordersList->size() == 0) {
        out << "  (none)\n";
    } else {
        for (size_t i = 0; i < p.ordersList->size(); i++) {
            out << "  " << (i + 1) << ". " 
                << *((*p.ordersList).orderAt(i)) << "\n";
        }
    }
    
    out << "========================================\n";
    
    return out;
}
