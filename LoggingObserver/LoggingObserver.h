#pragma once
#include <string>
#include <vector>
#include <fstream>

// forward declaration
class ILoggable;

class Observer {
public:
    virtual ~Observer() {}
    virtual void update(ILoggable* subject) = 0;
};

// interface for any class that can be logged
class ILoggable {
public:
    virtual ~ILoggable() {}
    virtual std::string stringToLog() const = 0;
};

class Subject {
public:
    Subject();
    Subject(const Subject& other);
    virtual ~Subject();
    Subject& operator=(const Subject& other);

    void attach(Observer* observer);
    void detach(Observer* observer);
    void notify();

private:
    std::vector<Observer*>* observers;
};

// Writes log entries to gamelog.txt
class LogObserver : public Observer {
public:
    LogObserver();
    ~LogObserver();
    void update(ILoggable* subject) override;

private:
    std::ofstream* logFile;
};
