#pragma once
#include "main.h"
#include "time.h"

using namespace std;

// state of the game
GAMESTATE GameState;
// functions fron CurlingSimulator.dll
HMODULE hCSDLL;
SIMULATION_FUNC Simulation = nullptr;
CREATESHOT_FUNC CreateShot = nullptr;
CREATEHITSHOT_FUNC CreateHitShot = nullptr;
GETSCORE_FUNC GetScore = nullptr;

//! initialize GAMESTATE
void initGameState(GAMESTATE *pgs) {
	memset(pgs->body, 0x00, sizeof(float) * 32);
	memset(pgs->Score, 0x00, sizeof(int) * 10);
	pgs->LastEnd = 0;
	pgs->CurEnd = 0;
	pgs->ShotNum = 0;
	pgs->WhiteToMove = 0;
}

//! send command
void sendCommand(char *message)
{
	DWORD NumberOfBytesWritten;
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), message, strlen(message), &NumberOfBytesWritten, NULL);
}

//! recieve command
void recvCommand(char *message, size_t size)
{
	DWORD NumberOfBytesRead;
	ReadFile(GetStdHandle(STD_INPUT_HANDLE), message, size, &NumberOfBytesRead, NULL);
}

//! load external library
bool LoadFunction()
{
	hCSDLL = LoadLibrary("CurlingSimulator.dll");

	if (hCSDLL != nullptr) {
		Simulation = (SIMULATION_FUNC)GetProcAddress(hCSDLL, "Simulation");
		CreateShot = (CREATESHOT_FUNC)GetProcAddress(hCSDLL, "CreateShot");
		CreateHitShot = (CREATEHITSHOT_FUNC)GetProcAddress(hCSDLL, "CreateHitShot");
		GetScore = (GETSCORE_FUNC)GetProcAddress(hCSDLL, "GetScore");
	}
	else {
		//MessageBox(nullptr, "CurlingSimulator.dll の読み込みに失敗しました", "エラー", MB_OK);
		return false;
	}

	return true;
}

//! delete newline
void DeleteNL(char *Message)
{
	char *p;
	p = Message;

	while (*p != 0x00) {
		if (*p == '\n' || *p == '\r') {
			*p = 0x00;
			break;
		}
		p++;
	}
	return;
}

//! get argument from command
bool GetArgument(char *lpResult, size_t numberOfElements, char *Message, int n)
{
	char *p, *q;

	if (Message != NULL) {
		p = Message;
		while (*p == ' ') {
			p++;
		}

		// ポインタを取得したい引数の先頭に合わせる
		for (int i = 0; i<n; i++) {
			while (*p != ' ') {
				if (*p == 0x00) {
					return false;
				}
				p++;
			}
			while (*p == ' ') {
				p++;
			}
		}

		// 取得したい引数をlpResultにコピーする
		q = strstr(p, " ");
		if (q == NULL) {
			strcpy(lpResult, p);
		}
		else {
			strncpy_s(lpResult, numberOfElements, p, q - p);
			if ((unsigned int)(q - p) < numberOfElements) {
				lpResult[q - p] = 0x00;
			}
		}
	}

	return true;
}

