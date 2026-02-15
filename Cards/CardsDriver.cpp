// CardsDriver.cpp
// Driver file to test the Cards component functionality

#include "Cards.h"
#include "../Players/Player.h"
#include "../Orders/Orders.h"
#include <iostream>

using namespace std;

void testCardsComponent() {
    cout << "========================================" << endl;
    cout << "    WARZONE CARDS COMPONENT DRIVER" << endl;
    cout << "========================================" << endl << endl;
    
    // ===== PART 1: Create a Deck of Cards =====
    cout << "PART 1: Creating Deck" << endl;
    Deck* deck = new Deck();
    cout << "\nDeck created with all card types:" << endl;
    cout << *deck << endl << endl;
    
    // ===== PART 2: Create a Hand and Draw Cards =====
    cout << "PART 2: Drawing Cards from Deck" << endl;
    
    // Create a player (hand needs to be associated with a player)
    Player* player = new Player("Alice");
    
    // Create a hand for the player
    Hand* hand = new Hand();
    hand->setPlayer(player);
    
    cout << "\nInitial hand (should be empty):" << endl;
    cout << *hand << endl << endl;
    
    // Draw 5 cards from the deck
    cout << "Drawing 5 cards from the deck..." << endl;
    for (int i = 0; i < 5; i++) {
        deck->draw(hand);
    }
    
    cout << "\nHand after drawing 5 cards:" << endl;
    cout << *hand << endl;
    
    cout << "\nDeck after drawing 5 cards:" << endl;
    cout << *deck << endl << endl;
    
    // ===== PART 3: Demonstrate draw() removes cards from deck =====
    cout << "PART 3: Verify Cards Removed from Deck" << endl;
    
    int initialDeckSize = deck->getCards()->size();
    cout << "Deck size before draw: " << initialDeckSize << endl;
    
    Card* drawnCard = deck->draw(hand);
    
    int newDeckSize = deck->getCards()->size();
    cout << "Deck size after draw: " << newDeckSize << endl;
    
    if (newDeckSize == initialDeckSize - 1) {
        cout << "Card successfully removed from deck" << endl;
    } else {
        cout << "ERROR: Deck size didn't decrease properly" << endl;
    }
    
    cout << "\nHand now has " << hand->getCards()->size() << " cards" << endl << endl;
    
    // ===== PART 4: Play All Cards in Hand =====
    cout << "PART 4: Playing All Cards in Hand" << endl;
    
    cout << "\nCurrent hand:" << endl;
    cout << *hand << endl;
    
    cout << "\nPlayer's orders before playing cards:" << endl;
    cout << *(player->getOrders()) << endl;
    
    int deckSizeBeforePlay = deck->getCards()->size();
    cout << "Deck size before playing cards: " << deckSizeBeforePlay << endl << endl;
    
    // Play all cards (need to iterate carefully since cards are removed)
    cout << "Playing all cards in hand..." << endl;
    cout << "----------------------------------------" << endl;
    
    int cardsToPlay = hand->getCards()->size();
    for (int i = 0; i < cardsToPlay; i++) {
        // Always play the first card since cards are removed after playing
        Card* cardToPlay = hand->getCards()->at(0);
        cout << "\n[" << (i+1) << "] Playing: " << *cardToPlay << endl;
        cardToPlay->play(deck, hand);
    }
    
    cout << "----------------------------------------" << endl << endl;
    
    // ===== PART 5: Verify Results After Playing Cards =====
    cout << "PART 5: Verification After Playing Cards" << endl;
    
    cout << "\nHand after playing all cards (should be empty):" << endl;
    cout << *hand << endl;
    
    if (hand->getCards()->empty()) {
        cout << "All cards removed from hand" << endl;
    } else {
        cout << "ERROR: Hand still contains cards" << endl;
    }
    
    cout << "\nPlayer's orders after playing cards:" << endl;
    cout << *(player->getOrders()) << endl;
    
    if (player->getOrders()->size() == cardsToPlay) {
        cout << cardsToPlay << " orders created" << endl;
    } else {
        cout << "ERROR: Wrong number of orders created" << endl;
    }
    
    int deckSizeAfterPlay = deck->getCards()->size();
    cout << "\nDeck size after playing cards: " << deckSizeAfterPlay << endl;
    
    if (deckSizeAfterPlay == deckSizeBeforePlay + cardsToPlay) {
        cout << "All " << cardsToPlay << " cards returned to deck" << endl;
    } else {
        cout << "ERROR: Cards not properly returned to deck" << endl;
    }
    
    cout << "\nFinal deck:" << endl;
    cout << *deck << endl << endl;
    

    // Cleanup
    delete hand;
    delete deck;
    delete player;
}

int main() {
    testCardsComponent();
    return 0;
}