#pragma once
#include <iostream>
#include <string>
#include <deque>
#include <fstream>

using namespace std;

//Forward declarations
class Command;
enum class GameState;
enum class CommandName;

class FileLineReader {
public:
    // Constructors
    FileLineReader(const string& filePath);
    FileLineReader(const FileLineReader& other);

    // Destructor
    ~FileLineReader();
    
    // Assignment and stream insertion operator
    FileLineReader& operator=(const FileLineReader& other);
    friend ostream& operator<<(ostream& os, const FileLineReader& flr);

    bool isOpen() const;

    string readLineFromFile();

private:
    const string* filePath;
    ifstream* fileStream; 
};

class Command {
public:
    // Constructors
    Command(CommandName cmdName);
    Command(CommandName cmdName, const vector<string>& args); 
    Command(const Command& other);

    //Destructor
    ~Command();
    
    // Assignment and stream insertion operator
    Command& operator=(const Command& other);
    friend ostream& operator<<(ostream& os, const Command& cmd);

    // Getters
    CommandName getCommandName() const;
    string getEffect() const;

    vector<string> getArguments() const;
    string getArgument(int index) const;
    int getNumArguments() const;

    void saveEffect(const string& effect); 

private:
    CommandName* commandName; 
    vector<string>* arguments;
    string* effect;  
};


class CommandProcessor {
    
public:
    // Constructors
    CommandProcessor();
    CommandProcessor(const CommandProcessor& other);

    // Destructor 
    virtual ~CommandProcessor(); 
    
    // Assignment and stream insertion operator
    CommandProcessor& operator=(const CommandProcessor& other);
    friend ostream& operator<<(ostream& os, const CommandProcessor& cp);
          
    Command* getCommand();
    
    bool validate(Command* command, GameState state);

    

        
protected:
    virtual string readCommand();
    deque<Command*>* commands; 
    int* currentIndex;
    
private:
    void saveCommand(Command* cmd);
    
};


class FileCommandProcessorAdapter : public CommandProcessor {
public:
    // Constructors
    FileCommandProcessorAdapter(const string& filename);
    FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other);
    
    // Destructor
    ~FileCommandProcessorAdapter();
    
    // Assignment and stream insertion operator
    FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter& other);
    friend ostream& operator<<(ostream& os, const FileCommandProcessorAdapter& adapter);

    bool isOpen() const;
    

protected:
    // This overrides the CommandProcessor's version to read from a file instead
    string readCommand() override;

private:
    FileLineReader* flr;
};