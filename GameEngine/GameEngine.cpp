#include "GameEngine.h"
#include "../CommandProcessor/CommandProcessing.h"
#include "../Maps/Map.h"
#include "../Players/Player.h"
#include "../Cards/Cards.h"
#include "../Orders/Orders.h"
#include "../PlayerStrategies/PlayerStrategies.h"
#include <cctype>
#include <sstream>
#include <algorithm>
#include <random>
#include <numeric>
#include <iomanip>
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
    transitions[{GameState::Start, CommandName::Tournament}] = GameState::Win;

    // Allow re-loading map while still setting up
    transitions[{GameState::MapLoaded, CommandName::LoadMap}] = GameState::MapLoaded;
    transitions[{GameState::MapLoaded, CommandName::ValidateMap}] = GameState::MapValidated;

    transitions[{GameState::MapValidated, CommandName::AddPlayer}] = GameState::PlayersAdded;

    // Add multiple players
    transitions[{GameState::PlayersAdded, CommandName::AddPlayer}] = GameState::PlayersAdded;

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
    
    if (token == "tournament") return CommandName::Tournament;
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
        case CommandName::Tournament: return "tournament";
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

    while (*state != GameState::End){

        startupPhase();

        if (*state == GameState::AssignReinforcement) {
            mainGameLoop();
        }

        //replay or quit
        if (*state == GameState::Win){
            endPhase();
        }
    }

    cout << "\nGame ended." << endl;
}

// ---------- Main game loop phases ----------

/**
 * Gives each player armies based on territories owned and continent bonuses. 
 * Minimum 3 armies per turn.
 */
void GameEngine::reinforcementPhase() {
    transition(GameState::AssignReinforcement);
    cout << "\n========== REINFORCEMENT PHASE ==========" << endl;

    for (Player* p : *players) {
        int numTerritories = static_cast<int>(p->getTerritoriesOwned()->size());
        int armies = max(3, numTerritories / 3);

        // Check continent control bonuses
        for (Continent& continent : *gameMap->getContinents()) {
            vector<Territory*>* contTerritories = continent.getTerritories();
            if (contTerritories->empty()) continue;
            bool ownsAll = true;
            for (Territory* t : *contTerritories) {
                if (t->getOwner() != p) { ownsAll = false; break; }
            }
            if (ownsAll) {
                cout << "  " << p->getName() << " controls continent '"
                     << continent.getName() << "' (+"
                     << continent.getBonusValue() << " bonus armies)" << endl;
                armies += continent.getBonusValue();
            }
        }

        p->addReinforcements(armies);
        cout << "  " << p->getName() << ": " << numTerritories
             << " territories -> +" << armies
             << " armies (pool now: " << p->getReinforcementPool() << ")" << endl;
    }
}

/**
 * Players issue orders during their turm.
 * Players deploy first, then advance and play cards.
 */
void GameEngine::issueOrdersPhase() {
    transition(GameState::IssueOrders);
    cout << "\n========== ISSUE ORDERS PHASE ==========" << endl;

    // Reset each player's per-turn state
    for (Player* p : *players) {
        p->resetOrderIssuingState();
    }

    vector<bool> done(players->size(), false);
    bool anyStillIssuing = true;

    while (anyStillIssuing) {
        anyStillIssuing = false;
        for (size_t i = 0; i < players->size(); ++i) {
            if (!done[i]) {
                bool issued = (*players)[i]->issueOrder(deck, players);
                if (issued) {
                    anyStillIssuing = true;
                } else {
                    done[i] = true;
                }
            }
        }
    }
}

/**
 * - Executes all Deploy orders first before any other orders. 
 * - Combat is handled inside Advance::execute(). 
 * - At the end of the turn, any player who conquered at least one territory draws one card.
 */
