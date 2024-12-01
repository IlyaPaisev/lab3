#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <set>
#include <ctime>
#include <queue>
#include <stack>

using namespace std;

// ----------------------------------------------------UTILITY FUNCTIONS-------------------------------------------------------------------------

bool isValidNumber(const std::string& str) {
    for (char const& c : str) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

template <typename T>
void getInput(const std::string& prompt, T& value, bool (*validate)(const std::string&), bool checkPositive = false) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        if (validate(input)) {
            std::istringstream(input) >> value;
            if (!checkPositive || value > 0) {
                break;
            }
            else {
                std::cout << "Value must be greater than 0. ";
            }
        }
        else {
            std::cout << "Invalid input. ";
        }
    }
}

// ----------------------------------------------------CLASSES-------------------------------------------------------------------------

template <typename T>
class Entity {
public:
    std::string id;
    std::string name;

    static std::string findNextAvailableID(const std::map<std::string, T>& entities, std::set<int>& usedIDs) {
        int maxID = 0;
        for (const auto& pair : entities) {
            int currentID = std::stoi(pair.first);
            if (currentID > maxID) {
                maxID = currentID;
            }
        }
        int nextID = maxID + 1;
        while (usedIDs.find(nextID) != usedIDs.end()) {
            nextID++;
        }
        return std::to_string(nextID);
    }

    void displayCommon() const {
        cout << "ID: " << id << endl;
        cout << "    Name: " << name << endl;
    }

    static void logAction(const std::string& action) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        tm localTime;
        localtime_s(&localTime, &now);
        std::ofstream logFile("logs.txt", std::ios::app);
        logFile << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << " - " << action << std::endl;
    }
};

class Pipe : public Entity<Pipe> {
public:
    int length;
    int diametre;
    bool repair_status;

    Pipe() : length(0), diametre(0), repair_status(false) {}

    void createPipe(std::map<std::string, Pipe>& pipes, std::set<int>& usedPipeIDs) {
        id = findNextAvailableID(pipes, usedPipeIDs);
        std::cout << "Next available ID for new pipe: " << id << std::endl;
        std::cout << "Please, enter name of your pipe >> ";
        std::getline(std::cin, name);

        getInput("Please, enter length of your pipe >> ", length, isValidNumber, true);
        getInput("Please, enter diametre of your pipe >> ", diametre, isValidNumber, true);
        getInput("Please, enter repair status of your pipe (0 for No, 1 for Yes) >> ", repair_status, [](const std::string& str) { return str == "0" || str == "1"; });

        pipes[id] = *this;
        usedPipeIDs.insert(std::stoi(id));
        std::cout << "Pipe created with ID: " << id << std::endl;
        logAction("Pipe created with ID: " + id);
    }

    void editPipe() {
        getInput("Please, enter new repair status of your pipe (0 for No, 1 for Yes) >> ", repair_status, [](const std::string& str) { return str == "0" || str == "1"; });
        logAction("Pipe with ID: " + id + " edited");
    }

    void display() const {
        displayCommon();
        cout << "    Length: " << length << endl;
        cout << "    Diametre: " << diametre << endl;
        cout << "    Repair Status: " << (repair_status ? "Yes" : "No") << endl;
    }
};

class Compressor_station : public Entity<Compressor_station> {
public:
    int workshop;
    int workshop_active;
    int effective;

    Compressor_station() : workshop(0), workshop_active(0), effective(0) {}

    void createCS(std::map<std::string, Compressor_station>& cs_list, std::set<int>& usedCSIDs) {
        id = findNextAvailableID(cs_list, usedCSIDs);
        std::cout << "Next available ID for new Compressor Station: " << id << std::endl;
        std::cout << "Please, enter name of your CS >> ";
        std::getline(std::cin, name);

        getInput("Please, enter amount of workshops of your CS >> ", workshop, isValidNumber, true);
        getInput("Please, enter amount of active workshops of your CS >> ", workshop_active, isValidNumber, true);
        while (workshop_active > workshop) {
            std::cout << "Number of active workshops cannot be greater than total workshops. ";
            getInput("Please, enter amount of active workshops of your CS >> ", workshop_active, isValidNumber, true);
        }
        getInput("Please, enter effectiveness of your CS >> ", effective, isValidNumber, true);

        cs_list[id] = *this;
        usedCSIDs.insert(std::stoi(id));
        std::cout << "Compressor Station created with ID: " << id << std::endl;
        logAction("Compressor Station created with ID: " + id);
    }

    void editCS() {
        getInput("Please, enter new amount of active workshops of your CS >> ", workshop_active, isValidNumber, true);
        while (workshop_active > workshop) {
            std::cout << "Number of active workshops cannot be greater than total workshops. ";
            getInput("Please, enter new amount of active workshops of your CS >> ", workshop_active, isValidNumber, true);
        }
        logAction("Compressor Station with ID: " + id + " edited");
    }

