#include "divide.h"
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>

using namespace std;
vector<string> min1, max1, min2, max2,RANK;
int map[100][100];
unordered_map<string, int> situation;

//最初に一度呼んで離散化させておく関数
//ファイルから数値を読み込むため、一度だけ呼ぶこと
void dividePolar() {
	string input = "out.csv";
	istringstream stream(input);
	string field;
	
	int i = 0;
	while (getline(stream, field)) {
		if (i == 0) {
			min1.push_back(field);
			i++;
		}
		else if (i == 1) {
			max1.push_back(field);
			i++;
		}
		else if (i == 2) {
			min2.push_back(field);
			i++;
		}
		else if(i == 3) {
			max2.push_back(field);
		}
		else {
			RANK.push_back(field);
		}
	}
}

void divideCartesian() {
	
}

//盤面を引数として、situationの番号を返す
int searchPolar(const GAMESTATE* const gs) {
	vector<int> 
	float* a = new float[16];
	float* r = new float[16];
	bool* c = new bool[16];
	string* body = new string[16];
	float _x_center = 2.375;
	float _y_center = 4.88;
	string pos = "";
	int count = 0;
	for (int i = 0; i < 16; i++) {
		
		if (gs->body[i][0] + gs->body[i][1] != 0) {
			a[i] = atan2(gs->body[i][1] - _y_center, gs->body[i][0] - _x_center) / M_PI;
			r[i] = sqrt((gs->body[i][1] - _y_center)*(gs->body[i][1] - _y_center) + (gs->body[i][0] - _x_center)*(gs->body[i][0] - _x_center));
			c[i] = gs->WhiteToMove;
			for (int t = 0; t < min1.size(); t++) {
				if (a[i] > stof(min1.at(t)) && a[i] < stof(max1.at(t)) && r[i] > stof(min2.at(t)) && r[i] < stof(max2.at(t))) {
					pos += to_string(i);
				}
			}
		}
	}
	if (situation[pos] == 0) {
		situation[pos] = situation.size() + 1;
	}
	return situation[pos];
}
void searchCartesian() {

}