//! process command
bool processCommand(char *command)
{
	char cmd[BUFSIZE];
	char buffer[BUFSIZE];
	SHOTVEC shot;

	cerr << "recieved: " << command << endl;

	// delete newline
	DeleteNL(command);

	// get command
	if (!GetArgument(cmd, sizeof(cmd), command, 0)) {
		return false;
	}
	// process command
	if (_stricmp(cmd, "NEWGAME") == 0) {
	}
	else if (_stricmp(cmd, "ISREADY") == 0) {
		// initialize GameState
		initGameState(&GameState);
		sendCommand("READYOK");
	}
	else if (_stricmp(cmd, "POSITION") == 0) {
		for (int i = 0; i < 16; i++) {
			// get x cordinate
			if (!GetArgument(buffer, sizeof(buffer), command, 2 * i + 1)) {
				return false;
			}
			GameState.body[i][0] = (float)atof(buffer);

			// get y cordinate
			if (!GetArgument(buffer, sizeof(buffer), command, 2 * i + 2)) {
				return false;
			}
			GameState.body[i][1] = (float)atof(buffer);
		}
	}
	else if (_stricmp(cmd, "SETSTATE") == 0) {
		// number of current shots
		if (GetArgument(buffer, sizeof(buffer), command, 1) == FALSE) {
			return false;
		}
		GameState.ShotNum = atoi(buffer);

		// number of current ends
		if (GetArgument(buffer, sizeof(buffer), command, 2) == FALSE) {
			return false;
		}
		GameState.CurEnd = atoi(buffer);

		// number of last
		if (GetArgument(buffer, sizeof(buffer), command, 3) == FALSE) {
			return false;
		}
		GameState.LastEnd = atoi(buffer);
		if (GetArgument(buffer, sizeof(buffer), command, 4) == FALSE) {
			return false;
		}
		if (atoi(buffer) == 1) {
			GameState.WhiteToMove = true;
		}
		else {
			GameState.WhiteToMove = false;
		}
	}
	else if (_stricmp(cmd, "GO") == 0) {
		SHOTVEC vec;
		// get a Shot by getBestShot (defined in 'strategy.cpp')
		getBestShot(&GameState, &vec);
		
		/*
		int stone_num[16];
		get_ranking(stone_num, &GameState);
		int rank = 0;
		stone_num[n]=x
		n : the n th Stone from the center of House
		x : the x th Shot in this End (corresponding to the number of GAMESTATE->body[x])
		*/
		sprintf_s(buffer, sizeof(char) * BUFSIZE, "BESTSHOT %f %f %d", vec.x, vec.y, vec.angle);
		//set my turn's situation and action
		// send BESTSHOT command
		//printf("saveGameState");
		//saveGameState(&GameState);
		sendCommand(buffer);
	}
	else if (_stricmp(cmd, "SCORE") == 0) {
		// get score of previous end
		if (GetArgument(buffer, sizeof(buffer), command, 1) == FALSE) {
			return false;
		}
		GameState.Score[GameState.CurEnd] = atoi(buffer);
		
		//スコアが代入された後に更新
		extern Node *savedNode[8];
		extern int savedIndexP[8];
		extern int savedIndexS[8];
		extern int savedAngle[8];
		for (int i = 0; i < 8; i++) {
			savedNode[i]->gsNode->Score[savedNode[i]->gsNode->CurEnd] = GameState.Score[GameState.CurEnd];
		}
		extern void Qlearning(Node *now, Node next, int indexP, int indexS, int angle);
		for (int i = 7; i >= 0; i--) {
			if (i == 7) {
				if (savedNode[i]->gsNode->Score[GameState.CurEnd] > 0) {
					savedNode[i]->Qtable[(savedIndexP[i] * 16 + savedIndexS[i]) * 2 + savedAngle[i]] = 1;
				}
				else {
					savedNode[i]->Qtable[(savedIndexP[i] * 16 + savedIndexS[i]) * 2 + savedAngle[i]] = -1;
				}
			}
			else {
				Qlearning(savedNode[i], *savedNode[i + 1], savedIndexP[i], savedIndexS[i], savedAngle[i]);
			}
		}
		thread t1(outLogs,savedNode);
		t1.detach();
	}
	else if (_stricmp(cmd, "Node") == 0) {
		float pos[2];
		PolarToCartesian(777, pos);
		cerr << "x: " << pos[0] << " y; " << pos[1] << endl;
	}

	return true;
}


int main()
{

	char message[BUFSIZE];
	// load CurlingSimulator.dll
	if (!LoadFunction()) {
		cerr << "failed to load CurlingSimulator.dll" << endl;
		return 0;
	}

	// process command
	dividePolar();
	while (1) {
		memset(message, 0x00, sizeof(message));
		recvCommand(message, sizeof(message));
		processCommand(message);
	}

	return 0;
}