    void display() const {
        displayCommon();
        cout << "    Workshops: " << workshop << endl;
        cout << "    Workshops in active: " << workshop_active << endl;
        cout << "    Effective: " << effective << endl;
    }
};

class GasTransportNetwork {
public:
    std::map<std::string, Pipe> pipes;
    std::set<int> usedPipeIDs;
    std::set<int> usedCSIDs;
    std::map<std::string, Compressor_station> cs_list;
    std::map<std::string, std::vector<std::pair<std::string, int>>> adjList;

    void connectStations(const std::string& from, const std::string& to, int diameter) {
        if (cs_list.find(from) == cs_list.end() || cs_list.find(to) == cs_list.end()) {
            std::cout << "Invalid station IDs." << std::endl;
            return;
        }

        Pipe* pipe = getAvailablePipe(diameter);
        if (!pipe) {
            Pipe newPipe;
            newPipe.diametre = diameter;
            newPipe.createPipe(pipes, usedPipeIDs);
            pipe = &pipes[newPipe.id];
        }

        adjList[from].push_back({ to, pipe->diametre });
        std::cout << "Connected " << from << " to " << to << " with a pipe of diameter: " << diameter << std::endl;
    }

    Pipe* getAvailablePipe(int diameter) {
        for (auto& pair : pipes) {
            if (pair.second.diametre == diameter && !pair.second.repair_status) {
                return &pair.second;
            }
        }
        return nullptr;
    }

    void topologicalSortUtil(const std::string& v, std::map<std::string, bool>& visited, std::stack<std::string>& Stack) {
        visited[v] = true;

        for (auto& neighbor : adjList[v]) {
            if (!visited[neighbor.first]) {
                topologicalSortUtil(neighbor.first, visited, Stack);
            }
        }

        Stack.push(v);
    }

    void topologicalSort() {
        std::stack<std::string> Stack;
        std::map<std::string, bool> visited;

        for (auto& pair : cs_list) {
            visited[pair.first] = false;
        }

        for (auto& pair : cs_list) {
            if (!visited[pair.first]) {
                topologicalSortUtil(pair.first, visited, Stack);
            }
        }

        std::cout << "Topological Sort Order: ";
        while (!Stack.empty()) {
            std::cout << Stack.top() << " ";
            Stack.pop();
        }
        std::cout << std::endl;
    }
};

// ----------------------------------------------------MAIN_FUNCTION------------------------------------------------------------------------

template <typename T>
void showEntities(const std::map<std::string, T>& entities, const std::string& entityName) {
    if (entities.empty()) {
        std::cout << "No " << entityName << " available." << std::endl;
        return;
    }

    std::cout << "List of " << entityName << ":" << std::endl;
    for (const auto& item : entities) {
        item.second.display();
    }
}

void showAll(const GasTransportNetwork& network) {
    if (network.pipes.empty() && network.cs_list.empty()) {
        std::cout << "Nothing created." << std::endl;
        return;
    }

    showEntities(network.pipes, "Pipes");
    showEntities(network.cs_list, "Compressor Stations");
}

template <typename T>
void editEntity(std::map<std::string, T>& entities, const std::string& entityName) {
    showEntities(entities, entityName);
    if (entities.empty()) {
        std::cout << "No " << entityName << " to edit." << std::endl;
        return;
    }

    std::cout << "Enter the ID of the " << entityName << " you want to edit: ";
    std::string id;
    std::getline(std::cin, id);

    auto it = entities.find(id);
    if (it == entities.end()) {
        std::cout << entityName << " not found." << std::endl;
        return;
    }

    if constexpr (std::is_same_v<T, Pipe>) {
        it->second.editPipe();
    }
    else if constexpr (std::is_same_v<T, Compressor_station>) {
        it->second.editCS();
    }
}

template <typename T>
void deleteEntity(std::map<std::string, T>& entities, const std::string& entityName) {
    showEntities(entities, entityName);
    if (entities.empty()) {
        std::cout << "No " << entityName << " to delete." << std::endl;
        return;
    }

    std::cout << "Enter the ID of the " << entityName << " you want to delete: ";
    std::string id;
    std::getline(std::cin, id);

    auto it = entities.find(id);
    if (it != entities.end()) {
        entities.erase(it);
        std::cout << entityName << " with ID " << id << " has been deleted." << std::endl;
        T::logAction(entityName + " with ID " + id + " has been deleted.");
    }
    else {
        std::cout << entityName << " with ID " << id << " not found." << std::endl;
        T::logAction("Attempt to delete non-existent " + entityName + " with ID " + id);
    }
}

