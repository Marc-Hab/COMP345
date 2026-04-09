#include "CommandProcessing.h"
#include "../GameEngine/GameEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

using namespace std;


// --- FileLineReader ---
// Constructor
FileLineReader::FileLineReader(const string& filePath) {
    this->filePath = new string(filePath);
    this->fileStream = new ifstream(*this->filePath);
    
    if (!fileStream->is_open()) {
        cout << "Failed to open file: " << filePath << endl;
        delete fileStream;
        fileStream = nullptr;
    }
}

// Copy constructor
FileLineReader::FileLineReader(const FileLineReader& other) {
    filePath = new string(*other.filePath);
    fileStream = new ifstream(*filePath);
    
    if (!fileStream->is_open()) {
        delete fileStream;
        fileStream = nullptr;
    }
}

// Destructor
FileLineReader::~FileLineReader() {
    if (fileStream) {
        if (fileStream->is_open()) {
            fileStream->close();
        }
        delete fileStream;
    }
    delete filePath;
}

// Assignment operator
FileLineReader& FileLineReader::operator=(const FileLineReader& other) {
    if (this == &other) return *this;
    
    // Remove old values from the heap
    if (fileStream) {
        if (fileStream->is_open()) {
            fileStream->close();
        }
        delete fileStream;
    }
    delete filePath;
    
    // Copy deeply new resources
    filePath = new string(*other.filePath);
    fileStream = new ifstream(*filePath);
    
    if (!fileStream->is_open()) {
        delete fileStream;
        fileStream = nullptr;
    }
    
    return *this;
}

// Stream insertion operator
ostream& operator<<(ostream& os, const FileLineReader& flr) {
    os << "FileLineReader [File: " << *flr.filePath;
    if (flr.fileStream && flr.fileStream->is_open()) {
        os << ", Status: OPEN";
    } else {
        os << ", Status: CLOSED";
    }
    os << "]";
    return os;
}

bool FileLineReader::isOpen() const{
    return fileStream->is_open();
}

string FileLineReader::readLineFromFile() {

    // Check if filestream is valid
    if (!fileStream || !fileStream->is_open()) {
        std::cerr << "Error: File is not open!" << std::endl;
        return "";
    }
    
    std::string line;
    if (std::getline(*fileStream, line)) {
        return line;
    }
    
    return "";
}

// --- Command ---
// Constructor
Command::Command(CommandName cmdName) {
    commandName = new CommandName(cmdName);
    arguments = new vector<string>();
    effect = new string("");
}

// Constructor with arguments
Command::Command(CommandName cmdName, const vector<string>& args) {
    commandName = new CommandName(cmdName);
    arguments = new vector<string>(args);
    effect = new string("");
}

// Copy constructor
Command::Command(const Command& other) {
    commandName = new CommandName(*other.commandName);
    arguments = new vector<string>(*other.arguments);
    effect = new string(*other.effect);
}

// Destructor
Command::~Command() {
    delete commandName;
    delete effect;
    delete arguments;
    commandName = nullptr;
    effect = nullptr;
    arguments = nullptr;
}

// Assignment operator
Command& Command::operator=(const Command& other) {
    if (this == &other) {
        return *this;
    }
    
    // Copy the values
    *commandName = *other.commandName;
    *arguments = *other.arguments;
    *effect = *other.effect;
    
    return *this;
}

// Stream insertion operator
ostream& operator<<(ostream& os, const Command& cmd) {
    os << "Command: " << cmd.getCommandName();
    
    if (!cmd.arguments->empty()) {
        os << " [";
        for (size_t i = 0; i < cmd.arguments->size(); i++) {
            os << cmd.arguments->at(i);
            if (i < cmd.arguments->size() - 1) os << ", ";
        }
        os << "]";
    }
    
    if (!cmd.effect->empty()) {
        os << " | Effect: " << *cmd.effect;
    }
    
    return os;
}

CommandName Command::getCommandName() const {
    return *commandName;
}

string Command::getEffect() const {
    return *effect;
}

vector<string> Command::getArguments() const {
    return *arguments;
}

string Command::getArgument(int index) const {
    if (index >= 0 && index < arguments->size()) {
        return arguments->at(index);
    }
    return "";
}

