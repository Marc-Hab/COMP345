#include "CommandProcessing.h"
#include <iostream>
#include <fstream>

// ---- Command ----

Command::Command(std::string cmd) : Subject() {
    this->command = new std::string(cmd);
    this->effect = new std::string("");
}

Command::Command(const Command& other) : Subject(other) {
    this->command = new std::string(*(other.command));
    this->effect = new std::string(*(other.effect));
}

Command& Command::operator=(const Command& other) {
    if (this != &other) {
        Subject::operator=(other);
        delete this->command;
        delete this->effect;
        this->command = new std::string(*(other.command));
        this->effect = new std::string(*(other.effect));
    }
    return *this;
}

Command::~Command() {
    delete this->command;
    delete this->effect;
}

// Stores the effect and notifies observers so it is written to the log file.
void Command::saveEffect(std::string effectText) {
    *(this->effect) = effectText;
    notify(*this);
}

std::string Command::getCommandNameText() {
    return *(this->command);
}

std::string Command::getEffect() {
    return *(this->effect);
}

// Returns the stored effect for the log file entry.
std::string Command::stringToLog() const {
    return "Command effect saved: " + *(this->effect);
}

std::ostream& operator<<(std::ostream& os, const Command& cmd) {
    os << "Command: " << *(cmd.command) << " | Effect: " << *(cmd.effect);
    return os;
}

// ---- CommandProcessor ----

CommandProcessor::CommandProcessor() : Subject() {
    this->commands = new std::list<Command*>();
}

CommandProcessor::CommandProcessor(const CommandProcessor& other) : Subject(other) {
    this->commands = new std::list<Command*>();
    for (Command* cmd : *(other.commands)) {
        this->commands->push_back(new Command(*cmd));
    }
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
    if (this != &other) {
        Subject::operator=(other);
        for (Command* cmd : *this->commands) {
            delete cmd;
        }
        this->commands->clear();
        for (Command* cmd : *other.commands) {
            this->commands->push_back(new Command(*cmd));
        }
    }
    return *this;
}

CommandProcessor::~CommandProcessor() {
    for (Command* cmd : *commands) {
        delete cmd;
    }
    commands->clear();
    delete commands;
    commands = nullptr;
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

// Stores the command and notifies observers so it is written to the log file.
void CommandProcessor::saveCommand(Command* cmd) {
    this->commands->push_back(cmd);
    notify(*this);
}

bool CommandProcessor::validate(Command* cmd, std::string currentState) {
    std::string text = cmd->getCommandNameText();

    if (currentState == "Start") {
        if (text.find("loadmap") == 0) return true;
    } else if (currentState == "MapLoaded") {
        if (text.find("loadmap") == 0 || text == "validatemap") return true;
    } else if (currentState == "MapValidated") {
        if (text.find("addplayer") == 0) return true;
    } else if (currentState == "PlayersAdded") {
        if (text.find("addplayer") == 0 || text == "gamestart") return true;
    } else if (currentState == "Win") {
        if (text == "replay" || text == "quit") return true;
    }

    cmd->saveEffect("Error: '" + text + "' is not valid in state " + currentState);
    return false;
}

// Returns the name of the most recently saved command for the log file entry.
std::string CommandProcessor::stringToLog() const {
    if (commands->empty()) return "CommandProcessor: no commands";
    return "Command saved: " + commands->back()->getCommandNameText();
}

std::ostream& operator<<(std::ostream& os, const CommandProcessor& cp) {
    os << "CommandProcessor containing " << cp.commands->size() << " commands:\n";
    for (Command* cmd : *(cp.commands)) {
        os << "  - " << *cmd << "\n";
    }
    return os;
}

// ---- FileLineReader ----

FileLineReader::FileLineReader() : fileStream(nullptr) {}

FileLineReader::FileLineReader(const FileLineReader& other) : fileStream(nullptr) {}

FileLineReader& FileLineReader::operator=(const FileLineReader& other) {
    return *this;
}

FileLineReader::~FileLineReader() {
    if (this->fileStream) {
        if (this->fileStream->is_open()) this->fileStream->close();
        delete this->fileStream;
    }
}

std::string FileLineReader::readLineFromFile(std::ifstream& fileStream) {
    std::string line;
    if (std::getline(fileStream, line)) {
        return line;
    }
    return "";
}

std::ostream& operator<<(std::ostream& os, const FileLineReader& flr) {
    os << "FileLineReader";
    return os;
}

// ---- FileCommandProcessorAdapter ----

FileCommandProcessorAdapter::FileCommandProcessorAdapter(std::string filename) : CommandProcessor() {
    this->filestream = new std::ifstream(filename);
    this->flr = new FileLineReader();
}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other) : CommandProcessor(other) {
    this->flr = new FileLineReader(*other.flr);
    this->filestream = new std::ifstream();
}

FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& other) {
    if (this != &other) {
        CommandProcessor::operator=(other);
        delete this->flr;
        delete this->filestream;
        this->flr = new FileLineReader(*other.flr);
        this->filestream = new std::ifstream();
    }
    return *this;
}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    if (this->filestream && this->filestream->is_open()) {
        this->filestream->close();
    }
    delete this->filestream;
    delete this->flr;
}

Command* FileCommandProcessorAdapter::readCommand() {
    std::string cmdText = flr->readLineFromFile(*(this->filestream));
    if (!cmdText.empty()) {
        return new Command(cmdText);
    }
    return nullptr;
}

std::ostream& operator<<(std::ostream& os, const FileCommandProcessorAdapter& adapter) {
    os << "FileCommandProcessorAdapter";
    return os;
}
