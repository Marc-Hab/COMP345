// OrdersDriver.cpp
// Main file to test the OrdersList functionality and Order Execution as per Part 4.

#include "Orders.h"
#include "../Players/Player.h"
#include "../Maps/Map.h"
#include <iostream>

void testOrders() {
    // Setting up objects for Part 4 execution testing
    std::cout << "========= SETTING UP GAME STATE =========\n";
    Player* p1 = new Player("Player 1");
    Player* p2 = new Player("Player 2");
    Territory* t1 = new Territory("Canada");
    Territory* t2 = new Territory("USA");
    
    t1->setOwner(p1);
    t1->setArmyCount(10);
    t2->setOwner(p2);
    t2->setArmyCount(5);
    t1->addAdjacentTerritory(t2);

    // 1. Create one of each kind of orders
    std::cout << "========= Creating Orders =========\n";
    // Now using parameterized constructors required for execution logic
    Deploy* deployOrder = new Deploy(p1, t1, 5);
    Advance* advanceOrder = new Advance(p1, t1, t2, 10); // Attack USA from Canada
    Bomb* bombOrder = new Bomb(p1, t2);
    Blockade* blockadeOrder = new Blockade(p1, t1);
    Airlift* airliftOrder = new Airlift(p1, t1, t1, 2);
    Negotiate* negotiateOrder = new Negotiate(p1, p2);

    // 2. Create an OrdersList and add the orders
    OrdersList player1Orders;
    player1Orders.addOrder(deployOrder);
    player1Orders.addOrder(advanceOrder);
    player1Orders.addOrder(bombOrder);
    player1Orders.addOrder(blockadeOrder);
    player1Orders.addOrder(airliftOrder);
    player1Orders.addOrder(negotiateOrder);
    std::cout << "Initial list of orders for Player 1:\n" << player1Orders << std::endl;

    // 3. Test the move() method
    std::cout << "========= Testing move() =========\n";
    player1Orders.move(5, 0); // Move Negotiate to the top
    std::cout << "List after moving order from index 5 to 0:\n" << player1Orders << std::endl;

    // 4. Test the remove() method
    std::cout << "========= Testing remove() =========\n";
    player1Orders.remove(3); // Remove the order now at index 3
    std::cout << "List after removing order at index 3:\n" << player1Orders << std::endl;

    // 5. Test the copy constructor
    std::cout << "========= Testing Copy Constructor =========\n";
    OrdersList player2Orders(player1Orders);
    std::cout << "Player 2's list (copied from Player 1):\n" << player2Orders << std::endl;

    // 6. Test the execute() method
    std::cout << "========= Testing execute() =========\n";
    // Demonstrates Part 4: validation, battle logic, and ownership changes
    player1Orders.executeAll();
    std::cout << "Player 1's list after execution:\n" << player1Orders << std::endl;

    std::cout << "Player 2's list is unchanged, proving it's a deep copy:\n" << player2Orders;

    // Cleanup for Driver
    delete p1;
    delete p2;
    delete t1;
    delete t2;
}

int main() {
    // Seed for battle randomization in Part 4
    srand(static_cast<unsigned int>(time(0)));
    
    testOrders();
    // The destructors will be called here to clean memory
    return 0;
}