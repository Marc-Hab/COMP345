#include "GameEngine.h"
#include "../CommandProcessor/CommandProcessing.h"
#include "../Maps/Map.h"
#include "../Players/Player.h"
#include "../Cards/Cards.h"
#include <cctype>
#include <sstream>
#include <algorithm>
#include <random>
#include <numeric>
#include <sys/types.h>
#include <dirent.h>

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
    delete gameMap;
    if (players) {
        for (Player* p : *players) {
            delete p;
        }
        delete players;
    }
    delete deck;
    state = nullptr;
    cmdProcessor = nullptr;
    gameMap = nullptr;
    players = nullptr;
    deck = nullptr;
}

void GameEngine::copyFrom(const GameEngine& other) {
    state = new GameState(*other.state);
    cmdProcessor = new CommandProcessor(*other.cmdProcessor);
    transitions = other.transitions;
    // Game data not copied because copied engine starts fresh
    gameMap = nullptr;
    players = new vector<Player*>();
    deck = new Deck();
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
    cmdProcessor = new CommandProcessor();
    gameMap = nullptr;
    players = new vector<Player*>();
    deck = new Deck();
    buildTransitions();
}

GameEngine::GameEngine(CommandProcessor* proc) : cmdProcessor(proc) {
    state = new GameState(GameState::Start);
    gameMap = nullptr;
    players = new vector<Player*>();
    deck = new Deck();
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
 * Game loop: calls startup phase then main game loop
 */
void GameEngine::run() {
    if (!cmdProcessor) {
        cout << "ERROR: No CommandProcessor set!" << endl;
        return;
    }

    startupPhase();

    // Continue with play phase if startup completed successfully
    if (*state == GameState::AssignReinforcement) {
        cout << "\n--- Play phase (type quit to exit) ---" << endl;
        while (true) {
            if (!processNextCommand()) {
                break;
            }
        }
    }

    cout << "\nGame ended." << endl;
}

/**
 * Process next command
 * Returns false only if user quits
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
        string error = cmd->getEffect();
        if (!error.empty()){
            cout << cmd->getEffect() << endl;
        }
        return true;  // Continue, try next command
    }
    
    // Command is valid - apply it (transition state)
    applyCommand(cmd);
    
    return true;  // Continue
}

/**
 * Apply validated command - transitions state then executes game action
 */
void GameEngine::applyCommand(Command* cmd) {
    CommandName cmdName = cmd->getCommandName();

    // Look up transition
    auto key = make_pair(*state, cmdName);
    auto it = transitions.find(key);

    if (it == transitions.end()) {
        cout << "ERROR: No transition found (validate should have caught this!)" << endl;
        cmd->saveEffect("ERROR: No transition found");
        return;
    }

    GameState oldState = *state;
    *state = it->second;

    // Execute the game action (may revert state on failure)
    bool actionOk = executeCommand(cmd, oldState);
    if (!actionOk) {
        cmd->saveEffect("ERROR: Action failed, state reverted to " + stateToString(*state));
        return;
    }

    // Print transition
    cout << "[" << stateToString(oldState) << " -> " << stateToString(*state) << "]" << endl;
    cmd->saveEffect("Transition successful: " + stateToString(oldState) +
                    " -> " + stateToString(*state));
}

GameState GameEngine::getState() const {
    return *state;
}

std::vector<Player*>* GameEngine::getPlayers() const {
    return players;
}

Map* GameEngine::getMap() const {
    return gameMap;
}

std::ostream& operator<<(std::ostream& os, const GameEngine& ge) {
    os << "Current state: " << stateToString(*ge.state);
    return os;
}

// ---------- Startup action handlers ----------

/**
 * Lists .map files found in map files directory
 */
void GameEngine::listAvailableMaps() const {
    const string mapDir = "../Maps/map files/";
    cout << "Available maps (in '" << mapDir << "'):" << endl;
    DIR* dir = opendir(mapDir.c_str());
    if (!dir) {
        cout << "  (could not open directory - run from project root)" << endl;
    } else {
        vector<string> files;
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            string name = entry->d_name;
            if (name.size() > 4 && name.substr(name.size() - 4) == ".map") {
                files.push_back(name);
            }
        }
        closedir(dir);
        sort(files.begin(), files.end());
        for (const string& f : files) {
            cout << "  - " << f << endl;
        }
        if (files.empty()) {
            cout << "  (no .map files found)" << endl;
        }
    }
    cout << endl;
}

/**
 * Delegates to the appropriate startup action handler
 * Returns true if command was executed successfully
 */
bool GameEngine::executeCommand(Command* cmd, GameState oldState) {
    switch (cmd->getCommandName()) {
        case CommandName::LoadMap:    return loadMap(cmd, oldState);
        case CommandName::ValidateMap: return validateMap(cmd, oldState);
        case CommandName::AddPlayer:  return addPlayer(cmd, oldState);
        case CommandName::GameStart:  return gameStart(cmd, oldState);
        default: return true; // no extra action for play-phase commands
    }
}

/**
 * Load map from file.
 * Tries the given name first, then looks in the map files directory.
 */
bool GameEngine::loadMap(Command* cmd, GameState oldState) {
    string filename = cmd->getArgument(0);

    Map* newMap = MapLoader::getInstance().loadMap(filename);
    if (!newMap) {
        newMap = MapLoader::getInstance().loadMap("../Maps/map files/" + filename);
    }
    if (!newMap) {
        cout << "ERROR: Could not load map '" << filename << "'. Check the filename and try again." << endl;
        *state = oldState;
        return false;
    }

    delete gameMap;
    gameMap = newMap;
    cout << "Map '" << filename << "' loaded successfully." << endl;
    cout << "  Continents : " << gameMap->getContinents()->size() << endl;
    cout << "  Territories: " << gameMap->getTerritories()->size() << endl;
    return true;
}

