#include "divide.h"
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>
#include <iostream>
#include <iomanip>
std::vector<std::string> min1, max1, min2, max2, RANK;
int map[100][100];
vector<unordered_map<string, int>> situation;

vector<string> split(string& input, char delimiter)
{
	istringstream stream(input);
	string field;
	vector<string> result;
	while (getline(stream, field, delimiter)) {
		result.push_back(field);
	}
	return result;
}

void makeSituationByQtable() {
	while (situation.size() < 16) {
		unordered_map<string, int> line;
		situation.push_back(line);
	}
	ifstream ifs("C:\\DigitalCurlingSimulate\\Release\\Qtable.csv");
	string line;
	string KEY;
	int turn = 0;
	while (getline(ifs, line)) {
		while (getline(ifs, line)) {
			vector<string> strvec = split(line, ',');
			KEY = strvec.at(1);
			turn = stoi(strvec.at(0));
			situation.at(turn)[KEY] = situation.at(turn).size() + 1;
		}
	}
}

//最初に一度呼んで離散化させておく関数
//ファイルから数値を読み込むため、一度だけ呼ぶこと
void dividePolar() {
	cerr <<"load dividePolar\n";
	ifstream ifs("out.csv");
	string line;
	int i = 0;
	while (getline(ifs, line)) {
		vector<string> strvec = split(line, ',');
		if (i == 0) {
			min1 = strvec;
			i++;
		}
		else if (i == 1) {
			max1 = strvec;
			i++;
		}
		else if (i == 2) {
			min2 = strvec;
			i++;
		}
		else if (i == 3) {
			max2 = strvec;
			i++;
		}
		else {
			RANK = strvec;
			i++;
		}
	}
	makeSituationByQtable();
}

void divideCartesian() {

}

//深さ順にソート
void get_ranking(int *rank, int depth[16])
{
	// init array
	for (int i = 0; i < 16; i++) {
		rank[i] = i;
	}
	// sort
	int tmp;
	for (int i = 1; i < 16; i++) {
		for (int j = i; j > 0; j--) {
			if (depth[j] < depth[j - 1]) {
				// swap
				tmp = rank[j];
				rank[j] = rank[j - 1];
				rank[j - 1] = tmp;
			}
			else {
				break;
			}
		}
	}
}

//盤面を引数として、situationの番号を返す
int searchPolar(const GAMESTATE* const gs, string* _pos) {
	float* a = new float[16];
	float* r = new float[16];
	bool* c = new bool[16];
	string* body = new string[16];
	float _x_center = 2.375;
	float _y_center = 4.88;
	string pos = "";
	int count = 0;
	int RANK_rank[16];
	int stone_num[16];
	//ランクでソート
	for (int i = 0; i < 16; i++) {
		if (gs->body[i][0] + gs->body[i][1] != 0) {
			a[i] = atan2(gs->body[i][1] - _y_center, gs->body[i][0] - _x_center) * 180 / M_PI;
			r[i] = sqrt((gs->body[i][1] - _y_center)*(gs->body[i][1] - _y_center) + (gs->body[i][0] - _x_center)*(gs->body[i][0] - _x_center));
			if (i % 2 == 0) {
				c[i] = true;
			}
			else {
				c[i] = false;
			}
			for (int t = 0; t < min1.size(); t++) {
				if (a[i] > stof(min1.at(t)) && a[i] < stof(max1.at(t)) && r[i] > stof(min2.at(t)) && r[i] < stof(max2.at(t))) {
					RANK_rank[i] = stoi(RANK[t]);
				}
			}
		}
	}
	for (int i = 0; i < 6; i++) {
		get_ranking(stone_num, RANK_rank);
		for (int t = 0; t < min1.size(); t++) {
			if (a[stone_num[i]] > stof(min1.at(t)) && a[stone_num[i]] < stof(max1.at(t)) && r[stone_num[i]] > stof(min2.at(t)) && r[stone_num[i]] < stof(max2.at(t))) {
				ostringstream sout;
				sout << setfill('0') << setw(4) << t;
	//			cerr << "a:" << sout.str() << endl;
				pos += sout.str() + to_string(int(c[stone_num[i]]));
			}
		}
	}
	//cerr << "pos="<< pos << endl;
	*_pos = pos;
	//cerr << *_pos << endl;
	//cerr << "gs->ShotNum=" << gs->ShotNum << endl;
	//cerr << "situation.at(0).size()=" << situation.at(0).size()<< endl;
	//cerr << "situation.at(gs->ShotNum).size()=" << situation.at(gs->ShotNum).size() << endl;
	if (situation.at(gs->ShotNum)[pos] == 0) {
		situation.at(gs->ShotNum)[pos] = situation.at(gs->ShotNum).size() + 1;
	}
	//cerr << "serchPolar is end\n";
	return situation.at(gs->ShotNum)[pos];
}
void searchCartesian() {

}

void PolarToCartesian(int number, float* pos) {
	pos[0] = 0;
	pos[1] = 0;
	//cerr << "min1.size=" << min1.size() << endl;
	float min_XA = cos(stof(min1[number]) * M_PI / 180);
	float max_XA = cos(stof(max1[number]) * M_PI / 180);
	float min_YA = sin(stof(min1[number]) * M_PI / 180);
	float max_YA = sin(stof(max1[number]) * M_PI / 180);
	float x = (stof(min2[number]) * min_XA + stof(max2[number]) * max_XA) / 2;
	float y = (stof(min2[number]) * min_YA + stof(max2[number]) * max_YA) / 2;
	pos[0] = x;
	pos[1] = y;
}

