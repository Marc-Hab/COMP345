#include "CommandProcessing.h"
#include <iostream>
#include <fstream>

Command::Command(const Command& other) {
    // We allocate NEW memory and copy the VALUE from the other object
    this->command = new std::string(*(other.command)); 
    this->effect = new std::string(*(other.effect)); 
}

Command& Command::operator=(const Command& other) {
    if (this != &other) {
        // Delete old memory to avoid leaks
        delete this->command;
        delete this->effect;

        //  Perform deep copy
        this->command = new std::string(*(other.command));
        this->effect = new std::string(*(other.effect));
    }
    return *this;
}
void Command::saveEffect(std::string effectText) {
    *(this->effect) = effectText; 
}

std::string Command::getCommandNameText() {
    return *(this->command);
}

CommandProcessor::CommandProcessor(const CommandProcessor& other) {
    this->commands = new std::list<Command*>();

    for (Command* cmd : *(other.commands)) {
        this->commands->push_back(new Command(*cmd));
    }
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
    if (this != &other) { 
        for (Command* cmd : *this->commands) {
            delete cmd;
        }
        this->commands->clear();

        // 2. Deep copy from 'other'
        for (Command* cmd : *other.commands) {
            // We create NEW Command objects using the Command copy constructor
            this->commands->push_back(new Command(*cmd));
        }
    }
    return *this;
}

Command* CommandProcessor::getCommand() {
    Command* cmd = readCommand(); 
    
    saveCommand(cmd); 
    
    return cmd;
}

Command* CommandProcessor::readCommand() {
    std::string input;
    std::cout << "Enter command: ";
    
    std::getline(std::cin, input); 
    
    return new Command(input);
}
void CommandProcessor::saveCommand(Command* cmd) {
    this->commands->push_back(cmd); 
}

bool CommandProcessor::validate(Command* cmd, std::string currentState) {
    std::string text = cmd->getCommandNameText();

    // Check transitions from Source
    if (currentState == "Start") {
        if (text.find("loadmap") == 0) return true;
    } 
    else if (currentState == "MapLoaded") {
        if (text.find("loadmap") == 0 || text == "validatemap") return true;
    } 
    else if (currentState == "MapValidated") {
        if (text.find("addplayer") == 0) return true;
    } 
    else if (currentState == "PlayersAdded") {
        if (text.find("addplayer") == 0 || text == "gamestart") return true;
    } 
    else if (currentState == "Win") {
        if (text == "replay" || text == "quit") return true;
    }

    cmd->saveEffect("Error: Command '" + text + "' is not valid in state " + currentState);
    return false;
}

std::string FileLineReader::readLineFromFile(std::ifstream& fileStream) { 
    std::string line; 
    if (std::getline(fileStream, line)) { 
        return line; 
    } 
    return ""; // Return empty if EOF 
}

Command* FileCommandProcessorAdapter::readCommand() {
    // We dereference 'filestream' because readLineFromFile requires a reference
    std::string cmdText = flr->readLineFromFile(*(this->filestream));
    
    if (!cmdText.empty()) {
        return new Command(cmdText);
    }
    return nullptr;
}

// Command Destructor
Command::~Command() {
    delete this->command; // Delete the command string pointer
    delete this->effect; // Delete the effect string pointer
}

// CommandProcessor Destructor
CommandProcessor::~CommandProcessor() {
    // We must delete every Command object inside the list
    for (Command* cmd : *commands) { 
        delete cmd;
    }
    // Then delete the list pointer itself if it's a pointer
    commands->clear();
    delete commands;
    commands = nullptr;
}

// Command: Should output the command text and its effect
std::ostream& operator<<(std::ostream& os, const Command& cmd) {
    os << "Command: " << *(cmd.command) 
       << " | Effect: " << *(cmd.effect);
    return os;
}

// CommandProcessor: Should output a list of all commands stored
std::ostream& operator<<(std::ostream& os, const CommandProcessor& cp) {
    os << "CommandProcessor containing " << cp.commands->size() << " commands:\n";
    for (Command* cmd : *(cp.commands)) {
        os << "  - " << *cmd << "\n"; 
    }
    return os;
}

FileLineReader::~FileLineReader() {
    if (this->fileStream) {
        if (this->fileStream->is_open()) this->fileStream->close();
        delete this->fileStream; 
    }
}


FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    delete this->flr;
}