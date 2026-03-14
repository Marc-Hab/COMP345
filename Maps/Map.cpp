#include "Map.h"
#include "../Players/Player.h"
#include <iostream>
#include <fstream>
#include <sstream>


// ========== Territory Implementation ==========

/**
 * Parameterized constructor for Territory
 */
Territory::Territory(const std::string& territoryName) {
    name = new std::string(territoryName);
    continent = nullptr;
    owner = nullptr;
    armyCount = new int(0);
    adjacentTerritories = new std::vector<Territory*>();
}

/**
 * Copy constructor for Territory (shallow copy)
 */
Territory::Territory(const Territory& other) {
    name = new std::string(*(other.name));
    continent = other.continent; // Shallow copy of pointer
    owner = other.owner;         // Shallow copy of pointer
    armyCount = new int(*(other.armyCount));
    adjacentTerritories = new std::vector<Territory*>(*(other.adjacentTerritories));
}

/**
 * Destructor for Territory
 */
Territory::~Territory() {
    delete name;
    delete armyCount;
    delete adjacentTerritories;
}


/**
 * Assignment operator for Territory
 */
Territory& Territory::operator=(const Territory& other) {
    if (this != &other) {
        delete name;
        delete armyCount;
        delete adjacentTerritories;

        name = new std::string(*(other.name));
        continent = other.continent;
        owner = other.owner;
        armyCount = new int(*(other.armyCount));
        adjacentTerritories = new std::vector<Territory*>(*(other.adjacentTerritories));
    }
    return *this;
}

/**
 * Stream insertion operator for Territory (JSON format)
 */
std::ostream& operator<<(std::ostream& out, const Territory& territory) {
    out << "{" << "\n";

    // name
    out << "  " << "\"name\": " << "\"" << *(territory.name) << "\"" << ",\n";

    // continent (name only)
    out << "  " << "\"continent\": ";

    if (territory.continent) {
        out << "\"" << territory.continent->getName()  << "\"" << ",\n";
    } else {
        out << "null" << ",\n";
    }

    // owner (name only)
    out << "  " << "\"owner\": ";
    if (territory.owner) {
        out << territory.owner->getName() << ",\n";   // assumes Player << outputs JSON
    } else {
        out << "null" << ",\n";
    }

    // army count
    out << "  " << "\"armies\": " << *(territory.armyCount) << ",\n";

    // adjacent territories (names only)
    out << "  " << "\"adjacentTerritories\": [";

    if (territory.adjacentTerritories) {
        for (size_t i = 0; i < territory.adjacentTerritories->size(); ++i) {
            const Territory* adj = territory.adjacentTerritories->at(i);
            out << "\"" << *(adj->name) << "\"";

            if (i != territory.adjacentTerritories->size() - 1) {
                out << ", ";
            }
        }
    }

    out << "]\n";

    out << "}\n";

    return out;
}


// Getters and Setters implementation 
std::string Territory::getName() const {
    return *name;
}

void Territory::setName(const std::string& territoryName) {
    *name = territoryName;
}

Continent* Territory::getContinent() const {
    return continent;
}

void Territory::setContinent(Continent* cont) {
    continent = cont;
}

Player* Territory::getOwner() const {
    return owner;
}

void Territory::setOwner(Player* own) {
    owner = own;
}

int Territory::getArmyCount() const {
    return *armyCount;
}

void Territory::setArmyCount(int count) {
    *armyCount = count;
}

std::vector<Territory*>* Territory::getAdjacentTerritories() const {
    return adjacentTerritories;
}

void Territory::addAdjacentTerritory(Territory* territory) {
    if (!isAdjacentTo(territory)) {
        adjacentTerritories->push_back(territory);
    }
}

bool Territory::isAdjacentTo(const Territory* territory) const {
    for (const Territory* t : *adjacentTerritories) {
        if (t == territory) {
            return true;
        }
    }
    return false;
}

// ========== End of Territory Implementation ==========

// ========== Continent Implementation ==========

/**
 * Parametrized Constructor#1 for Continent
 */
