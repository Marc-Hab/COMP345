// Orders.cpp
// This file contains the implementation for the Order and OrdersList classes.

#include "Orders.h"
#include "../Players/Player.h"
#include "../Maps/Map.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <sstream>

using namespace std;

// ---------------------------------------------------------------------------
// Neutral player singleton for Blockade transfers
// ---------------------------------------------------------------------------
static Player* getNeutralPlayer() {
    static Player* neutral = new Player("Neutral");
    return neutral;
}

// ---------------------------------------------------------------------------
// Order (base class)
// ---------------------------------------------------------------------------

Order::Order() {
    effect = new string("");
}

Order::Order(const Order& other) : Subject(other) {
    effect = new string(*other.effect);
}

Order::~Order() {
    delete effect;
    effect = nullptr;
}

Order& Order::operator=(const Order& other) {
    if (this != &other) {
        Subject::operator=(other);
        *effect = *other.effect;
    }
    return *this;
}

string Order::stringToLog() const {
    return "Order executed, effect: " + *effect;
}

ostream& operator<<(ostream& os, const Order& order) {
    order.print(os);
    if (!order.effect->empty()) {
        os << " | EFFECT: " << *order.effect;
    }
    return os;
}

// ---------------------------------------------------------------------------
// Deploy
// ---------------------------------------------------------------------------

Deploy::Deploy(Player* issuer, Territory* target, int armies)
    : Order(), issuer(issuer), target(target), armies(new int(armies)) {}

Deploy::Deploy(const Deploy& other)
    : Order(other), issuer(other.issuer), target(other.target),
      armies(new int(*other.armies)) {}

Deploy::~Deploy() {
    delete armies;
}

Deploy& Deploy::operator=(const Deploy& other) {
    if (this != &other) {
        Order::operator=(other);
        issuer = other.issuer;
        target = other.target;
        *armies = *other.armies;
    }
    return *this;
}

bool Deploy::validate() const {
    if (!issuer || !target || !armies || *armies <= 0) {
        return false;
    }
    if (target->getOwner() != issuer) {
        return false;
    }
    return true;
}

void Deploy::execute() {
    if (!validate()) {
        *effect = "INVALID Deploy: target territory '" +
                  (target ? target->getName() : "null") +
                  "' does not belong to " +
                  (issuer ? issuer->getName() : "null") + ".";
    } else {
        int newCount = target->getArmyCount() + *armies;
        target->setArmyCount(newCount);
        *effect = issuer->getName() + " deployed " + to_string(*armies) +
                  " armies to " + target->getName() +
                  " (now " + to_string(newCount) + " armies).";
    }
    cout << *effect << endl;
    notify();
}

Deploy* Deploy::clone() const {
    return new Deploy(*this);
}

void Deploy::print(ostream& os) const {
    os << "Deploy Order [issuer: " << (issuer ? issuer->getName() : "null")
       << ", target: " << (target ? target->getName() : "null")
       << ", armies: " << (armies ? to_string(*armies) : "null") << "]";
}

// ---------------------------------------------------------------------------
// Advance
// ---------------------------------------------------------------------------

Advance::Advance(Player* issuer, Territory* source, Territory* target, int armies)
    : Order(), issuer(issuer), source(source), target(target), armies(new int(armies)) {}

Advance::Advance(const Advance& other)
    : Order(other), issuer(other.issuer), source(other.source), target(other.target),
      armies(new int(*other.armies)) {}

Advance::~Advance() {
    delete armies;
}

Advance& Advance::operator=(const Advance& other) {
    if (this != &other) {
        Order::operator=(other);
        issuer = other.issuer;
        source = other.source;
        target = other.target;
        *armies = *other.armies;
    }
    return *this;
}

bool Advance::validate() const {
    if (!issuer || !source || !target || !armies || *armies <= 0) {
        return false;
    }
    if (source->getOwner() != issuer) {
        return false;
    }
    if (!source->isAdjacentTo(target)) {
        return false;
    }
    // Check if a truce has been negotiated
    if (target->getOwner() != issuer && target->getOwner() != nullptr) {
        if (issuer->hasNegotiatedWith(target->getOwner())) {
            return false;
        }
    }
    return true;
}

