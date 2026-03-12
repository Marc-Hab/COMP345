#pragma once
#include "CommandProcessor/CommandProcessing.h"
#include "GameEngine/GameEngine.h"
#include <iostream>
# include <string>
# include <list>
# include <fstream>

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
    // This overrides the CommandProcessor's version to read from a file instead
    Command* readCommand() override; 

private:
    FileLineReader* flr;          
    std::ifstream* filestream; 
};

class Command {
public:
    Command(std::string cmd);
    // Orthodox Canonical Form requirements
    Command(const Command& other);
    Command& operator=(const Command& other);
    friend std::ostream& operator<<(std::ostream& os, const Command& cmd);
    ~Command();

    void saveEffect(std::string effect);
    std::string getCommandNameText();
    std::string getEffect();

private:
    std::string* command; 
    std::string* effect;  
};
class CommandProcessor {
    std::string *command;
    public:
        CommandProcessor();
        CommandProcessor(const CommandProcessor& other);
        CommandProcessor& operator=(const CommandProcessor& other);
        friend std::ostream& operator<<(std::ostream& os, const CommandProcessor& cp);

        virtual ~CommandProcessor();       

        Command* getCommand();
        
        bool validate(Command* command,std::string state);

        
   protected:
    virtual Command* readCommand(); 

    private:
    void saveCommand(Command* cmd);
    std::list<Command*>* commands; 


};