Continent::Continent(const std::string& continentName) {
    name = new std::string(continentName);
    bonusValue = new int(0);
    territories = new std::vector<Territory*>();
}

/**
 * Parameterized Constructor#2 for Continent
 */
Continent::Continent(const std::string& continentName, int bonus) {
    name = new std::string(continentName);
    bonusValue = new int(bonus);
    territories = new std::vector<Territory*>();
}

/**
 * Copy constructor for Continent (shallow copy)
 */
Continent::Continent(const Continent& other) {
    name = new std::string(*(other.name));
    bonusValue = new int(*(other.bonusValue));
    territories = new std::vector<Territory*>(*(other.territories));
}

/**
 * Destructor for Continent
 */
Continent::~Continent() {
    delete name;
    delete bonusValue;
    delete territories;
}

/**
 * Assignment operator for Continent
 */
Continent& Continent::operator=(const Continent& other) {
    if (this != &other) {
        delete name;
        delete bonusValue;
        delete territories;

        name = new std::string(*(other.name));
        bonusValue = new int(*(other.bonusValue));
        territories = new std::vector<Territory*>(*(other.territories));
    }
    return *this;
}

/**
 * Stream insertion operator for Continent (JSON format)
 */
std::ostream& operator<<(std::ostream& out, const Continent& continent) {
    out << "{\n";

    // name
    out << "  " << "\"name\": "
        << "\"" << *(continent.name) << "\"" << ",\n";

    // bonus
    out << "  " << "\"bonus\": "
        << *(continent.bonusValue) << ",\n";

    // territories (names only)
    out << "  " << "\"territories\": [";

    if (continent.territories) {
        for (size_t i = 0; i < continent.territories->size(); ++i) {
            const Territory* territory = continent.territories->at(i);
            out << "\"" << territory->getName() << "\"";

            if (i != continent.territories->size() - 1) {
                out << ", ";
            }
        }
    }

    out << "]\n";

    out << "}\n";

    return out;
}

std::string Continent::getName() const {
    return *name;
}

void Continent::setName(const std::string& continentName) {
    *name = continentName;
}

int Continent::getBonusValue() const {
    return *bonusValue;
}

void Continent::setBonusValue(int bonus) {
    *bonusValue = bonus;
}

std::vector<Territory*>* Continent::getTerritories() const {
    return territories;
}

void Continent::addTerritory(Territory* territory) {
    territories->push_back(territory);
}

// ========== End of Continent Implementation ==========

// ========== Map Implementation ==========

/**
 * Default constructor for Map
 */
Map::Map() {
    territories = new std::vector<Territory>();
    continents = new std::vector<Continent>();
}

/**
 * Copy constructor for Map (shallow copy)
 */
Map::Map(const Map& other) {
    territories = new std::vector<Territory>(*(other.territories));
    continents = new std::vector<Continent>(*(other.continents));
}

/**
 * Destructor for Map
 */
Map::~Map() {
    delete territories;
    delete continents;
}

/**
 * Assignment operator for Map (shallow copy)
 */
Map& Map::operator=(const Map& other) {
    if (this != &other) {
        
        delete territories;
        delete continents;

        territories = new std::vector<Territory>(*(other.territories));
        continents = new std::vector<Continent>(*(other.continents));
    }

    return *this;
}

/**
 * Stream insertion operator for Map (JSON format)
 */
std::ostream& operator<<(std::ostream& out, const Map& map) {
    out << "{\n";

    // continents
    out << "  " << "\"continents\": [";

    if (map.continents) {
        for (size_t i = 0; i < map.continents->size(); ++i) {
            const Continent& continent = map.continents->at(i);
            out << "\"" << continent.getName() << "\"";

            if (i != map.continents->size() - 1) {
                out << ", ";
            }
        }
    }

    out << "],\n";

    // territories
    out << "  " << "\"territories\": [";

    if (map.territories) {
        for (size_t i = 0; i < map.territories->size(); ++i) {
            const Territory& territory = map.territories->at(i);
            out << "\"" << territory.getName() << "\"";

            if (i != map.territories->size() - 1) {
                out << ", ";
            }
        }
    }

    out << "]\n";

    out << "}\n";

    return out;
}

