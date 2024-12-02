#include "GasTransportNetwork.h"
#include "PipeManager.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <queue>

using namespace std;

extern map<int, Connection> connections;
int nextConnectionId = 1;

string GasTransportNetwork::saveConnections() const {
    ostringstream savedConnections; 
    for (const auto& pair : connections) {
        const Connection& connection = pair.second;
        savedConnections <<"connection "<< pair.first << ": " << connection.entryStationId << ";" 
                         << connection.exitStationId << ";" 
                         << connection.pipeId << ";" 
                         << connection.diameter << ";" 
                         << connection.length << "\n"; 
    }
    return savedConnections.str(); 
}

int GasTransportNetwork::inputDiameter() {

    while (true) {
        int diameter = readPositive<int>("Input diameter op pipe (500, 700, 1000, 1400): ", "Incorrect input! Diameter must be more than zero.");
        if (diameter == 500 || diameter == 700 || diameter == 1000 || diameter == 1400) {
            return diameter;
        }
        cout << "Incorrect input! Diameter must be equal 500, 700, 1000 or 1400." << endl;
    }
}

void GasTransportNetwork::connectStations(map<int, Pipe>& pipes, map<int, CompressorStation>& stations, PipeManager& pipeManager) {
    int entryId = readPositive<int>("Input ID CS in: ", "Incorrect input! ID CS must be more than zero.");
    int exitId = readPositive<int>("Input ID CS out: ", "Incorrect input! ID CS must be more than zero.");
    
    if (entryId == exitId) {
        cout << "Incorrect input! ID CS in cant be equal ID CS out." << endl;
        return;
    }
    
    if (stations.find(entryId) == stations.end() || stations.find(exitId) == stations.end()) {
        cout << "One ore two CS not found." << endl;
        return;
    }

    for (const auto& connection : connections) {
        if (connection.second.entryStationId == entryId || connection.second.exitStationId == exitId) {
            cout << "CS ID: " << entryId 
                  << " or ID: " << exitId << " already used in other connection." << endl;
            return;
        }
    }

    int diameter = inputDiameter();
    vector<pair<int, Pipe>> foundPipes;

    for (const auto& pair : pipes) {
        if (pair.second.getDiameter() == diameter && !pair.second.underRepair) {
            foundPipes.push_back(pair);
        }
    }
    
    int pipeId = -1;
    int length = 0;

    if (foundPipes.empty()) {
        cout << "---------------------------------------------------" << endl;
        cout << "Cant found a pipe. Create new." << endl;
        auto result = pipeManager.addPipeGTN();
        pipeId = get<0>(result);
        diameter = get<1>(result); 
        length = get<2>(result);
        addConnection(entryId, exitId, pipeId, diameter, length);
        viewConnections();
        return;
    }

    for (const auto& foundPipe : foundPipes) {
    pipeId = foundPipe.first;

        bool notUsed = true; 
        for (const auto& connection : connections) {
            if (connection.second.pipeId == pipeId) {
                notUsed = false; 
                break;
            }
        }

        if (notUsed) {
            diameter = foundPipe.second.diameter;
            length = foundPipe.second.length;
            addConnection(entryId, exitId, pipeId, diameter, length);
            viewConnections();
            return;
            break; 
        }
        else {
            cout << "---------------------------------------------------" << endl;
            cout << "Cant found a pipe. Create new." << endl;
            auto result = pipeManager.addPipeGTN();
            pipeId = get<0>(result);
            diameter = get<1>(result); 
            length = get<2>(result);
            addConnection(entryId, exitId, pipeId, diameter, length);
            viewConnections();
            return;
        }
    }
}

void GasTransportNetwork::viewConnections() const {
    if (connections.empty()) {
        cout << "None connection." << endl;
    }
    else {
        cout << "-----------------------------" << endl;
        cout << "List of connection:" << endl;
        for (const auto& pair : connections) {
            const Connection& connection = pair.second;
            cout << "ID of connection: " << pair.first
                    << ", (in) CS ID: " << connection.entryStationId
                    << " ,(out) connect with CS of ID: " << connection.exitStationId
                    << " by pipe of ID: " << connection.pipeId
                    << " with diameter: " << connection.diameter << " and length: " << connection.length <<endl;
        }
    }
}

