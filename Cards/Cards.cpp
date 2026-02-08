#include "Cards.h"

Hand;
Deck;

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

void Card::play( Deck* deck, Hand* hand) {
    switch (*type) {
        case CardType::BOMB:
            std::cout << "Making Bomb order" << std::endl;
            break;
        case CardType::REINFORCEMENT:
            std::cout << "Making Reinforcement order" << std::endl;
            break;
        case CardType::BLOCKADE:
            std::cout << "Making Blockade order" << std::endl;
            break;
        case CardType::AIRLIFT:
            std::cout << "Making Airlift order" << std::endl;
            break;
        case CardType::DIPLOMACY:
            std::cout << "Making Diplomacy order" << std::endl;
            break;
    }
    
    Card* removedCard = hand->removeCardFromHand(this);
    
    if (removedCard != nullptr) {
        deck->addCardToDeck(removedCard);
        std::cout << "Card removed from hand and readded to deck" << std::endl;
    };
}

void Deck::draw(Hand* hand) {
    if (deck->empty()) {
        std::cout << "the deck is empty" << std::endl;
        return;
    }
    int randomIndex = rand() % deck->size();
    Card* drawnCard = (*deck)[randomIndex];
    this->removeCardFromDeck(drawnCard);
    hand->addCardToHand(drawnCard);
    std::cout << "Drew: " << *drawnCard << std::endl;
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

Deck::~Deck() {
    for (Card* card : *deck) { delete card; }
    delete deck;
    deck = nullptr;
}

int Deck::size() {
    return deck->size();
}

Card* Deck::removeCardFromDeck(Card* card) {

    for (size_t i = 0; i < deck->size(); ++i) {
        if ((*deck)[i] == card) {
            Card* removedCard = (*deck)[i];
            deck->erase(deck->begin() + i);
            return removedCard;
        }
    }
    return nullptr;
}