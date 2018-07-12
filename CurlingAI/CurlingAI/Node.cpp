#pragma once
#include "Node.h"

#include <sstream>
#include <fstream>
#include <random>
#include <crtdbg.h>
extern vector<unordered_map<string, int>> situation;
Node *savedNode[16];
int savedIndexP[16];
int savedIndexS[16];
int savedAngle[16];
Node::Node(const GAMESTATE* const gs) {
	gsNode = new GAMESTATE(*gs);
	loadQtable();
}

void Node::loadQtable() {
	ifstream ifs("C:\DigitalCurling_Ver1.16 - VS2015\Release\Qtable.csv");
	string line;
	bool isExist = false;
	int size = 0;
	while (getline(ifs, line)) {
		while (situation.size() < 16) {
			unordered_map<string, int> line;
			situation.push_back(line);
		}
		int count = 0;
		vector<string> stringvec = split(line, ',');
		size = stringvec.size();
		//0=turn,1=KEY, 2~Qtable
		KEY = stoi(stringvec.at(1));
		if (stoi(stringvec.at(0)) == gsNode->ShotNum) {
			for (int i = 2; i < stringvec.size(); i++) {
				//cerr << count << endl;
				Qtable[count] = stof(stringvec.at(i));
				count++;
			}
			isExist = true;
		}
	}
	if (!isExist) {
		for (int i = 0; i < size; i++) {
			Qtable[i] = 0;
		}
	}
}


void Node::throwAndAddNode(SHOTVEC *vec, Node *next) {
	Node *curr = new Node(gsNode);
	float max = -100;
	vector<int> *sameValues = new vector<int>();
	for (int p = 0; p < shotVariation; p++) {//find highest value and its index
		for (int s = 0; s < stateNum; s++) {
			for (int a = 0; a < 2; a++) {
				if (Qtable[(p * 16 + s) * 2 + a] > max) {
					if(max!=-100)free(sameValues);
					max = Qtable[(p * 16 + s)];
					sameValues->push_back((p * 16 + s) * 2 + a);
				}
				else if (Qtable[(p * 16 + s) * 2 + a] == max) {
					sameValues->push_back((p * 16 + s) * 2 + a);
				}
			}
		}
	}  
	random_device rnd;     // 非決定的な乱数生成器を生成
	mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	uniform_int_distribution<> rand100(0, sameValues->size());        // [0, sameValues.size()] 範囲の一様乱数
	int index = sameValues->at(rand100(mt));
	int angle = index % 2;
	int indexS = ((index - (index % 2)) % (stateNum * 2)) / 2;
	int indexP = int(index / (stateNum * 2));
	cerr << "p,s,a" << endl;
	cerr << indexP << "," << indexS << "," << angle << endl;
	cerr << (indexP * 16 + indexS) * 2 + angle << endl;
	cerr << index << endl;
	float pos[2];
	SHOTPOS shot;
	pos[0] = 5;
	pos[1] = 5;

	cerr << "helloaaaa\n";
	PolarToCartesian(indexS, pos);
	cerr << "hellobbbbb\n";
	shot.x = pos[0];
	shot.y = pos[1];
	shot.angle = angle;
	GAMESTATE *nextGs = new GAMESTATE(*gsNode);
	CreateHitShot(shot, indexP, vec);
	Simulation(nextGs, *vec, 0.3f, NULL, -1);
	cerr << __LINE__;
	int number = searchPolar(nextGs);
	cerr << __LINE__;
	//int number = 1;
	string num = to_string(number);
	std::unordered_map<std::string, int> currState;
	cerr << "hello\n";
	currState = situation.at(gsNode->ShotNum);
	auto itr = currState.find(num);        // string(number) が設定されているか？
	next = new Node(nextGs);
	next->parent = this;
	savedNode[gsNode->ShotNum] = curr;
	savedIndexS[gsNode->ShotNum] = indexS;
	savedIndexP[gsNode->ShotNum] = indexP;
	savedAngle[gsNode->ShotNum] = angle;
	//投げてそのノードに移動するために代入
	/*if (itr != currState.end()) {
		//設定されている場合の処理
	}
	else {
		//設定されていない場合の処理
		//ノードを追加
		situation.at(gsNode->ShotNum).at(num) = situation.at(gsNode->ShotNum).size() + 1;
	}*/
}


