#include "Player.h"
#include "../Maps/Map.h"
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"


using namespace std;

Player::Player() {
    name = new string("Unnamed");
    territoriesOwned = new vector<Territory*>();
    hand = new Hand();
    ordersList = new OrdersList();
}

Player::Player(string playerName) {
    name = new string(playerName);
    territoriesOwned = new vector<Territory*>();
    hand = new Hand();
    ordersList = new OrdersList();
}

Player::Player(const Player& other) {
    name = new string(*other.name);
    territoriesOwned = new vector<Territory*>(*other.territoriesOwned);
    hand = new Hand(*other.hand);
    ordersList = new OrdersList(*other.ordersList);
}

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

Player::~Player() {
    delete name;
    delete territoriesOwned;
    delete hand;
    delete ordersList;
}

vector<Territory*> Player::toDefend() const {
    cout << *name << " deciding which territories to defend..." << endl;
    return *territoriesOwned; // arbitrary
}

vector<Territory*> Player::toAttack() const {
    cout << *name << " deciding which territories to attack..." << endl;
    return {}; // empty list, for now
}

void Player::issueOrder() {
    cout << *name << " is issuing an order..." << endl;
    
    Order* newOrder = new Bomb();

    ordersList->addOrder(newOrder);
}

string Player::getName() const {
    return *name;
}

OrdersList* Player::getOrders() const{
    return ordersList;
};

ostream& operator<<(ostream& out, const Player& p) {
    out << "Player: " << *p.name
        << " | Territories: " << p.territoriesOwned->size()
        << " | Orders: " << endl;
    return out;
}
