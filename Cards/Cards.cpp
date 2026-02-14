#include "Cards.h"
#include "../Orders/Orders.h"

Order* Card::play( Deck* deck, Hand* hand) {
    Order* order = nullptr;
    switch (*type) {
        case CardType::BOMB:
            std::cout << "Making Bomb order" << std::endl;
            order = new Bomb();
            break;
        case CardType::REINFORCEMENT:
            std::cout << "Making Reinforcement order (Deploy)" << std::endl;
            order = new Deploy();
            break;
        case CardType::BLOCKADE:
            std::cout << "Making Blockade order" << std::endl;
            order = new Blockade();
            break;
        case CardType::AIRLIFT:
            std::cout << "Making Airlift order" << std::endl;
            order = new Airlift();
            break;
        case CardType::DIPLOMACY:
            std::cout << "Making Diplomacy order (Negotiate)" << std::endl;
            order = new Negotiate();
            break;
    }
    
    Card* removedCard = hand->removeCardFromHand(this);
    
    if (removedCard != nullptr) {
        deck->addCardToDeck(removedCard);
        std::cout << "Card removed from hand and readded to deck" << std::endl;
    };
    return order;
}

void Deck::draw(Hand* hand) {
    if (deck->empty()) {
        std::cout << "the deck is empty" << std::endl;
        return;
    }
    int index = rand() % deck->size();
    Card* drawnCard = (*deck)[index];
    this->removeCardFromDeck(drawnCard);
    hand->addCardToHand(drawnCard);
    std::cout << "Drew: " << *drawnCard << std::endl;
}

void Deck::addCardToDeck(Card* card) {
    if (card != nullptr) {
        deck->push_back(card);
    }
}

Card::Card() {}

Card::Card(CardType cardType) {
    type = new CardType(cardType);
}

Card::Card(const Card& object) {
    type = new CardType(*object.type);
}

Card::~Card() {
    delete type;
    type = nullptr;
}

Card& Card::operator=(const Card& object) {
    if (this == &object) {
        return *this;
    }
    delete type;
    type = nullptr;

    type = new CardType(*object.type);
    return *this;
}

Card* Hand::getCard(int index){
    if (index >= 0 && index < static_cast<int>(hand->size())) {
        return (*hand)[index];
    }
    return nullptr;
}

void Hand::addCardToHand(Card* card) {
    if (card != nullptr) {
        hand->push_back(card);
    }
}

Card* Hand::removeCardFromHand(Card* card) {
    for (int i = 0; i < hand->size(); ++i) {
        if ((*hand)[i] == card) {
            Card* removedCard = (*hand)[i];
            hand->erase(hand->begin() + i);
            return removedCard;
        }
    }
    return nullptr;
}

Deck::Deck() {
    deck = new std::vector<Card*>();

    for (int i = 0; i < 3; i++) {
        deck->push_back(new Card(CardType::BOMB));
        deck->push_back(new Card(CardType::REINFORCEMENT));
        deck->push_back(new Card(CardType::BLOCKADE));
        deck->push_back(new Card(CardType::AIRLIFT));
        deck->push_back(new Card(CardType::DIPLOMACY));
    };
};

Deck::Deck(const Deck& object){
    deck = new std::vector<Card*>();
    for (Card* card : *object.deck) {
        deck->push_back(new Card(*card));
    }
}

Deck::~Deck() {
    for (Card* card : *deck) { delete card; }
    delete deck;
    deck = nullptr;
}

Deck& Deck::operator=(const Deck& object){
    if(this == &object) {
        return *this;
    }
    for (Card* card : *deck) {
        delete card;
    }
    delete deck;
    deck = nullptr;

    deck = new std::vector<Card*>();
    for (Card* card : *object.deck) {
        deck->push_back(new Card(*card));
    }

    return *this;
}

Card* Deck::removeCardFromDeck(Card* card) {

    for (int i = 0; i < deck->size(); ++i) {
        if ((*deck)[i] == card) {
            Card* removedCard = (*deck)[i];
            deck->erase(deck->begin() + i);
            return removedCard;
        }
    }
    return nullptr;
}

Hand::Hand() {
    hand = new std::vector<Card*>();
}

Hand::Hand(const Hand& object){
    hand = new std::vector<Card*>();
    for (Card* card : *object.hand){
        hand-> push_back(new Card(*card));
    }
}

Hand::~Hand() {
    for (Card* card : *hand) {delete card;}
    delete hand;
    hand = nullptr;
}
std::vector<Card*>* Hand::getHand() {
    return hand;
    }

Hand& Hand::operator=(const Hand& object) {
    if(this == &object) {
        return *this;
    }
    for (Card* card : *hand) {
        delete card;
    }
    delete hand;
    hand = nullptr;

    hand = new std::vector<Card*>();
    for (Card* card : *object.hand) {
        hand->push_back(new Card(*card));
    }
    return *this;
}

std::ostream& operator<<(std::ostream& out, const Card& card) {
    switch (*card.type) {
        case CardType::BOMB:
            out << "BOMB Card";
            break;
        case CardType::REINFORCEMENT:
            out << "REINFORCEMENT Card";
            break;
        case CardType::BLOCKADE:
            out << "BLOCKADE Card";
            break;
        case CardType::AIRLIFT:
            out << "AIRLIFT Card";
            break;
        case CardType::DIPLOMACY:
            out << "DIPLOMACY Card";
            break;
        default:
            out << "Invalid Card";
            break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const Hand& hand) {
    out << "Hand contains the following cards:" << std::endl;
    for (const Card* card : *hand.hand) {
        if (card) {
            out << "- " << *card << std::endl;
        }
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const Deck& deck) {
    out << "Deck contains the following cards:" << std::endl;
    for (const Card* card : *deck.deck) {
        if (card) {
            out << "- " << *card << std::endl;
        }
    }
    return out;
}