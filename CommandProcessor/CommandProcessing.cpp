#include "CommandProcessing.h"
#include "../GameEngine/GameEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


//----------------------------------------------------------
// File Line Reader Implementation
//----------------------------------------------------------

/**
 * Constructor
 */
FileLineReader::FileLineReader(const string& filePath) {
    this->filePath = new string(filePath);
    this->fileStream = new ifstream(*this->filePath);
    
    if (!fileStream->is_open()) {
        cout << "Failed to open file: " << filePath << endl;
        delete fileStream;
        fileStream = nullptr;
    }
}

/**
 * Copy constructor (deep copy)
 */
FileLineReader::FileLineReader(const FileLineReader& other) {
    filePath = new string(*other.filePath);
    fileStream = new ifstream(*filePath);
    
    if (!fileStream->is_open()) {
        delete fileStream;
        fileStream = nullptr;
    }
}

/**
 * Destructor
 */
FileLineReader::~FileLineReader() {
    if (fileStream) {
        if (fileStream->is_open()) {
            fileStream->close();
        }
        delete fileStream;
    }
    delete filePath;
}

/**
 * Assignment Operator
 */
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

/**
 * Stream Insertion Operator
 */
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

/**
 * Returns if the filestream is open
 */
bool FileLineReader::isOpen() const{
    return fileStream->is_open();
}

/**
 * Reads a line from the file.
 * If file closed, empty string will be returned.
 */
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
    
    return "";  // Return empty string if EOF or error
}

//----------------------------------------------------------
// End of File Line Reader Implementation
//----------------------------------------------------------


//----------------------------------------------------------
// Command Implementation
//----------------------------------------------------------

/**
 * Constructor : No arguments
 */
Command::Command(CommandName cmdName) {
    commandName = new CommandName(cmdName);
    arguments = new vector<string>();
    effect = new string("");
}

/**
 * Constructor :  With arguments
 */
Command::Command(CommandName cmdName, const vector<string>& args) {
    commandName = new CommandName(cmdName);
    arguments = new vector<string>(args);
    effect = new string("");
}

/**
 * Copy Constructor
 */
Command::Command(const Command& other) {
    commandName = new CommandName(*other.commandName);
    arguments = new vector<string>(*other.arguments);
    effect = new string(*other.effect);
}

/**
 * Destructor
 */
Command::~Command() {
    delete commandName;
    delete effect;
    delete arguments;
    commandName = nullptr;
    effect = nullptr;
    arguments = nullptr;
}

/**
 * Assignment Operator
 */
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

/**
 * Stream Insertion Operator
 */
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

/**
 * Get command name
 */
CommandName Command::getCommandName() const {
    return *commandName;
}

/**
 * Get Effect
 */
string Command::getEffect() const {
    return *effect;
}

/**
 * Get All Arguments
 */
vector<string> Command::getArguments() const {
    return *arguments;
}

/**
 * Get Specific Argument
 */
string Command::getArgument(int index) const {
    if (index >= 0 && index < arguments->size()) {
        return arguments->at(index);
    }
    return "";
}

/**
 * Get Number of Arguments
 */
int Command::getNumArguments() const {
    return arguments->size();
}
 
/**
 * Save Effect
 */
void Command::saveEffect(const string& eff) {
    *effect = eff;
}

//----------------------------------------------------------
// End of Command Implementation
//----------------------------------------------------------

//----------------------------------------------------------
// Command Processor Implementation
//----------------------------------------------------------

/**
 * Default Constructor
 */
CommandProcessor::CommandProcessor() {
    commands = new deque<Command*>();
    currentIndex = new int(0);
}

/**
 * Copy Constructor - Deep copy
 */
CommandProcessor::CommandProcessor(const CommandProcessor& other) {
    commands = new deque<Command*>();
    
    // Deep copy all commands
    for (Command* cmd : *(other.commands)) {
        commands->push_back(new Command(*cmd));
    }
    
    currentIndex = new int(*other.currentIndex);
}

/**
 * Destructor
 */
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

/**
 * Assignment Operator
 */
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

/**
 * Stream Insertion Operator
 */
ostream& operator<<(ostream& os, const CommandProcessor& cp) {
    os << "CommandProcessor [Total Commands: " << cp.commands->size() 
       << ", Current Index: " << *cp.currentIndex << "]";
    return os;
}

/**
 * Stream Insertion Operator
 */
ostream &operator<<(ostream &os, const FileCommandProcessorAdapter &adapter)
{
    os << "FileCommandProcessorAdapter [";
    os << "Commands: " << adapter.commands->size();
    os << ", Current Index: " << *adapter.currentIndex;
    os << ", File: " << *adapter.flr;
    os << "]";
    return os;
}

/**
 * Add command to command history
 */
void CommandProcessor::saveCommand(Command* cmd) {
    if (cmd) {
        commands->push_back(cmd);
    }
}

/**
 * Reads a command from the console (can be overridden)
 */
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

/**
 * Returns next command to execute
 */
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

// Helper methods for the validate method
namespace {
    
/**
* Verifies the arguments of a command
*/
bool commandWellFormed(Command* cmd){
    
    if (!cmd) {
        return false;
    }
 
    CommandName cmdName = cmd->getCommandName();
    vector<string> args = cmd->getArguments();
    
    switch (cmdName) {
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

/**
* Verifies the argument is valid given the current state
*/
bool commandValidInState(Command* cmd, GameState currentState){

    if (!cmd) {
        return false;
    }
    
    CommandName cmdName = cmd->getCommandName();

    if (currentState == GameState::Start) {
        if (cmdName == CommandName::LoadMap) {
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

}


bool CommandProcessor::validate(Command* cmd, GameState currentState) {

    if (commandWellFormed(cmd) && commandValidInState(cmd, currentState)){
        cmd->saveEffect("Command validated successfully");
        return true;
    }

    return false;
}

//----------------------------------------------------------
// End of Command Processor Implementation
//----------------------------------------------------------

//----------------------------------------------------------
// File Command Processor Adapter Implementation
//----------------------------------------------------------

/**
 * Constructor - Takes filename and creates FileLineReader
 */
FileCommandProcessorAdapter::FileCommandProcessorAdapter(const string& filename)
    : CommandProcessor() {
    flr = new FileLineReader(filename);
}

/**
 * Copy Constructor - Deep copy
 */
FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other)
    : CommandProcessor(other) {  // Call parent copy constructor
    flr = new FileLineReader(*other.flr);
}

/**
 * Destructor
 */
FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    delete flr;
    flr = nullptr;
}

/**
 * Assignment Operator
 */
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

/**
 * Verifies if the filestream is open
 */
bool FileCommandProcessorAdapter::isOpen() const {
    return flr->isOpen();
}

/**
 * Read Command overriden
 */
string FileCommandProcessorAdapter::readCommand() {
    // Read line from file using FileLineReader
    string line = flr->readLineFromFile();
    
    if (line.empty()) {
        cout << "End of file reached or empty line" << endl;
        return "";
    }
    
    // Echo the command read from file
    cout << "Reading from file: " << line << endl;
    
    return line;
}
