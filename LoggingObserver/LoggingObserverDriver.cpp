#include "LoggingObserver.h"
#include "../CommandProcessor/CommandProcessing.h"
#include "../Orders/Orders.h"
#include "../GameEngine/GameEngine.h"
#include "../Players/Player.h"
#include "../Maps/Map.h"
#include <fstream>
#include <iostream>
#include <cassert>

using namespace std;

// Helper: clear gamelog.txt so each run starts fresh
static void clearLog() {
    ofstream f("gamelog.txt", ios::trunc);
    f << "=== Game Log ===" << endl;
}

// Helper: dump gamelog.txt to stdout after each section
static void printLog() {
    cout << "\n--- gamelog.txt ---" << endl;
    ifstream f("gamelog.txt");
    string line;
    while (getline(f, line)) cout << "  " << line << endl;
    cout << "-------------------\n" << endl;
}

int main() {
    clearLog();
    LogObserver* log = new LogObserver();

    cout << "\n===== DEMO 1: INHERITANCE CHECK =====" << endl;
    static_assert(is_base_of<Subject,    Command>::value,          "Command must extend Subject");
    static_assert(is_base_of<ILoggable,  Command>::value,          "Command must extend ILoggable");
    static_assert(is_base_of<Subject,    CommandProcessor>::value,  "CommandProcessor must extend Subject");
    static_assert(is_base_of<ILoggable,  CommandProcessor>::value,  "CommandProcessor must extend ILoggable");
    static_assert(is_base_of<Subject,    Order>::value,             "Order must extend Subject");
    static_assert(is_base_of<ILoggable,  Order>::value,             "Order must extend ILoggable");
    static_assert(is_base_of<Subject,    OrdersList>::value,        "OrdersList must extend Subject");
    static_assert(is_base_of<ILoggable,  OrdersList>::value,        "OrdersList must extend ILoggable");
    static_assert(is_base_of<Subject,    GameEngine>::value,        "GameEngine must extend Subject");
    static_assert(is_base_of<ILoggable,  GameEngine>::value,        "GameEngine must extend ILoggable");
    cout << "[OK] All classes confirmed as subclasses of Subject and ILoggable." << endl;

    cout << "\n===== DEMO 2: Command::saveEffect() =====" << endl;
    {
        Command cmd(CommandName::LoadMap, {"canada.map"});
        cmd.attach(log);
        cmd.saveEffect("Map 'canada.map' loaded successfully.");
    }
    printLog();

    cout << "\n===== DEMO 3: CommandProcessor::saveCommand() =====" << endl;
    {
        // Write a small commands file to read from
        {
            ofstream cf("driver_commands.txt");
            cf << "loadmap canada.map" << endl;
            cf << "validatemap"        << endl;
            cf << "addplayer Alice"    << endl;
        }

        FileCommandProcessorAdapter fcp("driver_commands.txt");
        fcp.attach(log);

        fcp.getCommand();   // loadmap
        fcp.getCommand();   // validatemap
        fcp.getCommand();   // addplayer
    }
    printLog();

    cout << "\n===== DEMO 4: OrdersList::addOrder() =====" << endl;
    {
        Territory t1("Ontario");   t1.setArmyCount(10);
        Territory t2("Quebec");    t2.setArmyCount(5);
        Territory t3("Manitoba");  t3.setArmyCount(3);
        t1.addAdjacentTerritory(&t2);
        t2.addAdjacentTerritory(&t1);

        Player alice("Alice");
        alice.getTerritoriesOwned()->push_back(&t1);
        alice.getTerritoriesOwned()->push_back(&t3);
        t1.setOwner(&alice);
        t3.setOwner(&alice);

        Player bob("Bob");
        bob.getTerritoriesOwned()->push_back(&t2);
        t2.setOwner(&bob);

        OrdersList& olist = *alice.getOrders();
        olist.attach(log);

        Order* deploy    = new Deploy(&alice, &t1, 5);
        Order* advance   = new Advance(&alice, &t1, &t2, 3);
        Order* negotiate = new Negotiate(&alice, &bob);

        olist.addOrder(deploy);    // log: "Order added to list: Deploy [...]"
        olist.addOrder(advance);   // log: "Order added to list: Advance [...]"
        olist.addOrder(negotiate); // log: "Order added to list: Negotiate [...]"
    }
    printLog();

    cout << "\n===== DEMO 5: NEGOTIATE ORDER prevents attacks =====" << endl;
    {
        Territory src("Alberta");  src.setArmyCount(10);
        Territory dst("BC");       dst.setArmyCount(2);
        src.addAdjacentTerritory(&dst);
        dst.addAdjacentTerritory(&src);

        Player p1("Player1");
        p1.getTerritoriesOwned()->push_back(&src);
        src.setOwner(&p1);

        Player p2("Player2");
        p2.getTerritoriesOwned()->push_back(&dst);
        dst.setOwner(&p2);

        Deploy*   dep  = new Deploy(&p1, &src, 4);
        Advance*  adv  = new Advance(&p1, &src, &dst, 5);
        Negotiate* neg = new Negotiate(&p1, &p2);

        dep->attach(log);
        adv->attach(log);
        neg->attach(log);

        dep->execute();   // log: "Order executed, effect: ..."
        adv->execute();   // log: "Order executed, effect: ..."
        neg->execute();   // log: "Order executed, effect: ..."

        delete dep;
        delete adv;
        delete neg;
    }
    printLog();

    cout << "\n===== DEMO 6: GameEngine::transition() =====" << endl;
    {
        GameEngine ge;
        ge.attach(log);

        ge.transition(GameState::MapLoaded);          // log: "GameEngine state changed to: MapLoaded"
        ge.transition(GameState::MapValidated);        // log: "GameEngine state changed to: MapValidated"
        ge.transition(GameState::PlayersAdded);        // log: "GameEngine state changed to: PlayersAdded"
        ge.transition(GameState::AssignReinforcement); // log: "GameEngine state changed to: AssignReinforcement"
        ge.transition(GameState::IssueOrders);         // log: "GameEngine state changed to: IssueOrders"
        ge.transition(GameState::ExecuteOrders);       // log: "GameEngine state changed to: ExecuteOrders"
        ge.transition(GameState::Win);                 // log: "GameEngine state changed to: Win"
        ge.transition(GameState::End);                 // log: "GameEngine state changed to: End"
    }
    printLog();

    cout << "=== Driver complete — see gamelog.txt for full log ===" << endl;

    delete log;
    return 0;
}
