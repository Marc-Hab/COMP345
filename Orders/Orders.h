// Orders.h

#ifndef ORDERS_H
#define ORDERS_H

#include <iostream>
#include <string>
#include <vector>
#include "../LoggingObserver/LoggingObserver.h"

// Relative paths based on your folder structure
#include "../Players/Player.h"
#include "../Maps/Map.h"

// FORWARD DECLARATIONS
class Player;
class Territory;
class Order : public Subject, public ILoggable {
public:
    Order();
    Order(const Order& other);
    virtual ~Order();
    Order& operator=(const Order& other);

    virtual bool validate() const = 0;
    virtual void execute() = 0;
    virtual Order* clone() const = 0;

    // Returns "Order executed: <effect>". Concrete in base class; all subclasses inherit it.
    std::string stringToLog() const override;

protected:
    std::string* effect;

private:
    friend std::ostream& operator<<(std::ostream& os, const Order& order);
    virtual void print(std::ostream& os) const = 0;
};

// ---------- Concrete order types ----------

class Deploy : public Order {
public:
    Deploy(); // Default constructor for compatibility
    Deploy(Player* player, Territory* target, int armies);
    Deploy(const Deploy& other);
    ~Deploy();
    Deploy& operator=(const Deploy& other);
    bool validate() const override;
    void execute() override;
    Deploy* clone() const override;
private:
    Player* issuingPlayer;
    Territory* targetTerritory;
    int numArmies;
    void print(std::ostream& os) const override;
};

class Advance : public Order {
public:
    Advance(); // Default constructor for compatibility
    Advance(Player* player, Territory* source, Territory* target, int armies);
    Advance(const Advance& other);
    ~Advance();
    Advance& operator=(const Advance& other);
    bool validate() const override;
    void execute() override;
    Advance* clone() const override;
private:
    Player* issuingPlayer;
    Territory* sourceTerritory;
    Territory* targetTerritory;
    int numArmies;
    void print(std::ostream& os) const override;
};

class Bomb : public Order {
public:
    Bomb(); // Default constructor for compatibility
    Bomb(Player* player, Territory* target);
    Bomb(const Bomb& other);
    ~Bomb();
    Bomb& operator=(const Bomb& other);
    bool validate() const override;
    void execute() override;
    Bomb* clone() const override;
private:
    Player* issuingPlayer;
    Territory* targetTerritory;
    void print(std::ostream& os) const override;
};

class Blockade : public Order {
public:
    Blockade(); // Default constructor for compatibility
    Blockade(Player* player, Territory* target);
    Blockade(const Blockade& other);
    ~Blockade();
    Blockade& operator=(const Blockade& other);
    bool validate() const override;
    void execute() override;
    Blockade* clone() const override;
private:
    Player* issuingPlayer;
    Territory* targetTerritory;
    void print(std::ostream& os) const override;
};

class Airlift : public Order {
public:
    Airlift(); // Default constructor for compatibility
    Airlift(Player* player, Territory* source, Territory* target, int armies);
    Airlift(const Airlift& other);
    ~Airlift();
    Airlift& operator=(const Airlift& other);
    bool validate() const override;
    void execute() override;
    Airlift* clone() const override;
private:
    Player* issuingPlayer;
    Territory* sourceTerritory;
    Territory* targetTerritory;
    int numArmies;
    void print(std::ostream& os) const override;
};

class Negotiate : public Order {
public:
    Negotiate(); // Default constructor for compatibility
    Negotiate(Player* player, Player* target);
    Negotiate(const Negotiate& other);
    ~Negotiate();
    Negotiate& operator=(const Negotiate& other);
    bool validate() const override;
    void execute() override;
    Negotiate* clone() const override;
private:
    Player* issuingPlayer;
    Player* targetPlayer;
    void print(std::ostream& os) const override;
};

// A class to hold a list of orders.
class OrdersList : public Subject, public ILoggable {
public:
    OrdersList();
    OrdersList(const OrdersList& other);
    ~OrdersList();
    OrdersList& operator=(const OrdersList& other);

    // Adds the order and notifies observers (triggers logging).
    void addOrder(Order* order);

    void remove(int index);
    void move(int fromIndex, int toIndex);
    void executeAll();
    int size() const;
    Order* orderAt(int index) const;

    std::string stringToLog() const override;

private:
    std::vector<Order*>* orders;

    friend std::ostream& operator<<(std::ostream& os, const OrdersList& list);
};

#endif