int Command::getNumArguments() const {
    return arguments->size();
}
 
// notifies observers (triggers log write via LogObserver)
void Command::saveEffect(const string& eff) {
    *effect = eff;
    notify();
}

// returns the saved effect for gamelog.txt
string Command::stringToLog() const {
    return "Command effect saved: " + *effect;
}

// --- CommandProcessor ---
// Constructor
CommandProcessor::CommandProcessor() {
    commands = new deque<Command*>();
    currentIndex = new int(0);
}

// Copy constructor
CommandProcessor::CommandProcessor(const CommandProcessor& other) {
    commands = new deque<Command*>();
    
    // Deep copy all commands
    for (Command* cmd : *(other.commands)) {
        commands->push_back(new Command(*cmd));
    }
    
    currentIndex = new int(*other.currentIndex);
}

// Destructor
CommandProcessor::~CommandProcessor() {
    // Delete all commands
    for (Command* cmd : *commands) {
        delete cmd;
    }
    delete commands;
    delete currentIndex;
    
    commands = nullptr;
    currentIndex = nullptr;
}

// Assignment operator
CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
    if (this == &other) {
        return *this;
    }
    
    // Clean up existing commands
    for (Command* cmd : *commands) {
        delete cmd;
    }
    commands->clear();
    
    // Deep copy new commands
    for (Command* cmd : *(other.commands)) {
        commands->push_back(new Command(*cmd));
    }
    
    *currentIndex = *other.currentIndex;
    
    return *this;
}

// Stream insertion operator
ostream& operator<<(ostream& os, const CommandProcessor& cp) {
    os << "CommandProcessor [Total Commands: " << cp.commands->size() 
       << ", Current Index: " << *cp.currentIndex << "]";
    return os;
}

// Stream insertion operator
ostream &operator<<(ostream &os, const FileCommandProcessorAdapter &adapter)
{
    os << "FileCommandProcessorAdapter [";
    os << "Commands: " << adapter.commands->size();
    os << ", Current Index: " << *adapter.currentIndex;
    os << ", File: " << *adapter.flr;
    os << "]";
    return os;
}

// notifies observers (triggers log write via LogObserver)
void CommandProcessor::saveCommand(Command* cmd) {
    if (cmd) {
        commands->push_back(cmd);
        notify();
    }
}

// returns info about the most recently saved command for gamelog.txt
string CommandProcessor::stringToLog() const {
    if (commands->empty()) return "CommandProcessor: no commands saved";
    Command* last = commands->back();
    return "Command saved to CommandProcessor: " + commandNameToString(last->getCommandName());
}

string CommandProcessor::readCommand() {
    cout << "Enter command: ";
    string input;
    getline(cin, input);
    
    // Remove whitespace
    size_t start = input.find_first_not_of(" \t\r\n");
    size_t end = input.find_last_not_of(" \t\r\n");
    if (start != string::npos && end != string::npos) {
        input = input.substr(start, end - start + 1);
    }
    
    return input;
}

namespace {
    
    // Basic struct for parsing strings into commands
    struct ParsedCommand {
        CommandName name;
        vector<string> arguments;
    };

    // Parses a string into a ParsedCommand
    ParsedCommand parseCommand(const string& input) {
    
    // Split input into tokens
    vector<string> tokens;
    stringstream ss(input);
    string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.empty()) {
        return {CommandName::Invalid, {}};
    }
    
    // First token is command name
    string cmdStr = tokens[0];
    transform(cmdStr.begin(), cmdStr.end(), cmdStr.begin(), ::tolower);
    
    // Rest are arguments
    vector<string> args(tokens.begin() + 1, tokens.end());
    
    CommandName cmdName = parseCommandName(cmdStr);
    
    return {cmdName, args};
    }
}

