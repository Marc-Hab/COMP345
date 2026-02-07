#define CARDS_H

#include <vector>

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
    CardType getType() const;
    void play(Deck* deck, Hand* hand);
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

};

class Hand {
private:
    std::vector<Card*>* hand;
public:
    Hand();
    Hand(const Hand& object);
    Hand& operator=(const Hand& object);
    ~Hand();
};