/**
 * Validate the currently loaded map.
 */
bool GameEngine::validateMap(Command* cmd, GameState oldState) {
    if (!gameMap) {
        cout << "ERROR: No map loaded." << endl;
        *state = oldState;
        return false;
    }
    if (!gameMap->validate()) {
        cout << "Map validation FAILED. Please load a valid map." << endl;
        *state = GameState::MapLoaded; // let user reload
        return false;
    }
    cout << "Map is valid!" << endl;
    return true;
}

/**
 * Add a player to the game (max 6).
 */
bool GameEngine::addPlayer(Command* cmd, GameState oldState) {
    if (players->size() >= 6) {
        cout << "ERROR: Maximum 6 players allowed." << endl;
        *state = oldState;
        return false;
    }

    // Check if name already exists.
    string playerName = cmd->getArgument(0);
    for (Player* p : *players) {
        if (p->getName() == playerName) {
            cout << "ERROR: A player named '" << playerName << "' already exists." << endl;
            *state = oldState;
            return false;
        }
    }

    Player* newPlayer = new Player(playerName);
    newPlayer->getHand()->setPlayer(newPlayer); // link hand to player
    players->push_back(newPlayer);
    cout << "Player '" << playerName << "' added  (" << players->size() << " player(s) so far)" << endl;
    return true;
}

/**
 * gamestart:
 *   a) Distribute territories fairly (round-robin over shuffled list)
 *   b) Randomise player order
 *   c) Give each player 50 armies in their reinforcement pool
 *   d) Each player draws 2 cards from the deck
 *   e) Transition to AssignReinforcement (already done before this call)
 */
bool GameEngine::gameStart(Command* cmd, GameState oldState) {
    if (players->size() < 2 || players->size() > 6) {
        cout << "ERROR: Need 2-6 players to start (currently "
             << players->size() << ")." << endl;
        *state = oldState;
        return false;
    }
    if (!gameMap) {
        cout << "ERROR: No map loaded." << endl;
        *state = oldState;
        return false;
    }

    mt19937 rng(random_device{}());

    // a) Fairly distribute territories
    cout << "\n--- (a) Distributing territories ---" << endl;
    vector<Territory>& territories = *gameMap->getTerritories();
    int numTerr   = static_cast<int>(territories.size());
    int numPlayers = static_cast<int>(players->size());

    vector<int> indices(numTerr);
    iota(indices.begin(), indices.end(), 0);
    shuffle(indices.begin(), indices.end(), rng);

    for (int i = 0; i < numTerr; ++i) {
        Player*    player    = (*players)[i % numPlayers];
        Territory* territory = &territories[indices[i]];
        territory->setOwner(player);
        player->getTerritoriesOwned()->push_back(territory);
    }
    for (Player* p : *players) {
        cout << "  " << p->getName() << ": "
             << p->getTerritoriesOwned()->size() << " territories" << endl;
    }

    // b) Randomise order of play
    cout << "\n--- (b) Randomising play order ---" << endl;
    shuffle(players->begin(), players->end(), rng);
    cout << "  Order: ";
    for (size_t i = 0; i < players->size(); ++i) {
        if (i > 0) cout << " -> ";
        cout << (*players)[i]->getName();
    }
    cout << endl;

    // c) Give each player 50 armies in their reinforcement pool
    cout << "\n--- (c) Assigning reinforcement pools ---" << endl;
    for (Player* p : *players) {
        p->setReinforcementPool(50);
        cout << "  " << p->getName() << ": 50 armies" << endl;
    }

    // d) Each player draws 2 initial cards
    cout << "\n--- (d) Drawing initial cards ---" << endl;
    for (Player* p : *players) {
        cout << "  " << p->getName() << " draws: " << endl;
        cout << "    Card 1 - "; deck->draw(p->getHand());
        cout << "    Card 2 - "; deck->draw(p->getHand());
    }

    // e) State already set to AssignReinforcement by applyCommand
    cout << "\n--- (e) Switching to play phase ---" << endl;
    cout << "  Game state: " << stateToString(*state) << endl;

    return true;
}


/**
 * Runs the startup phase until the game transitions to AssignReinforcement.
 */
void GameEngine::startupPhase() {
    cout << "========================================" << endl;
    cout << "        WARZONE - STARTUP PHASE" << endl;
    cout << "========================================" << endl;
    cout << "Commands:" << endl;
    cout << "  loadmap <filename>  - load a map" << endl;
    cout << "  validatemap         - validate loaded map" << endl;
    cout << "  addplayer <name>    - add a player (2-6 total)" << endl;
    cout << "  gamestart           - distribute territories & begin" << endl;
    cout << "  quit                - exit" << endl;
    cout << endl;
    listAvailableMaps();

    while (*state != GameState::AssignReinforcement) {
        cout << "(state: " << stateToString(*state) << ") ";
        if (!processNextCommand()) {
            return; // quit
        }
    }

    // Print final summary
    cout << "\n========================================" << endl;
    cout << "       STARTUP PHASE COMPLETE" << endl;
    cout << "========================================" << endl;
    cout << "Players in game (" << players->size() << "):" << endl;
    for (Player* p : *players) {
        cout << "  " << p->getName()
             << " | territories: " << p->getTerritoriesOwned()->size()
             << " | reinforcement pool: " << p->getReinforcementPool()
             << " | cards in hand: " << p->getHand()->getCards()->size()
             << endl;
    }
    cout << "Game is ready - entering play phase." << endl;
    cout << "========================================" << endl;
}
