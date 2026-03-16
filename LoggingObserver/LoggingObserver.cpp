#include "LoggingObserver.h"
#include <algorithm>
#include <iostream>

// --- Subject ---
Subject::Subject() {
    observers = new std::vector<Observer*>();
}

Subject::Subject(const Subject& other) {
    observers = new std::vector<Observer*>();
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

void Subject::notify() {
    for (Observer* obs : *observers) {
        obs->update(dynamic_cast<ILoggable*>(this));
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
