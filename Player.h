#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Map.h"
#include "Orders.h"
#include "Cards.h"

class Player {
private:
    std::string* name;
    std::vector<Territory*>* territoriesOwned;
    Hand* hand;
    OrdersList* ordersList;

public:
    // Constructors, destructor, operator overloads
    Player();
    Player(std::string playerName);
    Player(const Player& other);
    Player& operator=(const Player& other);
    ~Player();

    // Methods
    std::vector<Territory*> toDefend() const;
    std::vector<Territory*> toAttack() const;
    void issueOrder();

    // Getters
    std::string getName() const;

    // Operator Overload
    friend std::ostream& operator<<(std::ostream& out, const Player& p);
};
