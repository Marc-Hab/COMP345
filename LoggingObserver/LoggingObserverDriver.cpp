// Proof of each requirement:
//  (1) Command, CommandProcessor, Order, OrdersList, GameEngine all inherit Subject + ILoggable
//  (2) saveCommand(), execute(), saveEffect(), addOrder(), transition() call notify()
//  (3) Commands entered on console (or via file) are logged to gamelog.txt
//  (4) Adding an order to an OrdersList logs the order description
//  (5) Executing an order logs the order effect
//  (6) GameEngine state transitions are logged

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

// ---------------------------------------------------------------------------
// Helper: clear gamelog.txt before the demo so results are fresh each run
// ---------------------------------------------------------------------------
static void clearLog() {
    ofstream f("gamelog.txt", ios::trunc);
    f << "=== Game Log ===" << endl;
}

// ---------------------------------------------------------------------------
// Helper: print gamelog.txt contents to stdout after each section
// ---------------------------------------------------------------------------
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

    // =========================================================
    // Requirement (1): Confirm inheritance at compile time
    //   These static_assert checks verify all classes satisfy
    //   the Subject and ILoggable contracts.
    // =========================================================
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

    // =========================================================
    // Requirement (3): Command::saveEffect() → notify → log
    // =========================================================
    cout << "\n=== (3) Command::saveEffect() ===" << endl;
    {
        Command cmd(CommandName::LoadMap, {"canada.map"});
        cmd.attach(log);
        cmd.saveEffect("Map 'canada.map' loaded successfully.");
        // Expected log entry: "Command effect saved: Map 'canada.map' loaded successfully."
    }
    printLog();

    // =========================================================
    // Requirement (3): CommandProcessor::saveCommand() → notify → log
    //   getCommand() internally calls saveCommand() when a new command
    //   is read, which triggers the notification.
    // =========================================================
    cout << "=== (3) CommandProcessor::saveCommand() via FileCommandProcessorAdapter ===" << endl;
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

        // Each getCommand() reads one line → calls saveCommand() → notifies
        fcp.getCommand();   // loadmap
        fcp.getCommand();   // validatemap
        fcp.getCommand();   // addplayer
        // Expected log entries (3 lines): "Command saved to CommandProcessor: loadmap/validatemap/addplayer"
    }
    printLog();

    // =========================================================
    // Requirement (4): OrdersList::addOrder() → notify → log
    //   When an order is added to a player's list, its description
    //   is written to gamelog.txt.
    // =========================================================
    cout << "=== (4) OrdersList::addOrder() ===" << endl;
    {
        // Build minimal map / player setup so Order::validate() can work
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

        Order* deploy   = new Deploy(&alice, &t1, 5);
        Order* advance  = new Advance(&alice, &t1, &t2, 3);
        Order* negotiate = new Negotiate(&alice, &bob);

        olist.addOrder(deploy);    // log: "Order added to list: Deploy Order [...]"
        olist.addOrder(advance);   // log: "Order added to list: Advance Order [...]"
        olist.addOrder(negotiate); // log: "Order added to list: Negotiate Order [...]"
        // Expected: 3 log entries describing each order
    }
    printLog();

    // =========================================================
    // Requirement (5): Order::execute() → notify → log
    //   After execution each order logs its effect.
    // =========================================================
    cout << "=== (5) Order::execute() ===" << endl;
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
        // Expected: 3 log entries with the effect of each order

        delete dep;
        delete adv;
        delete neg;
    }
    printLog();

    // =========================================================
    // Requirement (6): GameEngine::transition() → notify → log
    //   Every meaningful state change is logged.
    // =========================================================
    cout << "=== (6) GameEngine::transition() ===" << endl;
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
        // Expected: 8 log entries with each state name
    }
    printLog();

    cout << "=== Driver complete — see gamelog.txt for full log ===" << endl;

    delete log;
    return 0;
}
