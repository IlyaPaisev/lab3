#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

void log(const string &action);
template <typename T>
T readPositive(const string &prompt, const string &errorMessage, const T maxValue = numeric_limits<T>::max());
long numberOrDefault(const string &input, long defaultValue = -1);
int inputIntInRange(const string &prompt, int minValue, int maxValue);

#endif 