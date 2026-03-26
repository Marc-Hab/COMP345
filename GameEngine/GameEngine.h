#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "../LoggingObserver/LoggingObserver.h"

// forward declarations
class CommandProcessor;
class Command;
class Map;
class Player;
class Deck;

enum class GameState {
    Start,
    MapLoaded,
    MapValidated,
    PlayersAdded,
    AssignReinforcement,
    IssueOrders,
    ExecuteOrders,
    Win,
    End
};

// Free function to convert state to string
std::string stateToString(GameState s);

enum class CommandName {
    Tournament,
    LoadMap,
    ValidateMap,
    AddPlayer,
    GameStart,
    IssueOrder,
    EndIssueOrders,
    ExecOrder,
    EndExecOrders,
    WinCmd,
    Replay,
    Quit,
    Invalid
};

// Free function to convert CommandName to string
std::string commandNameToString(CommandName c);

// Free function to convert string to CommandName
CommandName parseCommandName(const std::string& input);

class GameEngine : public Subject, public ILoggable {
private:
    GameState* state; // pointer to satisfy rubric requirement

    CommandProcessor* cmdProcessor;

    std::map<std::pair<GameState, CommandName>, GameState> transitions;

    // Game data
    Map* gameMap;
    std::vector<Player*>* players;
    Deck* deck;

    void buildTransitions();
    void clear();
    void copyFrom(const GameEngine& other);

    // Startup action handlers
    bool executeCommand(Command* cmd);
    bool playTournament(Command* cmd);
    bool loadMap(Command* cmd);
    bool validateMap(Command* cmd);
    bool addPlayer(Command* cmd);
    bool gameStart(Command* cmd);

    // Resets game data to start a new game
    void newGame();

    // Print available map files
    void listAvailableMaps() const;

public:
    GameEngine();
    GameEngine(CommandProcessor* proc);
    GameEngine(const GameEngine& other);
    GameEngine& operator=(const GameEngine& other);
    ~GameEngine();

    // Startup phase: loadmap -> validatemap -> addplayer(s) -> gamestart
    void startupPhase();

    // end phase: win -> start || win -> end
    void endPhase();

    // Main game loop (called after startupPhase)
    void mainGameLoop();

    // Individual game loop phases (also callable directly for testing/demo)
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();

    // Game loop (startup + main loop)
    void run();

    // Process single command
    bool processNextCommand();

    // Apply validated command to transition state
    void applyCommand(Command* cmd);

    // Transitions to newState and notifies observers
    void transition(GameState newState);

    // returns the current state for the game log
    std::string stringToLog() const override;

    GameState getState() const;
    std::vector<Player*>* getPlayers() const;
    Map* getMap() const;

    friend std::ostream& operator<<(std::ostream& os, const GameEngine& ge);
};

#endif