void Advance::execute() {
    if (!validate()) {
        string reason;
        if (!issuer || !source || !target || !armies || *armies <= 0)
            reason = "null or zero-army parameters.";
        else if (source->getOwner() != issuer)
            reason = source->getName() + " does not belong to " + issuer->getName() + ".";
        else if (!source->isAdjacentTo(target))
            reason = source->getName() + " is not adjacent to " + target->getName() + ".";
        else
            reason = "a negotiate truce prevents attacks between " +
                     issuer->getName() + " and " + target->getOwner()->getName() + ".";
        *effect = "INVALID Advance: " + reason;
    } else if (target->getOwner() == issuer) {
        // Friendly move: both territories belong to issuer
        int moved = min(*armies, source->getArmyCount());
        source->setArmyCount(source->getArmyCount() - moved);
        target->setArmyCount(target->getArmyCount() + moved);
        *effect = issuer->getName() + " moved " + to_string(moved) +
                  " armies from " + source->getName() + " to " + target->getName() + ".";
    } else {
        // Random device to simulate a battle
        mt19937 rng(random_device{}());

        int attackers = min(*armies, source->getArmyCount());
        int defenders = target->getArmyCount();

        // Deduct attackers from source immediately
        source->setArmyCount(source->getArmyCount() - attackers);

        // Each attacker has 60% chance to kill one defender
        // Each defender has 70% chance to kill one attacker
        int attackerKills = 0;
        int defenderKills = 0;
        for (int i = 0; i < attackers; i++)
            if (rng() % 100 < 60) attackerKills++;
        for (int i = 0; i < defenders; i++)
            if (rng() % 100 < 70) defenderKills++;

        int survivingAttackers = max(0, attackers - defenderKills);
        int survivingDefenders = max(0, defenders - attackerKills);

        ostringstream oss;
        oss << issuer->getName() << " attacks " << target->getName()
            << " from " << source->getName()
            << " (" << attackers << " vs " << defenders << "). ";

        if (survivingDefenders == 0) {
            // Attacker captures the territory
            Player* prev = target->getOwner();
            if (prev != nullptr) {
                auto& prevList = *prev->getTerritoriesOwned();
                prevList.erase(remove(prevList.begin(), prevList.end(), target), prevList.end());
            }
            target->setOwner(issuer);
            target->setArmyCount(survivingAttackers);
            issuer->getTerritoriesOwned()->push_back(target);
            issuer->setConqueredThisTurn(true);
            oss << issuer->getName() << " CAPTURES " << target->getName()
                << " with " << survivingAttackers << " surviving armies!";
        } else {
            // Defenders hold; surviving attackers return to source
            source->setArmyCount(source->getArmyCount() + survivingAttackers);
            target->setArmyCount(survivingDefenders);
            oss << "Attack repelled. " << target->getName() << " holds with "
                << survivingDefenders << " armies. "
                << survivingAttackers << " attacker(s) returned to " << source->getName() << ".";
        }
        *effect = oss.str();
    }
    cout << *effect << endl;
    notify();
}

Advance* Advance::clone() const {
    return new Advance(*this);
}

void Advance::print(ostream& os) const {
    os << "Advance Order [issuer: " << (issuer ? issuer->getName() : "null")
       << ", from: " << (source ? source->getName() : "null")
       << " -> to: " << (target ? target->getName() : "null")
       << ", armies: " << (armies ? to_string(*armies) : "null") << "]";
}

// ---------------------------------------------------------------------------
// Bomb
// ---------------------------------------------------------------------------

Bomb::Bomb(Player* issuer, Territory* target)
    : Order(), issuer(issuer), target(target) {}

Bomb::Bomb(const Bomb& other)
    : Order(other), issuer(other.issuer), target(other.target) {}

Bomb::~Bomb() {}

Bomb& Bomb::operator=(const Bomb& other) {
    if (this != &other) {
        Order::operator=(other);
        issuer = other.issuer;
        target = other.target;
    }
    return *this;
}

bool Bomb::validate() const {
    if (!issuer || !target) {
        return false;
    }
    // Cannot bomb own territory
    if (target->getOwner() == issuer) {
        return false;
    }

    // Check if a truce has been negotiated
    if (issuer->hasNegotiatedWith(target->getOwner())) {
        return false;
    }
    

    // Target must be adjacent to at least one territory owned by issuer
    for (Territory* t : *issuer->getTerritoriesOwned()) {
        if (t->isAdjacentTo(target)) {
            return true;
        }
    }
    return false;
}

