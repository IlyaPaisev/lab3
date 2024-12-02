#ifndef COMPRESSORSTATION_H
#define COMPRESSORSTATION_H

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

class CompressorStationManager;

class CompressorStation {
public:
    string full_name; 
    int totalShops = 0; 
    int operationalShops = 0; 
    double unusedOperationalShops; 
    double efficiency; 

    void read();
    void display() const;
    void load(const string &data);
    friend class CompressorStationManager;
};

#endif 