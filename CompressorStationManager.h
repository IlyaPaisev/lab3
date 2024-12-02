#ifndef COMPRESSOR_STATION_MANAGER_H
#define COMPRESSOR_STATION_MANAGER_H

#include <string>
#include <iostream>
#include <map>
#include "CompressorStation.h" 
#include "GasTransportNetwork.h"
#include "utils.h"


class CompressorStationManager {
private:
    int nextIdKS = 1;
    map<int, Connection>& connections;

public:
    CompressorStationManager(map<int, Connection>& connections) : connections(connections) {}
    std::string saveCS(const CompressorStation &station) const;
    void toggleShop(CompressorStation &station, int id);
    void addStation();
    void displayStations() const;
    void removeStation();
    void editStation();
    void searchStationName();
    void searchStationUnused();
    void searchStationMenu();
    void searchStationSwitch();
    void displayCompressorStationMenu();
    void CompressorStationSwitch();
};

#endif 