Command* CommandProcessor::getCommand() {
    // If no unread commands, read a new one
    if (*currentIndex >= commands->size()) {
        string cmdStr = readCommand();
        
        if (cmdStr.empty()) {
            cout << "Please enter a command." << cmdStr << endl;
            return new Command(CommandName::Invalid);
        }
        
        // Parse command string (gets name + arguments)
        ParsedCommand parsed = parseCommand(cmdStr);
        
        // Check if valid
        if (parsed.name == CommandName::Invalid) {
            cout << "Invalid command: " << cmdStr << endl;
            return new Command(CommandName::Invalid);
        }
        
        // Create command with arguments
        Command* cmd = new Command(parsed.name, parsed.arguments);
        saveCommand(cmd);
    }
    
    // Return next command and advance index
    Command* cmd = commands->at(*currentIndex);
    (*currentIndex)++;
    
    return cmd;
}


//------ Helper methods -------------------------------------

// Validates tournament command arguments.
// Format: tournament -M <maps...> -P <strategies...> -G <numGames> -D <maxTurns>
static bool validateTournamentArgs(Command* cmd) {
    vector<string> args = cmd->getArguments();

    vector<string> maps, strategies;
    int numGames = -1, maxTurns = -1;
    bool hasM = false, hasP = false, hasG = false, hasD = false;

    // parse each flag section
    string section;
    for (const string& arg : args) {
        if      (arg == "-M" || arg == "-m") { section = "M"; hasM = true; }
        else if (arg == "-P" || arg == "-p") { section = "P"; hasP = true; }
        else if (arg == "-G" || arg == "-g") { section = "G"; hasG = true; }
        else if (arg == "-D" || arg == "-d") { section = "D"; hasD = true; }
        else if (section == "M") { maps.push_back(arg); }
        else if (section == "P") { strategies.push_back(arg); }
        else if (section == "G") {
            try { numGames = stoi(arg); } catch (...) { numGames = -1; }
        }
        else if (section == "D") {
            try { maxTurns = stoi(arg); } catch (...) { maxTurns = -1; }
        }
    }

    if (!hasM || !hasP || !hasG || !hasD) {
        cmd->saveEffect(
            "ERROR: tournament requires all four flags: -M, -P, -G, -D. "
            "Usage: tournament -M <maps> -P <strategies> -G <numGames> -D <maxTurns>");
        return false;
    }

    // -M: 1-5 map files
    if (maps.empty() || maps.size() > 5) {
        cmd->saveEffect("ERROR: -M requires 1 to 5 map files (got " +
                        to_string(maps.size()) + ")");
        return false;
    }

    // -P: 2-4 strategies, valid names only, no duplicates
    const vector<string> validStrats = {"aggressive", "benevolent", "neutral", "cheater"};
    if (strategies.size() < 2 || strategies.size() > 4) {
        cmd->saveEffect(
            "ERROR: -P requires 2 to 4 player strategies (got " +
            to_string(strategies.size()) +
            "). Valid: Aggressive, Benevolent, Neutral, Cheater");
        return false;
    }

    set<string> seen;
    for (const string& s : strategies) {
        string lower = s;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        if (find(validStrats.begin(), validStrats.end(), lower) == validStrats.end()) {
            cmd->saveEffect(
                "ERROR: '" + s + "' is not a valid computer strategy. "
                "Valid: Aggressive, Benevolent, Neutral, Cheater");
            return false;
        }
        if (seen.count(lower)) {
            cmd->saveEffect("ERROR: duplicate strategy '" + s + "'");
            return false;
        }
        seen.insert(lower);
    }

    // -G: 1-5 games per map
    if (numGames < 1 || numGames > 5) {
        cmd->saveEffect("ERROR: -G requires 1 to 5 games per map (got " +
                        (numGames == -1 ? string("non-integer") : to_string(numGames)) + ")");
        return false;
    }

    // -D: 10-50 max turns per game
    if (maxTurns < 10 || maxTurns > 50) {
        cmd->saveEffect("ERROR: -D requires 10 to 50 max turns per game (got " +
                        (maxTurns == -1 ? string("non-integer") : to_string(maxTurns)) + ")");
        return false;
    }

    return true;
}