void Save(const GasTransportNetwork& network) {
    std::string filename;
    std::cout << "Enter the filename (without extension): ";
    std::getline(std::cin, filename);
    filename += ".txt";

    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return;
    }

    for (const auto& item : network.pipes) {
        outFile << item.second.id << "," << item.second.name << ","
            << item.second.length << "," << item.second.diametre << "," << item.second.repair_status << std::endl;
    }

    outFile << "-------------------------------------------------------------------" << std::endl;

    for (const auto& item : network.cs_list) {
        outFile << item.second.id << "," << item.second.name << ","
            << item.second.workshop << "," << item.second.workshop_active << "," << item.second.effective << std::endl;
    }

    outFile.close();
    std::cout << "Data saved to " << filename << std::endl;
    Pipe::logAction("Data saved to " + filename);
}

void Load(GasTransportNetwork& network) {
    std::string filename;
    std::cout << "Enter the filename (without extension): ";
    std::getline(std::cin, filename);
    filename += ".txt";

    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Error opening file for reading!" << std::endl;
        return;
    }

    std::string line;
    bool isLoadingPipes = true;

    while (std::getline(inFile, line)) {
        if (line == "-------------------------------------------------------------------") {
            isLoadingPipes = false;
            continue;
        }

        if (isLoadingPipes) {
            std::istringstream ss(line);
            std::string id, name, length_str, diameter_str, repair_status_str;

            std::getline(ss, id, ',');
            std::getline(ss, name, ',');
            std::getline(ss, length_str, ',');
            std::getline(ss, diameter_str, ',');
            std::getline(ss, repair_status_str, ',');

            try {
                Pipe pipe;
                pipe.id = id;
                pipe.name = name;
                pipe.length = std::stoi(length_str);
                pipe.diametre = std::stoi(diameter_str);
                pipe.repair_status = (repair_status_str == "1");

                network.pipes[id] = pipe;
            }
            catch (const std::exception& e) {
                std::cerr << "Invalid data for pipe: " << line << " - Error: " << e.what() << std::endl;
            }
        }
        else {
            std::istringstream ss(line);
            std::string id, name, workshop_str, workshop_active_str, effective_str;

            std::getline(ss, id, ',');
            std::getline(ss, name, ',');
            std::getline(ss, workshop_str, ',');
            std::getline(ss, workshop_active_str, ',');
            std::getline(ss, effective_str, ',');

            try {
                Compressor_station cs;
                cs.id = id;
                cs.name = name;
                cs.workshop = std::stoi(workshop_str);
                cs.workshop_active = std::stoi(workshop_active_str);
                cs.effective = std::stoi(effective_str);

                network.cs_list[id] = cs;
            }
            catch (const std::exception& e) {
                std::cerr << "Invalid data for compressor station: " << line << " - Error: " << e.what() << std::endl;
            }
        }
    }

    inFile.close();
    std::cout << "Data loaded from " << filename << std::endl;
    Pipe::logAction("Data loaded from " + filename);
}

void Pause() {
    std::cout << "Enter to continue...";
    std::cin.get();
    system("cls");
}

int main() {
    GasTransportNetwork network;

    while (true) {
        std::cout << "1. Create a pipe\n2. Create a CS\n3. View all objects\n4. Edit a pipe\n5. Delete a pipe\n";
        std::cout << "6. Edit a CS\n7. Delete a CS\n8. Connect CS with pipe\n9. Topological Sort\n10. Save\n11. Load\n0. Exit\nChoice item of menu: ";

        int choice;
        std::string input;
        std::getline(std::cin, input);
        if (!isValidNumber(input)) {
            std::cout << "Please input a valid number.\n";
            continue;
        }
        choice = std::stoi(input);

        switch (choice) {
        case 1: {
            system("cls");
            Pipe pipe;
            pipe.createPipe(network.pipes, network.usedPipeIDs);
            Pause();
            break;
        }
        case 2: {
            system("cls");
            Compressor_station cs;
            cs.createCS(network.cs_list, network.usedCSIDs);
            Pause();
            break;
        }
        case 3:
            system("cls");
            showAll(network);
            Pause();
            break;
        case 4:
            system("cls");
            editEntity(network.pipes, "pipe");
            Pause();
            break;
        case 5:
            system("cls");
            deleteEntity(network.pipes, "pipe");
            Pause();
            break;
        case 6:
            system("cls");
            editEntity(network.cs_list, "CS");
            Pause();
            break;
        case 7:
            system("cls");
            deleteEntity(network.cs_list, "CS");
            Pause();
            break;
        case 8: {
            system("cls");
            std::string from, to;
            int diameter;
            std::cout << "Enter ID of the first CS: ";
            std::getline(std::cin, from);
            std::cout << "Enter ID of the second CS: ";
            std::getline(std::cin, to);
            getInput("Enter diameter of the pipe (500, 700, 1000, 1400): ", diameter, isValidNumber, true);
            network.connectStations(from, to, diameter);
            Pause();
            break;
        }
        case 9:
            system("cls");
            network.topologicalSort();
            Pause();
            break;
        case 10:
            system("cls");
            Save(network);
            Pause();
            break;
        case 11:
            system("cls");
            Load(network);
            Pause();
            break;
        case 0:
            return 0;
        default:
            std::cout << "Incorrect choice.\n";
        }
    }

    return 0;
}