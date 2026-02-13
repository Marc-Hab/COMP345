// Orders.cpp
// This file contains the implementation for the Order and OrdersList classes.

#include "Orders.h"
#include <iostream>
#include <algorithm>

// Order Class

Order::Order() {
    effect = new std::string("");
}

Order::Order(const Order& other) {
    effect = new std::string(*other.effect);
}

Order::~Order() {
    delete effect;
    effect = nullptr;
}

Order& Order::operator=(const Order& other) {
    if (this != &other) {
        *effect = *other.effect;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Order& order) {
    order.print(os);
    if (!order.effect->empty()) {
        os << " | EFFECT: " << *order.effect;
    }
    return os;
}

// Deploy Class

Deploy::Deploy() : Order() {}
Deploy::Deploy(const Deploy& other) : Order(other) {}
Deploy::~Deploy() {}
Deploy& Deploy::operator=(const Deploy& other) {
    Order::operator=(other);
    return *this;
}
bool Deploy::validate() const {
    return true; // For now we'll consider all orders as true (valid)
}
void Deploy::execute() {
    if (validate()) {
        *effect = "Deployed armies.";
    }
}
Deploy* Deploy::clone() const {
    return new Deploy(*this);
}
void Deploy::print(std::ostream& os) const {
    os << "Deploy Order";
}

//Advance Class

Advance::Advance() : Order() {}
Advance::Advance(const Advance& other) : Order(other) {}
Advance::~Advance() {}
Advance& Advance::operator=(const Advance& other) { Order::operator=(other); return *this; }
bool Advance::validate() const { return true; }
void Advance::execute() { if (validate()) { *effect = "Advanced armies."; } }
Advance* Advance::clone() const { return new Advance(*this); }
void Advance::print(std::ostream& os) const { os << "Advance Order"; }

// Bomb Class

Bomb::Bomb() : Order() {}
Bomb::Bomb(const Bomb& other) : Order(other) {}
Bomb::~Bomb() {}
Bomb& Bomb::operator=(const Bomb& other) { Order::operator=(other); return *this; }
bool Bomb::validate() const { return true; }
void Bomb::execute() { if (validate()) { *effect = "Bombed a territory."; } }
Bomb* Bomb::clone() const { return new Bomb(*this); }
void Bomb::print(std::ostream& os) const { os << "Bomb Order"; }

// Blockade Class

Blockade::Blockade() : Order() {}
Blockade::Blockade(const Blockade& other) : Order(other) {}
Blockade::~Blockade() {}
Blockade& Blockade::operator=(const Blockade& other) { Order::operator=(other); return *this; }
bool Blockade::validate() const { return true; }
void Blockade::execute() { if (validate()) { *effect = "Blockaded a territory."; } }
Blockade* Blockade::clone() const { return new Blockade(*this); }
void Blockade::print(std::ostream& os) const { os << "Blockade Order"; }

//Airlift Class

Airlift::Airlift() : Order() {}
Airlift::Airlift(const Airlift& other) : Order(other) {}
Airlift::~Airlift() {}
Airlift& Airlift::operator=(const Airlift& other) { Order::operator=(other); return *this; }
bool Airlift::validate() const { return true; }
void Airlift::execute() { if (validate()) { *effect = "Airlifted armies."; } }
Airlift* Airlift::clone() const { return new Airlift(*this); }
void Airlift::print(std::ostream& os) const { os << "Airlift Order"; }

// Negotiate Class

Negotiate::Negotiate() : Order() {}
Negotiate::Negotiate(const Negotiate& other) : Order(other) {}
Negotiate::~Negotiate() {}
Negotiate& Negotiate::operator=(const Negotiate& other) { Order::operator=(other); return *this; }
bool Negotiate::validate() const { return true; }
void Negotiate::execute() { if (validate()) { *effect = "Negotiated a truce."; } }
Negotiate* Negotiate::clone() const { return new Negotiate(*this); }
void Negotiate::print(std::ostream& os) const { os << "Negotiate Order"; }


// OrdersList Class 

OrdersList::OrdersList() {
    orders = new std::vector<Order*>();
}

OrdersList::OrdersList(const OrdersList& other) {
    orders = new std::vector<Order*>();
    // Make a deep copy of each order in the other list
    for (int i = 0; i < other.orders->size(); i++) {
        orders->push_back((*other.orders)[i]->clone());
    }
}

// to delete the Order objects
OrdersList::~OrdersList() {
    for (int i = 0; i < orders->size(); i++) {
        delete (*orders)[i];
    }
    delete orders;
}

// Assignment operator also for deep copy
OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this != &other) {
        // First, delete the old orders in this list
        for (int i = 0; i < orders->size(); i++) {
            delete (*orders)[i];
        }
        orders->clear();

        // copy  new orders from the other list
        for (int i = 0; i < other.orders->size(); i++) {
            orders->push_back((*other.orders)[i]->clone());
        }
    }
    return *this;
}

void OrdersList::addOrder(Order* order) {
    orders->push_back(order);
}

void OrdersList::remove(int index) {
    if (index >= 0 && index < orders->size()) {
        delete (*orders)[index]; // delete order object
        orders->erase(orders->begin() + index); // remove  pointer from vector
    }
}

void OrdersList::move(int fromIndex, int toIndex) {
    if (fromIndex >= 0 && fromIndex < orders->size() &&
        toIndex >= 0 && toIndex < orders->size()) {
        Order* orderToMove = (*orders)[fromIndex];
        orders->erase(orders->begin() + fromIndex);
        orders->insert(orders->begin() + toIndex, orderToMove);
    }
}

void OrdersList::executeAll() {
    for (int i = 0; i < orders->size(); i++) {
        (*orders)[i]->execute();
    }
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