//use for Logging
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

void Node::saveNode() {
	ofstream logging;
	string filename = "nodeLog.csv";
	vector<vector<string>> table;
	bool status = false;
	status = status = GetContents(filename, table);
	logging.open("nodeLog.csv", ios::app);
	logging << gsNode->ShotNum <<",";
	logging << KEY << ",";
	for (int p = 0; p < shotVariation; p++) {//find highest value and its index
		for (int s = 0; s < stateNum; s++) {
			for (int a = 0; a < 2; a++) {
				logging << Qtable[(p * 16 + s) * 2 + a];
				if (!(p == shotVariation - 1 && s == stateNum - 1 && a == 1)) {
					logging << ",";
				}
			}
		}
	}
	logging.close();
}


/*
Node::Node(const GAMESTATE* const gs) {
gsNode	= new GAMESTATE(*gs);
eval = getScore();
setQtable();
}
void Node::setQtable() {
for (int i = 0; i < stateNum; i++) {
for (int j = 0; j < shotVariation; j++) {
Qtable[i*stateNum + j] = 0;
}
}
}
double Node::getScore() {
double a;
a = 3.0 / 2.0;
return a;
}

void Node::updateQ(Node *curr) {

}
void Node::changeEval() {
Node *curr = this;
while (curr->gsNode->ShotNum != 0) {
updateQ(curr);
curr = curr->parent;
}
}

void Node::addChildrenByMontecarlo(const GAMESTATE* const gs) {
float pos[2];
SHOTVEC vec;
GAMESTATE *currGS;
for (int i = 0; i < situation.size(); i++) {
for (int p = 0; p <= 16; p++) {
currGS = new GAMESTATE(*gs);
SHOTPOS shot;
PolarToCartesian(i, pos);//posに狙う座標が代入される
float posX = pos[0];
float posY = pos[1];
shot.x = posX;
shot.y = posY;
shot.angle = 0;
CreateHitShot(shot, p, &vec);
Simulation(currGS, vec, 0.3f, NULL, -1);
shoted->push_back(shot);
power->push_back(p);
gsNode = currGS;
Node tmp = Node(gsNode);
if (tmp.eval >= 0000000) {
tmp.parent = this;
children.push_back(&tmp);
}
}
}
}

void Node::addChildrenByQ(const GAMESTATE* const gs) {
float pos[2];
SHOTVEC vec;
GAMESTATE *currGS;
currGS = new GAMESTATE(*gs);
SHOTPOS shot;
int indexS;
int indexP;
int max = -9999;
for (int j = 0; j < shotVariation; j++) {
for (int i = 0; i < stateNum; i++) {
int qval = Qtable[j*shotVariation + i];
if (qval > max) {
max = qval;
indexS = i;
indexP = j;
}
}
}
PolarToCartesian(indexS, pos);//posに狙う座標が代入される
float posX = pos[0];
float posY = pos[1];
shot.x = posX;
shot.y = posY;
shot.angle = 0;
CreateHitShot(shot, indexP, &vec);
Simulation(currGS, vec, 0.3f, NULL, -1);
int no = searchPolar(currGS);
//currGSをstring情報に変換して同じものがなければpush
//if(states.at(no)==(string)0){}
Node tmp = Node(gsNode);
}



*/
/*
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
*/



void outLogs(Node *curr) {
	ofstream logging;
	string filename = "Qtable.csv";
	vector<vector<string>> table;
	bool status = false;
	status = status = GetContents(filename, table);
	logging.open(filename, ios::app);
	logging << curr->gsNode->ShotNum << ",";
	logging << searchPolar(curr->gsNode) << ",";
	for (int i = 0; i < stateNum*shotVariation*2; i++) {
		logging << curr->Qtable[i];
		if (i < stateNum*shotVariation * 2 - 1)logging << ",";
	}
	logging.close();
}