static bool commandWellFormed(Command* cmd){
    
    if (!cmd) {
        return false;
    }
 
    CommandName cmdName = cmd->getCommandName();
    vector<string> args = cmd->getArguments();
    
    switch (cmdName) {
        case CommandName::Tournament:
            return validateTournamentArgs(cmd);
        case CommandName::LoadMap:
            if (args.size() != 1) {
                cmd->saveEffect("ERROR: 'loadmap' requires exactly 1 argument (map filename). Usage: loadmap <filename>");
                return false;
            }

        case CommandName::AddPlayer:
            if (args.size() != 1) {
                cmd->saveEffect("ERROR: 'addplayer' requires exactly 1 argument (player name). Usage: addplayer <name>");
                return false;
            }
            
        case CommandName::ValidateMap:
        case CommandName::GameStart:
        case CommandName::IssueOrder:
        case CommandName::EndIssueOrders:
        case CommandName::ExecOrder:
        case CommandName::EndExecOrders:
        case CommandName::WinCmd:
        case CommandName::Replay:
        case CommandName::Quit:

            return true;
            
        case CommandName::Invalid:
            return false;
            
        default:
            cmd->saveEffect("ERROR: Unknown command");
            return false;
    }
           
}

static bool commandValidInState(Command* cmd, GameState currentState){

    if (!cmd) {
        return false;
    }
    
    CommandName cmdName = cmd->getCommandName();

    if (currentState == GameState::Start) {
        if (cmdName == CommandName::LoadMap || cmdName == CommandName::Tournament) {
            return true;
        }
    }
    else if (currentState == GameState::MapLoaded) {
        if (cmdName == CommandName::LoadMap || cmdName == CommandName::ValidateMap) {
            return true;
        }
    }
    else if (currentState == GameState::MapValidated) {
        if (cmdName == CommandName::AddPlayer) {
            return true;
        }
    }
    else if (currentState == GameState::PlayersAdded) {
        if (cmdName == CommandName::AddPlayer || cmdName == CommandName::GameStart) {
            return true;
        }
    }
    else if (currentState == GameState::AssignReinforcement) {
        if (cmdName == CommandName::IssueOrder) {
            return true;
        }
    }
    else if (currentState == GameState::IssueOrders) {
        if (cmdName == CommandName::IssueOrder || cmdName == CommandName::EndIssueOrders) {
            return true;
        }
    }
    else if (currentState == GameState::ExecuteOrders) {
        if (cmdName == CommandName::ExecOrder || 
            cmdName == CommandName::EndExecOrders || 
            cmdName == CommandName::WinCmd) {
            return true;
        }
    }
    else if (currentState == GameState::Win) {
        if (cmdName == CommandName::Replay || cmdName == CommandName::Quit) {
            return true;
        }
    }

    string error = "ERROR: Command '" + commandNameToString(cmdName) + 
    "' is not allowed in state " + stateToString(currentState);
    
    cmd->saveEffect(error);
    return false;

}

//------ End of Helper methods -------------------------------------

bool CommandProcessor::validate(Command* cmd, GameState currentState) {

    if (commandWellFormed(cmd) && commandValidInState(cmd, currentState)){
        cmd->saveEffect("Command validated successfully");
        return true;
    }

    return false;
}

// --- FileCommandProcessorAdapter ---
// Constructor
FileCommandProcessorAdapter::FileCommandProcessorAdapter(const string& filename)
    : CommandProcessor() {
    flr = new FileLineReader(filename);
}

// Copy constructor
FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other)
    : CommandProcessor(other) {  // Call parent copy constructor
    flr = new FileLineReader(*other.flr);
}

// Destructor
FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    delete flr;
    flr = nullptr;
}

// Assignment operator
FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& other) {
    if (this == &other) {
        return *this;
    }
    
    // Call parent assignment operator
    CommandProcessor::operator=(other);
    
    // Clean up and copy FileLineReader
    delete flr;
    flr = new FileLineReader(*other.flr);
    
    return *this;
}

bool FileCommandProcessorAdapter::isOpen() const {
    return flr->isOpen();
}

// reads from file instead of console
string FileCommandProcessorAdapter::readCommand() {
    string line = flr->readLineFromFile();
    
    if (line.empty()) {
        cout << "End of file reached or empty line" << endl;
        return "";
    }
    
    // Echo the command read from file
    cout << "Reading from file: " << line << endl;
    
    return line;
}
