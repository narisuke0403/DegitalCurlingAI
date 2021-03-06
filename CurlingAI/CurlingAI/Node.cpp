#pragma once
#include "Node.h"

#include <sstream>
#include <fstream>
#include <random>
#include <crtdbg.h>
#include <time.h>
#include <Windows.h>
extern vector<unordered_map<string, int>> situation;
Node *savedNode[8];
int savedIndexP[8];
int savedIndexS[8];
int savedAngle[8];
Node::Node(const GAMESTATE* const gs) {
	gsNode = new GAMESTATE(*gs);
	loadQtable();
}

vector<string> checksplit(string& input, char delimiter)
{
	istringstream stream(input);
	string field;
	vector<string> result;
	int count = 0;
	while (getline(stream, field, delimiter)) {
		if (count == 2) {
			return result;
		}
		result.push_back(field);
		count++;
	}
}



//Qテーブルを読み込んで代入
void Node::loadQtable() {
	string loadfilename = "";

	ifstream file;
	file.open("C:\\DigitalCurlingSimulate\\data\\latestversion.txt");
	vector<string> result;
	while (getline(file, loadfilename)) {
		result.push_back(loadfilename);
	}
	loadfilename = result.at(0);
	cerr << loadfilename << endl;
	ifstream ifs(loadfilename);
	if (ifs.is_open()) {
		cerr << "file can open" << endl;
	}
	else {
		cerr << "file cannot open" << endl;
		ifstream ifs("C:\\DigitalCurlingSimulate\\data\\Qtable.csv");
	}
	string line;
	bool isExist = false;
	int size = 0;
	while (getline(ifs, line)) {
		string pos = "";
		int number = searchPolar(gsNode,&pos);
		vector<string> stringvec = checksplit(line, ',');
		KEY = stringvec.at(1);
		//シチュエーションとして現在の状態が保存されているか？
		if (situation.at(gsNode->ShotNum)[KEY] != 0) {
			//シチュエーションとして登録してある場合、それはどこの場所か
			if (situation.at(gsNode->ShotNum)[KEY] == situation.at(gsNode->ShotNum)[pos]) {
				cerr << "find same situation" << endl;
				vector<string> stringvec = split(line, ',');
				cerr << stringvec.at(135);
				for (int i = 2; i < stringvec.size(); i++) {
					//cerr << count << endl;
					Qtable[i-2] = stof(stringvec.at(i));
				}
				isExist = true;
				break;
			}
		}
	}
	if (!isExist) {
		cerr << "cannot find same situation" << endl;
		for (int i = 0; i < size; i++) {
			Qtable[i] = 0;
		}
	}
}

//現ノードでの石の投げ方
void Node::throwAndAddNode(SHOTVEC *vec, Node *curr) {
	float max = 0;
	vector<int> *sameValues = new vector<int>();
	vector<int> *powerValues = new vector<int>();
	float margin = 3;
	//Qテーブルの最大値とそのインデックスを保存
	for (int s = 0; s < stateNum; s++) {
		if (searchRANK(s) > 6) {
			for (int p = 0; p < shotVariation; p++) {//find highest value and its index
				for (int a = 0; a < 2; a++) {
					if (Qtable[((p+1) * s -1) * 2 + a] - max > margin) {
						sameValues->clear();
						powerValues->clear();
						max = Qtable[((p + 1) * s) * 2 + a];
						sameValues->push_back(((p+1) * s -1) * 2 + a);
						powerValues->push_back(p);
					}
					else if (Qtable[((p+1) * s -1) * 2 + a] >= max) {
						sameValues->push_back(((p+1) * s -1) * 2 + a);
						powerValues->push_back(p+1);
					}
				}
			}
		}
	}
	cerr << sameValues->size() << endl;
	cerr << "finish throw search" << endl;
	random_device rnd;     // 非決定的な乱数生成器を生成
	mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	uniform_int_distribution<> rand100(0, sameValues->size());        // [0, sameValues.size()] 範囲の一様乱数
	int rand = rand100(mt);
	int index = sameValues->at(rand);//同じ値のデータをランダムに選択
	//インデックスからそれぞれの値を求める
	int angle = index % 2;
	int tmp = (index - angle) / 2;
	int indexP = powerValues->at(rand);
	int indexS = tmp / indexP;
	//ショットを打つ場所を決めるポジションを初期化
	float pos[2];
	SHOTPOS shot;
	pos[0] = 0;
	pos[1] = 0;

	PolarToCartesian(indexS, pos);//posに座標を代入
	shot.x = pos[0];
	shot.y = pos[1];
	shot.angle = angle;

	string key="";
	GAMESTATE *nextGs = new GAMESTATE(*gsNode);
	//ここで代入されたベクターの値でショットを打つ
	CreateHitShot(shot, indexP, vec);//ショットを打つベクターを生成
	Simulation(nextGs, *vec, 0.3f, NULL, -1);

	//Qテーブル更新用にそれぞれ保存
	savedNode[(gsNode->ShotNum)/2] = curr;
	savedIndexS[(gsNode->ShotNum) / 2] = indexS;
	savedIndexP[(gsNode->ShotNum) / 2] = indexP;
	savedAngle[(gsNode->ShotNum) / 2] = angle;
}


