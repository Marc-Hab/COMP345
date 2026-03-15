#pragma once
#include <string>
#include <vector>
#include <fstream>

// Forward declaration
class ILoggable;

// Abstract Observer base class
class Observer {
public:
    virtual ~Observer() {}
    virtual void update(ILoggable* subject) = 0;
};

// ILoggable interface: all loggable subjects must implement stringToLog()
class ILoggable {
public:
    virtual ~ILoggable() {}
    virtual std::string stringToLog() const = 0;
};

// Subject base class: manages a list of observers and notifies them
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

// Concrete observer: writes log entries to gamelog.txt
class LogObserver : public Observer {
public:
    LogObserver();
    ~LogObserver();
    void update(ILoggable* subject) override;

private:
    std::ofstream* logFile;
};
