#ifndef PIPEMANAGER_H
#define PIPEMANAGER_H

#include <iostream>
#include <map>
#include <vector>
#include "Pipe.h"
#include "utils.h"
#include "GasTransportNetwork.h"

using namespace std;


class PipeManager {
private:
    int nextId = 1;
    map<int, Connection>& connections;

public:
    PipeManager(map<int, Connection>& connections) : connections(connections) {}
    int getNextId() const { return nextId; }
    int getLastPipeId();
    void removeConnectionsUsingPipe(int pipeId);
    std::string savePipe(const Pipe &pipe) const;
    void toggleRepair(Pipe &pipe, int id);
    tuple<int, int, int> addPipeGTN();
    void addPipe();
    void editPipes();
    void deletePipes();
    void displayAllPipes();
    void editPipesFound(const vector<int>& foundIds);
    void deletePipesFound(const vector<int>& foundIds);
    void displayPipeMenuFound();
    void handleFoundPipeMenu(const vector<int>& foundPipeIds);
    void searchPipesName();
    void searchPipesRepair();
    void searchPipeMenu();
    void searchPipeSwitch();
    void displayPipeMenu();
    void PipeSwitch();
};

#endif // PIPEMANAGER_H