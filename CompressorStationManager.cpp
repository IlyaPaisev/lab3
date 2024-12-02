#include "CompressorStationManager.h"
#include "CompressorStation.h"
#include <iostream>
#include <string>

extern std::map<int, CompressorStation> stations; 

using namespace std;

    string CompressorStationManager::saveCS(const CompressorStation &stations) const {
        return stations.full_name + ";" + to_string(stations.totalShops) + ";" + 
               to_string(stations.operationalShops) + ";" + 
               to_string(stations.unusedOperationalShops) + ";" + 
               to_string(stations.efficiency);
    }

    void CompressorStationManager::toggleShop(CompressorStation &stations, int id) {
        if (stations.full_name.empty() && stations.totalShops <= 0 && stations.operationalShops <= 0) {
            cout << "None CS. Start/stop workshop is impossible." << endl;
        }
        else {
            int choice;
            cout << "1. Start workshop" << endl;
            cout << "2. Stop workshop" << endl;
            cout << "-----------------------------" << endl;
            choice = inputIntInRange("Choose action: ", 1, 3);
            cin.ignore();
            log("User choice: " + choice);

            cout << "-----------------------------" << endl;

            switch (choice) {
                case 1: 
                    log("User choice start workshop");
                    if (stations.operationalShops < stations.totalShops) {
                        stations.operationalShops++;
                        stations.unusedOperationalShops = ((stations.totalShops - stations.operationalShops) / static_cast<double>(stations.totalShops)) * 100;
                        cout << "Start new workshop. Now is working: " << stations.operationalShops << endl;
                    } else {
                        cout << "All workshops is on." << endl;
                    }
                    break;
                case 2: 
                    log("User choice stop workshop");
                    if (stations.operationalShops > 0) {
                        stations.operationalShops--;
                        stations.unusedOperationalShops = ((stations.totalShops - stations.operationalShops) / static_cast<double>(stations.totalShops)) * 100;
                        cout << "Stop workshop. Now is working: " << stations.operationalShops << endl;
                    } else {
                        cout << "Its not any workshops for stop" << endl;
                        
                    }
                    break;
                default:
                    cout << "Incorrect choice. Try again." << endl;
                    
                    break;
            }
            
        }
    }

    void CompressorStationManager::addStation() {
        CompressorStation station;

        while (stations.find(nextIdKS) != stations.end()) {
            nextIdKS++;
        }

        station.read();
        stations[nextIdKS] = station;

        cout << "CS was added" << endl;
        log("Cs was added. ID: " + to_string(nextIdKS));

        nextIdKS++;
    }

    void CompressorStationManager::displayStations() const {
        if (stations.empty()) {
            cout << "Its not any availiable CS." << endl;
            return;
        }
        cout << "List of CS:" << endl;
        cout << endl;
        for (const auto& station : stations) {
            cout << "ID " <<station.first;
            station.second.display();
        }
        cout << endl;
        log("User check list of CS.");
    }

    void CompressorStationManager::removeStation() {
        if (stations.empty()) {
            cout << "List of CS empty. Delete is impossible" << endl;
            return; 
        }
        int id;
        id = readPositive<int>("Input ID CS for delete: ", "Incorrect ID of CS.");
        if (id == -1) {
            cout << "Incorrect input. Try again!" << endl;
            return;
        }

        auto it = stations.find(id);
        if (it == stations.end()) {
            cout << "Error. CS with this ID already in list" << endl;
            return;
        }

        // Удаляем соединения, связанные с данной компрессорной станцией
        for (auto connIt = connections.begin(); connIt != connections.end(); ) {
            const Connection &conn = connIt->second;
            if (conn.entryStationId == id || conn.exitStationId == id) {
                // Удаляем соединение
                connIt = connections.erase(connIt);
                cout << "Connection with pipe ID was completed: " << conn.pipeId << endl;
            } else {
                ++connIt;
            }
        }

        stations.erase(it);
        cout << "CS was deleted." << endl;
        log("CS with ID: " + to_string(id) + " was deleted with connection.");
    }

    void CompressorStationManager::editStation() {
        if (stations.empty()) {
            cout << "List of CS empty. Edit is impossible." << endl;
            return; 
        }
        cout << "Enter ID of CS for editing: ";
        string command;
        long id;
        getline(cin, command);
        id = numberOrDefault(command);
        if (id == -1){
            cout << "Incorrect input. Try again!" << endl;
            return;
        }

        auto it = stations.find(id);
        if (it != stations.end()) {
            cout << "Editing CS with ID " << id << endl;
            toggleShop(it->second,it->first); 
            log("User update workshop of CS with ID: " + to_string(id));
        } else {
            cout << "CS with ID " << id << " not found." << endl;
        }
    }

    void CompressorStationManager::searchStationName(){
        string searchName;
        log("User start search CS by filter 'Name'.");
        cout << "Enter name of CS for search: ";
        cin >> searchName;
        cin.ignore();
        log("Searching CS 'Name': Input name is:" + searchName);

        if (stations.empty()) {
            cout << "-----------------------------" << endl;
            cout << "Nothing was found for this query." << endl;
            log("User is found nothing by this filter.");
        } else {
            cout << "-----------------------------" << endl;
            cout << "Result of searching CS:\n";
            bool found = false; 
            for (const auto& station : stations) {
                if (station.second.full_name.find(searchName) != string::npos) {
                    cout << "ID " << station.first;
                    station.second.display();
                    log("User found by filter 'Name' CS with ID: " + to_string(station.first));
                    found = true;
                }
            }

            if (!found) { 
                cout << "-----------------------------" << endl;
                cout << "Nothing was found for this query." << endl;
                log("User is found nothing by this filter.");
            }
        }
    }

    void CompressorStationManager::searchStationUnused() {
        double unusedOperationalShopsCheck;
        long aroundUnusedOperationalShops;
        string command;
        log("User start search CS by filter 'Unused workshops'.");
        cout << "Input percent of unused workshops: ";
        getline(cin, command);
        aroundUnusedOperationalShops = numberOrDefault(command);
        if (aroundUnusedOperationalShops == -1){
            cout << "Incorrect input. Try again!" << endl;
            return;
        }

        log("CS search by 'Unused workshops': Inputing percent: " + to_string(unusedOperationalShopsCheck));


        cout << "Menu:" << endl;
        cout << "1. Equal to the entered percentage" << endl;
        cout << "2. More than the entered percentage" << endl;
        cout << "3. Less than the entered percentage" << endl;
        cout << "-----------------------------" << endl;

        int choice;
        choice = inputIntInRange("Choose action: ", 1, 3);
        log("User choice: " + to_string(choice));

        if (stations.empty()) {
            cout << "-----------------------------" << endl;
            cout << "Nothing was found for this query." << endl;
            log("User is found nothing by this filter.");
            return;
        }

        cout << "-----------------------------" << endl;
        cout << "Result of search CS:\n";

        for (const auto& station : stations) {
            aroundUnusedOperationalShops = station.second.unusedOperationalShops;
            
            bool conditionMet = false;
            switch (choice) {
                case 1: 
                    conditionMet = (aroundUnusedOperationalShops == unusedOperationalShopsCheck);
                    log("User choice: Equal to the entered percentage");
                    break;
                case 2: 
                    conditionMet = (aroundUnusedOperationalShops > unusedOperationalShopsCheck);
                    log("User choice: More than the entered percentage");
                    break;
                case 3: 
                    conditionMet = (aroundUnusedOperationalShops < unusedOperationalShopsCheck);
                    log("User choice: Less than the entered percentage");
                    break;
                default:
                    cout << "Incorrect choice!" << endl;
                    return; 
            }

            if (conditionMet) {
                cout << "ID " << station.first;
                station.second.display(); 
                log("User found by filter 'Unused workshops' CS with ID: " + to_string(station.first));
            }
        }
    }

    void CompressorStationManager::searchStationMenu() {
        cout << "Menu:" << endl;
        cout << "1. Search by name" << endl;
        cout << "2. Search by percent" << endl;
        cout << "   unused workshops" << endl;
        cout << "0. Back" << endl;
        cout << "-----------------------------" << endl;
    }

    void CompressorStationManager::searchStationSwitch() {
        if (stations.empty()) {
            cout << "List of CS empty. Searching is impossible." << endl;
            return; 
        }
        int choice;
        while (true) {
            searchStationMenu();
            choice = inputIntInRange("Choose action: ", 0, 2);
            log("User choice: " + to_string(choice));
            cout << "-----------------------------" << endl;
            
            switch (choice) {
            case 1:
                searchStationName();
                break;
            case 2:
                searchStationUnused();
                break;
            case 0:
                CompressorStationSwitch();
                log("User go to menu 'managment CS'");
            default:
                cout << "Incorrect choice, try again." << endl;
                log("Incorrect choice: " + choice);
                break;
            }
        }
    }

    void CompressorStationManager::displayCompressorStationMenu() {
        cout << "Menu:" << endl;
        cout << "1. Add CS" << endl;
        cout << "2. Edit CS" << endl;
        cout << "3. Delete CS" << endl;
        cout << "4. Show list of CS" << endl;
        cout << "5. Search by filter" << endl;
        cout << "0. Back" << endl;
        cout << "-----------------------------" << endl;
        
    }

    void CompressorStationManager::CompressorStationSwitch() {
        void runProgram();
        int choice;

        while (true) {
            displayCompressorStationMenu();
            choice = inputIntInRange("Choose action: ", 0, 5);
            log("User choice: " + to_string(choice));
            cout << "-----------------------------" << endl;

            switch (choice) {
            case 1:
                addStation();
                break;
            case 2:
                editStation();
                break;
            case 3:
                removeStation();
                break;
            case 4:
                displayStations();
                break;
            case 5:
                log("User go to 'Filter CS'");
                searchStationSwitch();
                break;
            case 0:
                log("User go to 'Main menu'");
                runProgram();
            default:
                cout << "Incorrect choice. Try again/" << endl;
                log("Incorrect choice: " + choice);
                break;
            }
        }
    }