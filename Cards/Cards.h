#ifndef CARDS_H
#define CARDS_H

#include <vector>
#include <iostream>
#include "../Orders/Orders.h"

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
    Card();
    Card(CardType cardType);
    Card(const Card& object);
    ~Card();
    Card& operator=(const Card& object);
    Order* play(Deck* deck, Hand* hand);
    friend std::ostream& operator<<(std::ostream& outs, const Card& card);
};

class Deck {
private:
    std::vector<Card*>* deck;

public:
    void draw(Hand* hand);
    Deck();
    Deck(const Deck& object);
    Deck& operator=(const Deck& object);
    ~Deck();
    void addCardToDeck(Card* card);
    Card* removeCardFromDeck(Card* card);
    friend std::ostream& operator<<(std::ostream& outs, const Deck& deck);
};

class Hand {
private:
    std::vector<Card*>* hand;
public:
    Hand();
    Hand(const Hand& object);
    Hand& operator=(const Hand& object);
    ~Hand();
    void addCardToHand(Card* card);
    Card* removeCardFromHand(Card* card);
    friend std::ostream& operator<<(std::ostream& outs, const Hand& hand);
    Card* getCard(int index);
    std::vector<Card*>* getHand();
};

#endif
