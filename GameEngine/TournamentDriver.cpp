#include "GameEngine.h"
#include "../CommandProcessor/CommandProcessing.h"
#include "../LoggingObserver/LoggingObserver.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Demonstrates tournament command validation through the CommandProcessor
static void demoValidation() {
    cout << "\n========================================" << endl;
    cout << "  PART 2 — COMMAND VALIDATION DEMO" << endl;
    cout << "========================================" << endl;

    CommandProcessor cp;

    struct TestCase {
        string         description;
        vector<string> args;
        bool           expectValid;
    };

    const vector<TestCase> tests = {
        { "VALID  — 2 maps, 2 strategies, G=3, D=20",
          {"-M","austria.map","canada.map","-P","Aggressive","Benevolent","-G","3","-D","20"}, true },
        { "VALID  — 1 map, 4 strategies, G=5, D=10 (boundary values)",
          {"-M","austria.map","-P","Aggressive","Benevolent","Neutral","Cheater","-G","5","-D","10"}, true },
        { "VALID  — 5 maps, 2 strategies, G=1, D=50 (upper boundary)",
          {"-M","m1","m2","m3","m4","m5","-P","Aggressive","Neutral","-G","1","-D","50"}, true },
        { "INVALID — missing -G and -D flags",
          {"-M","austria.map","-P","Aggressive","Benevolent"}, false },
        { "INVALID — -P has only 1 strategy (minimum is 2)",
          {"-M","austria.map","-P","Aggressive","-G","2","-D","20"}, false },
        { "INVALID — -P has 5 strategies (maximum is 4)",
          {"-M","austria.map","-P","Aggressive","Benevolent","Neutral","Cheater","Aggressive","-G","2","-D","20"}, false },
        { "INVALID — Human is not a valid computer strategy",
          {"-M","austria.map","-P","Aggressive","Human","-G","2","-D","20"}, false },
        { "INVALID — duplicate strategy",
          {"-M","austria.map","-P","Aggressive","Aggressive","-G","2","-D","20"}, false },
        { "INVALID — -G out of range (6 > max 5)",
          {"-M","austria.map","-P","Aggressive","Benevolent","-G","6","-D","20"}, false },
        { "INVALID — -D too small (5 < min 10)",
          {"-M","austria.map","-P","Aggressive","Benevolent","-G","2","-D","5"}, false },
        { "INVALID — -D too large (51 > max 50)",
          {"-M","austria.map","-P","Aggressive","Benevolent","-G","2","-D","51"}, false },
        { "INVALID — 6 maps (maximum is 5)",
          {"-M","m1","m2","m3","m4","m5","m6","-P","Aggressive","Benevolent","-G","2","-D","20"}, false },
    };

    int passed = 0;
    for (const TestCase& tc : tests) {
        Command* cmd = new Command(CommandName::Tournament, tc.args);
        bool result  = cp.validate(cmd, GameState::Start);

        bool ok = (result == tc.expectValid);
        if (ok) ++passed;

        cout << (ok ? "[PASS]" : "[FAIL]") << " " << tc.description << "\n";
        if (!result)
            cout << "       -> " << cmd->getEffect() << "\n";
        cout << "\n";

        delete cmd;
    }

    cout << passed << "/" << static_cast<int>(tests.size()) << " validation tests passed.\n";
}

// Runs the GameEngine with a tournament command to demonstrate end-to-end execution
static void demoExecution(int argc, char* argv[]) {
    cout << "\n========================================" << endl;
    cout << "  PART 2 — TOURNAMENT EXECUTION DEMO" << endl;
    cout << "========================================" << endl;

    CommandProcessor* proc = nullptr;

    if (argc >= 3 && string(argv[1]) == "-file") {
        FileCommandProcessorAdapter* fp = new FileCommandProcessorAdapter(argv[2]);
        if (!fp->isOpen()) {
            delete fp;
            cout << "ERROR: Cannot open command file '" << argv[2] << "'" << endl;
            return;
        }
        proc = fp;
    } else {
        FileCommandProcessorAdapter* fp = new FileCommandProcessorAdapter("tournament_commands.txt");
        if (!fp->isOpen()) {
            delete fp;
            cout << "ERROR: tournament_commands.txt not found. Run from the GameEngine directory." << endl;
            return;
        }
        proc = fp;
    }

    GameEngine* engine = new GameEngine(proc);
    engine->run();
    delete engine;
}

int main(int argc, char* argv[]) {
    Subject::attachGlobal(new LogObserver());

    cout << "========================================" << endl;
    cout << "   COMP 345 A3 — PART 2 TOURNAMENT DRIVER" << endl;
    cout << "========================================\n" << endl;

    demoValidation();
    demoExecution(argc, argv);

    return 0;
}
