#pragma once
#include "main.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "divide.h"
const int stateNum = 1315;
const int shotVariation = 16;


class Node {
public:
	GAMESTATE * gsNode;
	Node *parent;
	double eval;
	std::string KEY;
	float Qtable[stateNum*shotVariation * 2];
	std::vector<std::vector<std::string> > states;
	std::vector<Node*> children;

	Node(const GAMESTATE* const gs);
	void loadQtable();
	void throwAndAddNode(SHOTVEC *vec, Node *next);
};
void outLogs(Node *curr[8]);