void Bomb::execute() {
    if (!validate()) {
        string reason;
        if (!issuer || !target)
            reason = "null parameters.";
        else if (target->getOwner() == issuer)
            reason = "cannot bomb own territory " + target->getName() + ".";
        else if (issuer->hasNegotiatedWith(target->getOwner()))
            reason = "a negotiate truce prevents attacks between " +
                     issuer->getName() + " and " + target->getOwner()->getName() + ".";
        else
            reason = target->getName() + " is not adjacent to any territory owned by " + issuer->getName() + ".";
        *effect = "INVALID Bomb: " + reason;
    } else {
        int halved = target->getArmyCount() / 2;
        target->setArmyCount(halved);
        *effect = issuer->getName() + " bombed " + target->getName() +
                  " — armies reduced to " + to_string(halved) + ".";
    }
    cout << *effect << endl;
    notify();
}

Bomb* Bomb::clone() const {
    return new Bomb(*this);
}

void Bomb::print(ostream& os) const {
    os << "Bomb Order [issuer: " << (issuer ? issuer->getName() : "null")
       << ", target: " << (target ? target->getName() : "null") << "]";
}

// ---------------------------------------------------------------------------
// Blockade
// ---------------------------------------------------------------------------

Blockade::Blockade(Player* issuer, Territory* target)
    : Order(), issuer(issuer), target(target) {}

Blockade::Blockade(const Blockade& other)
    : Order(other), issuer(other.issuer), target(other.target) {}

Blockade::~Blockade() {}

Blockade& Blockade::operator=(const Blockade& other) {
    if (this != &other) {
        Order::operator=(other);
        issuer = other.issuer;
        target = other.target;
    }
    return *this;
}

bool Blockade::validate() const {
    if (!issuer || !target) {
        return false;
    }
    if (target->getOwner() != issuer) {
        return false;
    }
    return true;
}

void Blockade::execute() {
    if (!validate()) {
        string reason;
        if (!issuer || !target)
            reason = "null parameters.";
        else
            reason = target->getName() + " does not belong to " + issuer->getName() + ".";
        *effect = "INVALID Blockade: " + reason;
    } else {
        int doubled = target->getArmyCount() * 2;
        target->setArmyCount(doubled);

        // Transfer the territory to the neutral player
        Player* neutral = getNeutralPlayer();
        // Remove from issuer's territory list
        auto& issuerList = *issuer->getTerritoriesOwned();
        issuerList.erase(remove(issuerList.begin(), issuerList.end(), target), issuerList.end());
        // Add to neutral player's list
        neutral->getTerritoriesOwned()->push_back(target);
        target->setOwner(neutral);

        *effect = issuer->getName() + " blockaded " + target->getName() +
                  " — armies doubled to " + to_string(doubled) +
                  " and ownership transferred to Neutral.";
    }
    cout << *effect << endl;
    notify();
}

Blockade* Blockade::clone() const {
    return new Blockade(*this);
}

void Blockade::print(ostream& os) const {
    os << "Blockade Order [issuer: " << (issuer ? issuer->getName() : "null")
       << ", target: " << (target ? target->getName() : "null") << "]";
}

// ---------------------------------------------------------------------------
// Airlift
// ---------------------------------------------------------------------------

Airlift::Airlift(Player* issuer, Territory* source, Territory* target, int armies)
    : Order(), issuer(issuer), source(source), target(target), armies(new int(armies)) {}

Airlift::Airlift(const Airlift& other)
    : Order(other), issuer(other.issuer), source(other.source), target(other.target),
      armies(new int(*other.armies)) {}

Airlift::~Airlift() {
    delete armies;
}

Airlift& Airlift::operator=(const Airlift& other) {
    if (this != &other) {
        Order::operator=(other);
        issuer = other.issuer;
        source = other.source;
        target = other.target;
        *armies = *other.armies;
    }
    return *this;
}

bool Airlift::validate() const {
    if (!issuer || !source || !target || !armies || *armies <= 0) {
        return false;
    }
    if (source->getOwner() != issuer) {
        return false;
    }
    if (target->getOwner() != issuer) {
        return false;
    }
    return true;
}

void Airlift::execute() {
    if (!validate()) {
        string reason;
        if (!issuer || !source || !target || !armies || *armies <= 0)
            reason = "null or zero-army parameters.";
        else if (source->getOwner() != issuer)
            reason = source->getName() + " does not belong to " + issuer->getName() + ".";
        else
            reason = target->getName() + " does not belong to " + issuer->getName() + ".";
        *effect = "INVALID Airlift: " + reason;
    } else {
        int moved = min(*armies, source->getArmyCount());
        source->setArmyCount(source->getArmyCount() - moved);
        target->setArmyCount(target->getArmyCount() + moved);
        *effect = issuer->getName() + " airlifted " + to_string(moved) +
                  " armies from " + source->getName() + " to " + target->getName() +
                  " (non-adjacent).";
    }
    cout << *effect << endl;
    notify();
}