std::vector<Territory>* Map::getTerritories() const {
    return territories;
}

std::vector<Continent>* Map::getContinents() const {
    return continents;
}

void Map::addTerritory(Territory& territory) {
    
    territories->push_back(std::move(territory));
}

void Map::addContinent(Continent& continent) {
    
    continents->push_back(std::move(continent));
}

/**
 * DFS helper method to visit all reachable territories
 */
void Map::dfsVisit(const Territory* territory, std::vector<bool>& visited, const std::vector<Territory*>& territoryList) const {

    // Find the index of current territory
    int index = -1;
    for (size_t i = 0; i < territoryList.size(); i++) {
        if (territoryList[i] == territory) {
            index = i;
            break;
        }
    }

    // Skip territory if not in the list or already visited
    if (index == -1 || visited[index]) {
        return;
    }

    visited[index] = true;

    // Visit all adjacent territories
    for (const Territory* adj : *(territory->getAdjacentTerritories())) {
        dfsVisit(adj, visited, territoryList);
    }
}

/**
 * Check if the map is a connected graph
 */
bool Map::isConnectedGraph() const {

    // Edge case
    if (territories->empty()) {
        return true;
    }

    // Create a vector of pointers to territories in the map
    std::vector<Territory*> territoriesRefs;
    for (Territory& territory : *territories){
        territoriesRefs.push_back(&territory);
    }

    std::vector<bool> visited(territories->size(), false);
    dfsVisit(&(territories->at(0)), visited, territoriesRefs);

    // Check if all territories were visited
    for (bool v : visited) {
        if (!v) {
            return false;
        }
    }
    return true;
}

/**
 * Check if all continents are connected subgraphs
 */
