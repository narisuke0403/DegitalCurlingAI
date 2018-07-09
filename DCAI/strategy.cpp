#include "strategy.h"
#include "time.h"
#include "math.h"

using namespace std;

//! get distance^2 from center of House
float get_dist(float x, float y)
{
	return pow(x - TEE_X, 2) + pow(y - TEE_Y, 2);
}

//! get distance^2 of two cordinates
float get_dist(float x1, float y1, float x2, float y2)
{
	return pow(x1 - x2, 2) + pow(y1 - y2, 2);
}

//! is a Stone in House
bool is_in_House(float x, float y)
{
	if (get_dist(x, y) < pow(HOUSE_R + STONE_R, 2)) {
		return true;
	}
	else {
		return false;
	}
}

//! sort Shot number (rank[] = {0, 1, 2 ... 15})
//  by distance from center of House (TEEX, TEEY)
void get_ranking(int *rank, const GAMESTATE* const gs)
{
	// init array
	for (int i = 0; i < 16; i++) {
		rank[i] = i;
	}

	// sort
	int tmp;
	for (int i = 1; i < gs->ShotNum; i++) {
		for (int j = i; j > 0; j--) {
			if (get_dist(gs->body[rank[j]][0], gs->body[rank[j]][1]) < get_dist(gs->body[rank[j - 1]][0], gs->body[rank[j - 1]][1])) {
				// swap
				tmp = rank[j];
				rank[j] = rank[j - 1];
				rank[j - 1] = tmp;
			}
			else {
				break;
			}
		}
	}
}

//! get best Shot to takeout the objective stone
void CreateTakeoutShot(const GAMESTATE* const gs, unsigned int num_target, SHOTVEC *vec_ret)
{
	/*** this is a sample of 'Simulation' function ***/

	const unsigned int shot_variations = 2;
	SHOTPOS target;
	SHOTVEC vec[shot_variations];

	// get position of target stone
	target.x = gs->body[num_target][0];
	target.y = gs->body[num_target][1];

	// get a HitShot (right Curl)
	target.angle = 0;
	CreateHitShot(target, 16, &(vec[0]));

	// get a HitShot (left Curl)
	target.angle = 1;
	CreateHitShot(target, 16, &(vec[1]));

	// try a shot 100 times
	const unsigned int trials = 100;
	unsigned int count[shot_variations] = { 0, 0 };

	// create temporary GAMESTATE
	GAMESTATE *gstmp;
	gstmp = (GAMESTATE *)malloc(sizeof(GAMESTATE));

	// simulate each shot
	for (unsigned int i = 0; i < trials; i++) {
		for (int j = 0; j < shot_variations; j++) {
			// copy current state to temporary state
			memcpy_s(gstmp, sizeof(GAMESTATE), gs, sizeof(GAMESTATE));

			// Simulation
			Simulation(gstmp, vec[j], RANDOM, NULL, -1);

			// check objective stone was in House or not
			// NOTE: 'Simulation' rewites gstmp as a state after the simulation
			if ( !is_in_House(gstmp->body[num_target][0], gstmp->body[num_target][1])) {
				count[j]++;
			}
		}
	}

	// calculate accuracy of Takeout Shot
	unsigned int best_num = 0;
	unsigned int count_max = trials + 1;
	for (int i = 0; i < shot_variations; i++) {
		if (count_max < count[i]) {
			count_max = count[i];
			best_num = i;
		}


		// NOTE: use cerr if you want output something on command line
		cerr << "shot[" << i << "] (" << count[i] << "/" << trials << ")" << endl;
	}

	cerr << "best_num = " << best_num << endl;

	memcpy_s(vec_ret, sizeof(SHOTVEC), &(vec[best_num]), sizeof(SHOTVEC));
	
}

