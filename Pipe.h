#ifndef PIPE_H
#define PIPE_H

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

class PipeManager;
class GasTransportNetwork;

class Pipe {
private:
    string full_name; 
    double length = 0; 
    double diameter = 0;
    bool underRepair;
public:
    Pipe() : diameter(0) {} 
    Pipe(int diameter) : diameter(diameter) {}
    int getDiameter() const { return diameter; }
    void read();
    void display() const;
    void load(const string &data);
    friend class PipeManager;
    friend class GasTransportNetwork;
};

#endif