// Orders.h

#ifndef ORDERS_H
#define ORDERS_H

#include <iostream>
#include <string>
#include <vector>
#include "../LoggingObserver/LoggingObserver.h"

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
    Deploy();
    Deploy(const Deploy& other);
    ~Deploy();
    Deploy& operator=(const Deploy& other);
    bool validate() const override;
    void execute() override;
    Deploy* clone() const override;
private:
    void print(std::ostream& os) const override;
};

class Advance : public Order {
public:
    Advance();
    Advance(const Advance& other);
    ~Advance();
    Advance& operator=(const Advance& other);
    bool validate() const override;
    void execute() override;
    Advance* clone() const override;
private:
    void print(std::ostream& os) const override;
};

class Bomb : public Order {
public:
    Bomb();
    Bomb(const Bomb& other);
    ~Bomb();
    Bomb& operator=(const Bomb& other);
    bool validate() const override;
    void execute() override;
    Bomb* clone() const override;
private:
    void print(std::ostream& os) const override;
};

class Blockade : public Order {
public:
    Blockade();
    Blockade(const Blockade& other);
    ~Blockade();
    Blockade& operator=(const Blockade& other);
    bool validate() const override;
    void execute() override;
    Blockade* clone() const override;
private:
    void print(std::ostream& os) const override;
};

class Airlift : public Order {
public:
    Airlift();
    Airlift(const Airlift& other);
    ~Airlift();
    Airlift& operator=(const Airlift& other);
    bool validate() const override;
    void execute() override;
    Airlift* clone() const override;
private:
    void print(std::ostream& os) const override;
};

class Negotiate : public Order {
public:
    Negotiate();
    Negotiate(const Negotiate& other);
    ~Negotiate();
    Negotiate& operator=(const Negotiate& other);
    bool validate() const override;
    void execute() override;
    Negotiate* clone() const override;
private:
    void print(std::ostream& os) const override;
};
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
