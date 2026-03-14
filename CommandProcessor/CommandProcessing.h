#pragma once

#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include "../LoggingObserver/LoggingObserver.h"

class Command : public Subject, public ILoggable {
public:
    Command(std::string cmd);
    Command(const Command& other);
    Command& operator=(const Command& other);
    friend std::ostream& operator<<(std::ostream& os, const Command& cmd);
    ~Command();

    void saveEffect(std::string effect);
    std::string getCommandNameText();
    std::string getEffect();
    std::string stringToLog() const override;

private:
    std::string* command;
    std::string* effect;
};

class CommandProcessor : public Subject, public ILoggable {
public:
    CommandProcessor();
    CommandProcessor(const CommandProcessor& other);
    CommandProcessor& operator=(const CommandProcessor& other);
    friend std::ostream& operator<<(std::ostream& os, const CommandProcessor& cp);
    virtual ~CommandProcessor();

    Command* getCommand();
    bool validate(Command* command, std::string state);

    void saveCommand(Command* cmd);
    std::string stringToLog() const override;

protected:
    virtual Command* readCommand();

private:
    std::list<Command*>* commands;
};

class FileLineReader {
public:
    FileLineReader();
    FileLineReader(const FileLineReader& other);
    FileLineReader& operator=(const FileLineReader& other);
    friend std::ostream& operator<<(std::ostream& os, const FileLineReader& flr);
    ~FileLineReader();

    std::string readLineFromFile(std::ifstream& file);

private:
    std::ifstream* fileStream;
};

class FileCommandProcessorAdapter : public CommandProcessor {
public:
    FileCommandProcessorAdapter(std::string filename);
    FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other);
    FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter& other);
    friend std::ostream& operator<<(std::ostream& os, const FileCommandProcessorAdapter& adapter);
    ~FileCommandProcessorAdapter();

protected:
    Command* readCommand() override;

private:
    FileLineReader* flr;
    std::ifstream* filestream;
};
