#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include <iostream>

// FORWARD DECLARATIONS
class Territory;
class Hand;
class OrdersList;

class Player {
private:
    std::string* name;
    std::vector<Territory*>* territoriesOwned;
    Hand* hand;
    OrdersList* ordersList;

public:
    // Constructors, destructor, operator overloads
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
    OrdersList* getOrders() const;
    std::vector<Territory*>* getTerritoriesOwned() const;
    Hand* getHand() const;

    // Operator Overload
    friend std::ostream& operator<<(std::ostream& out, const Player& p);
};

#endif
