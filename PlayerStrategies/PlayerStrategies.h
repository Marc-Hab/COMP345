#pragma once
#include <vector>
#include <ostream>

// FORWARD DECLARATIONS
class Player;
class Territory;
class Deck;

// Abstract base class for all player strategies
class PlayerStrategy {
public:
    // Default implementation since no class members
    PlayerStrategy() = default;
    PlayerStrategy(const PlayerStrategy& other) = default;
    PlayerStrategy& operator=(const PlayerStrategy& other) = default;
    virtual ~PlayerStrategy() = default;

    virtual PlayerStrategy* clone() const = 0;

    // Returns true if an order was issued, false when the player is done for the turn
    virtual bool issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) = 0;
    virtual std::vector<Territory*> toAttack(const Player* player) = 0;
    virtual std::vector<Territory*> toDefend(const Player* player) = 0;

    // Stream insertion operator: delegates to virtual print() for polymorphic output
    friend std::ostream& operator<<(std::ostream& os, const PlayerStrategy& ps);

protected:
    virtual std::ostream& print(std::ostream& os) const = 0;
};

// Human player: prompts the user to make all decisions interactively
class HumanPlayerStrategy : public PlayerStrategy {
public:
    HumanPlayerStrategy() = default;
    HumanPlayerStrategy(const HumanPlayerStrategy& other) = default;
    HumanPlayerStrategy& operator=(const HumanPlayerStrategy& other) = default;
    ~HumanPlayerStrategy() override = default;

    PlayerStrategy* clone() const override;
    bool issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) override;
    std::vector<Territory*> toAttack(const Player* player) override;
    std::vector<Territory*> toDefend(const Player* player) override;

protected:
    std::ostream& print(std::ostream& os) const override;
};

// Aggressive player: deploys/advances on its strongest territory, always attacks enemy territories
class AggressivePlayerStrategy : public PlayerStrategy {
public:
    AggressivePlayerStrategy() = default;
    AggressivePlayerStrategy(const AggressivePlayerStrategy& other) = default;
    AggressivePlayerStrategy& operator=(const AggressivePlayerStrategy& other) = default;
    ~AggressivePlayerStrategy() override = default;

    PlayerStrategy* clone() const override;
    bool issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) override;
    std::vector<Territory*> toAttack(const Player* player) override;
    std::vector<Territory*> toDefend(const Player* player) override;

protected:
    std::ostream& print(std::ostream& os) const override;
};

// Benevolent player: deploys/advances on its weakest territories, never attacks
class BenevolentPlayerStrategy : public PlayerStrategy {
public:
    BenevolentPlayerStrategy() = default;
    BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other) = default;
    BenevolentPlayerStrategy& operator=(const BenevolentPlayerStrategy& other) = default;
    ~BenevolentPlayerStrategy() override = default;

    PlayerStrategy* clone() const override;
    bool issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) override;
    std::vector<Territory*> toAttack(const Player* player) override;
    std::vector<Territory*> toDefend(const Player* player) override;

protected:
    std::ostream& print(std::ostream& os) const override;
};

// Neutral player: never issues any order; becomes Aggressive if attacked
class NeutralPlayerStrategy : public PlayerStrategy {
public:
    NeutralPlayerStrategy() = default;
    NeutralPlayerStrategy(const NeutralPlayerStrategy& other) = default;
    NeutralPlayerStrategy& operator=(const NeutralPlayerStrategy& other) = default;
    ~NeutralPlayerStrategy() override = default;

    PlayerStrategy* clone() const override;
    bool issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) override;
    std::vector<Territory*> toAttack(const Player* player) override;
    std::vector<Territory*> toDefend(const Player* player) override;

protected:
    std::ostream& print(std::ostream& os) const override;
};

// Cheater player: automatically conquers all adjacent enemy territories once per turn
class CheaterPlayerStrategy : public PlayerStrategy {
public:
    CheaterPlayerStrategy() = default;
    CheaterPlayerStrategy(const CheaterPlayerStrategy& other) = default;
    CheaterPlayerStrategy& operator=(const CheaterPlayerStrategy& other) = default;
    ~CheaterPlayerStrategy() override = default;

    PlayerStrategy* clone() const override;
    bool issueOrder(Player* player, Deck* deck, const std::vector<Player*>* allPlayers) override;
    std::vector<Territory*> toAttack(const Player* player) override;
    std::vector<Territory*> toDefend(const Player* player) override;

protected:
    std::ostream& print(std::ostream& os) const override;
};
