// LoggingObserver.h

#ifndef LOGGINGOBSERVER_H
#define LOGGINGOBSERVER_H

#include <string>
#include <vector>
#include <iostream>

class Observer;
class ILoggable {
public:
    virtual ~ILoggable() = default;
    virtual std::string stringToLog() const = 0;
};
class Subject {
private:
    std::vector<Observer*>* observers;

public:
    Subject();
    Subject(const Subject& other);
    Subject& operator=(const Subject& other);
    virtual ~Subject();

    void attach(Observer* o);
    void detach(Observer* o);

    void notify(ILoggable& loggable);

    friend std::ostream& operator<<(std::ostream& os, const Subject& s);
};

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(ILoggable& subject) = 0;
    friend std::ostream& operator<<(std::ostream& os, const Observer& o);
};

class LogObserver : public Observer {
private:
    std::string* logFileName;

public:
    explicit LogObserver(const std::string& filename = "gamelog.txt");
    LogObserver(const LogObserver& other);
    LogObserver& operator=(const LogObserver& other);
    ~LogObserver();

    void update(ILoggable& subject) override;

    friend std::ostream& operator<<(std::ostream& os, const LogObserver& lo);
};

#endif
