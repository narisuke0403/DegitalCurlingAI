#ifndef NODE_H
#define NODE_H
#include <stdio.h>
#include <iostream>
#include "main.h"
//#include "strategy.h"
//#include "divide.h"


class Node {
public:
	Node *parent;
	GAMESTATE *gsNode;
	double eval;
	std::vector<Node*> children;
	std::vector<SHOTPOS> *shots;
	Node(const GAMESTATE* const gs);
	void pushChildren(const GAMESTATE* const currGS, const GAMESTATE* const nextGS);
	GAMESTATE* getNextState(const GAMESTATE* const gs);
	void addChildren(const GAMESTATE* const gs);
	double getEval();
};

GAMESTATE* playMontecarlo(const GAMESTATE* const gs);

#endif