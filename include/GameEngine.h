#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>
#include <map>
#include <string>
#include <utility>

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

enum class Command {
    LoadMap,
    ValidateMap,
    AddPlayer,
    AssignCountries,
    IssueOrder,
    EndIssueOrders,
    ExecOrder,
    EndExecOrders,
    WinCmd,
    Play,
    Quit,
    Invalid
};

class GameEngine {
private:
    GameState* state; // pointer to satisfy rubric requirement

    std::map<std::pair<GameState, Command>, GameState> transitions;
    std::map<std::string, Command> commandLookup;

    void buildCommandLookup();
    void buildTransitions();

    void clear();
    void copyFrom(const GameEngine& other);

    Command parseCommand(const std::string& input) const;

    static std::string stateToString(GameState s);
    static std::string commandToString(Command c);

public:
    GameEngine();
    GameEngine(const GameEngine& other);
    GameEngine& operator=(const GameEngine& other);
    ~GameEngine();

    // Applies one command: validates using the transition map; changes state if valid.
    // Returns false when the engine should stop (End).
    bool applyCommand(const std::string& input);

    GameState getState() const;

    friend std::ostream& operator<<(std::ostream& os, const GameEngine& ge);
};

#endif
