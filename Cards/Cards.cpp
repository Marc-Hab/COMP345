#include "Cards.h"
#include "../Orders/Orders.h"
#include "../Players/Player.h"
#include <algorithm>
#include <random>


// ============== Card implementation ======================

/**
 * Parameterized Constructor
 */
Card::Card(CardType cardType) {
    type = new CardType(cardType);
}

/**
 * Copy Constructor (deep copy)
 */
Card::Card(const Card& other) {
    type = new CardType(*(other.type));
}

/**
 * Destructor
 */
Card::~Card() {
    delete type;
    type = nullptr;
}

/**
 * Assignment Operator
 */
Card& Card::operator=(const Card& other) {
    if (this == &other) {
        return *this;
    }
    
    // Copy the type value
    *type = *(other.type);
    
    return *this;
}

/**
 * Stream Insertion Operator
 */
std::ostream& operator<<(std::ostream& outs, const Card& card) {
    switch (*(card.type)) {
        case CardType::BOMB:
            outs << "Bomb Card";
            break;
        case CardType::REINFORCEMENT:
            outs << "Reinforcement Card";
            break;
        case CardType::BLOCKADE:
            outs << "Blockade Card";
            break;
        case CardType::AIRLIFT:
            outs << "Airlift Card";
            break;
        case CardType::DIPLOMACY:
            outs << "Diplomacy Card";
            break;
    }
    return outs;
}

/**
 * Get Type
 */
CardType Card::getType() const {
    return *type;
}


void Card::play(Deck* deck, Hand* hand) {

    // Remove card from hand 
    Card* removedCard = hand->removeCard(this);

    // If card was not in hand, don't do anything
    if (removedCard == nullptr){ return; }

    Order* newOrder = nullptr;

    switch (*type) {
        case CardType::BOMB:
            std::cout << "Making Bomb order" << std::endl;
            newOrder = new Bomb();
            break;
        case CardType::REINFORCEMENT:
            std::cout << "Making Reinforcement order (Deploy)" << std::endl;
            newOrder = new Deploy();
            break;
        case CardType::BLOCKADE:
            std::cout << "Making Blockade order" << std::endl;
            newOrder = new Blockade();
            break;
        case CardType::AIRLIFT:
            std::cout << "Making Airlift order" << std::endl;
            newOrder = new Airlift();
            break;
        case CardType::DIPLOMACY:
            std::cout << "Making Diplomacy order (Negotiate)" << std::endl;
            newOrder = new Negotiate();
            break;
    }

    // Add order to the player's orders list
    hand->getPlayer()->getOrders()->addOrder(newOrder);
    
    // Add card back to deck
    deck->addCard(removedCard);
    std::cout << "Card removed from hand and readded to deck" << std::endl;
}


// ============== End of Card implementation ================






// ============== Hand implementation ======================

/*
*   Default constructor
*/
Hand::Hand() {
    cards = new std::vector<Card*>();
    player = nullptr;
}

/*
*   Copy constructor (shallow copy)
*/
Hand::Hand(const Hand& other) {
    cards = new std::vector<Card*>();
    cards->reserve(other.cards->size());
    
    for (const Card* card : *(other.cards)) {
        cards->push_back(new Card(*card));  // Deep copy
    }
    
    player = other.player;  // Shallow copy
}

/*
*   Destructor
*/
Hand::~Hand() {
    
    // Delete cards
    for (Card* card : *cards) {
        delete card;
    }
    delete cards;

    // Don't delete player
    player = nullptr;
}

/*
*   Assignment operator (shallow copy)
*/
Hand& Hand::operator=(const Hand& other) {
    if (this == &other) return *this;
    
    for (Card* card : *cards) {
        delete card;
    }
    cards->clear();
    cards->reserve(other.cards->size());
    
    for (const Card* card : *(other.cards)) {
        cards->push_back(new Card(*card));
    }
    
    player = other.player;
    return *this;
}


/*
*   Stream insertion operator
*/
std::ostream& operator<<(std::ostream& outs, const Hand& hand) {
    if (hand.player) {
        outs << hand.player->getName() << "'s Hand:\n";
    } else {
        outs << "Hand:\n";
    }
    
    if (hand.cards->empty()) {
        outs << "  (empty)";
    } else {
        outs << "  " << hand.cards->size() << " card(s):\n";
        for (size_t i = 0; i < hand.cards->size(); i++) {
            outs << "    " << (i + 1) << ". " << *(hand.cards->at(i)) << "\n";
        }
    }
    
    return outs;
}

