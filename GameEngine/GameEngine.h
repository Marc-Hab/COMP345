#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>
#include <map>
#include <string>
#include <utility>

// forward declarations
class CommandProcessor;
class Command;

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

class GameEngine {
private:
    GameState* state; // pointer to satisfy rubric requirement

    CommandProcessor* cmdProcessor;

    std::map<std::pair<GameState, CommandName>, GameState> transitions;

    void buildTransitions();

    void clear();
    void copyFrom(const GameEngine& other);

public:
    GameEngine();
    GameEngine(CommandProcessor* proc);
    GameEngine(const GameEngine& other);
    GameEngine& operator=(const GameEngine& other);
    ~GameEngine();

    // Main game loop
    void run();
    
    // Process single command
    bool processNextCommand();
    
    // Apply validated command (transitions state)
    void applyCommand(Command* cmd);

    GameState getState() const;

    friend std::ostream& operator<<(std::ostream& os, const GameEngine& ge);
};

#endif
