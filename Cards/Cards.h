#ifndef CARDS_H
#define CARDS_H

#include <vector>
#include <deque>
#include <iostream>

// FORWARD DECLARATIONS
class Player;
class Order;
class Deck;
class Hand;

enum class CardType {
    BOMB,
    REINFORCEMENT,
    BLOCKADE,
    AIRLIFT,
    DIPLOMACY
};

class Card {
private:
    CardType* type;

public:
    // Constructors and Destructor
    Card(CardType cardType);
    Card(const Card& object);
    ~Card();

    // Assignment and stream insertion operators
    Card& operator=(const Card& object);
    friend std::ostream& operator<<(std::ostream& outs, const Card& card);
    
    void play(Deck* deck, Hand* hand);
    
    // Getter
    CardType getType() const;
};

class Hand {
private:
    std::vector<Card*>* cards;
    Player* player;
public:
    // Constructors and Destructor
    Hand();
    Hand(const Hand& object);
    ~Hand();
    
    // Assignment and stream insertion operators
    Hand& operator=(const Hand& object);
    friend std::ostream& operator<<(std::ostream& outs, const Hand& hand);


    void addCard(Card* card);
    Card* removeCard(Card* card);

    // Getters and setters
    std::vector<Card*>* getCards() const;
    Player* getPlayer() const;

    void setPlayer(Player* p);

};

class Deck {
private:
    std::deque<Card*>* cards;

public:
    // Constructors and Destructor
    Deck();
    Deck(const Deck& object);
    ~Deck();

    // Assignment and stream insertion operators
    Deck& operator=(const Deck& object);
    friend std::ostream& operator<<(std::ostream& outs, const Deck& deck);
    

    void shuffle();
    Card* draw(Hand* hand);

    void addCard(Card* card);

    std::deque<Card*>* getCards() const;
};

#endif
