#pragma once

#include "main.h"
#include <vector>//ArrayList‚Æ“¯‚¶

/*** constant numbers ***/
static const float RANDOM = (float)0.145;  // sigma of normal random number

//! positions on sheet
static const float TEE_X = (float)2.375;    // x of center of house
static const float TEE_Y = (float)4.880;    // y of center of house
static const float HOUSE_R = (float)1.830;  // radius of house
static const float STONE_R = (float)0.145;  // radius of stone

//! coodinate (x, y) is in playarea IF:
//   (PLAYAREA_X_MIN < x < PLAYAREA_X_MAX && PLAYAREA_Y_MIN < y < PLAYAREA_Y_MAX)
static const float PLAYAREA_X_MIN = (float)0.000 + STONE_R;
static const float PLAYAREA_X_MAX = (float)0.000 + (float)4.750 - STONE_R;
static const float PLAYAREA_Y_MIN = (float)3.050 + STONE_R;
static const float PLAYAREA_Y_MAX = (float)3.050 + (float)8.230 + STONE_R;

void get_ranking(int *rank, const GAMESTATE* const gs);
//! choose best shot 
void getBestShot(const GAMESTATE* const gs, SHOTVEC *vec_ret);

int getScore(const GAMESTATE* const gs, int num);//num=0->1st player 1->2nd player
void selectRandomVecter(const GAMESTATE* const gs, SHOTVEC *vec_ret);

class Vector {
public:
	double x;
	double y;
	double length;
	double theta;
	int stone_rank;
	bool myStone;
	Vector(const GAMESTATE* const gs, int num);
	Vector();
};

class Node {
public:
	int depth;
	GAMESTATE* gs_node;
	std::vector<Node*> child;//“®“I”z—ñ
	Vector* vecs[16];
	Node(const GAMESTATE* const gs,int depth);
};

class Tree {
public:
	Node * root;
	Tree(const GAMESTATE* const gs);
	void addRandomChildren(Node* curr);
	void checkPointer(Node *root);
};
bool CreateHitAndStayShot(const GAMESTATE* const gs, int stone, SHOTVEC* vec_ret);
bool CreateWickShot(const GAMESTATE* const gs, int stone, SHOTVEC* vec_ret);
bool CreateFreezeShot(const GAMESTATE* const gs, int stone, SHOTVEC* vec_ret);
bool EasyCanShotReach(const GAMESTATE* const gs,SHOTPOS pos);


double vec_length(Vector* v);
double innerProduct(Vector* v1, Vector* v2);
//double angle(Vector* v1, Vector* v2);
double angleFromCentor(Vector* v1);