#pragma once
#include "Node.h"
#include <fstream>
#define _USE_MATH_DEFINES

using namespace std;
extern vector<string> min1, max1, min2, max2, RANK;
extern int map[100][100];
extern unordered_map<string, int> situation;
GAMESTATE gameState[16];
SHOTPOS shotPos[16];
float shotPower[16];

Node::Node(const GAMESTATE* const gs) {
	gsNode	= new GAMESTATE(*gs);
	eval = getEval();
	//children=new vector();
}
void Node::pushChildren(const GAMESTATE* const currGS, const GAMESTATE* const nextGS) {
	int childNumber = children.size();
	Node curr = Node(new GAMESTATE(*currGS));
	Node child = Node(new GAMESTATE(*nextGS));
	children.push_back(&child);
	children.at(childNumber)->parent = &curr;
}

GAMESTATE* Node::getNextState(const GAMESTATE* const gs) {
	GAMESTATE* state = new GAMESTATE(*gs);
	GAMESTATE* tmp = new GAMESTATE(*gs);
	state = playMontecarlo(tmp);
	return state;
}
void Node::addChildren(const GAMESTATE* const gs) {
	GAMESTATE *currGS = new GAMESTATE(*gs);
	GAMESTATE *nextGS = getNextState(gs);
	pushChildren(currGS, nextGS);
}

double Node::getEval() {
	double a;
	a = 3.0 / 2.0;
	return a;
}

GAMESTATE* playMontecarlo(const GAMESTATE* const gs) {
	GAMESTATE *currGS = new GAMESTATE(*gs);
	float pos[2];
	for (int i = 0; i < situation.size(); i++) {
		PolarToCartesian(i, pos);
		float posX = pos[0];
		float posY = pos[1];

	}
	return currGS;
}

void saveGameState(const GAMESTATE* const gs) {
	gameState[gs->ShotNum] = GAMESTATE(*gs);
}
void saveShotAndStateAndPower(const GAMESTATE* const gs, SHOTPOS *pos,float power) {
	gameState[gs->ShotNum] = GAMESTATE(*gs);
	shotPos[gs->ShotNum] = SHOTPOS(*pos);
	shotPower[gs->ShotNum] = power;
}
void initState() {
	cout << "a";
	for (int i = 0; i < 16; i++) {
		memset(gameState[i].body, 0x00, sizeof(float) * 32);
		memset(gameState[i].Score, 0x00, sizeof(int) * 10);
		gameState[i].LastEnd = 0;
		gameState[i].CurEnd = 0;
		gameState[i].ShotNum = 0;
		gameState[i].WhiteToMove = 0;
	}
}
void initPos() {
	for (int i = 0; i < 16; i++) {
		shotPos[i].angle = 0;
		shotPos[i].x = 0.0;
		shotPos[i].y = 0.0;
	}
}
void initPower() {
	for (int i = 0; i < 16; i++) {
		shotPower[i] = 0;
	}
}


bool GetContents(const string& filename, vector<vector<string>>& table, const char delimiter = ',')
{
	// ファイルを開く
	fstream filestream(filename);
	if (!filestream.is_open())
	{
		// ファイルが開けなかった場合は終了する
		return false;
	}

	// ファイルを読み込む
	while (!filestream.eof())
	{
		// １行読み込む
		string buffer;
		filestream >> buffer;

		// ファイルから読み込んだ１行の文字列を区切り文字で分けてリストに追加する
		vector<string> record;              // １行分の文字列のリスト
		istringstream streambuffer(buffer); // 文字列ストリーム
		string token;                       // １セル分の文字列
		while (getline(streambuffer, token, delimiter))
		{
			// １セル分の文字列をリストに追加する
			record.push_back(token);
		}

		// １行分の文字列を出力引数のリストに追加する
		table.push_back(record);
	}

	return true;
}
void outLogs() {
	ofstream logging;
	string filename = "StateLog.csv";
	vector<vector<string>> table;
	bool status = false;
	status= status = GetContents(filename, table);
	logging.open("StateLog.csv", ios::app);
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			logging << gameState[i].body[j][0] << ",";
			logging << gameState[i].body[j][1] << ",";
		}
		for (int j = 0; j < 8; j++) {
			logging << gameState[i].Score[j] << ",";
		}
		logging << gameState[i].LastEnd << ",";
		logging << gameState[i].CurEnd << ",";
		logging << gameState[i].ShotNum << ",";
		logging << gameState[i].WhiteToMove << ",";
		if (shotPos[i].x + shotPos[i].y != 0) {
			logging << shotPos[i].x << ",";
			logging << shotPos[i].y << ",";
			logging << shotPos[i].angle << ",";
			logging << shotPower[i] << endl;
		}
		else logging << endl;
	}
	logging.close();
}




