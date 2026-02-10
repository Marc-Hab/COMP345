#include "Cards.h"
#include <iostream>
#include <ctime>

int main() {

    Deck* deck = new Deck();
    std::cout << "\nInitial deck of cards:" << std::endl;
    std::cout << *deck << std::endl;

    Hand* hand = new Hand();
    std::cout << "\nDrawing 3 cards from the deck:" << std::endl;
    for (int i = 0; i < 3; ++i) {
        deck->draw(hand);
    }

    std::cout << "\nHand after drawing cards:" << std::endl;
    std::cout << *hand << std::endl;

    std::cout << "\nDeck after drawing cards:" << std::endl;
    std::cout <<*deck << std::endl;

    std::cout <<"\nPlaying cards:" << std::endl;
    std::cout << std::endl;

    while (hand->getHand()->size() > 0) {
        Card* picked = hand->getCard(0);
        picked->play(deck, hand);
        std::cout << std::endl;
    }

    std::cout << "\nHand after playing cards:" << std::endl;
    std::cout << *hand << std::endl;

    std::cout << "\nDeck after playing cards:" << std::endl;
    std::cout << *deck << std::endl;

    delete hand;
    delete deck;

    return 0;
}