#include "PipeManager.h"
#include "Pipe.h"
#include <vector>
#include <iostream>
#include <map>

using namespace std;

extern map<int, Pipe> pipes;

    string PipeManager::savePipe(const Pipe &pipe) const {
            return pipe.full_name + ";" + to_string(pipe.length) + ";" + to_string(pipe.diameter) + ";" + to_string(pipe.underRepair);
        }

    void PipeManager::toggleRepair(Pipe &pipe, int id) {
        if (pipe.length > 0) {
            pipe.underRepair = !pipe.underRepair;
            cout << "ID: " << id << " - The 'in repair' changed to: " << (pipe.underRepair ? "Yes" : "No") << endl;
            log("User state repair status Pipe ID " + to_string(id) + string(pipe.underRepair ? " In repair" : " Ok"));
        }
        else {
            cout << "None Pipe" << endl;
        }
    }
    
    void PipeManager::addPipe() {
        Pipe newPipe; 
        newPipe.read(); 

        while (pipes.find(nextId) != pipes.end()) {
            nextId++;
        }

        pipes[nextId] = newPipe;

        cout << "Pipe added. ID: " << nextId << endl;
        log("Pipe was added. ID: " + to_string(nextId));

        nextId++; 
    }

    tuple<int, int, int> PipeManager::addPipeGTN() {
        Pipe newPipe;
        cout << "Create a pipe with a diameter of 500, 700, 1000 or 1400" << endl;
        cout << "---------------------------------------------------" << endl;

        newPipe.read();

        while (pipes.find(nextId) != pipes.end()) {
            nextId++;
        }

        while (newPipe.diameter != 500.0 && newPipe.diameter != 700.0 &&
               newPipe.diameter != 1000.0 && newPipe.diameter != 1400.0) {
            cout << "Incorrect diameter. Enter it again." << endl;
            cout << "--------------------------------" << endl;

            if (!pipes.empty()) {
                auto lastElement = --pipes.end(); 
                cout << "Removing a pipe with an ID: " << lastElement->first << endl;
                pipes.erase(lastElement); 
            }

            newPipe.read();
        }

        pipes[nextId] = newPipe;

        cout << "The pipe has been added. ID: " << nextId << endl;

        return make_tuple(nextId, newPipe.diameter, newPipe.length); 
    }

    void PipeManager::removeConnectionsUsingPipe(int pipeId) {
        for (auto it = connections.begin(); it != connections.end();) {
            if (it->second.pipeId == pipeId) {
                it = connections.erase(it); 
            } else {
                ++it; 
            }
        }
        cout << "Connections using a pipe with an ID " << pipeId << ", was deleted." << endl;
    }

    void PipeManager::editPipes() {
    if (pipes.empty()) {
        cout << "The pipe list is empty. Editing is not possible." << endl;
        return; 
    }
    cout << "Enter the pipe ID to edit, separated by commas (or * to edit all): ";
    string input;
    getline(cin, input);
    stringstream ss(input);
    string idStr;
    vector<int> ids;
    vector<int> nonExistentIds;

    if (input == "*") {
        for (auto &pipePair : pipes) { 
            toggleRepair(pipePair.second, pipePair.first); 
            if (pipePair.second.underRepair) {
                removeConnectionsUsingPipe(pipePair.first);
            }
        }
        cout << "All pipes have been updated." << endl;
        log("The user has updated the 'in repair' status for all pipes.");
    } else {
        while (getline(ss, idStr, ',')) {
            try {
                int id = stoi(idStr); 
                ids.push_back(id);
            } catch (const invalid_argument &) {
                cout << "Mistake: is not a valid integer. Try again." << endl;
                return;
            } catch (const out_of_range &) {
                cout << "Error: The value is out of the acceptable range." << endl;
                return;
            }
        }

        bool edited = false;
        for (int id : ids) {
            auto it = pipes.find(id); 
            if (it != pipes.end()) { 
                toggleRepair(it->second, id); 
                if (it->second.underRepair) {
                    removeConnectionsUsingPipe(id);
                }
                edited = true;
            } else {
                nonExistentIds.push_back(id); 
            }
        }

        if (edited) {
            cout << "The status of the pipes has been updated." << endl;
        }

        if (!nonExistentIds.empty()) {
            cout << "Pipe or pipes with ID: ";
            for (size_t i = 0; i < nonExistentIds.size(); ++i) {
                cout << nonExistentIds[i];
                if (i < nonExistentIds.size() - 1) {
                    cout << ", ";
                }
            }
            cout << " None." << endl;
        }
    }
}

    void PipeManager::deletePipes() {
    if (pipes.empty()) {
        cout << "The pipe list is empty. Deletion is not possible." << endl;
        return; 
    }
    cout << "Enter the pipe ID to delete, separated by commas (or * to delete all): ";
    string input;
    getline(cin, input);

    if (input == "*") {
        pipes.clear(); 
        connections.clear(); 
        cout << "All pipes and connections have been removed." << endl;
        log("The user deleted the entire list of pipes and connections.");
        return;
    }

    stringstream ss(input);
    string idStr;
    vector<int> ids;
    vector<int> nonExistentIds;

    while (getline(ss, idStr, ',')) {
        try {
            int id = stoi(idStr);
            ids.push_back(id);
        } catch (const invalid_argument&) {
            cout << "Error: '" << idStr << "' is not a valid number." << endl;
            log("Error: '" + idStr + "' is not a valid number.");
            return;
        } catch (const out_of_range&) {
            cout << "Error: Value '" << idStr << "' out of the acceptable range." << endl;
            log("Error: Value '" + idStr + "' out of the acceptable range.");
            return;
        }
    }

    for (int id : ids) {
        auto it = pipes.find(id); 

        if (it != pipes.end()) {
            pipes.erase(it); 
            cout << "Pipe with ID " << id << " deleted." << endl;
            log("Pipe with ID was deleted: " + to_string(id));
            
            for (auto connIt = connections.begin(); connIt != connections.end(); ) {
                if (connIt->second.pipeId == id) {
                    connIt = connections.erase(connIt); 
                } else {
                    ++connIt; 
                }
            }
        } else {
            nonExistentIds.push_back(id); 
        }
    }

    if (!nonExistentIds.empty()) {
        cout << "Pipe / pipes with ID: ";
        for (size_t i = 0; i < nonExistentIds.size(); ++i) {
            cout << nonExistentIds[i];
            if (i < nonExistentIds.size() - 1) {
                cout << ", ";
            }
        }
        cout << " None." << endl;
    }
}

    void PipeManager::displayAllPipes() {
        if (pipes.empty()) {
            cout << "There are no pipes available." << endl;
        } else {
            cout << "List of Pipes:" << endl;
            cout << endl;
            for (const auto &pipe : pipes) {
                cout << "ID " <<pipe.first;
                pipe.second.display();
            }
            cout << endl;
        }
        cout << "-----------------------------" << endl;
        log("The user requested a list of pipes.");
    }

    void PipeManager::editPipesFound(const vector<int> &foundIds) {
        cout << "Enter the pipe ID to edit, separated by commas (or '' to edit all): ";
        string input;
        getline(cin, input);
        stringstream ss(input);
        string idStr;
        vector<int> ids;
        vector<int> nonExistentIds;

        if (input == "") {
            for (int id : foundIds) {
                auto it = pipes.find(id); 
                if (it != pipes.end()) {
                    toggleRepair(it->second, id);
                    if (it->second.underRepair) {
                        removeConnectionsUsingPipe(id); // Удаляем соединения, если труба в ремонте
                    }
                }
            }
            cout << "All found pipes have been updated." << endl;
            log("The user has updated the 'in repair' status for all found pipes.");
        } else {
            while (getline(ss, idStr, ',')) {
                try {
                    int id = stoi(idStr); 
                    ids.push_back(id);
                } catch (const invalid_argument &) {
                    cout << "Error: is not a valid integer. Try again." << endl;
                    return;
                } catch (const out_of_range &) {
                    cout << "Error: the value is out of the acceptable range." << endl;
                    return;
                }
            }

            bool edited = false;
            for (int id : ids) {
                if (find(foundIds.begin(), foundIds.end(), id) != foundIds.end()) {
                auto it = pipes.find(id); 
                if (it != pipes.end()) { 
                    toggleRepair(it->second, id);
                    if (it->second.underRepair) {
                        removeConnectionsUsingPipe(id); // Удаляем соединения, если труба в ремонте
                    }
                edited = true;
            } else {
                nonExistentIds.push_back(id); 
                }
            }

            if (edited) {
                cout << "The status of the pipes has been updated." << endl;
            }

            if (!nonExistentIds.empty()) {
                cout << "Pipe / pipes with ID: ";
                for (size_t i = 0; i < nonExistentIds.size(); ++i) {
                    cout << nonExistentIds[i];
                    if (i < nonExistentIds.size() - 1) {
                        cout << ", ";
                    }
                }
                cout << " None" << endl;
            }
        }
    }
}

    void PipeManager::deletePipesFound(const vector<int>& foundIds) {
        cout << "Enter the pipe ID to delete, separated by commas (or * to delete all found pipes): ";
        string input;
        getline(cin, input);

        if (input == "*") {
            for (int id : foundIds) {
                auto it = pipes.find(id);
                if (it != pipes.end()) {
                    removeConnectionsUsingPipe(id); // Удаляем соединения перед удалением трубы
                    pipes.erase(it);
                    cout << "Pipe with ID " << id << " deleted." << endl;
                    log("Pipe with ID deleted: " + to_string(id));
                }
            }
            cout << "All found pipes have been deleted." << endl;
            log("The user deleted all found pipes.");
            return;
        }

        stringstream ss(input);
        string idStr;
        vector<int> ids;
        vector<int> nonExistentIds;

        while (getline(ss, idStr, ',')) {
            try {
                int id = stoi(idStr);
                if (find(foundIds.begin(), foundIds.end(), id) != foundIds.end()) {
                    ids.push_back(id);
                } else {
                    nonExistentIds.push_back(id);
                }
            } catch (const invalid_argument&) {
                cout << "Error: '" << idStr << "' is not a valid integer. Try again." << endl;
                log("Error: '" + idStr + "' is not a valid integer.");
                return;
            } catch (const out_of_range&) {
                cout << "Error: value '" << idStr << "' out of the acceptable range." << endl;
                log("Error: value '" + idStr + "' out of the acceptable range.");
                return;
            }
        }

        for (int id : ids) {
            auto it = pipes.find(id);
            if (it != pipes.end()) {
                removeConnectionsUsingPipe(id); // Удаляем соединения перед удалением трубы
                pipes.erase(it);
                cout << "Pipe with ID " << id << " удалена." << endl;
                log("Pipe with ID was deleted: " + to_string(id));
            }
        }

        if (!nonExistentIds.empty()) {
            cout << "Pipe / pipes with ID: ";
            for (size_t i = 0; i < nonExistentIds.size(); ++i) {
                cout << nonExistentIds[i];
                if (i < nonExistentIds.size() - 1) {
                    cout << ", ";
                }
            }
            cout << " None." << endl;
        }
    }

    void PipeManager::displayPipeMenuFound() {
            cout << "Menu:" << endl;
            cout << "1. Delete Pipes" << endl;
            cout << "2. Edit Pipes" << endl;
            cout << "0. Back" << endl;
            cout << "-----------------------------" << endl;
        }

    void PipeManager::handleFoundPipeMenu(const vector<int>& foundPipeIds) {
        int choice;
        while (true) {
            displayPipeMenuFound();
            choice = inputIntInRange("Choose action: ", 0, 2);
            log("User choice: " + to_string(choice));

            cout << "-----------------------------" << endl;

            switch (choice) {
                case 1:
                    deletePipesFound(foundPipeIds); 
                    break;
                case 2:
                    editPipesFound(foundPipeIds); 
                    break;
                case 0:
                    log("The user went to 'Pipe Filtering'");
                    return;
                default:
                    cout << "Incorrect choice, try again." << endl;
                    log("Incorrect choice: " + choice);
                    break;
            }
        }
    }

    void PipeManager::searchPipesName() {
        string searchName;
        log("The user launched a pipe search using the 'Name' filter.");
        cout << "-----------------------------" << endl;
        cout << "Enter the name of the pipe to search for: ";
        cin >> searchName;
        cin.ignore();
        log("Pipe Search 'Name': Name entered:" + searchName);

        if (pipes.empty()) {
            cout << "-----------------------------" << endl;
            cout << "Nothing was found for this query" << endl;
            log("The user did not find anything for this filter.");
        } else {
            cout << "-----------------------------" << endl;
            cout << "Pipe Search Results:\n";
            vector<int> foundPipeIds; 
            bool found = false;

            for (const auto& pipe : pipes) {
                if (pipe.second.full_name.find(searchName) != string::npos) {
                    cout << "ID " << pipe.first;
                    pipe.second.display();
                    log("The user found a pipe with an ID using the 'Name' filter: " + to_string(pipe.first));
                    foundPipeIds.push_back(pipe.first); 
                    found = true;
                }
            }
            if (!found) {
                cout << "-----------------------------" << endl;
                cout << "Nothing was found for this query" << endl;
                log("The user did not find anything for this filter.");
                return;
            }
            else{
                handleFoundPipeMenu(foundPipeIds);
            }
        }
    }

    void PipeManager::searchPipesRepair() {
    bool repairStatus;
    log("The user launched a search for pipes by the filter 'Repair status'.");
    cout << "-----------------------------" << endl;
    cout << "Searching for pipes 'under repair'? (1 - yes, 0 - no):";
    repairStatus = inputIntInRange(" ", 0, 1);
    log("Pipe search 'Repair Status': Status entered (1 - yes, 0 - no):" + to_string(repairStatus));

    if (pipes.empty()) {
        cout << "-----------------------------" << endl;
        cout << "Nothing was found for this query" << endl;
        log("The user did not find anything for this filter.");
        return;
    }

    cout << "-----------------------------" << endl;
    cout << "Pipe Search Results:\n";
    
    vector<int> foundPipeIds;
    
    for (const auto& pipe : pipes) {
        if (pipe.second.underRepair == repairStatus) {
            cout << "ID " << pipe.first;
            pipe.second.display();
            log("The user found a pipe with an ID using the 'Repair Status' filter: " + to_string(pipe.first));
            foundPipeIds.push_back(pipe.first); 
            
        }
    }
    handleFoundPipeMenu(foundPipeIds);
    if (foundPipeIds.empty()) {
        cout << "-----------------------------" << endl;
        cout << "Nothing was found for this query" << endl;
        log("The user did not find anything for this filter.");
        return;
    }
}

    void PipeManager::searchPipeMenu() {
        cout << "Menu:" << endl;
        cout << "1. Searching by name" << endl;
        cout << "2. Searching by status" << endl;
        cout << "0. Back" << endl;
        cout << "-----------------------------" << endl;
    }

    void PipeManager::searchPipeSwitch() {
        if (pipes.empty()) {
            cout << "The pipe list is empty. Search is not possible." << endl;
            return; 
        }
        int choice;
        while (true) {
            searchPipeMenu();
            choice = inputIntInRange("Choose action: ", 0, 5);
            log("The user has selected an action: " + to_string(choice));

            cout << "-----------------------------" << endl;

            switch (choice) {
            case 1:
                searchPipesName();
                break;
            case 2:
                searchPipesRepair();
                break;
            case 0:
                PipeSwitch();
                log("The user went to the menu 'Pipe management'");
            default:
                cout << "Wrong choice, try again." << endl;
                log("Wrong choice: " + choice);
                break;
            }
        }
    }

    void PipeManager::displayPipeMenu() {
        cout << "Menu:" << endl;
        cout << "1. Add pipe" << endl;
        cout << "2. Edit pipe" << endl;
        cout << "3. Delete pipe" << endl;
        cout << "4. List of pipe" << endl;
        cout << "5. Search by filter" << endl;
        cout << "0. Back" << endl;
        cout << "-----------------------------" << endl;
    }

    void PipeManager::PipeSwitch() {
        void runProgram();
        int choice;
        while (true) {
            displayPipeMenu();
            choice = inputIntInRange("Choose action: ", 0, 5);
            log("User choice: " + to_string(choice));

            cout << "-----------------------------" << endl;

            switch (choice) {
            case 1:
                addPipe();
                break;
            case 2:
                editPipes();
                break;
            case 3:
                deletePipes();
                break;
            case 4:
                displayAllPipes();
                break;
            case 5:
                searchPipeSwitch();
                log("The user went to the 'Pipe filtration' menu");
                break;
            case 0:
                log("The user went to the 'Main Menu'");
                runProgram();
            default:
                cout << "Wrong choice, try again." << endl;
                log("Wrong choice: " + choice);
                break;
            }
        }
    }