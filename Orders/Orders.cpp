// Orders.cpp
// This file contains the implementation for the Order and OrdersList classes.

#include "Orders.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstdlib> 
#include <ctime>   

// Order Class

Order::Order() : Subject() {
    effect = new std::string("");
}

Order::Order(const Order& other) { //copy constructor
    effect = new std::string(*other.effect);
}

Order::~Order() { //destructor
    delete effect;
    effect = nullptr;
}

Order& Order::operator=(const Order& other) { //copy assignement
    if (this != &other) {
        Subject::operator=(other);
        *effect = *other.effect;
    }
    return *this;
}

// Returns the effect string set by execute(), used by LogObserver.
std::string Order::stringToLog() const {
    return "Order executed: " + *effect;
}

std::ostream& operator<<(std::ostream& os, const Order& order) {
    order.print(os);
    if (!order.effect->empty()) {
        os << " | EFFECT: " << *order.effect;
    }
    return os;
}

// Deploy

Deploy::Deploy() : Order(), issuingPlayer(nullptr), targetTerritory(nullptr), numArmies(0) {}
Deploy::Deploy(Player* player, Territory* target, int armies) : Order(), issuingPlayer(player), targetTerritory(target), numArmies(armies) {}
Deploy::Deploy(const Deploy& other) : Order(other), issuingPlayer(other.issuingPlayer), targetTerritory(other.targetTerritory), numArmies(other.numArmies) {} //copy constructor
Deploy::~Deploy() {} 
Deploy& Deploy::operator=(const Deploy& other) {
    Order::operator=(other);
    this->issuingPlayer = other.issuingPlayer;
    this->targetTerritory = other.targetTerritory;
    this->numArmies = other.numArmies;
    return *this;
}
bool Deploy::validate() const {
    if (issuingPlayer == nullptr || targetTerritory == nullptr) return false;
    return targetTerritory->getOwner() == issuingPlayer;
}
void Deploy::execute() {
    if (validate()) {
        targetTerritory->setArmyCount(targetTerritory->getArmyCount() + numArmies);
        *effect = "Deployed " + std::to_string(numArmies) + " armies.";
    }
}

Deploy* Deploy::clone() const { return new Deploy(*this); }
void Deploy::print(std::ostream& os) const { os << "Deploy Order"; }

// Advance

Advance::Advance() : Order(), issuingPlayer(nullptr), sourceTerritory(nullptr), targetTerritory(nullptr), numArmies(0) {}
Advance::Advance(Player* player, Territory* source, Territory* target, int armies) : Order(), issuingPlayer(player), sourceTerritory(source), targetTerritory(target), numArmies(armies) {}
Advance::Advance(const Advance& other) : Order(other), issuingPlayer(other.issuingPlayer), sourceTerritory(other.sourceTerritory), targetTerritory(other.targetTerritory), numArmies(other.numArmies) {}
Advance::~Advance() {}
Advance& Advance::operator=(const Advance& other) { Order::operator=(other); return *this; }
bool Advance::validate() const { 
    if (sourceTerritory == nullptr || targetTerritory == nullptr) return false;
    if (sourceTerritory->getOwner() != issuingPlayer) return false;
    if (!sourceTerritory->isAdjacentTo(targetTerritory)) return false;
    return true; 
}
void Advance::execute() { 
    if (!validate()) return;
    if (sourceTerritory->getOwner() == targetTerritory->getOwner()) {
        sourceTerritory->setArmyCount(sourceTerritory->getArmyCount() - numArmies);
        targetTerritory->setArmyCount(targetTerritory->getArmyCount() + numArmies);
        *effect = "Advanced armies."; 
    } else {
        int attackers = numArmies;
        int defenders = targetTerritory->getArmyCount();
        while (attackers > 0 && defenders > 0) {
            if ((rand() % 100) < 60) defenders--;
            if (defenders > 0 && (rand() % 100) < 70) attackers--;
        }
        if (defenders <= 0) {
            targetTerritory->setOwner(issuingPlayer);
            targetTerritory->setArmyCount(attackers);
            *effect = "Territory conquered.";
        } else {
            sourceTerritory->setArmyCount(sourceTerritory->getArmyCount() - numArmies);
            targetTerritory->setArmyCount(defenders);
            *effect = "Attack failed.";
        }
    }
}
Advance* Advance::clone() const { return new Advance(*this); }
void Advance::print(std::ostream& os) const { os << "Advance Order"; }

