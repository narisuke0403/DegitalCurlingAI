#include "Node.h"

#define _USE_MATH_DEFINES

using namespace std;
extern vector<string> min1, max1, min2, max2, RANK;
extern int map[100][100];
extern unordered_map<string, int> situation;

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
		PolarToCartesian(1, pos);
		float posX = pos[0];
		float posY = pos[1];
	}
	return currGS;
}