bool Map::areContinentsConnectedSubgraphs() const {
    for (const Continent& continent : *continents) {
        std::vector<Territory*>* contTerritories = continent.getTerritories();
        
        // Edge Case
        if (contTerritories->empty()) {
            continue;
        }

        std::vector<bool> visited(contTerritories->size(), false);
        dfsVisit((*contTerritories)[0], visited, *contTerritories);

        // Check if all territories in continent were visited
        for (bool v : visited) {
            if (!v) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Check if each territory belongs to exactly one continent
 */
bool Map::doesEachTerritoryBelongToOneContinent() const {
    for (const Territory& territory : *territories) {
        
        if (territory.getContinent() == nullptr) {
            return false;
        }

        int count = 0;
        for (const Continent& continent : *continents) {
            for (const Territory* t : *(continent.getTerritories()) ) {
                if (t == &territory) {
                    count++;
                }
            }
        }

        if (count != 1) {
            return false;
        }
    }
    return true;
}

/**
 * Validate the map
 */
bool Map::validate() const {
    std::cout << "Validating map..." << std::endl;
    
    bool connected = isConnectedGraph();
    std::cout << "  Map is connected graph: " << (connected ? "YES" : "NO") << std::endl;
    
    bool continentsConnected = areContinentsConnectedSubgraphs();
    std::cout << "  Continents are connected subgraphs: " << (continentsConnected ? "YES" : "NO") << std::endl;
    
    bool uniqueContinent = doesEachTerritoryBelongToOneContinent();
    std::cout << "  Each territory belongs to one continent: " << (uniqueContinent ? "YES" : "NO") << std::endl;

    bool validMap = connected && continentsConnected && uniqueContinent;
    std::cout << "  Map is valid: " << (validMap ? "YES" : "NO") << std::endl;
    
    return validMap;
}

// ========== End of Map Implementation ==========

// ========== MapLoader Implementation ==========

/**
 * Stream insertion operator for MapLoader
 */
std::ostream& operator<<(std::ostream& out, const MapLoader& loader) {
    out << "MapLoader instance loads map files and creates Map objects.";
    return out;
}

/** 
 * Count the number of continents and countries in a map file 
 */
std::tuple<int, int> MapLoader::countContinentsAndCountries(const std::string& filePath){
    int continentsCount = 0;
    int countriesCount = 0;

    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filePath << std::endl;
        return {0, 0};
    }

    enum class Section {
        CONTINENTS,
        COUNTRIES,
        BORDERS,
        OTHER
    };

    std::string line;
    Section section = Section::OTHER;

    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip empty lines and comments
        if (line.empty() || line[0] == ';') {
            continue;
        }

        // Check for section headers
        if (line == "[continents]") {
            section = Section::CONTINENTS;
            continue;
        } else if (line == "[countries]" || line == "[territories]") {
            section = Section::COUNTRIES;
            continue;
        } else if (line == "[borders]") {
            section = Section::BORDERS;
            continue;
        } else if (line.front() == '[' && line.back() == ']') {
            section = Section::OTHER;
            continue;
        }

        // Increment counters
        if (section == Section::CONTINENTS){
            ++continentsCount;
        } else if (section == Section::COUNTRIES) {
            ++countriesCount;
        }


    }

    file.close();

    return {continentsCount, countriesCount};
}


/**
 * Load a map from file
 */
Map* MapLoader::loadMap(const std::string& filePath) {
    
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filePath << std::endl;
        return nullptr;
    }

    Map* map = new Map();

    // Count the exact number of continents and countries
    std::tuple<int, int> continentsAndCountriesCounts = countContinentsAndCountries(filePath);

    int continentCount = std::get<0>(continentsAndCountriesCounts);
    int territoryCount = std::get<1>(continentsAndCountriesCounts);

    // Reserve the exact amount needed to prevent reallocation
    map->getContinents()->reserve(continentCount);
    map->getTerritories()->reserve(territoryCount);


    enum class Section {
        CONTINENTS,
        COUNTRIES,
        BORDERS,
        OTHER
    };

    std::string line;
    Section section = Section::OTHER;

    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip empty lines and comments
        if (line.empty() || line[0] == ';') {
            continue;
        }

        // Check for section headers
        if (line == "[continents]") {
            section = Section::CONTINENTS;
            continue;
        } else if (line == "[countries]" || line == "[territories]") {
            section = Section::COUNTRIES;
            continue;
        } else if (line == "[borders]") {
            section = Section::BORDERS;
            continue;
        } else if (line.front() == '[' && line.back() == ']') {
            section = Section::OTHER;
            continue;
        }

        // Process based on current section
        if (section == Section::OTHER) {
            continue;
        } else if (section == Section::CONTINENTS) {
            std::istringstream iss(line);
            std::string name;
            int bonus;
            iss >> name >> bonus;
            
            Continent cont(name, bonus);
            map->addContinent(cont);

        } else if (section == Section::COUNTRIES) {
            std::istringstream iss(line);
            int id;
            std::string name;
            int continentId;
            iss >> id >> name >> continentId;
            
            Territory territory(name);
            
            // Set continent BEFORE adding to map (continentId is 1-based)
            if (continentId > 0 && continentId <= (int)map->getContinents()->size()) {
                Continent* cont = &(map->getContinents()->at(continentId - 1));
                territory.setContinent(cont);
            }
            
            // Add territory to map
            map->addTerritory(territory);
            
            // Now add the territory IN THE MAP to the continent's list
            if (continentId > 0 && continentId <= (int)map->getContinents()->size()) {
                Continent* cont = &(map->getContinents()->at(continentId - 1));
                Territory* territoryInMap = &(map->getTerritories()->at(map->getTerritories()->size() - 1));
                cont->addTerritory(territoryInMap);
            }

        } else if (section == Section::BORDERS) {
            std::istringstream iss(line);
            int territoryId;
            iss >> territoryId;
            
            // Get the territory based on the id 
            if (territoryId > 0 && territoryId <= (int)map->getTerritories()->size()) {
                Territory* territory = &(map->getTerritories()->at(territoryId - 1));
                
                // Add all adjacent territories based on their ids
                int adjacentId;
                while (iss >> adjacentId) {
                    if (adjacentId > 0 && adjacentId <= (int)map->getTerritories()->size()) {
                        Territory* adjacent = &(map->getTerritories()->at(adjacentId - 1));
                        territory->addAdjacentTerritory(adjacent);
                    }
                }
            }
        }
    }

    file.close();
    return map;
}