/*
*   Add a card to hand
*/
void Hand::addCard(Card* card) {
    if (card != nullptr) {
        cards->push_back(card);
    }
}

/*
*   Remove a card from hand
*/
Card* Hand::removeCard(Card* card) {
    
    for (size_t i = 0; i < cards->size(); ++i) {
        
        if ((*cards)[i] == card) {
            
            Card* removedCard = (*cards)[i];
            cards->erase(cards->begin() + i);
            
            return removedCard;
        }
    }
    
    return nullptr;
}

/*
*   Getters and setters
*/

std::vector<Card*>* Hand::getCards() const{
    return cards;
};

Player* Hand::getPlayer() const{
    return player;
};

void Hand::setPlayer(Player* p){
    player = p;
};



// ============== End of Hand implementation ======================


// ============== Deck implementation ======================

/*
*   Shuffle cards in the deck
*/
void Deck::shuffle() {
    std::random_device rd;   // Get random seed
    std::mt19937 g(rd());    // Create random generator
    std::shuffle(cards->begin(), cards->end(), g);
}

/*
*   Default constructor (50 cards total 10 of each type)
*/
Deck::Deck() {

    cards = new std::deque<Card*>();

    const int NUMBER_OF_CARDS_OF_EACH_TYPE = 10;

    for (int i = 0; i < NUMBER_OF_CARDS_OF_EACH_TYPE; ++i) {
        cards->push_back(new Card(CardType::BOMB));
        cards->push_back(new Card(CardType::REINFORCEMENT));
        cards->push_back(new Card(CardType::BLOCKADE));
        cards->push_back(new Card(CardType::AIRLIFT));
        cards->push_back(new Card(CardType::DIPLOMACY));
    };

    shuffle();
};

/*
*   Destructor
*/
Deck::~Deck() {
    for (Card* card : *cards) { 
        delete card; 
    }

    delete cards;
    cards = nullptr;
}

/**
 * Copy Constructor (deep copy)
 */
Deck::Deck(const Deck& other) {
    cards = new std::deque<Card*>();
    
    for (const Card* card : *(other.cards)) {
        cards->push_back(new Card(*card));  // Deep copy
    }
}

/**
 * Assignment Operator (deep copy)
 */
Deck& Deck::operator=(const Deck& other) {
    if (this == &other) {
        return *this;  // Self-assignment check
    }
    
    // Delete existing cards
    for (Card* card : *cards) {
        delete card;
    }
    cards->clear();  // Clear the vector
    
    
    // Deep copy new cards
    for (const Card* card : *(other.cards)) {
        cards->push_back(new Card(*card));
    }
    
    return *this;
}

/*
* Stream Insertion Operator
*/
std::ostream& operator<<(std::ostream& outs, const Deck& deck) {
    outs << "Deck contains " << deck.cards->size() << " cards";
    
    if (!deck.cards->empty()) {
        int counts[5] = {0};  // Count each type
        
        for (const Card* card : *(deck.cards)) {
            counts[static_cast<int>(card->getType())]++;
        }
        
        outs << ":\n";
        outs << "  Bomb: " << counts[0] << "\n";
        outs << "  Reinforcement: " << counts[1] << "\n";
        outs << "  Blockade: " << counts[2] << "\n";
        outs << "  Airlift: " << counts[3] << "\n";
        outs << "  Diplomacy: " << counts[4];
    }
    
    return outs;
}

/*
* Draw card from deck and insert it into hand
*/
Card* Deck::draw(Hand* hand) {

    if (cards->empty()) {
        std::cout << "Cannot draw: deck is empty" << std::endl;
        return nullptr;
    }

    Card* drawnCard = cards->front();
    cards->pop_front();

    hand->addCard(drawnCard);

    std::cout << "Drew: " << *drawnCard << std::endl;

    return drawnCard;
}

/*
* Adds a card to the back of the deck
*/
void Deck::addCard(Card* card) {
    
    if (card != nullptr) {
        cards->push_back(card);
    }
}


/*
* Get cards in deck
*/
std::deque<Card*>* Deck::getCards() const {
    return cards;
}

// ============== End of Deck implementation ======================