// Bomb

Bomb::Bomb() : Order(), issuingPlayer(nullptr), targetTerritory(nullptr) {}
Bomb::Bomb(Player* player, Territory* target) : Order(), issuingPlayer(player), targetTerritory(target) {}
Bomb::Bomb(const Bomb& other) : Order(other), issuingPlayer(other.issuingPlayer), targetTerritory(other.targetTerritory) {}
Bomb::~Bomb() {}
Bomb& Bomb::operator=(const Bomb& other) { Order::operator=(other); return *this; }
bool Bomb::validate() const { 
    if (issuingPlayer == nullptr || targetTerritory == nullptr) return false;
    if (targetTerritory->getOwner() == issuingPlayer) return false;
    for (Territory* t : *issuingPlayer->getTerritoriesOwned()) {
        if (t->isAdjacentTo(targetTerritory)) return true;
    }
    return false;
}
void Bomb::execute() { 
    if (validate()) { 
        targetTerritory->setArmyCount(targetTerritory->getArmyCount() / 2);
        *effect = "Bombed a territory."; 
    } 
}
Bomb* Bomb::clone() const { return new Bomb(*this); }
void Bomb::print(std::ostream& os) const { os << "Bomb Order"; }

// Blockade

Blockade::Blockade() : Order(), issuingPlayer(nullptr), targetTerritory(nullptr) {}
Blockade::Blockade(Player* player, Territory* target) : Order(), issuingPlayer(player), targetTerritory(target) {}
Blockade::Blockade(const Blockade& other) : Order(other), issuingPlayer(other.issuingPlayer), targetTerritory(other.targetTerritory) {}
Blockade::~Blockade() {}
Blockade& Blockade::operator=(const Blockade& other) { Order::operator=(other); return *this; }
bool Blockade::validate() const { 
    if (issuingPlayer == nullptr || targetTerritory == nullptr) return false;
    return targetTerritory->getOwner() == issuingPlayer; 
}
void Blockade::execute() { 
    if (validate()) { 
        targetTerritory->setArmyCount(targetTerritory->getArmyCount() * 2);
        *effect = "Blockaded a territory."; 
    } 
}
Blockade* Blockade::clone() const { return new Blockade(*this); }
void Blockade::print(std::ostream& os) const { os << "Blockade Order"; }

// Airlift

Airlift::Airlift() : Order(), issuingPlayer(nullptr), sourceTerritory(nullptr), targetTerritory(nullptr), numArmies(0) {}
Airlift::Airlift(Player* player, Territory* source, Territory* target, int armies) : Order(), issuingPlayer(player), sourceTerritory(source), targetTerritory(target), numArmies(armies) {}
Airlift::Airlift(const Airlift& other) : Order(other), issuingPlayer(other.issuingPlayer), sourceTerritory(other.sourceTerritory), targetTerritory(other.targetTerritory), numArmies(other.numArmies) {}
Airlift::~Airlift() {}
Airlift& Airlift::operator=(const Airlift& other) { Order::operator=(other); return *this; }
bool Airlift::validate() const { 
    if (issuingPlayer == nullptr || sourceTerritory == nullptr || targetTerritory == nullptr) return false;
    return sourceTerritory->getOwner() == issuingPlayer && targetTerritory->getOwner() == issuingPlayer; 
}
void Airlift::execute() { 
    if (validate()) { 
        sourceTerritory->setArmyCount(sourceTerritory->getArmyCount() - numArmies);
        targetTerritory->setArmyCount(targetTerritory->getArmyCount() + numArmies);
        *effect = "Airlifted armies."; 
    } 
}
Airlift* Airlift::clone() const { return new Airlift(*this); }
void Airlift::print(std::ostream& os) const { os << "Airlift Order"; }

