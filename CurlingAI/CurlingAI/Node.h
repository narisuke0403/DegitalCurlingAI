#pragma once
#include "main.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "divide.h"
const int stateNum = 1350;
const int shotVariation = 16;



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
};
/*
void saveGameState(const GAMESTATE* const gs);
void saveShotAndStateAndPower(const GAMESTATE* const gs,SHOTPOS *pos,float power);
void initState();
void initPos();
void outLogs();
*/