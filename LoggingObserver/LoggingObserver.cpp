#include "LoggingObserver.h"
#include <algorithm>
#include <iostream>

// --- Subject ---

// Initialize the static global observers list
std::vector<Observer*>* Subject::globalObservers = new std::vector<Observer*>();

Subject::Subject() {
    observers = new std::vector<Observer*>();
}

Subject::Subject(const Subject& other) {
    observers = new std::vector<Observer*>();
    // global observers are not copied, because they are shared
}

Subject::~Subject() {
    delete observers;
}

Subject& Subject::operator=(const Subject& other) {
    return *this;
}

void Subject::attach(Observer* observer) {
    observers->push_back(observer);
}

void Subject::detach(Observer* observer) {
    auto it = std::find(observers->begin(), observers->end(), observer);
    if (it != observers->end()) {
        observers->erase(it);
    }
}

void Subject::attachGlobal(Observer* observer) {
    globalObservers->push_back(observer);
}

void Subject::detachGlobal(Observer* observer) {
    auto it = std::find(globalObservers->begin(), globalObservers->end(), observer);
    if (it != globalObservers->end()) {
        globalObservers->erase(it);
    }
}

void Subject::notify() {
    ILoggable* loggable = dynamic_cast<ILoggable*>(this);
    for (Observer* obs : *observers) {
        obs->update(loggable);
    }
    for (Observer* obs : *globalObservers) {
        obs->update(loggable);
    }
}

// --- LogObserver ---
LogObserver::LogObserver() {
    logFile = new std::ofstream("gamelog.txt", std::ios::app);
    if (!logFile->is_open()) {
        std::cerr << "LogObserver: Could not open gamelog.txt for writing." << std::endl;
    }
}

LogObserver::~LogObserver() {
    if (logFile && logFile->is_open()) {
        logFile->close();
    }
    delete logFile;
}

void LogObserver::update(ILoggable* subject) {
    if (subject && logFile && logFile->is_open()) {
        *logFile << subject->stringToLog() << std::endl;
    }
}
