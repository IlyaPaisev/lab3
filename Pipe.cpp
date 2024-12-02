#include "Pipe.h"
#include "utils.h"

using namespace std;

void Pipe::read() {
    cout << "Please, enter name of your pipe >> ";
    getline(cin, full_name);
    log("User entered name of pipe: " + full_name);

    length = readPositive<double>("Please, enter length of your pipe (km) >> ", "Entered incorrect number for length.");
    log("User entered length of pipe: " + to_string(length) + " km");

    diameter = readPositive<double>("Please, enter diameter of your pipe (mm) >> ", "Entered incorrect number for diameter.");
    log("User entered diameter of pipe: " + to_string(diameter) + " mm");
    underRepair = false;
}

void Pipe::display() const {
    if (length <= 0 && diameter <= 0) {
        cout << "None Pipe" << endl;
        log("The user requested data about pipes that are missing."); 
        cout << "-----------------------------" << endl;
    } else {
        cout << " : Pipe: " << full_name 
             << ", Length: " << length 
             << " km, Diameter: " << diameter 
             << " mm, In repair: " 
             << (underRepair ? "Yes" : "No") 
             << endl;
        log("The user displayed the data about the pipe: " + full_name); 
    }
}

void Pipe::load(const string &data) {
    size_t pos1 = data.find(';'); 
    size_t pos2 = data.find(';', pos1 + 1);
    size_t pos3 = data.find(';', pos2 + 1); 

    full_name = data.substr(0, pos1); 
    length = stod(data.substr(pos1 + 1, pos2 - pos1 - 1));
    diameter = stod(data.substr(pos2 + 1, pos3 - pos2 - 1)); 
    underRepair = data.substr(pos3 + 1) == "1"; 
}