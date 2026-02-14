#include "Map.h"
#include <iostream>

using namespace std;

/**
 * Creates a simple valid map programmatically
 */
Map* createValidMap() {
    Map* map = new Map();
    
    // Create continents and add to map
    Continent northAmerica = Continent("North America", 5);
    Continent europe = Continent("Europe", 3);
    
    map->addContinent(northAmerica);
    map->addContinent(europe);

    // Get pointers to the continents IN THE MAP (not the local variables)
    Continent* pNorthAmerica = &(map->getContinents()->at(0));
    Continent* pEurope = &(map->getContinents()->at(1));
    
    // Create territories
    Territory canada = Territory("Canada");
    Territory usa = Territory("USA");
    Territory mexico = Territory("Mexico");
    Territory uk = Territory("UK");
    Territory france = Territory("France");
    Territory germany = Territory("Germany");

    // Set continents BEFORE adding to map
    canada.setContinent(pNorthAmerica);
    usa.setContinent(pNorthAmerica);
    mexico.setContinent(pNorthAmerica);
    uk.setContinent(pEurope);
    france.setContinent(pEurope);
    germany.setContinent(pEurope);

    // Add territories to map (this copies them)
    map->addTerritory(canada);
    map->addTerritory(usa);
    map->addTerritory(mexico);
    map->addTerritory(uk);
    map->addTerritory(france);
    map->addTerritory(germany);
    
    // NOW get pointers to the territories IN THE MAP
    Territory* pCanada = &(map->getTerritories()->at(0));
    Territory* pUsa = &(map->getTerritories()->at(1));
    Territory* pMexico = &(map->getTerritories()->at(2));
    Territory* pUk = &(map->getTerritories()->at(3));
    Territory* pFrance = &(map->getTerritories()->at(4));
    Territory* pGermany = &(map->getTerritories()->at(5));
    
    // Add territories to continents
    pNorthAmerica->addTerritory(pCanada);
    pNorthAmerica->addTerritory(pUsa);
    pNorthAmerica->addTerritory(pMexico);
    pEurope->addTerritory(pUk);
    pEurope->addTerritory(pFrance);
    pEurope->addTerritory(pGermany);
    
    // Create adjacencies within North America (connected subgraph)
    pCanada->addAdjacentTerritory(pUsa);
    pUsa->addAdjacentTerritory(pCanada);
    pUsa->addAdjacentTerritory(pMexico);
    pMexico->addAdjacentTerritory(pUsa);
    
    // Create adjacencies within Europe (connected subgraph)
    pUk->addAdjacentTerritory(pFrance);
    pFrance->addAdjacentTerritory(pUk);
    pFrance->addAdjacentTerritory(pGermany);
    pGermany->addAdjacentTerritory(pFrance);
    
    // Connect continents to make the whole map connected
    pUk->addAdjacentTerritory(pCanada);
    pCanada->addAdjacentTerritory(pUk);
    
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
    
    Continent* pCont1 = &(map->getContinents()->at(0));
    Continent* pCont2 = &(map->getContinents()->at(1));
    
    Territory t1 = Territory("T1");
    Territory t2 = Territory("T2");
    Territory t3 = Territory("T3");
    Territory t4 = Territory("T4");

    t1.setContinent(pCont1);
    t2.setContinent(pCont1);
    t3.setContinent(pCont2);
    t4.setContinent(pCont2);

    map->addTerritory(t1);
    map->addTerritory(t2);
    map->addTerritory(t3);
    map->addTerritory(t4);
    
    Territory* pT1 = &(map->getTerritories()->at(0));
    Territory* pT2 = &(map->getTerritories()->at(1));
    Territory* pT3 = &(map->getTerritories()->at(2));
    Territory* pT4 = &(map->getTerritories()->at(3));
    
    pCont1->addTerritory(pT1);
    pCont1->addTerritory(pT2);
    pCont2->addTerritory(pT3);
    pCont2->addTerritory(pT4);
    
    // Connect within continents but NOT between them
    pT1->addAdjacentTerritory(pT2);
    pT2->addAdjacentTerritory(pT1);
    pT3->addAdjacentTerritory(pT4);
    pT4->addAdjacentTerritory(pT3);
    
    return map;
}

/**
 * Creates an invalid map (continent not connected)
 */
Map* createInvalidMapContinentDisconnected() {
    Map* map = new Map();
    
    Continent continent = Continent("Continent", 5);
    map->addContinent(continent);
    
    Continent* pCont = &(map->getContinents()->at(0));
    
    Territory t1 = Territory("T1");
    Territory t2 = Territory("T2");
    Territory t3 = Territory("T3");

    t1.setContinent(pCont);
    t2.setContinent(pCont);
    t3.setContinent(pCont);

    map->addTerritory(t1);
    map->addTerritory(t2);
    map->addTerritory(t3);
    
    Territory* pT1 = &(map->getTerritories()->at(0));
    Territory* pT2 = &(map->getTerritories()->at(1));
    Territory* pT3 = &(map->getTerritories()->at(2));
    
    pCont->addTerritory(pT1);
    pCont->addTerritory(pT2);
    pCont->addTerritory(pT3);
    
    // T1 and T2 connected, but T3 isolated within the continent
    pT1->addAdjacentTerritory(pT2);
    pT2->addAdjacentTerritory(pT1);
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
    
    Continent* pCont1 = &(map->getContinents()->at(0));
    Continent* pCont2 = &(map->getContinents()->at(1));
    
    Territory t1 = Territory("T1");
    Territory t2 = Territory("T2");

    t1.setContinent(pCont1);
    t2.setContinent(pCont2);

    map->addTerritory(t1);
    map->addTerritory(t2);
    
    Territory* pT1 = &(map->getTerritories()->at(0));
    Territory* pT2 = &(map->getTerritories()->at(1));
    
    // Territory belongs to both continents (invalid)
    pCont1->addTerritory(pT1);
    pCont2->addTerritory(pT1); // Added to second continent too
    pCont2->addTerritory(pT2);
    
    pT1->addAdjacentTerritory(pT2);
    pT2->addAdjacentTerritory(pT1);
    
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