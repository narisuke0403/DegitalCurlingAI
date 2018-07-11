#pragma once
#include "main.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

//GAMESTATE gameState[16];
//SHOTPOS shotPos[16];
//float shotPower[16];
const int stateNum = 1350;
const int shotVariation = 16;
std::unordered_map<std::string, int> situation;



class Node {
public:
	GAMESTATE * gsNode;
	Node *parent;
	double eval;
	int depth;
	float Qtable[stateNum*shotVariation * 2];
	std::vector<std::vector<std::string> > states;
	std::vector<Node*> children;

	Node(const GAMESTATE* const gs);
	void loadQtable();
	void through();

	/*
	void addChildrenByMontecarlo(const GAMESTATE* const gs);
	void addChildrenByQ(const GAMESTATE* const gs);
	double getScore();
	void changeEval();
	void setQtable();
	void updateQ(Node *curr);
	*/
};
/*
void saveGameState(const GAMESTATE* const gs);
void saveShotAndStateAndPower(const GAMESTATE* const gs,SHOTPOS *pos,float power);
void initState();
void initPos();
void outLogs();
*/