#ifndef GAS_TRANSPORT_NETWORK_H
#define GAS_TRANSPORT_NETWORK_H

#include "Pipe.h"
#include "CompressorStation.h"
#include <map>
#include <set>
#include <stack>
#include <vector>


class PipeManager;
class CompressorStationManager;

struct Connection {
    int entryStationId;
    int exitStationId;
    int pipeId;
    int diameter;
    int length;

    Connection() = default; 
    Connection(int entry, int exit, int pipe, int dia, int len)
        : entryStationId(entry), exitStationId(exit), pipeId(pipe), diameter(dia), length(len) {}
};

class GasTransportNetwork {
private:
    void dfs(int node, map<int, vector<int>>& graph, set<int>& visited, set<int>& recursionStack, stack<int>& Stack, bool& hasCycle);
    void findConnectedComponents(map<int, Connection>& connections, vector<vector<int>>& components);

public:
    int inputDiameter();
    void connectStations(std::map<int, Pipe>& pipes, std::map<int, CompressorStation>& stations, PipeManager& pipeManager);
    void viewConnections() const; 
    void topologicalSort(const map<int, Connection>& connections);
    void removeConnection();
    void addConnection(int entryId, int exitId, int pipeId, int diameter, int length);
    std::string saveConnections() const;
    
};

#endif // GAS_TRANSPORT_NETWORK_H