//! choose best Shot
//  This sample choose:
//    1.Takeout Shot: hit and push the objective Stone away from House 
//      - IF No.1 Stone (the nearest Stone from center of House) is opponent Stone.
//    2.Guard Shot: put a Stone as hiding the objective Stone.
//      - IF No.1 Stone is my Stone.
//    3.Tee Shot: put a Stone on center of House
//      - IF there is no Stone in House
void getBestShot(const GAMESTATE* const gs, SHOTVEC *vec_ret)
{
	SHOTPOS pos;
	// ranking of Shot number
	// rank[n] = x;
	//   n : the n th Stone from the center of House
	//   x : the x th Shot in this End (corresponding to the number of GAMESTATE->body[x])
	int rank[16];

	// sort by distane from center
	get_ranking(rank, gs);

	// create Shot according to condition of No.1 Stone
	if (is_in_House(gs->body[rank[0]][0], gs->body[rank[0]][1]))
	{
		// get position of the objective Stone
		pos.x = gs->body[rank[0]][0];
		pos.y = gs->body[rank[0]][1];

		if (rank[0] % 2 != gs->ShotNum % 2) {
			// choose Shot 1.
			if (0) {
				// without simulation
				if (gs->body[rank[0]][0] < TEE_X) {
					pos.angle = 1;
					//turn left
				}
				else {
					pos.angle = 0;
					//turn right
				}

				// generate a Shot(SHOTVEC) by CreateHitShot (pass the objective position)
				CreateHitShot(pos, 16, vec_ret);
			}
			else {
				// with simulation
				CreateTakeoutShot(gs, rank[0], vec_ret);
			}
		}
		else {
			// choose Shot 2.
			// generate a Shot by CreateShot (put a Stone on objective position: pos)
			pos.y += 2.0;
			if (gs->body[rank[0]][0] < TEE_X) {
				pos.angle = 0;
			}
			else {
				pos.angle = 1;
			}

			CreateShot(pos, vec_ret);
		}
	}
	else {
		// choose Shot 3.

		// set the objective position as center of House
		pos.x = TEE_X;
		pos.y = TEE_Y;
		pos.angle = 0;

		// generate a Shot by CreateShot (put a Stone on objective position: pos)
		CreateShot(pos, vec_ret);
	}
	/*
	GAMESTATE* gs_copy = new GAMESTATE(*gs);
	Simulation(gs_copy, *vec_ret, 0.30f, NULL, -1);
	printf("OK");
	*/
}

//get the score
int getScore(const GAMESTATE* const gs,int num) {
	int score[2];
	score[0] = 0;//score of 1st player 
	score[1] = 0;//score of 2nd player
	for (int i = 0; i < 10; i++) {
		if (gs->Score[i] < 0) {
			score[0] += gs->Score[i];
		}
		else if(gs->Score[i]>0){
			score[1] += gs->Score[i];
		}
		else {
			
		}
	}   
	return score[num];
}


//make random shot
void selectRandomVecter(const GAMESTATE* const gs, SHOTVEC *vec) {
	SHOTPOS pos; 
	for (int i = 0; i < 5; i++) {
		rand();
	}
	pos.x = (float)( ((float)rand() / 32768.0) * 4.75); //from 0 to 4.75
	pos.y = (float)(((float)rand() / 32768.0) * 11.28); //from 0 to 11.28
	pos.angle = ((int)rand())%2;//0 or 1

	//printf("%s%d%s%f%s%f", "angle=", pos.angle,"x=",pos.x,"y=",pos.y); //to debug

	CreateShot(pos, vec); //set values to vec in accordance with pos.x,y,angle

}

Tree::Tree(const GAMESTATE* const gs) {
	root = new Node(gs,0);
}

Node::Node(const GAMESTATE* const gs,int d) {
	gs_node= new GAMESTATE(*gs);
	depth = d; 
	for (int i = 0; i < 16; i++) {
		vecs[i] =new Vector() ;
	}
}


Vector::Vector() {}
Vector::Vector(const GAMESTATE* const gs, int rank) {
	x = gs->body[rank][0] - 2.375;//distance from (0,0)
	y = gs->body[rank][1] - 4.88;
	length = vec_length(this);
	theta = angleFromCentor(this);
	stone_rank = rank;
	if (rank % 2 == gs->ShotNum % 2)myStone=true;//when my stone
	else myStone = false;
}
double vec_length(Vector* v) {
		return sqrt(v->x*v->x + v->y*v->y);
}
double innerProduct(Vector* v1, Vector* v2) {
		return v1->x*v2->x + v1->y*v2->y;
}
/*
double angle(Vector* v1, Vector* v2) {//angle between v1 and v2
	double length1 = dist(v1);
	double length2 = dist(v2);
	double inner = innerProduct(v1, v2);
	double cos = inner / (length1*length2);
	return acos(cos);
}
*/

double angleFromCentor(Vector* v1) {
	double PI = 3.141592653589793;
	//return atan2(v1->y, v1->x) //radian
	return 180*(atan2(v1->y,v1->x))/PI;//degree
}

