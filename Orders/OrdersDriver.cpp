// OrdersDriver.cpp
// Main file to test the OrdersList functionality.

#include "Orders.h"
#include <iostream>

void testOrders() {
    // 1. Create one of each kind of orders
    std::cout << "========= Creating Orders =========\n";
    Deploy* deployOrder = new Deploy();
    Advance* advanceOrder = new Advance();
    Bomb* bombOrder = new Bomb();
    Blockade* blockadeOrder = new Blockade();
    Airlift* airliftOrder = new Airlift();
    Negotiate* negotiateOrder = new Negotiate();

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
    player1Orders.executeAll();
    std::cout << "Player 1's list after execution:\n" << player1Orders << std::endl;

    std::cout << "Player 2's list is unchanged, proving it's a deep copy:\n" << player2Orders;
}

int main() {
    testOrders();
    // The destructors will be called here to clean memory
    return 0;
}