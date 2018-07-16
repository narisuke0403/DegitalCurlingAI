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

//Q�e�[�u����ǂݍ���ő��
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

	//Q�e�[�u���̍ő�l�Ƃ��̃C���f�b�N�X��ۑ�
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
	int index = sameValues->at(rand100(mt));//�����l�̃f�[�^�������_���ɑI��
	
	//�C���f�b�N�X���炻�ꂼ��̒l�����߂�
	int angle = index % 2;
	int tmp = (index - angle) / 2;
	int indexS = tmp%shotVariation;
	int indexP = int(tmp%shotVariation);

	//�V���b�g��łꏊ�����߂�|�W�V������������
	float pos[2];
	SHOTPOS shot;
	pos[0] = 0;
	pos[1] = 0;

	PolarToCartesian(indexS, pos);//pos�ɍ��W����
	shot.x = pos[0];
	shot.y = pos[1];
	shot.angle = angle;


	string key="";
	GAMESTATE *nextGs = new GAMESTATE(*gsNode);
	//�����ő�����ꂽ�x�N�^�[�̒l�ŃV���b�g��ł�
	CreateHitShot(shot, indexP, vec);//�V���b�g��łx�N�^�[�𐶐�
	Simulation(nextGs, *vec, 0.3f, NULL, -1);

	//Q�e�[�u���X�V�p�ɂ��ꂼ��ۑ�
	savedNode[(gsNode->ShotNum)/2] = curr;
	savedIndexS[(gsNode->ShotNum) / 2] = indexS;
	savedIndexP[(gsNode->ShotNum) / 2] = indexP;
	savedAngle[(gsNode->ShotNum) / 2] = angle;
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



void outLogs(Node *curr[8]) {
	ifstream ifs("C:\\DigitalCurlingSimulate\\Release\\Qtable.csv");
	string line;
	int size = 0;
	vector<vector<string>>strvec;
	while (getline(ifs, line)) {//��U�\��strvec�ɂ��܂��Ă���
		strvec.push_back(split(line, ','));
	}
	ofstream logging;
	string filename = "C:\\DigitalCurlingSimulate\\Release\\Qtable.csv";
	vector<vector<string>> table;
	bool status = false;
	logging.open(filename, ios::trunc);//�\�����ׂď���
	logging.close();
	logging.open(filename, ios::app);//�\��ǋL���[�h�ɂ���
	status = status = GetContents(filename, table);
	//cerr << "strvec.size()=" << strvec.size() << endl;
	string polar[8];
	bool isExist[8];
	for (int n = 0; n < 8; n++) {//8�̃m�[�h�ɑ΂���
		//cerr << "--------------------------------------\n";
		//cerr << curr[n]->gsNode->ShotNum << endl;
		//cerr << searchPolar(curr[n]->gsNode) << endl;
		//cerr << "--------------------------------------\n";
		isExist[n] = false;
		searchPolar(curr[n]->gsNode, &polar[n]);
		cerr << polar[n] << endl;
	//	cerr << "now\n";
		for (int i = 0; i < strvec.size(); i++) {
			//�����^�[���A�󋵂Ȃ�f�[�^������������
			if (stoi(strvec.at(i).at(0)) == curr[n]->gsNode->ShotNum&& stoi(strvec.at(i).at(1)) == stoi(polar[n])) {
				isExist[n] = true;//�����f�[�^���L�������ǂ���
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
	//������������̕\�������o��
	for (int i = 0; i < strvec.size(); i++) {
		for (int j = 0; j < strvec.at(i).size(); j++) {
			logging << strvec.at(i).at(j);
			if (j < strvec.at(i).size() - 1)logging << ",";
		}
		cerr << "processing complete rate is" << i << "/" << strvec.size() << endl;
		logging << endl;
	}

	//�����f�[�^�����������m�[�h��ǉ��ŏ�������
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