// Negotiate

Negotiate::Negotiate() : Order(), issuingPlayer(nullptr), targetPlayer(nullptr) {}
Negotiate::Negotiate(Player* player, Player* target) : Order(), issuingPlayer(player), targetPlayer(target) {}
Negotiate::Negotiate(const Negotiate& other) : Order(other), issuingPlayer(other.issuingPlayer), targetPlayer(other.targetPlayer) {}
Negotiate::~Negotiate() {}
Negotiate& Negotiate::operator=(const Negotiate& other) { Order::operator=(other); return *this; }
bool Negotiate::validate() const { 
    if (issuingPlayer == nullptr || targetPlayer == nullptr) return false;
    return issuingPlayer != targetPlayer; 
}
void Negotiate::execute() { if (validate()) { *effect = "Negotiated a truce."; } }
Negotiate* Negotiate::clone() const { return new Negotiate(*this); }
void Negotiate::print(std::ostream& os) const { os << "Negotiate Order"; }

// OrdersList

OrdersList::OrdersList() : Subject() {
    orders = new std::vector<Order*>();
}

OrdersList::OrdersList(const OrdersList& other) : Subject(other) {
    orders = new std::vector<Order*>();
    for (int i = 0; i < other.orders->size(); i++) {
        orders->push_back((*other.orders)[i]->clone());
    }
}

OrdersList::~OrdersList() {
    for (int i = 0; i < orders->size(); i++) {
        delete (*orders)[i];
    }
    delete orders;
}

OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this != &other) {
        Subject::operator=(other);
        for (int i = 0; i < orders->size(); i++) {
            delete (*orders)[i];
        }
        orders->clear();
        for (int i = 0; i < other.orders->size(); i++) {
            orders->push_back((*other.orders)[i]->clone());
        }
    }
    return *this;
}

// Adds the order to the list and notifies observers so the addition is logged.
void OrdersList::addOrder(Order* order) {
    orders->push_back(order);
    notify(*this);
}

void OrdersList::remove(int index) {
    if (index >= 0 && index < static_cast<int>(orders->size())) {
        delete (*orders)[index]; // delete order object
        orders->erase(orders->begin() + index); // remove  pointer from vector
    }
}

void OrdersList::move(int fromIndex, int toIndex) {
    if (fromIndex >= 0 && fromIndex < static_cast<int>(orders->size()) &&
        toIndex >= 0 && toIndex < static_cast<int>(orders->size())) {
        Order* orderToMove = (*orders)[fromIndex];
        orders->erase(orders->begin() + fromIndex);
        orders->insert(orders->begin() + toIndex, orderToMove);
    }
}

void OrdersList::executeAll() {
    for (int i = 0; i < static_cast<int>(orders->size()); i++) {
        (*orders)[i]->execute();
    }
}

int OrdersList::size() const {
    return static_cast<int>(orders->size());
}

Order* OrdersList::orderAt(int index) const {
    return orders->at(index);
}

// Describes the most recently added order for the log file.
std::string OrdersList::stringToLog() const {
    if (orders->empty()) {
        return "OrdersList: no orders";
    }
    std::ostringstream oss;
    oss << "Order added to list: " << *orders->back();
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, const OrdersList& list) {
    os << "--- Orders List ---\n";
    if (list.orders->empty()) {
        os << "(empty)\n";
    } else {
        for (size_t i = 0; i < list.orders->size(); ++i) {
            os << i << ": " << *(*list.orders)[i] << "\n";
        }
    }
    os << "-------------------\n";
    return os;
}
