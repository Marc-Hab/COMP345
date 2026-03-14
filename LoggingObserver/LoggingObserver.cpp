#include "LoggingObserver.h"
#include <algorithm>
#include <fstream>

// Subject

Subject::Subject() {
    observers = new std::vector<Observer*>();
}

Subject::Subject(const Subject& other) {
    observers = new std::vector<Observer*>();
}

Subject& Subject::operator=(const Subject& other) {
    return *this;
}

Subject::~Subject() {
    delete observers;
}

void Subject::attach(Observer* o) {
    observers->push_back(o);
}

void Subject::detach(Observer* o) {
    auto it = std::find(observers->begin(), observers->end(), o);
    if (it != observers->end()) {
        observers->erase(it);
    }
}

void Subject::notify(ILoggable& loggable) {
    for (Observer* obs : *observers) {
        obs->update(loggable);
    }
}

std::ostream& operator<<(std::ostream& os, const Subject& s) {
    os << "Subject[observers=" << s.observers->size() << "]";
    return os;
}

// Observer

std::ostream& operator<<(std::ostream& os, const Observer& o) {
    os << "Observer";
    return os;
}

// LogObserver

LogObserver::LogObserver(const std::string& filename) {
    logFileName = new std::string(filename);
}

LogObserver::LogObserver(const LogObserver& other) {
    logFileName = new std::string(*other.logFileName);
}

LogObserver& LogObserver::operator=(const LogObserver& other) {
    if (this != &other) {
        *logFileName = *other.logFileName;
    }
    return *this;
}

LogObserver::~LogObserver() {
    delete logFileName;
}

// Appends the log string produced by the subject to the log file.
void LogObserver::update(ILoggable& subject) {
    std::ofstream logFile(*logFileName, std::ios::app);
    if (logFile.is_open()) {
        logFile << subject.stringToLog() << "\n";
    }
}

std::ostream& operator<<(std::ostream& os, const LogObserver& lo) {
    os << "LogObserver[file=" << *lo.logFileName << "]";
    return os;
}
