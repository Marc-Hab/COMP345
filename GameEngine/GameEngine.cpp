#include "GameEngine.h"
#include <cctype>
#include <sstream>

// Helpers
static std::string toLowerTrimToken(const std::string& s) {
    // Take first token, lower-case it (commands may have args like "loadmap file.map")
    std::istringstream iss(s);
    std::string token;
    iss >> token;

    for (char& ch : token) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return token;
}

// GameEngine private
void GameEngine::buildCommandLookup() {
    commandLookup.clear();
    commandLookup["loadmap"] = GameCommand::LoadMap;
    commandLookup["validatemap"] = GameCommand::ValidateMap;
    commandLookup["addplayer"] = GameCommand::AddPlayer;
    commandLookup["assigncountries"] = GameCommand::AssignCountries;
    commandLookup["gamestart"]       = GameCommand::AssignCountries; // alias per assignment spec
    commandLookup["issueorder"] = GameCommand::IssueOrder;
    commandLookup["endissueorders"] = GameCommand::EndIssueOrders;
    commandLookup["execorder"] = GameCommand::ExecOrder;
    commandLookup["endexecorders"] = GameCommand::EndExecOrders;
    commandLookup["win"] = GameCommand::WinCmd;   // helper command to demonstrate win state
    commandLookup["play"] = GameCommand::Play;
    commandLookup["quit"] = GameCommand::Quit;
}

void GameEngine::buildTransitions() {
    transitions.clear();

    // --- Startup phase ---
    transitions[{GameState::Start, GameCommand::LoadMap}] = GameState::MapLoaded;

    // Allow re-loading map while still setting up
    transitions[{GameState::MapLoaded, GameCommand::LoadMap}] = GameState::MapLoaded;
    transitions[{GameState::MapLoaded, GameCommand::ValidateMap}] = GameState::MapValidated;

    // In many versions, you can loadmap again even after validate; adjust if your diagram differs
    transitions[{GameState::MapValidated, GameCommand::LoadMap}] = GameState::MapLoaded;
    transitions[{GameState::MapValidated, GameCommand::AddPlayer}] = GameState::PlayersAdded;

    // Add multiple players
    transitions[{GameState::PlayersAdded, GameCommand::AddPlayer}] = GameState::PlayersAdded;

    // Often allowed to loadmap again before assigning countries; adjust if needed
    transitions[{GameState::PlayersAdded, GameCommand::LoadMap}] = GameState::MapLoaded;

    transitions[{GameState::PlayersAdded, GameCommand::AssignCountries}] = GameState::AssignReinforcement;

    // --- Play phase loop ---
    // AssignReinforcement -> IssueOrders (first issueorder moves you into issue state)
    transitions[{GameState::AssignReinforcement, GameCommand::IssueOrder}] = GameState::IssueOrders;

    // IssueOrders: can issue many orders
    transitions[{GameState::IssueOrders, GameCommand::IssueOrder}] = GameState::IssueOrders;
    transitions[{GameState::IssueOrders, GameCommand::EndIssueOrders}] = GameState::ExecuteOrders;

    // ExecuteOrders: execute many orders
    transitions[{GameState::ExecuteOrders, GameCommand::ExecOrder}] = GameState::ExecuteOrders;
    transitions[{GameState::ExecuteOrders, GameCommand::EndExecOrders}] = GameState::AssignReinforcement;

    // Demo win transition (since we aren't implementing full gameplay)
    transitions[{GameState::ExecuteOrders, GameCommand::WinCmd}] = GameState::Win;

    // Win state
    transitions[{GameState::Win, GameCommand::Play}] = GameState::Start;
}

void GameEngine::clear() {
    delete state;
    state = nullptr;
}

void GameEngine::copyFrom(const GameEngine& other) {
    state = new GameState(*other.state);
    transitions = other.transitions;
    commandLookup = other.commandLookup;
}

GameCommand GameEngine::parseCommand(const std::string& input) const {
    std::string token = toLowerTrimToken(input);
    auto it = commandLookup.find(token);
    if (it == commandLookup.end()) return GameCommand::Invalid;
    return it->second;
}

std::string GameEngine::stateToString(GameState s) {
    switch (s) {
        case GameState::Start: return "Start";
        case GameState::MapLoaded: return "MapLoaded";
        case GameState::MapValidated: return "MapValidated";
        case GameState::PlayersAdded: return "PlayersAdded";
        case GameState::AssignReinforcement: return "AssignReinforcement";
        case GameState::IssueOrders: return "IssueOrders";
        case GameState::ExecuteOrders: return "ExecuteOrders";
        case GameState::Win: return "Win";
        case GameState::End: return "End";
        default: return "Unknown";
    }
}

std::string GameEngine::commandToString(GameCommand c) {
    switch (c) {
        case GameCommand::LoadMap: return "loadmap";
        case GameCommand::ValidateMap: return "validatemap";
        case GameCommand::AddPlayer: return "addplayer";
        case GameCommand::AssignCountries: return "assigncountries";
        case GameCommand::IssueOrder: return "issueorder";
        case GameCommand::EndIssueOrders: return "endissueorders";
        case GameCommand::ExecOrder: return "execorder";
        case GameCommand::EndExecOrders: return "endexecorders";
        case GameCommand::WinCmd: return "win";
        case GameCommand::Play: return "play";
        case GameCommand::Quit: return "quit";
        case GameCommand::Invalid: return "invalid";
        default: return "unknown";
    }
}

// GameEngine transition + logging

void GameEngine::transition(GameState newState) {
    *state = newState;
    notify(*this);
}

std::string GameEngine::stringToLog() const {
    return "GameEngine new state: " + stateToString(*state);
}

// GameEngine public
GameEngine::GameEngine() : Subject() {
    state = new GameState(GameState::Start);
    buildCommandLookup();
    buildTransitions();
}

GameEngine::GameEngine(const GameEngine& other) : Subject(other), state(nullptr) {
    copyFrom(other);
}

GameEngine& GameEngine::operator=(const GameEngine& other) {
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}

GameEngine::~GameEngine() {
    clear();
}

bool GameEngine::applyCommand(const std::string& input) {
    GameCommand cmd = parseCommand(input);

    if (cmd == GameCommand::Quit) {
        transition(GameState::End);
        std::cout << "Quitting. State -> End\n";
        return false;
    }

    if (cmd == GameCommand::Invalid) {
        std::cout << "ERROR: Unknown command.\n";
        return true;
    }

    auto key = std::make_pair(*state, cmd);
    auto it = transitions.find(key);

    if (it == transitions.end()) {
        std::cout << "ERROR: Command '" << commandToString(cmd)
                  << "' is not allowed in state " << stateToString(*state) << ".\n";
        return true;
    }

    transition(it->second);
    std::cout << "Transitioned using '" << commandToString(cmd)
              << "' -> " << stateToString(*state) << "\n";

    // Stop if reached End (optional; quit already does this)
    if (*state == GameState::End) return false;
    return true;
}

GameState GameEngine::getState() const {
    return *state;
}

std::ostream& operator<<(std::ostream& os, const GameEngine& ge) {
    os << "Current state: " << GameEngine::stateToString(*ge.state);
    return os;
}
