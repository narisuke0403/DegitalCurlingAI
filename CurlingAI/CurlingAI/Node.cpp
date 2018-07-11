#pragma once
#include "Node.h"
#include "divide.h"

#include <sstream>
#include <fstream>



Node::Node(const GAMESTATE* const gs) {
	gsNode = new GAMESTATE(*gs);
	loadQtable();
}

void Node::loadQtable() {
	ifstream ifs("Qtable.csv");
	std::string line;
	int count = 0;
	while (getline(ifs, line)) {
		std::vector<std::string> stringvec = split(line, ',');
		for (int i = 0; i < stringvec.size(); i++) {
			Qtable[count] = stof(stringvec.at(i));
			count++;
		}
	}
}

void Node::through() {
	float max = -100;
	int indexP = 0;
	int indexS = 0;
	int angle = 0;
	for (int p = 0; p < shotVariation; p++) {//find highest value and its index
		for (int s = 0; s < stateNum; s++) {
			for (int a = 0; a < 2; a++) {
				if (Qtable[(p * 16 + s) * 2 + a] > max) {
					max = Qtable[(p * 16 + s)];
					indexP = p;
					indexS = s;
					angle = a;
				}
			}
		}
	}
	float pos[2];
	SHOTPOS shot;
	SHOTVEC vec;
	PolarToCartesian((indexP * 16 + indexS) + angle, pos);
	shot.x = pos[0];
	shot.y = pos[1];
	shot.angle = angle;
	GAMESTATE *nextGs = new GAMESTATE(*gsNode);
	CreateHitShot(shot, indexP, &vec);
	Simulation(nextGs, vec, 0.3f, NULL, -1);
	int number = searchPolar(nextGs);
	//std::unordered_map<std::string, int> currState
	//= /*hoge.at(nextGs->ShotNum)*/;
	//auto itr = currState.find(number);        // "xyz" ���ݒ肳��Ă��邩�H
	//if (itr != mp.end()) {
		//�ݒ肳��Ă���ꍇ�̏���
	//}
	//else {
		//�ݒ肳��Ă��Ȃ��ꍇ�̏���
//	}

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
PolarToCartesian(i, pos);//pos�ɑ_�����W����������
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
PolarToCartesian(indexS, pos);//pos�ɑ_�����W����������
float posX = pos[0];
float posY = pos[1];
shot.x = posX;
shot.y = posY;
shot.angle = 0;
CreateHitShot(shot, indexP, &vec);
Simulation(currGS, vec, 0.3f, NULL, -1);
int no = searchPolar(currGS);
//currGS��string���ɕϊ����ē������̂��Ȃ����push
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

//use for Logging
bool GetContents(const string& filename, vector<vector<string>>& table, const char delimiter = ',')
{
	// �t�@�C�����J��
	fstream filestream(filename);
	if (!filestream.is_open())
	{
		// �t�@�C�����J���Ȃ������ꍇ�͏I������
		return false;
	}

	// �t�@�C����ǂݍ���
	while (!filestream.eof())
	{
		// �P�s�ǂݍ���
		string buffer;
		filestream >> buffer;

		// �t�@�C������ǂݍ��񂾂P�s�̕��������؂蕶���ŕ����ă��X�g�ɒǉ�����
		vector<string> record;              // �P�s���̕�����̃��X�g
		istringstream streambuffer(buffer); // ������X�g���[��
		string token;                       // �P�Z�����̕�����
		while (getline(streambuffer, token, delimiter))
		{
			// �P�Z�����̕���������X�g�ɒǉ�����
			record.push_back(token);
		}

		// �P�s���̕�������o�͈����̃��X�g�ɒǉ�����
		table.push_back(record);
	}

	return true;
}
/*
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



*/