void GasTransportNetwork::addConnection(int entryId, int exitId, int pipeId, int diameter, int length) {
    connections.emplace(nextConnectionId++, Connection(entryId, exitId, pipeId, diameter, length));
    cout << "-----------------------------" << endl;
    cout << "Connection CS of ID: " << entryId 
        << " with CS of ID: " << exitId 
        << " by pipe of ID: " << pipeId << " with diameter: " << diameter << " and length: " << length << endl;
}

void GasTransportNetwork::removeConnection() {
    int connectionId = readPositive<int>("Input Id connection for delete: ", 
                                          "Incorrect input ID.");

    auto it = connections.find(connectionId);
    if (it != connections.end()) {
        connections.erase(it);
        cout << "Connection with ID: " << connectionId << " was deleted." << endl;
    } else {
        cout << "Connection with ID: " << connectionId << " not found." << endl;
    }
}

void GasTransportNetwork::dfs(int node, map<int, vector<int>>& graph, set<int>& visited, set<int>& recursionStack, stack<int>& Stack, bool& hasCycle) {
    visited.insert(node);
    recursionStack.insert(node);

    for (int neighbor : graph[node]) {
        if (recursionStack.find(neighbor) != recursionStack.end()) {
            hasCycle = true; 
            return;
        }
        if (visited.find(neighbor) == visited.end()) {
            dfs(neighbor, graph, visited, recursionStack, Stack, hasCycle);
        }
    }

    recursionStack.erase(node);
    Stack.push(node);
}

void GasTransportNetwork::findConnectedComponents(map<int, Connection>& connections, vector<vector<int>>& components) {
    map<int, vector<int>> graph;
    set<int> visited;
    set<int> recursionStack; 

    for (const auto& pair : connections) {
        int from = pair.second.entryStationId;
        int to = pair.second.exitStationId;
        graph[from].push_back(to);
        graph[to]; 
    }

    for (const auto& pair : graph) {
        int node = pair.first;
        if (visited.find(node) == visited.end()) {
            vector<int> component;
            stack<int> Stack;
            bool hasCycle = false; 
            dfs(node, graph, visited, recursionStack, Stack, hasCycle);
            if (hasCycle) {
                cout << "A graph with a source" << node << " contains loops, topological sorting is not possible." << endl;
                cout << "---------------------------------------------------------------------------------------------" << endl;
            } else {
                while (!Stack.empty()) {
                    component.push_back(Stack.top());
                    Stack.pop();
                }
                components.push_back(component);
            }
        }
    }
}

void GasTransportNetwork::topologicalSort(const map<int, Connection>& connections) {
    vector<vector<int>> components;
    findConnectedComponents(const_cast<map<int, Connection>&>(connections), components);   

    for (const auto& component : components) {
        map<int, vector<int>> graph;
        map<int, map<int, int>> weights; 
        map<int, int> inDegree; 

        for (const auto& compNode : component) {
            for (const auto& pair : connections) {
                if (pair.second.entryStationId == compNode) {
                    int to = pair.second.exitStationId;
                    int length = pair.second.length;
                    graph[compNode].push_back(to);
                    weights[compNode][to] = length;
                    inDegree[to]++;
                    if (inDegree.find(compNode) == inDegree.end()) {
                        inDegree[compNode] = 0; 
                    }
                }
            }
        }

        stack<int> Stack;
        set<int> visited;
        set<int> recursionStack;

        bool hasCycle = false; 
        for (int node : component) {
            if (visited.find(node) == visited.end()) {
                dfs(node, graph, visited, recursionStack, Stack, hasCycle);
            }
        }

        if (!hasCycle) {
            vector<int> topologicalOrder;
            while (!Stack.empty()) {
                topologicalOrder.push_back(Stack.top());
                Stack.pop();
            }

            cout << "Topological sorting of the graph: ";
            cout << "---------------------------------------------------" << endl;
            for (int i : topologicalOrder) {
                cout << i << " ";
            }
            cout << endl;

            cout << "Adjacency matrix (with weights): " << endl;
            cout << "    ";
            for (int j : topologicalOrder) {
                cout << setw(3) << j << "|"; 
            }
            cout << endl;

            for (int i : topologicalOrder) {
                cout << setw(3) << i << "|"; 
                for (int j : topologicalOrder) {
                    if (weights[i].find(j) != weights[i].end()) {
                        cout << setw(3) << weights[i][j] << " "; 
                    } else {
                        cout << setw(3) << 0 << " "; 
                    }
                }
                cout << endl;
            }
        }
    }
}