Airlift* Airlift::clone() const {
    return new Airlift(*this);
}

void Airlift::print(ostream& os) const {
    os << "Airlift Order [issuer: " << (issuer ? issuer->getName() : "null")
       << ", from: " << (source ? source->getName() : "null")
       << " -> to: " << (target ? target->getName() : "null")
       << ", armies: " << (armies ? to_string(*armies) : "null") << "]";
}

// ---------------------------------------------------------------------------
// Negotiate
// ---------------------------------------------------------------------------

Negotiate::Negotiate(Player* issuer, Player* targetPlayer)
    : Order(), issuer(issuer), targetPlayer(targetPlayer) {}

Negotiate::Negotiate(const Negotiate& other)
    : Order(other), issuer(other.issuer), targetPlayer(other.targetPlayer) {}

Negotiate::~Negotiate() {}

Negotiate& Negotiate::operator=(const Negotiate& other) {
    if (this != &other) {
        Order::operator=(other);
        issuer = other.issuer;
        targetPlayer = other.targetPlayer;
    }
    return *this;
}

bool Negotiate::validate() const {
    if (!issuer || !targetPlayer) {
        return false;
    }
    if (issuer == targetPlayer) {
        return false;
    }
    return true;
}

void Negotiate::execute() {
    if (!validate()) {
        string reason;
        if (!issuer || !targetPlayer)
            reason = "null parameters.";
        else
            reason = "cannot negotiate with yourself.";
        *effect = "INVALID Negotiate: " + reason;
    } else {
        issuer->addNegotiation(targetPlayer);
        targetPlayer->addNegotiation(issuer);
        *effect = issuer->getName() + " and " + targetPlayer->getName() +
                  " have negotiated a truce — neither can attack the other this turn.";
    }
    cout << *effect << endl;
    notify();
}

Negotiate* Negotiate::clone() const {
    return new Negotiate(*this);
}

void Negotiate::print(ostream& os) const {
    os << "Negotiate Order [issuer: " << (issuer ? issuer->getName() : "null")
       << ", target player: " << (targetPlayer ? targetPlayer->getName() : "null") << "]";
}

// ---------------------------------------------------------------------------
// OrdersList
// ---------------------------------------------------------------------------

OrdersList::OrdersList() {
    orders = new vector<Order*>();
}

OrdersList::OrdersList(const OrdersList& other) {
    orders = new vector<Order*>();
    for (size_t i = 0; i < other.orders->size(); i++) {
        orders->push_back((*other.orders)[i]->clone());
    }
}

OrdersList::~OrdersList() {
    for (size_t i = 0; i < orders->size(); i++) {
        delete (*orders)[i];
    }
    delete orders;
}

OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this != &other) {
        for (size_t i = 0; i < orders->size(); i++) {
            delete (*orders)[i];
        }
        orders->clear();
        for (size_t i = 0; i < other.orders->size(); i++) {
            orders->push_back((*other.orders)[i]->clone());
        }
    }
    return *this;
}

void OrdersList::addOrder(Order* order) {
    orders->push_back(order);
    notify();
}

string OrdersList::stringToLog() const {
    if (orders->empty()) return "Order added to list: (unknown)";
    ostringstream oss;
    oss << *orders->back();  // calls operator<< of order
    return "Order added to list: " + oss.str();
}

void OrdersList::remove(int index) {
    if (index >= 0 && index < (int)orders->size()) {
        delete (*orders)[index];
        orders->erase(orders->begin() + index);
    }
}

void OrdersList::move(int fromIndex, int toIndex) {
    if (fromIndex >= 0 && fromIndex < (int)orders->size() &&
        toIndex >= 0 && toIndex < (int)orders->size()) {
        Order* orderToMove = (*orders)[fromIndex];
        orders->erase(orders->begin() + fromIndex);
        orders->insert(orders->begin() + toIndex, orderToMove);
    }
}

void OrdersList::executeAll() {
    for (size_t i = 0; i < orders->size(); i++) {
        (*orders)[i]->execute();
    }
}

int OrdersList::size() const {
    return (int)orders->size();
}

Order* OrdersList::orderAt(int index) const {
    return orders->at(index);
}

ostream& operator<<(ostream& os, const OrdersList& list) {
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
