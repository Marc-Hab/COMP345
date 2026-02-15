// Orders.h
// This file declares the classes for the orders part of the project.

#ifndef ORDERS_H
#define ORDERS_H

#include <iostream>
#include <string>
#include <vector>

class Order;

// Base class for all orders
class Order {
public:
    Order();
    Order(const Order& other);
    virtual ~Order(); // Trying out virtual destructor
    Order& operator=(const Order& other);

    virtual bool validate() const = 0;
    virtual void execute() = 0;
    virtual Order* clone() const = 0; // Doing a deep copy

protected:
    std::string* effect;

private:
    friend std::ostream& operator<<(std::ostream& os, const Order& order);
    virtual void print(std::ostream& os) const = 0;
};

// Classes for each type of Order

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

private:
    // This is a pointer to a vector of Order pointers, as per the assignment requirements.
    std::vector<Order*>* orders;

    friend std::ostream& operator<<(std::ostream& os, const OrdersList& list);
};

#endif