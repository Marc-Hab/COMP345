#include "GameEngine.h"
#include "../CommandProcessor/CommandProcessing.h"
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

void GameEngine::buildTransitions() {
    transitions.clear();

    // --- Startup phase ---
    transitions[{GameState::Start, CommandName::LoadMap}] = GameState::MapLoaded;

    // Allow re-loading map while still setting up
    transitions[{GameState::MapLoaded, CommandName::LoadMap}] = GameState::MapLoaded;
    transitions[{GameState::MapLoaded, CommandName::ValidateMap}] = GameState::MapValidated;

    // In many versions, you can loadmap again even after validate; adjust if your diagram differs
    transitions[{GameState::MapValidated, CommandName::LoadMap}] = GameState::MapLoaded;
    transitions[{GameState::MapValidated, CommandName::AddPlayer}] = GameState::PlayersAdded;

    // Add multiple players
    transitions[{GameState::PlayersAdded, CommandName::AddPlayer}] = GameState::PlayersAdded;

    // Often allowed to loadmap again before assigning countries; adjust if needed
    transitions[{GameState::PlayersAdded, CommandName::LoadMap}] = GameState::MapLoaded;

    transitions[{GameState::PlayersAdded, CommandName::GameStart}] = GameState::AssignReinforcement;

    // --- Play phase loop ---
    // AssignReinforcement -> IssueOrders (first issueorder moves you into issue state)
    transitions[{GameState::AssignReinforcement, CommandName::IssueOrder}] = GameState::IssueOrders;

    // IssueOrders: can issue many orders
    transitions[{GameState::IssueOrders, CommandName::IssueOrder}] = GameState::IssueOrders;
    transitions[{GameState::IssueOrders, CommandName::EndIssueOrders}] = GameState::ExecuteOrders;

    // ExecuteOrders: execute many orders
    transitions[{GameState::ExecuteOrders, CommandName::ExecOrder}] = GameState::ExecuteOrders;
    transitions[{GameState::ExecuteOrders, CommandName::EndExecOrders}] = GameState::AssignReinforcement;

    // Demo win transition (since we aren't implementing full gameplay)
    transitions[{GameState::ExecuteOrders, CommandName::WinCmd}] = GameState::Win;

    // Win state
    transitions[{GameState::Win, CommandName::Replay}] = GameState::Start;
}

void GameEngine::clear() {
    delete state;
    delete cmdProcessor;
    state = nullptr;
    cmdProcessor = nullptr;
}

void GameEngine::copyFrom(const GameEngine& other) {
    state = new GameState(*other.state);
    cmdProcessor = new CommandProcessor(*other.cmdProcessor);
    transitions = other.transitions;
}

CommandName parseCommandName(const std::string& input) {
    std::string token = toLowerTrimToken(input);
    
    if (token == "loadmap") return CommandName::LoadMap;
    if (token == "validatemap") return CommandName::ValidateMap;
    if (token == "addplayer") return CommandName::AddPlayer;
    if (token == "gamestart") return CommandName::GameStart;
    if (token == "issueorder") return CommandName::IssueOrder;
    if (token == "endissueorders") return CommandName::EndIssueOrders;
    if (token == "execorder") return CommandName::ExecOrder;
    if (token == "endexecorders") return CommandName::EndExecOrders;
    if (token == "win") return CommandName::WinCmd;
    if (token == "replay") return CommandName::Replay;
    if (token == "quit") return CommandName::Quit;

    return CommandName::Invalid;
}

std::string stateToString(GameState s) {
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

std::string commandNameToString(CommandName c) {
    switch (c) {
        case CommandName::LoadMap: return "loadmap";
        case CommandName::ValidateMap: return "validatemap";
        case CommandName::AddPlayer: return "addplayer";
        case CommandName::GameStart: return "gamestart";
        case CommandName::IssueOrder: return "issueorder";
        case CommandName::EndIssueOrders: return "endissueorders";
        case CommandName::ExecOrder: return "execorder";
        case CommandName::EndExecOrders: return "endexecorders";
        case CommandName::WinCmd: return "win";
        case CommandName::Replay: return "replay";
        case CommandName::Quit: return "quit";
        case CommandName::Invalid: return "invalid";
        default: return "unknown";
    }
}

// ---------- GameEngine public ----------
GameEngine::GameEngine() {
    state = new GameState(GameState::Start);
    cmdProcessor = new CommandProcessor(); // Reads from the console by default
    buildTransitions();
}

GameEngine::GameEngine(CommandProcessor* proc) : cmdProcessor(proc) {
    state = new GameState(GameState::Start);
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

/**
 * Main game loop - processes commands until quit
 */
void GameEngine::run() {
    if (!cmdProcessor) {
        cout << "ERROR: No CommandProcessor set!" << endl;
        return;
    }
    
    cout << "========================================" << endl;
    cout << "         WARZONE GAME ENGINE" << endl;
    cout << "========================================" << endl;
    cout << "Starting state: " << stateToString(getState()) << endl << endl;
    
    while (true) {
        // Process next command
        if (!processNextCommand()) {
            break;  // Quit command received
        }
    }
    
    cout << "\nGame ended." << endl;
}

/**
 * Process next command - returns false if quit
 */
bool GameEngine::processNextCommand() {
    // Get next command from processor
    Command* cmd = cmdProcessor->getCommand();
    
    if (!cmd) {
        cout << "ERROR: Failed to get command" << endl;
        return true;  // Continue
    }
    
    // Check for quit
    if (cmd->getCommandName() == CommandName::Quit) {
        cout << "\nQuitting game..." << endl;
        cmd->saveEffect("Game quit by user");
        return false;  // Stop game loop
    }
    
    // Validate command
    if (!cmdProcessor->validate(cmd, *state)) {
        // Validation failed - effect contains error message
        cout << cmd->getEffect() << endl;
        return true;  // Continue, try next command
    }
    
    // Command is valid - apply it (transition state)
    applyCommand(cmd);
    
    return true;  // Continue
}

/**
 * Apply validated command - transitions to new state
 */
void GameEngine::applyCommand(Command* cmd) {
    CommandName cmdName = cmd->getCommandName();
    
    // Look up transition
    auto key = make_pair(*state, cmdName);
    auto it = transitions.find(key);
    
    if (it == transitions.end()) {
        // This shouldn't happen if validate() worked correctly
        cout << "ERROR: No transition found (validate should have caught this!)" << endl;
        cmd->saveEffect("ERROR: No transition found");
        return;
    }
    
    // Transition to new state
    GameState oldState = *state;
    *state = it->second;
    
    // Print transition
    cout << "Transitioned from " << stateToString(oldState) 
         << " -> " << stateToString(*state) 
         << " (command: " << commandNameToString(cmdName);
    
    // Print arguments if any
    vector<string> args = cmd->getArguments();
    if (!args.empty()) {
        cout << " " << args[0];
        for (size_t i = 1; i < args.size(); i++) {
            cout << ", " << args[i];
        }
    }
    cout << ")" << endl;
    
    // Save success effect
    cmd->saveEffect("Transition successful: " + stateToString(oldState) + 
                    " -> " + stateToString(*state));
}

GameState GameEngine::getState() const {
    return *state;
}

std::ostream& operator<<(std::ostream& os, const GameEngine& ge) {
    os << "Current state: " << stateToString(*ge.state);
    return os;
}
