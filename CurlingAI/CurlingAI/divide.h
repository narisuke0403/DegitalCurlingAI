#ifndef DIVIDE_H
#define DIVIDE_H

#include <stdio.h>
#include <iostream>
#include "main.h"
//#include "strategy.h"
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "math.h"
#include <unordered_map>
#include <iostream>
#include <iomanip>
using namespace std;
vector<string> split(string& input, char delimiter);
void dividePolar();
void divideCartesian();
int searchPolar(const GAMESTATE* const gs,string* _pos);
int searchCartesian(const GAMESTATE* const gs, string* _pos);
void PolarToCartesian(int number, float* pos);

#endif