void GameEngine::executeOrdersPhase() {
    transition(GameState::ExecuteOrders);
    cout << "\n========== EXECUTE ORDERS PHASE ==========" << endl;

    // Execute all Deploy orders first
    cout << "\n--- Executing Deploy orders ---" << endl;
    bool anyDeploys = true;
    while (anyDeploys) {
        anyDeploys = false;
        for (Player* p : *players) {
            OrdersList* orders = p->getOrders();
            for (int i = 0; i < orders->size(); ++i) {
                if (orders->orderAt(i)->isDeploy()) {
                    cout << "  [" << p->getName() << "] ";
                    orders->orderAt(i)->execute();
                    orders->remove(i);
                    anyDeploys = true;
                    break; // restart scan after removal
                }
            }
        }
    }

    // Execute all other orders in round robin
    cout << "\n--- Executing non-Deploy orders ---" << endl;
    bool anyOrders = true;
    while (anyOrders) {
        anyOrders = false;
        for (Player* p : *players) {
            OrdersList* orders = p->getOrders();
            if (orders->size() > 0) {
                cout << "  [" << p->getName() << "] ";
                orders->orderAt(0)->execute();
                orders->remove(0);
                anyOrders = true;
            }
        }
    }

    // Player who conquered at least one territory draws a card.
    cout << "\n--- Awarding cards for conquests ---" << endl;
    bool anyReward = false;
    for (Player* p : *players) {
        if (p->getConqueredThisTurn()) {
            if (deck && !deck->getCards()->empty()) {
                cout << "  " << p->getName()
                     << " conquered territory this turn — drawing a card: ";
                deck->draw(p->getHand());
            }
            p->setConqueredThisTurn(false);
            anyReward = true;
        }
    }
    if (!anyReward) {
        cout << "  No territories conquered this round — no cards awarded." << endl;
    }
}

/**
 * Main game loop: repeats Reinforcement -> IssueOrders -> ExecuteOrders
 * until one player owns all territories or only one player remains.
 * Players with no territories are eliminated after each round.
 */
void GameEngine::mainGameLoop() {
    cout << "\n========================================" << endl;
    cout << "          MAIN GAME LOOP" << endl;
    cout << "========================================" << endl;

    int round = 0;
    while (true) {
        round++;
        cout << "\n============== ROUND " << round << " ==============" << endl;

        reinforcementPhase();
        issueOrdersPhase();
        executeOrdersPhase();

        // Remove players with no territories
        auto it = players->begin();
        while (it != players->end()) {
            if ((*it)->getTerritoriesOwned()->empty()) {
                cout << "\n*** " << (*it)->getName()
                     << " controls no territories and is eliminated! ***" << endl;
                delete *it;
                it = players->erase(it);
            } else {
                ++it;
            }
        }

        // Check win condition: one player owns all territories
        if (players->size() == 1) {
            transition(GameState::Win);
            cout << "\n========================================" << endl;
            cout << "  WINNER: " << (*players)[0]->getName() << "!" << endl;
            cout << "========================================" << endl;
            return;
        }
        if (players->empty()) {
            cout << "All players eliminated." << endl;
            return;
        }
        int totalTerr = static_cast<int>(gameMap->getTerritories()->size());
        for (Player* p : *players) {
            if (static_cast<int>(p->getTerritoriesOwned()->size()) == totalTerr) {
                transition(GameState::Win);
                cout << "\n========================================" << endl;
                cout << "  WINNER: " << p->getName()
                     << " controls all " << totalTerr << " territories!" << endl;
                cout << "========================================" << endl;
                return;
            }
        }
    }
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
        transition(GameState::End);
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

    // Execute the game action (may revert state on failure)
    bool actionOk = executeCommand(cmd);
    if (!actionOk) {
        cmd->saveEffect("ERROR: Action failed, state reverted to " + stateToString(*state));
        return;
    }

    transition(it->second);

    // Print transition
    cout << "[" << stateToString(oldState) << " -> " << stateToString(*state) << "]" << endl;
    cmd->saveEffect("Transition successful: " + stateToString(oldState) +
                    " -> " + stateToString(*state));
}

/**
 * Transitions to a new state and notifies all attached observers.
 * Used only for meaningful state changes (not rollbacks).
 */
void GameEngine::transition(GameState newState) {
    *state = newState;
    notify();
}

/**
 * Returns the current state name for the game log
 */
