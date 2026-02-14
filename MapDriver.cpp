#include "Map.h"
#include <iostream>

using namespace std;

/**
 * Creates a simple valid map programmatically
 */
Map* createValidMap() {
    Map* map = new Map();
    
    // Create continents
    Continent northAmerica = Continent("North America", 5);
    Continent europe = Continent("Europe", 3);
    
    // Create territories
    Territory canada    = Territory("Canada");
    Territory usa       = Territory("USA");
    Territory mexico    = Territory("Mexico");
    Territory uk        = Territory("UK");
    Territory france    = Territory("France");
    Territory germany   = Territory("Germany");
    
    // Set continents
    canada.setContinent(&northAmerica);
    usa.setContinent(&northAmerica);
    mexico.setContinent(&northAmerica);
    uk.setContinent(&europe);
    france.setContinent(&europe);
    germany.setContinent(&europe);

    
    // Add territories to continents
    northAmerica.addTerritory(&canada);
    northAmerica.addTerritory(&usa);
    northAmerica.addTerritory(&mexico);
    europe.addTerritory(&uk);
    europe.addTerritory(&france);
    europe.addTerritory(&germany);
    
    // Create adjacencies within North America (connected subgraph)
    canada.addAdjacentTerritory(&usa);
    usa.addAdjacentTerritory(&canada);
    usa.addAdjacentTerritory(&mexico);
    mexico.addAdjacentTerritory(&usa);
    
    // Create adjacencies within Europe (connected subgraph)
    uk.addAdjacentTerritory(&france);
    france.addAdjacentTerritory(&uk);
    france.addAdjacentTerritory(&germany);
    germany.addAdjacentTerritory(&france);
    
    // Connect continents to make the whole map connected
    uk.addAdjacentTerritory(&canada);
    canada.addAdjacentTerritory(&uk);

    // Add territories to map
    map->addTerritory(canada);  // Index 0
    map->addTerritory(usa);     // Index 1
    map->addTerritory(mexico);  // Index 2
    map->addTerritory(uk);      // Index 3
    map->addTerritory(france);  // Index 4
    map->addTerritory(germany); // Index 5

    map->addContinent(northAmerica); // Index 0
    map->addContinent(europe);       // Index 1
    
    return map;
}

/**
 * Creates an invalid map (disconnected graph)
 */
Map* createInvalidMapDisconnected() {
    Map* map = new Map();
    
    Continent continent1 = Continent("Continent1", 5);
    Continent continent2 = Continent("Continent2", 3);
    
    map->addContinent(continent1);
    map->addContinent(continent2);
    
    Territory t1 = Territory("T1");
    Territory t2 = Territory("T2");
    Territory t3 = Territory("T3");
    Territory t4 = Territory("T4");

    map->addTerritory(t1);
    map->addTerritory(t2);
    map->addTerritory(t3);
    map->addTerritory(t4);
    
    t1.setContinent(&continent1);
    t2.setContinent(&continent1);
    t3.setContinent(&continent2);
    t4.setContinent(&continent2);
    
    continent1.addTerritory(&t1);
    continent1.addTerritory(&t2);
    continent2.addTerritory(&t3);
    continent2.addTerritory(&t4);
    
    // Connect within continents but NOT between them
    t1.addAdjacentTerritory(&t2);
    t2.addAdjacentTerritory(&t1);
    t3.addAdjacentTerritory(&t4);
    t4.addAdjacentTerritory(&t3);
    
    return map;
}

/**
 * Creates an invalid map (continent not connected)
 */
Map* createInvalidMapContinentDisconnected() {
    Map* map = new Map();
    
    Continent continent = Continent("Continent", 5);
    map->addContinent(continent);
    
    Territory t1 = Territory("T1");
    Territory t2 = Territory("T2");
    Territory t3 = Territory("T3");

    map->addTerritory(t1);
    map->addTerritory(t2);
    map->addTerritory(t3);
    
    t1.setContinent(&continent);
    t2.setContinent(&continent);
    t3.setContinent(&continent);
    
    continent.addTerritory(&t1);
    continent.addTerritory(&t2);
    continent.addTerritory(&t3);
    
    // T1 and T2 connected, but T3 isolated within the continent
    t1.addAdjacentTerritory(&t2);
    t2.addAdjacentTerritory(&t1);
    // T3 has no adjacencies within its continent
    
    return map;
}