//use for Logging
bool GetContents(const string& filename, vector<vector<string>>& table, const char delimiter = ',')
{
	// ファイルを開く
	fstream filestream(filename);
	if (!filestream.is_open())
	{
		// ファイルが開けなかった場合は終了する
		cerr << "i can't open the fiel\n";
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



void outLogs(Node *curr[8]) {
	//一旦表をstrvecにしまっておく
	string loadfilename = "";

	ifstream file;
	file.open("C:\\DigitalCurlingSimulate\\data\\latestversion.txt");
	vector<string> result;
	while (getline(file, loadfilename)) {
		result.push_back(loadfilename);
	}
	loadfilename = result.at(0);
	cerr << loadfilename << endl;
	ifstream ifs(loadfilename);
	if (ifs.is_open()) {
		cerr << "file can open" << endl;
	}
	else {
		cerr << "file cannot open" << endl;
		ifstream ifs("C:\\DigitalCurlingSimulate\\data\\Qtable.csv");
	}
	string line;
	int size = 0;
	vector<vector<string>>strvec;
	while (getline(ifs, line)) {
		strvec.push_back(split(line, ','));
	}

	ofstream logging;
	time_t now = time(NULL); 
	struct tm *pnow = localtime(&now);
	string filename = "C:\\DigitalCurlingSimulate\\data\\Qtable" + to_string(pnow->tm_hour) + "-" + to_string(pnow->tm_min) + "-" + to_string(pnow->tm_sec) + ".csv";
	cerr << filename << endl;
	vector<vector<string>> table;
	bool status = false;
	logging.open(filename, ios::trunc);//表をすべて消す
	logging.close();
	logging.open(filename, ios::app);//表を追記モードにする
	status = GetContents(filename, table);
	string polar[8];
	bool isExist[8];
	for (int n = 0; n < 8; n++) {//8つのノードに対して
		isExist[n] = false;
		searchPolar(curr[n]->gsNode, &polar[n]);

		//同じターンの場合
		for (int i = 0; i < strvec.size(); i++) {
			//同じターン、状況ならデータを書き換える
			if (stoi(strvec.at(i).at(0)) == curr[n]->gsNode->ShotNum&& strvec.at(i).at(1) == polar[n]) {
				isExist[n] = true;//同じデータが有ったかどうか
				vector<string> newStr;
				newStr.push_back(to_string(curr[n]->gsNode->ShotNum));
				newStr.push_back(polar[n]);
				for (int j = 0; j < stateNum*shotVariation * 2; j++) {
					newStr.push_back(to_string(curr[n]->Qtable[j]));
				}
				strvec.at(i) = newStr;
			}
		}
	}
	//書き換えた後の表を書き出す
	for (int i = 0; i < strvec.size(); i++) {
		for (int j = 0; j < strvec.at(i).size(); j++) {
			logging << strvec.at(i).at(j);
			if (j < strvec.at(i).size() - 1)logging << ",";
		}
		logging << endl;
	}

	//同じデータが無かったノードを追加で書き込む
	for (int n = 0; n < 8; n++) {
		if (!isExist[n]) {
			logging << curr[n]->gsNode->ShotNum << ",";
			logging << polar[n] << ",";
			for (int i = 0; i < stateNum*shotVariation * 2; i++) {
				logging << curr[n]->Qtable[i];
				if (i < stateNum*shotVariation * 2 - 1)logging << ",";
			}
			logging << endl;
		}
	}
	cerr << "end logging\n";
	logging.close();

	ofstream latestversion;
	latestversion.open("C:\\DigitalCurlingSimulate\\data\\latestversion.txt");
	latestversion << filename;
	cerr << filename << endl;
	latestversion.close();
}