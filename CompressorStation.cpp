#include "CompressorStation.h"
#include "utils.h"

using namespace std;

void CompressorStation::read() {
    cout << "Please, enter name of your CS >> ";
    getline(cin, full_name);
    log("User entered new CS name: " + full_name); 
    totalShops = readPositive<int>("Please, enter amount of workshops of your CS >> ", "Invalid input. Please enter a valid number for workshops.");
    log("User entered amount of workshops of CS: " + to_string(totalShops));
    operationalShops = readPositive<int>("Please, enter amount of active workshops of your CS >> ", "Number of active workshops cannot be greater than total workshops and be less than zero. Please enter a valid number for active workshops.",totalShops+1);
    log("User entered amount of active workshops of CS: " + to_string(totalShops));
    efficiency = readPositive<double>("Please, enter effective of your CS in % (1-100): ","Invalid Input! The efficiency must be a non-negative number and less than 100 >> ",101 );
    log("User entered effective of CS: " + to_string(totalShops) + "%");
    unusedOperationalShops = ((totalShops - operationalShops) / static_cast<double>(totalShops)) * 100;
}

void CompressorStation::display() const {
    if (full_name.empty() && totalShops <= 0 && operationalShops <= 0) {
        cout << "None CS" << endl;
    } else {
        cout << " : CS: " << full_name 
             << ", amount of workshops: " << totalShops
             << ", amount of workshops in active: " << operationalShops
             << setprecision(2) << fixed 
             << ", Percentage of unused workshops: " << unusedOperationalShops << "%"
             << ", Effective: " << efficiency << "%" << endl;
    }
}

void CompressorStation::load(const string &data) {
    size_t pos1 = data.find(';');  
    size_t pos2 = data.find(';', pos1 + 1);  
    size_t pos3 = data.find(';', pos2 + 1);  
    size_t pos4 = data.find(';', pos3 + 1);  

    full_name = data.substr(0, pos1);  
    totalShops = stoi(data.substr(pos1 + 1, pos2 - pos1 - 1));  
    operationalShops = stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));  
    unusedOperationalShops = stoi(data.substr(pos3 + 1, pos4 - pos3 - 1));
    efficiency = stod(data.substr(pos4 + 1, data.length() - pos4 - 1));
}