void Tree::addRandomChildren(Node* curr) {
	SHOTVEC vec;
	if (curr->depth < 2) {
		if (curr->child.size() < 10) {//add Parallels children to curr
			//printf("%d\n", curr->child.size());
			Node *copyNode = new Node(curr->gs_node,curr->depth+1);
			selectRandomVecter(copyNode->gs_node, &vec);
			Simulation(copyNode->gs_node, vec, 0.30f, NULL, -1);//to simulate
			curr->child.push_back(copyNode);//add child
			addRandomChildren(curr);//recursive
		}
		for (int i = 0; i < (int)curr->child.size(); i++) {//add next child to Paralells children
			Node *copyNode = new Node(curr->child.at(i)->gs_node, curr->child.at(i)->depth + 1);
			selectRandomVecter(copyNode->gs_node, &vec);
			Simulation(copyNode->gs_node, vec, 0.30f, NULL, -1);//to simulate
			curr->child.at(i)->child.push_back(copyNode);//add child
			addRandomChildren(curr->child.at(i));//recursive
		}
		return;
	}
	return;
}

void Tree::checkPointer(Node *curr) {//check Nodes not to be null
	for (int i = 0; i < (int)curr->child.size(); i++) {
		printf("%p\n", &curr->child.at(i));
		for (int j = 0; j < (int)curr->child.at(i)->child.size(); j++) {
			printf("%p\n", &curr->child.at(i)->child.at(j));
		}
	}
}

//remove opponent's stone and stay my stone 
bool CreateHitAndStayShot(const GAMESTATE* const gs, int stone, SHOTVEC* vec_ret) {
	if (stone % 2 == gs->ShotNum % 2)return false;//when my stone
	//if moving second, return false
	//if moving first, go next

	SHOTPOS pos;
	pos.x = gs->body[stone][0];
	pos.y = gs->body[stone][1] + (float)3.0*STONE_R;
	pos.angle = 1;//turn left
	CreateHitShot(pos, 14, vec_ret);
	if (EasyCanShotReach(gs, pos)) {
		return true;
	}

	pos.angle = 0;//turn right
	CreateHitShot(pos, 14, vec_ret);
	if (EasyCanShotReach(gs, pos)) {
		return true;
	}
	return false;
}

//if guardstone is not exist, return true
bool EasyCanShotReach(const GAMESTATE* const gs, SHOTPOS pos) {
	if (pos.angle == 1) {//turn left
		float X[16];
		float Y[16];
		for (int k = 0; k < gs->ShotNum; k++) {
			X[k] = gs->body[k][0];
			Y[k] = gs->body[k][1];
			if (X[k] > pos.x&&Y[k] > pos.y&&11.28 > Y[k] && Y[k] > (((11.28 - pos.y) / 6.1)*(X[k] - pos.x) + pos.y)) {
				printf("easyShot=false");
				return false;
			}
		}
	}
	else {//turn right
		float X[16];
		float Y[16];
		for (int k = 0; k < gs->ShotNum; k++) {
			X[k] = gs->body[k][0];
			Y[k] = gs->body[k][1];
			if (X[k] < pos.x&&Y[k] > pos.y&&11.28 > Y[k] && Y[k] >  (((11.28 - pos.y) / (-6.1))*(X[k] - pos.x) + pos.y)) {
				printf("easyShot=false");
				return false;
			}
		}

	}
	printf("easyShot=true");
	return true;
}

//hit opponent's stone not to go out of field
bool CreateWickShot(const GAMESTATE* const gs, int stone, SHOTVEC* vec_ret) {
	if (stone % 2 == gs->ShotNum % 2)return false;//when my turn
												  //if moving second, return false
												  //if moving first, go next

	SHOTPOS pos;
	pos.x = gs->body[stone][0];
	pos.y = gs->body[stone][1] + (float)3.0*STONE_R;
	pos.angle = 1;//turn left
	CreateHitShot(pos, 10, vec_ret);
	if (EasyCanShotReach(gs, pos)) {
		return true;
	}

	pos.angle = 0;//turn right
	CreateHitShot(pos, 10, vec_ret);
	if (EasyCanShotReach(gs, pos)) {
		return true;
	}
	return false;

}

//attach to opponent's stone
bool CreateFreezeShot(const GAMESTATE* const gs, int stone, SHOTVEC* vec_ret) {
	if (stone % 2 == gs->ShotNum % 2)return false;//when my turn
												  //if moving second, return false
												  //if moving first, go next
	SHOTPOS pos;
	pos.x = gs->body[stone][0];
	pos.y = gs->body[stone][1] + (float)3.0*STONE_R;
	pos.angle = 1;//turn left
	CreateShot(pos,vec_ret);
	if (EasyCanShotReach(gs, pos)) {
		return true;
	}

	pos.angle = 0;//turn right
	CreateShot(pos, vec_ret);
	if (EasyCanShotReach(gs, pos)) {
		return true;
	}
	return false;

}