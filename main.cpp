#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "Pipe.h"
#include "CompressorStation.h"
#include "utils.h"
#include "PipeManager.h"
#include "CompressorStationManager.h"
#include "GasTransportNetwork.h"
#include <locale>

using namespace std;

map<int, Pipe> pipes;
map<int, CompressorStation> stations;
map<int, Connection> connections; 

void setupLocale() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
}

void saveToFile(const map<int, Pipe>& pipes, const map<int, CompressorStation>& stations, CompressorStationManager& cmanager, PipeManager& pmanager, GasTransportNetwork& gasNetwork) {
    string filename;
    cout << "Enter filename for save CS and pipes: ";
    cin >> filename;
    cin.ignore();
    ofstream ofs(filename + ".txt");
    log("Saving..: " + filename + ".txt");
    
    if (ofs) {
        for (const auto& station : stations) {
            ofs << "CS " << station.first << ": " << cmanager.saveCS(station.second) << endl;
        }

        for (const auto& pipe : pipes) {
            ofs << "pipe " << pipe.first << ": " << pmanager.savePipe(pipe.second) << endl;
        }

        ofs << gasNetwork.saveConnections() << endl;

        ofs.close();
        cout << "Data of CS and pipes was saved in " << filename << ".txt" << endl;
        log("Successfully saved file: " + filename + ".txt");
    } else {
        cerr << "Error open file for save." << endl;
        log("Error open file for save: " + filename + ".txt");
    }
}

void loadFromFile(map<int, Pipe>& pipes, map<int, CompressorStation>& stations, map<int, Connection>& connections) {
    string filename;
    cout << "Enter a file name to download compressor stations, pipes and connections: ";
    cin >> filename;
    cin.ignore();
    log("Try to download data from a file: " + filename + ".txt");

    ifstream ifs(filename + ".txt");
    if (ifs) {
        string line;
        while (getline(ifs, line)) {
            try {
                if (line.substr(0, 2) == "KS") {
                    size_t pos = line.find(':');
                    if (pos == string::npos) throw runtime_error("Invalid string format for the compressor station");

                    int stationKey = stoi(line.substr(3, pos - 3)); 
                    CompressorStation station;
                    station.load(line.substr(pos + 1)); 
                    
                    if (stations.find(stationKey) == stations.end()) {
                        stations[stationKey] = station;
                        log("CS loaded with ID: " + to_string(stationKey));
                    } else {
                        cerr << "CS with ID " << stationKey << " already exists." << endl;
                        log("Error: CS with ID " + to_string(stationKey) + " already exists.");
                    }
                } 
                else if (line.substr(0, 4) == "pipe") {
                    size_t pos = line.find(':');
                    if (pos == string::npos) throw runtime_error("Invalid string format for the pipe");

                    int pipeKey = stoi(line.substr(5, pos - 5)); 
                    Pipe pipe;
                    pipe.load(line.substr(pos + 1)); 

                    if (pipes.find(pipeKey) == pipes.end()) {
                        pipes[pipeKey] = pipe;
                        log("Pipe with an ID has been loaded: " + to_string(pipeKey));
                    } else {
                        cerr << "Pipe with ID " << pipeKey << " already exist." << endl;
                        log("Error: Pipe with ID " + to_string(pipeKey) + " already exist.");
                    }
                } 
                else if (line.substr(0, 10) == "connection") {
                    size_t pos = line.find(':');
                    if (pos == string::npos) throw runtime_error("Invalid string format for the connection");

                    int connectionKey = stoi(line.substr(11, pos - 11)); 
                    string params = line.substr(pos + 1);
                    replace(params.begin(), params.end(), ',', '.'); 
                    stringstream ss(params);
                    string token;
                    int entryStationId, exitStationId, pipeId, diameter, length;

                    getline(ss, token, ';');
                    entryStationId = stoi(token);
                    getline(ss, token, ';');
                    exitStationId = stoi(token);
                    getline(ss, token, ';');
                    pipeId = stoi(token);
                    getline(ss, token, ';');
                    diameter = stoi(token);
                    getline(ss, token);
                    length = stoi(token);

                    Connection connection(entryStationId, exitStationId, pipeId, diameter, length);

                    if (connections.find(connectionKey) == connections.end()) {
                        connections[connectionKey] = connection;
                        log("Connection with ID is loaded: " + to_string(connectionKey));
                    } else {
                        cerr << "Connection with ID " << connectionKey << " already exist." << endl;
                        log("Ошибка: Connection with ID " + to_string(connectionKey) + " already exist.");
                    }
                }
            } catch (const exception& e) {
                cerr << "Error loading: " << e.what() << endl;
            }
        }
        ifs.close();
    } else {
        cerr << "The file could not be opened: " << filename << ".txt" << endl;
    }
}

void runProgram() {
    PipeManager pManager(connections);
    CompressorStationManager sManager(connections);
    GasTransportNetwork gasNetwork;
    int choice;

    while (true) {
        cout << " Menu:" << endl;
        cout << " 1. Pipe" << endl;
        cout << " 2. CS" << endl;
        cout << " 3. Viewing all objects" << endl;
        cout << " 4. Save data" << endl;
        cout << " 5. Upload data" << endl;
        cout << " 6. Create a CS connection" << endl;
        cout << " 7. Delete the CS connection" << endl;
        cout << " 8. Topological sorting" << endl;
        cout << " 0. Exit" << endl;
        cout << "---------------------------------" << endl;
        choice = inputIntInRange("Choose action: ", 0, 8);
        log("User choice: " + to_string(choice));

        cout << "-----------------------------" << endl;

        switch (choice) {
            case 1:
                pManager.PipeSwitch();
                break;
            case 2:
                sManager.CompressorStationSwitch();
                break;
            case 3:
                pManager.displayAllPipes();
                sManager.displayStations();
                gasNetwork.viewConnections();
                log("The display of all pipes and stations has been completed.");
                break;
            case 4:
                saveToFile(pipes,stations,sManager,pManager,gasNetwork);
                break;
            case 5:
                loadFromFile(pipes, stations, connections);
                break;
            case 6:{
                gasNetwork.connectStations(pipes, stations, pManager);}
                break;
            case 7:{
                gasNetwork.removeConnection();}
                break;
            case 8:{
                gasNetwork.topologicalSort(connections);}
                break;
            case 0:
                cout << "Exiting the program." << endl;
                log("Exiting the program.");
                exit( 1 );
            default:
                cout << "Wrong choice, try again." << endl;
                log("Incorrect choice: " + choice);
                break;
        }
    }
}

int main() {
    setupLocale();
    log("The program has been successfully launched!");
    runProgram();
    return 0;
}
