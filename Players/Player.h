#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include <iostream>

// FORWARD DECLARATIONS
class Territory;
class Hand;
class OrdersList;
class Deck;
class PlayerStrategy;

class Player {
private:
    std::string* name;
    std::vector<Territory*>* territoriesOwned;
    Hand* hand;
    OrdersList* ordersList;
    int* reinforcementPool;
    PlayerStrategy* strategy;

    // Data for each turn
    int* ordersIssuedThisTurn;              // number of non-deploy orders issued this turn
    std::vector<Player*>* negotiatedWith;   // players who have been negotiated with
    bool* conqueredThisTurn;                // true if a player conquered a territory this turn

public:
    // Constructors, destructor, operator overloads
    Player(std::string playerName);
    Player(const Player& other);
    Player& operator=(const Player& other);
    ~Player();

    // Methods
    std::vector<Territory*> toDefend() const;
    std::vector<Territory*> toAttack() const;
    // Returns true if an order was issued, false when done for the turn.
    // allPlayers is needed to create Negotiate orders from Diplomacy cards.
    bool issueOrder(Deck* deck, const std::vector<Player*>* allPlayers = nullptr);
    void resetOrderIssuingState(); // resets the data before the start of each turn
    void incrementOrdersIssuedThisTurn(){(*ordersIssuedThisTurn)++;};

    // Getters
    std::string getName() const;
    OrdersList* getOrders() const;
    std::vector<Territory*>* getTerritoriesOwned() const;
    Hand* getHand() const;
    int getReinforcementPool() const;
    bool getConqueredThisTurn() const;
    int getOrdersIssuedThisTurn() const;

    // Setters
    void setReinforcementPool(int count);
    void addReinforcements(int count);
    void setConqueredThisTurn(bool val);

    // Negotiation helpers
    void addNegotiation(Player* other);
    bool hasNegotiatedWith(Player* other) const;

    // Operator Overload
    friend std::ostream& operator<<(std::ostream& out, const Player& p);
};

#endif
