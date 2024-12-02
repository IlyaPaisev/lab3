#include "utils.h"


using namespace std;

void log(const string &action) {
    ofstream logFile("log.txt", ios::app);
    if (logFile.is_open()) {
        time_t now = time(0);
        char *dt = ctime(&now);
        logFile << dt << " - " << action << endl;
        logFile.close();
    } else {
        cerr << "The logging file could not be opened!" << endl;
    }
}

template <typename T>
T readPositive(const string &prompt, const string &errorMessage, const T maxValue) {
    T value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail() ||
            (std::is_floating_point<T>::value ? value <= 0 : value < 0) ||
            value >= maxValue) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << errorMessage << endl;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            log("Entered value: " + to_string(value));
            return value;
        }
    }
}

template double readPositive<double>(const string &prompt, const string &errorMessage, const double);
template int readPositive<int>(const string &prompt, const string &errorMessage, const int);

long numberOrDefault(const string &input, long defaultValue) {
    if (input.length() < 2 || input.empty() ||
        find_if(input.begin(), input.end(), [](unsigned char c) { return !isdigit(c); }) != input.end()) {
        return defaultValue;
    }
    return strtol(input.c_str(), NULL, 10);
}

int inputIntInRange(const string &prompt, int minValue, int maxValue) {
    int value;

    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);
        stringstream ss(input);
        if (ss >> value) {
            if (ss.eof()) {
                if (value >= minValue && value <= maxValue) {
                    return value;
                } else {
                    cout << "----------------------------------------" << endl;
                    cout << "Error: enter a number from " << minValue << " to " << maxValue << "." << endl;
                    cout << "----------------------------------------" << endl;
                }
            } else {
                cout << "---------------------------------------" << endl;
                cout << "Error: Enter a valid number." << endl;
                cout << "----------------------------------------" << endl;
            }
        } else {
            cout << "----------------------------------------" << endl;
            cout << "Error: Enter a valid number." << endl;
            cout << "----------------------------------------" << endl;
        }
    }
}