std::string GameEngine::stringToLog() const {
    return "GameEngine state changed to: " + stateToString(*state);
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
bool GameEngine::executeCommand(Command* cmd) {
    switch (cmd->getCommandName()) {
        case CommandName::Tournament:  return playTournament(cmd);
        case CommandName::LoadMap:     return loadMap(cmd);
        case CommandName::ValidateMap: return validateMap(cmd);
        case CommandName::AddPlayer:   return addPlayer(cmd);
        case CommandName::GameStart:   return gameStart(cmd);
        default: return true; // no extra action for play-phase commands
    }
}

/**
 * Plays a full tournament:
 *   > P strategies play G games per each map M
 *   > A game ends when one player conquers all territories (win) or D turns 
 *     have elapsed (draw)
 *   > Prints the results table to the console and saves it to the log via cmd->saveEffect
 */
bool GameEngine::playTournament(Command* cmd) {
    // ── 1. Parse arguments (already validated by CommandProcessor) ───────────
    vector<string> args = cmd->getArguments();
    vector<string> mapFiles, strategyNames;
    int numGames = 0, maxTurns = 0;
    string section;
    for (const string& arg : args) {
        if      (arg == "-M" || arg == "-m") section = "M";
        else if (arg == "-P" || arg == "-p") section = "P";
        else if (arg == "-G" || arg == "-g") section = "G";
        else if (arg == "-D" || arg == "-d") section = "D";
        else if (section == "M") mapFiles.push_back(arg);
        else if (section == "P") strategyNames.push_back(arg);
        else if (section == "G") { try { numGames = stoi(arg); } catch (...) {} }
        else if (section == "D") { try { maxTurns = stoi(arg); } catch (...) {} }
    }

    int M = (int)mapFiles.size();
    int G = numGames;

    // results[mapIdx][gameIdx] = winner name or "Draw"
    vector<vector<string>> results(M, vector<string>(G, "Draw"));

    // ── 2. Print tournament header ─────────────────────────────────────────
    cout << "\n========================================\n";
    cout << "           TOURNAMENT MODE\n";
    cout << "========================================\n";
    cout << "Maps      : ";
    for (int i = 0; i < M; i++) { if (i) cout << ", "; cout << mapFiles[i]; }
    cout << "\nStrategies: ";
    for (int i = 0; i < (int)strategyNames.size(); i++) { if (i) cout << ", "; cout << strategyNames[i]; }
    cout << "\nGames/map : " << G
         << "\nMax turns : " << maxTurns << "\n";

    // ── 3. Play every game on every map ────────────────────────────────────
    for (int m = 0; m < M; m++) {
        cout << "\n\n========== MAP " << (m + 1) << ": " << mapFiles[m] << " ==========\n";

        for (int g = 0; g < G; g++) {
            cout << "\n--- Map " << (m + 1) << ", Game " << (g + 1)
                 << " / " << G << " ---\n";

            // Reset all game data (clears players, old map, refreshes deck)
            newGame();

            // Reload the map fresh from file for each game so that territory
            // adjacency pointers are valid within the new Map object.
            gameMap = MapLoader::getInstance().loadMap(mapFiles[m]);
            if (!gameMap)
                gameMap = MapLoader::getInstance().loadMap("../Maps/map files/" + mapFiles[m]);

            if (!gameMap || !gameMap->validate()) {
                cout << "ERROR: could not load/validate '" << mapFiles[m]
                     << "' — game recorded as Draw.\n";
                delete gameMap;
                gameMap = nullptr;
                continue; // results[m][g] stays "Draw"
            }

            // Create one player per strategy
            for (const string& stratName : strategyNames) {
                string lower = stratName;
                transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

                Player* p = new Player(stratName);
                p->getHand()->setPlayer(p);

                PlayerStrategy* strat = nullptr;
                if      (lower == "aggressive") strat = new AggressivePlayerStrategy();
                else if (lower == "benevolent") strat = new BenevolentPlayerStrategy();
                else if (lower == "neutral")    strat = new NeutralPlayerStrategy();
                else if (lower == "cheater")    strat = new CheaterPlayerStrategy();
                if (strat) p->setStrategy(strat);

                players->push_back(p);
            }

            // Distribute territories, randomise order, give armies, draw cards
            Command fakeStart(CommandName::GameStart);
            if (!gameStart(&fakeStart)) {
                cout << "ERROR: gameStart failed — game recorded as Draw.\n";
                continue;
            }

            // Run game with a turn limit
            results[m][g] = playOneGame(maxTurns);
            cout << "\n  >> Result: " << results[m][g] << "\n";
        }
    }

    // ── 4. Build and output the results table ──────────────────────────────
    const int MAP_COL  = 23;   // width of "Map N  " column
    const int GAME_COL = 15;  // width of each "Game N" column

    ostringstream table;
    table << "\nTournament mode:\n";
    table << "M: ";
    for (int i = 0; i < M; i++) { if (i) table << ", "; table << mapFiles[i]; }
    table << "\nP: ";
    for (int i = 0; i < (int)strategyNames.size(); i++) { if (i) table << ", "; table << strategyNames[i]; }
    table << "\nG: " << G << "\nD: " << maxTurns << "\n\nResults:\n";

    // Header row
    table << setw(MAP_COL) << left << "";
    for (int g = 0; g < G; g++)
        table << setw(GAME_COL) << left << ("Game " + to_string(g + 1));
    table << "\n";

    // Data rows
    for (int m = 0; m < M; m++) {
        table << setw(MAP_COL) << left << ("Map " + to_string(m + 1) + " (" + mapFiles[m] + ")");
        for (int g = 0; g < G; g++)
            table << setw(GAME_COL) << left << results[m][g];
        table << "\n";
    }

    string tableStr = table.str();
    cout << "\n" << tableStr;
    cmd->saveEffect(tableStr);  // logged to gamelog.txt via LogObserver

    return true;
}

/**
 * Runs one game for up to maxTurns rounds.
 * Returns the winning player's name, or "Draw" if the turn limit is reached.
 * Assumes players and gameMap are already set up (call gameStart first).
 */
string GameEngine::playOneGame(int maxTurns) {
    for (int turn = 1; turn <= maxTurns; turn++) {
        cout << "\n----- Turn " << turn << " / " << maxTurns << " -----\n";

        reinforcementPhase();
        issueOrdersPhase();
        executeOrdersPhase();

        // Eliminate players with no territories
        auto it = players->begin();
        while (it != players->end()) {
            if ((*it)->getTerritoriesOwned()->empty()) {
                cout << "\n*** " << (*it)->getName()
                     << " is eliminated! ***\n";
                delete *it;
                it = players->erase(it);
            } else {
                ++it;
            }
        }

        if (players->size() == 1) return (*players)[0]->getName();
    }

    return "Draw"; // max turns reached without a winner
}

/**
 * Load map from file.
 * Tries the given name first, then looks in the map files directory.
 */
bool GameEngine::loadMap(Command* cmd) {
    string filename = cmd->getArgument(0);

    Map* newMap = MapLoader::getInstance().loadMap(filename);
    if (!newMap) {
        newMap = MapLoader::getInstance().loadMap("../Maps/map files/" + filename);
    }
    if (!newMap) {
        cout << "ERROR: Could not load map '" << filename << "'. Check the filename and try again." << endl;
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
bool GameEngine::validateMap(Command* cmd) {
    if (!gameMap) {
        cout << "ERROR: No map loaded." << endl;
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
bool GameEngine::addPlayer(Command* cmd) {
    if (players->size() >= 6) {
        cout << "ERROR: Maximum 6 players allowed." << endl;
        return false;
    }

    // Check if name already exists.
    string playerName = cmd->getArgument(0);
    for (Player* p : *players) {
        if (p->getName() == playerName) {
            cout << "ERROR: A player named '" << playerName << "' already exists." << endl;
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
 *   e) Transition to AssignReinforcement
 */
bool GameEngine::gameStart(Command* cmd) {
    if (players->size() < 2 || players->size() > 6) {
        cout << "ERROR: Need 2-6 players to start (currently "
             << players->size() << ")." << endl;
        return false;
    }
    if (!gameMap) {
        cout << "ERROR: No map loaded." << endl;
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
    cout << "  Game state: " << stateToString(GameState::AssignReinforcement) << endl;

    return true;
}


/**
 * Runs the startup phase until the game transitions to AssignReinforcement.
 */
void GameEngine::startupPhase() {
    cout << "\n" << endl;
    cout << "========================================" << endl;
    cout << "        WARZONE - STARTUP PHASE" << endl;
    cout << "========================================" << endl;
    cout << "Commands:" << endl;
    cout << "  loadmap <filename>      - load a map" << endl;
    cout << "  validatemap             - validate loaded map" << endl;
    cout << "  addplayer <name>        - add a player (2-6 total)" << endl;
    cout << "  gamestart               - distribute territories & begin" << endl;
    cout << "  tournament -M <maps> -P <strategies> -G <games> -D <maxturns>" << endl;
    cout << "      M: 1-5 map files    P: 2-4 of: Aggressive Benevolent Neutral Cheater" << endl;
    cout << "      G: 1-5 games        D: 10-50 max turns per game" << endl;
    cout << "  quit                    - exit" << endl;
    cout << endl;
    listAvailableMaps();

    while (! (*state == GameState::AssignReinforcement || *state == GameState::Win)) {
        cout << "\n(state: " << stateToString(*state) << ") ";
        if (!processNextCommand()) {
            return; // quit
        }
    }

    if (*state == GameState::AssignReinforcement){
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
}

/**
 * Resets game data to start a new game
 */
void GameEngine::newGame(){
    
    for (Player* p : *players) {
        delete p;
    }

    players->clear();

    delete gameMap;
    gameMap = nullptr;

    delete deck;
    deck = new Deck();

}

/**
 * Runs the end phase i.e. replay or quit
 */
void GameEngine::endPhase(){

    while(*state == GameState::Win){
        cout << "\n(state: " << stateToString(*state) << ") ";
        if (!processNextCommand()){
            return;
        }
    }

    newGame();
}
