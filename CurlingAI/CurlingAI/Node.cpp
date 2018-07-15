#pragma once
#include "Node.h"

#include <sstream>
#include <fstream>
#include <random>
#include <crtdbg.h>
extern vector<unordered_map<string, int>> situation;
Node *savedNode[8];
int savedIndexP[8];
int savedIndexS[8];
int savedAngle[8];
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
	for (int p = 10; p < shotVariation; p++) {//find highest value and its index
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
	random_device rnd;     // �񌈒�I�ȗ���������𐶐�
	mt19937 mt(rnd());     //  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
	uniform_int_distribution<> rand100(0, sameValues->size());        // [0, sameValues.size()] �͈͂̈�l����
	int index = sameValues->at(rand100(mt));
	//cerr << sameValues->size()<<endl;
	int angle = index % 2;
	int tmp = (index - angle) / 2;
	int indexS = tmp%shotVariation;
	int indexP = int(tmp%shotVariation);
	//cerr << "p,s,a" << endl;
	//cerr << indexP << "," << indexS << "," << angle << endl;
	//cerr << (indexP * 16 + indexS) * 2 + angle << endl;
	//cerr << index << endl;
	float pos[2];
	SHOTPOS shot;
	pos[0] = 5;
	pos[1] = 5;

	//cerr << "helloaaaa\n";
	//cerr << indexS << endl;
	PolarToCartesian(indexS, pos);
	//cerr << "hellobbbbb\n";
	shot.x = pos[0];
	shot.y = pos[1];
	shot.angle = angle;
	string key="";
	GAMESTATE *nextGs = new GAMESTATE(*gsNode);
	CreateHitShot(shot, indexP, vec);
	Simulation(nextGs, *vec, 0.3f, NULL, -1);
	int number = searchPolar(nextGs,&key);
	//int number = 1;
	string num = to_string(number);
	std::unordered_map<std::string, int> currState;
	//cerr << "hello\n";
	currState = situation.at(gsNode->ShotNum);
	auto itr = currState.find(num);        // string(number) ���ݒ肳��Ă��邩�H
	next = new Node(nextGs);
	next->parent = this;
	savedNode[(gsNode->ShotNum)/2] = curr;
	savedIndexS[(gsNode->ShotNum) / 2] = indexS;
	savedIndexP[(gsNode->ShotNum) / 2] = indexP;
	savedAngle[(gsNode->ShotNum) / 2] = angle;
	//�����Ă��̃m�[�h�Ɉړ����邽�߂ɑ��
	/*if (itr != currState.end()) {
		//�ݒ肳��Ă���ꍇ�̏���
	}
	else {
		//�ݒ肳��Ă��Ȃ��ꍇ�̏���
		//�m�[�h��ǉ�
		situation.at(gsNode->ShotNum).at(num) = situation.at(gsNode->ShotNum).size() + 1;
	}*/
}


//use for Logging
bool GetContents(const string& filename, vector<vector<string>>& table, const char delimiter = ',')
{
	// �t�@�C�����J��
	fstream filestream(filename);
	if (!filestream.is_open())
	{
		// �t�@�C�����J���Ȃ������ꍇ�͏I������
		cerr << "i can't open the fiel\n";
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
void initState(<< "a";
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

void outLogs(Node *curr[8]) {
	//cerr << searchPolar(curr->gsNode) << endl;
	//cerr << "loadQtable\n";
	ifstream ifs("C:\\DigitalCurlingSimulate\\Release\\Qtable.csv");
	string line;
	int size = 0;
	vector<vector<string>>strvec;
	while (getline(ifs, line)) {
		strvec.push_back(split(line, ','));
	}
	//cerr << "start logging\n";
	ofstream logging;
	string filename = "C:\\DigitalCurlingSimulate\\Release\\Qtable.csv";
	vector<vector<string>> table;
	bool status = false;
	logging.open(filename, ios::trunc);
	logging.close();
	logging.open(filename, ios::app);
	status = status = GetContents(filename, table);
	//cerr << "strvec.size()=" << strvec.size() << endl;
	string polar[8];
	bool isExist[8];
	for (int n = 0; n < 8; n++) {
		//cerr << "--------------------------------------\n";
		//cerr << curr[n]->gsNode->ShotNum << endl;
		//cerr << searchPolar(curr[n]->gsNode) << endl;
		//cerr << "--------------------------------------\n";
		isExist[n] = false;
		searchPolar(curr[n]->gsNode, &polar[n]);
		cerr << polar[n] << endl;
	//	cerr << "now\n";
		for (int i = 0; i < strvec.size(); i++) {
			if (stoi(strvec.at(i).at(0)) == curr[n]->gsNode->ShotNum&& stoi(strvec.at(i).at(1)) == stoi(polar[n])) {
				isExist[n] = true;
				vector<string> newStr;
				newStr.push_back(to_string(curr[n]->gsNode->ShotNum));
				newStr.push_back(polar[n]);
				for (int j = 0; j < stateNum*shotVariation * 2; j++) {
					newStr.push_back(to_string(curr[n]->Qtable[j]));
				}
				strvec.at(i) = newStr;
			}
	//		cerr << "hello\n";
		}
	//	cerr << "here\n";
	}
	for (int i = 0; i < strvec.size(); i++) {
		for (int j = 0; j < strvec.at(i).size(); j++) {
			logging << strvec.at(i).at(j);
			if (j < strvec.at(i).size() - 1)logging << ",";
		}
		cerr << "processing complete rate is" << i << "/" << strvec.size() << endl;
		logging << endl;
	}
	for (int n = 0; n < 8; n++) {
		if (isExist[n])	cerr << "isExist["<<n<<"] ==true" << endl;
		if (!isExist[n])	cerr << "isExist[" << n << "] ==false" << endl;
		if (!isExist[n]) {
			cerr << "no\n";
			logging << curr[n]->gsNode->ShotNum << ",";
			cerr << "yes\n";
			logging << polar[n] << ",";
			cerr << "all number is" << stateNum * shotVariation * 2 << endl;
			for (int i = 0; i < stateNum*shotVariation * 2; i++) {
				if (i % 5000 == 0)cerr << i << "/" << stateNum * shotVariation * 2 << "is ended\n";
				logging << curr[n]->Qtable[i];
				if (i < stateNum*shotVariation * 2 - 1)logging << ",";
			}
			logging << endl;
			cerr << "endlogging" << n << endl;
		}
	}
	cerr << "end logging\n";
	logging.close();
}
/*

*/