#include "GameEngine.h"
#include <cctype>
#include <sstream>

// ---------- Helpers ----------
static std::string toLowerTrimToken(const std::string& s) {
    // Take first token, lower-case it (commands may have args like "loadmap file.map")
    std::istringstream iss(s);
    std::string token;
    iss >> token;

    for (char& ch : token) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return token;
}

// ---------- GameEngine private ----------
void GameEngine::buildCommandLookup() {
    commandLookup.clear();
    commandLookup["loadmap"] = Command::LoadMap;
    commandLookup["validatemap"] = Command::ValidateMap;
    commandLookup["addplayer"] = Command::AddPlayer;
    commandLookup["assigncountries"] = Command::AssignCountries;
    commandLookup["issueorder"] = Command::IssueOrder;
    commandLookup["endissueorders"] = Command::EndIssueOrders;
    commandLookup["execorder"] = Command::ExecOrder;
    commandLookup["endexecorders"] = Command::EndExecOrders;
    commandLookup["win"] = Command::WinCmd;   // helper command to demonstrate win state
    commandLookup["play"] = Command::Play;
    commandLookup["quit"] = Command::Quit;
}

void GameEngine::buildTransitions() {
    transitions.clear();

    // --- Startup phase ---
    transitions[{GameState::Start, Command::LoadMap}] = GameState::MapLoaded;

    // Allow re-loading map while still setting up
    transitions[{GameState::MapLoaded, Command::LoadMap}] = GameState::MapLoaded;
    transitions[{GameState::MapLoaded, Command::ValidateMap}] = GameState::MapValidated;

    // In many versions, you can loadmap again even after validate; adjust if your diagram differs
    transitions[{GameState::MapValidated, Command::LoadMap}] = GameState::MapLoaded;
    transitions[{GameState::MapValidated, Command::AddPlayer}] = GameState::PlayersAdded;

    // Add multiple players
    transitions[{GameState::PlayersAdded, Command::AddPlayer}] = GameState::PlayersAdded;

    // Often allowed to loadmap again before assigning countries; adjust if needed
    transitions[{GameState::PlayersAdded, Command::LoadMap}] = GameState::MapLoaded;

    transitions[{GameState::PlayersAdded, Command::AssignCountries}] = GameState::AssignReinforcement;

    // --- Play phase loop ---
    // AssignReinforcement -> IssueOrders (first issueorder moves you into issue state)
    transitions[{GameState::AssignReinforcement, Command::IssueOrder}] = GameState::IssueOrders;

    // IssueOrders: can issue many orders
    transitions[{GameState::IssueOrders, Command::IssueOrder}] = GameState::IssueOrders;
    transitions[{GameState::IssueOrders, Command::EndIssueOrders}] = GameState::ExecuteOrders;

    // ExecuteOrders: execute many orders
    transitions[{GameState::ExecuteOrders, Command::ExecOrder}] = GameState::ExecuteOrders;
    transitions[{GameState::ExecuteOrders, Command::EndExecOrders}] = GameState::AssignReinforcement;

    // Demo win transition (since we aren't implementing full gameplay)
    transitions[{GameState::ExecuteOrders, Command::WinCmd}] = GameState::Win;

    // Win state
    transitions[{GameState::Win, Command::Play}] = GameState::Start;
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

Command GameEngine::parseCommand(const std::string& input) const {
    std::string token = toLowerTrimToken(input);
    auto it = commandLookup.find(token);
    if (it == commandLookup.end()) return Command::Invalid;
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

std::string GameEngine::commandToString(Command c) {
    switch (c) {
        case Command::LoadMap: return "loadmap";
        case Command::ValidateMap: return "validatemap";
        case Command::AddPlayer: return "addplayer";
        case Command::AssignCountries: return "assigncountries";
        case Command::IssueOrder: return "issueorder";
        case Command::EndIssueOrders: return "endissueorders";
        case Command::ExecOrder: return "execorder";
        case Command::EndExecOrders: return "endexecorders";
        case Command::WinCmd: return "win";
        case Command::Play: return "play";
        case Command::Quit: return "quit";
        case Command::Invalid: return "invalid";
        default: return "unknown";
    }
}

// ---------- GameEngine public ----------
GameEngine::GameEngine() : state(nullptr) {
    state = new GameState(GameState::Start);
    buildCommandLookup();
    buildTransitions();
}

GameEngine::GameEngine(const GameEngine& other) : state(nullptr) {
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
    Command cmd = parseCommand(input);

    if (cmd == Command::Quit) {
        *state = GameState::End;
        std::cout << "Quitting. State -> End\n";
        return false;
    }

    if (cmd == Command::Invalid) {
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

    *state = it->second;
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
