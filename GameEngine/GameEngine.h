#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include "../LoggingObserver/LoggingObserver.h"

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

enum class GameCommand {
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

// GameEngine: manages game state transitions. Inherits Subject and ILoggable (Part 5)
// so that transition() can notify observers whenever the state changes.
class GameEngine : public Subject, public ILoggable {
private:
    GameState* state; // pointer to satisfy rubric requirement

    std::map<std::pair<GameState, GameCommand>, GameState> transitions;
    std::map<std::string, GameCommand> commandLookup;

    void buildCommandLookup();
    void buildTransitions();

    void clear();
    void copyFrom(const GameEngine& other);

    GameCommand parseCommand(const std::string& input) const;

    static std::string commandToString(GameCommand c);

    // Updates the game state and notifies observers (Part 5).
    void transition(GameState newState);

public:
    GameEngine();
    GameEngine(const GameEngine& other);
    GameEngine& operator=(const GameEngine& other);
    ~GameEngine();

    // Applies one command: validates using the transition map; changes state if valid.
    // Returns false when the engine should stop (End).
    bool applyCommand(const std::string& input);

    GameState getState() const;

    static std::string stateToString(GameState s);

    std::string stringToLog() const override;

    friend std::ostream& operator<<(std::ostream& os, const GameEngine& ge);
};

#endif