/**
 * Creates an invalid map (territory belongs to multiple continents)
 */
Map* createInvalidMapMultipleContinents() {
    Map* map = new Map();
    
    Continent continent1 = Continent("Continent1", 5);
    Continent continent2 = Continent("Continent2", 3);
    
    map->addContinent(continent1);
    map->addContinent(continent2);
    
    Territory t1 = Territory("T1");
    Territory t2 = Territory("T2");

    map->addTerritory(t1);
    map->addTerritory(t2);
    
    // Territory belongs to both continents (invalid)
    t1.setContinent(&continent1);
    continent1.addTerritory(&t1);
    continent2.addTerritory(&t1); // Added to second continent too
    
    t2.setContinent(&continent2);
    continent2.addTerritory(&t2);
    
    t1.addAdjacentTerritory(&t2);
    t2.addAdjacentTerritory(&t1);
    
    return map;
}

int main() {
    cout << "===== WARZONE MAP DRIVER =====" << endl << endl;
    
    // Test 1: Valid map
    cout << "TEST 1: Creating and validating a VALID map" << endl;
    cout << "--------------------------------------------" << endl;
    Map* validMap = createValidMap();
    for (const Continent& c : *validMap->getContinents()){
        cout << c << endl;
    }
    for (const Territory& t : *validMap->getTerritories()){
        cout << t << endl;
    }
    cout << *validMap << endl;
    bool isValid = validMap->validate();
    cout << "Overall validation result: " << (isValid ? "VALID" : "INVALID") << endl;
    cout << endl;
    
    // Test 2: Invalid map (disconnected)
    cout << "TEST 2: Creating and validating an INVALID map (disconnected graph)" << endl;
    cout << "-------------------------------------------------------------------" << endl;
    Map* invalidMap1 = createInvalidMapDisconnected();
    cout << *invalidMap1 << endl;
    bool isValid1 = invalidMap1->validate();
    cout << "Overall validation result: " << (isValid1 ? "VALID" : "INVALID") << endl;
    cout << endl;
    
    // Test 3: Invalid map (continent not connected)
    cout << "TEST 3: Creating and validating an INVALID map (continent not connected)" << endl;
    cout << "-------------------------------------------------------------------------" << endl;
    Map* invalidMap2 = createInvalidMapContinentDisconnected();
    cout << *invalidMap2 << endl;
    bool isValid2 = invalidMap2->validate();
    cout << "Overall validation result: " << (isValid2 ? "VALID" : "INVALID") << endl;
    cout << endl;
    
    // Test 4: Invalid map (territory in multiple continents)
    cout << "TEST 4: Creating and validating an INVALID map (territory in multiple continents)" << endl;
    cout << "----------------------------------------------------------------------------------" << endl;
    Map* invalidMap3 = createInvalidMapMultipleContinents();
    cout << *invalidMap3 << endl;
    bool isValid3 = invalidMap3->validate();
    cout << "Overall validation result: " << (isValid3 ? "VALID" : "INVALID") << endl;
    cout << endl;
    
    // Test 5: MapLoader with file (would fail if file doesn't exist)
    cout << "TEST 5: Testing MapLoader" << endl;
    cout << "-------------------------" << endl;
    MapLoader& loader = MapLoader::getInstance();
    cout << loader << endl;
    Map* loadedMap = loader.loadMap("test.map");
    if (loadedMap) {
        cout << "Map loaded successfully" << endl;
        loadedMap->validate();
        delete loadedMap;
    } else {
        cout << "Failed to load map (file may not exist)" << endl;
    }
    cout << endl;
    
    // Test 6: Copy constructor and assignment operator
    cout << "TEST 6: Testing copy constructor and assignment operator" << endl;
    cout << "---------------------------------------------------------" << endl;
    Map* copiedMap = new Map(*validMap);
    cout << "Copied map: " << *copiedMap << endl;
    
    Map* assignedMap = new Map();
    *assignedMap = *validMap;
    cout << "Assigned map: " << *assignedMap << endl;
    cout << endl;
    
    // Cleanup
    delete validMap;
    delete invalidMap1;
    delete invalidMap2;
    delete invalidMap3;
    delete copiedMap;
    delete assignedMap;
    
    cout << "===== MAP DRIVER COMPLETE =====" << endl;
    
    return 0;
}