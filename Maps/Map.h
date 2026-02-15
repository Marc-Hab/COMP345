#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>



#include <iostream>


// Forward declarations
class Player{
public:
    Player(const Player& other);
    std::string getName() const{return"";};
    friend std::ostream& operator<<(std::ostream& out, const Player& territory){out << "{}"; return out;}
};
class Territory;
class Continent;

/**
 * Territory class represents a territory (node) in the map graph.
 * Each territory has a name, belongs to a continent, is owned by a player,
 * and has a number of armies. It also maintains adjacency information.
 */
class Territory{
private:
    std::string* name;
    Continent* continent;
    Player* owner;
    int* armyCount;
    std::vector<Territory*>* adjacentTerritories;

public:
    // Constructors and destructor
    Territory(const std::string& territoryName);
    Territory(const Territory& other);
    ~Territory();

    // Assignment operator
    Territory& operator=(const Territory& other);

    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& out, const Territory& territory);

    // Getters and setters
    std::string getName() const;
    void setName(const std::string& territoryName);
    
    Continent* getContinent() const;
    void setContinent(Continent* cont);
    
    Player* getOwner() const;
    void setOwner(Player* owner);
    
    int getArmyCount() const;
    void setArmyCount(int count);
    
    std::vector<Territory*>* getAdjacentTerritories() const;
    void addAdjacentTerritory(Territory* territory);
    bool isAdjacentTo(const Territory* territory) const;

};

/**
 * Continent class represents a continent (connected subgraph) in the map.
 * Each continent has a name, bonus value, and contains territories.
 */
class Continent{
private:
    std::string* name;
    int* bonusValue;
    std::vector<Territory*>* territories;

public:
    // Constructors and destructor
    Continent(const std::string& continentName);
    Continent(const std::string& continentName, int bonus);
    Continent(const Continent& other);
    ~Continent();

    // Assignment operator
    Continent& operator=(const Continent& other);

    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& out, const Continent& continent);

    // Getters and setters
    std::string getName() const;
    void setName(const std::string& continentName);
    
    int getBonusValue() const;
    void setBonusValue(int bonus);
    
    std::vector<Territory*>* getTerritories() const;
    void addTerritory(Territory* territory);
};

/**
 * Map class represents the game map as a connected graph.
 * The map contains territories and continents with validation methods.
 */
class Map {
private:
    std::vector<Territory>* territories;
    std::vector<Continent>* continents;

    // Helper methods for validation
    bool isConnectedGraph() const;
    bool areContinentsConnectedSubgraphs() const;
    bool doesEachTerritoryBelongToOneContinent() const;
    void dfsVisit(const Territory* territory, std::vector<bool>& visited, 
                  const std::vector<Territory*>& territoryList) const;

public:
    // Constructors and destructor
    Map();
    Map(const Map& other);
    ~Map();

    // Assignment operator
    Map& operator=(const Map& other);

    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& out, const Map& map);

    // Getters
    std::vector<Territory>* getTerritories() const;
    std::vector<Continent>* getContinents() const;

    // Add methods
    void addTerritory(Territory& territory);
    void addContinent(Continent& continent);

    // Validation method
    bool validate() const;
};

/**
 * MapLoader class loads map files and creates Map objects.
 * Implemented as a Singleton
 */
class MapLoader {
private:
    //Private constructor
    MapLoader() {}

    // Helper method for loadMap
    std::tuple<int, int> countContinentsAndCountries(const std::string& filePath);

public:

    static MapLoader& getInstance(){
        static MapLoader instance;
        return instance;
    }

    // Delete copy constructor and assignment
    MapLoader(const MapLoader&) = delete;
    MapLoader& operator=(const MapLoader&) = delete;

    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& out, const MapLoader& loader);

    // Load method
    Map* loadMap(const std::string& filePath);

};

#endif // MAP_H