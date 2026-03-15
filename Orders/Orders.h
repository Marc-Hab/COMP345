// Orders.h
// This file declares the classes for the orders part of the project.

#ifndef ORDERS_H
#define ORDERS_H

#include <iostream>
#include <string>
#include <vector>

// FORWARD DECLARATIONS
class Player;
class Territory;

// Base class for all orders
class Order {
public:
    Order();
    Order(const Order& other);
    virtual ~Order();
    Order& operator=(const Order& other);

    virtual bool validate() const = 0;
    virtual void execute() = 0;
    virtual Order* clone() const = 0;
    virtual bool isDeploy() const { return false; }

protected:
    std::string* effect;

private:
    friend std::ostream& operator<<(std::ostream& os, const Order& order);
    virtual void print(std::ostream& os) const = 0;
};

// Deploy: move armies from reinforcement pool to an owned territory
class Deploy : public Order {
public:
    Deploy(Player* issuer, Territory* target, int armies);
    Deploy(const Deploy& other);
    ~Deploy();
    Deploy& operator=(const Deploy& other);
    bool validate() const override;
    void execute() override;
    Deploy* clone() const override;
    bool isDeploy() const override { return true; }
private:
    void print(std::ostream& os) const override;
    Player*    issuer;  // non-owning
    Territory* target;  // non-owning
    int*       armies;  // owning
};

// Advance: move armies from source to adjacent target (defend or attack)
class Advance : public Order {
public:
    Advance(Player* issuer, Territory* source, Territory* target, int armies);
    Advance(const Advance& other);
    ~Advance();
    Advance& operator=(const Advance& other);
    bool validate() const override;
    void execute() override;
    Advance* clone() const override;
private:
    void print(std::ostream& os) const override;
    Player*    issuer;  // non-owning
    Territory* source;  // non-owning
    Territory* target;  // non-owning
    int*       armies;  // owning
};

// Bomb: remove half the armies from an adjacent enemy territory (requires Bomb card)
class Bomb : public Order {
public:
    Bomb(Player* issuer, Territory* target);
    Bomb(const Bomb& other);
    ~Bomb();
    Bomb& operator=(const Bomb& other);
    bool validate() const override;
    void execute() override;
    Bomb* clone() const override;
private:
    void print(std::ostream& os) const override;
    Player*    issuer;  // non-owning
    Territory* target;  // non-owning
};

// Blockade: double armies on own territory and transfer it to the Neutral player (requires Blockade card)
class Blockade : public Order {
public:
    Blockade(Player* issuer, Territory* target);
    Blockade(const Blockade& other);
    ~Blockade();
    Blockade& operator=(const Blockade& other);
    bool validate() const override;
    void execute() override;
    Blockade* clone() const override;
private:
    void print(std::ostream& os) const override;
    Player*    issuer;  // non-owning
    Territory* target;  // non-owning
};

// Airlift: move armies between two owned territories regardless of adjacency (requires Airlift card)
class Airlift : public Order {
public:
    Airlift(Player* issuer, Territory* source, Territory* target, int armies);
    Airlift(const Airlift& other);
    ~Airlift();
    Airlift& operator=(const Airlift& other);
    bool validate() const override;
    void execute() override;
    Airlift* clone() const override;
private:
    void print(std::ostream& os) const override;
    Player*    issuer;  // non-owning
    Territory* source;  // non-owning
    Territory* target;  // non-owning
    int*       armies;  // owning
};

// Negotiate: prevent attacks between issuer and target player for the remainder of the turn (requires Diplomacy card)
class Negotiate : public Order {
public:
    Negotiate(Player* issuer, Player* targetPlayer);
    Negotiate(const Negotiate& other);
    ~Negotiate();
    Negotiate& operator=(const Negotiate& other);
    bool validate() const override;
    void execute() override;
    Negotiate* clone() const override;
private:
    void print(std::ostream& os) const override;
    Player* issuer;        // non-owning
    Player* targetPlayer;  // non-owning
};


// A class to hold a list of orders.
class OrdersList {
public:
    OrdersList();
    OrdersList(const OrdersList& other);
    ~OrdersList();
    OrdersList& operator=(const OrdersList& other);

    void addOrder(Order* order);
    void remove(int index);
    void move(int fromIndex, int toIndex);
    void executeAll();
    int size() const;
    Order* orderAt(int index) const;

private:
    std::vector<Order*>* orders;

    friend std::ostream& operator<<(std::ostream& os, const OrdersList& list);
};

#endif
