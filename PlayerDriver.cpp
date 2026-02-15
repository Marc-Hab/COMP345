#include "Player.h"
#include <iostream>
using namespace std;

int main() {
    cout << " PLAYER DRIVER TEST " << endl;

    // Create a player
    Player p1("Alice");
    cout << p1 << endl;

    // Add sample territories
    Territory* t1 = new Territory("Canada", 5);
    Territory* t2 = new Territory("USA", 3);
    vector<Territory*> defendList = p1.toDefend();
    vector<Territory*> attackList = p1.toAttack();

    cout << "Number of territories to defend: " << defendList.size() << endl;
    cout << "Number of territories to attack: " << attackList.size() << endl;

    // Issue orders
    p1.issueOrder();

    cout << "=== END OF TEST ===" << endl;
    return 0;
}
