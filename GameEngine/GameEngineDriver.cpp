#include "GameEngine.h"
#include <iostream>
#include <string>

int main() {
    GameEngine engine;

    std::cout << "=== GameEngine FSM Driver ===\n";
    std::cout << "Type commands (you can include args, e.g., 'loadmap map1.txt')\n";
    std::cout << "Commands: loadmap, validatemap, addplayer, assigncountries,\n";
    std::cout << "          issueorder, endissueorders, execorder, endexecorders,\n";
    std::cout << "          win, play, quit\n\n";

    std::string input;
    while (true) {
        std::cout << engine << "\n> ";
        std::getline(std::cin, input);

        if (!engine.applyCommand(input)) {
            break;
        }
    }

    std::cout << "Driver ended.\n";
    return 0;
}
