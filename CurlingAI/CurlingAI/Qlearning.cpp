#include <stdio.h>
#include <iostream>
#include "main.h"
#include "strategy.h"
extern GAMESTATE situation[16];
extern SHOTVEC action[16];

float Qlearning(const GAMESTATE* const gs, float Qtable[3000][400][6],int turn);
float Qlearning(const GAMESTATE* const gs, float Qtable[3000][400][6],int turn) {//Qtable[s][a][t]
	float gamma = 0.99;
	float alpha = 0.5;
	float next_max_Q;
	float max = -9999;
	int s = 0;
	//s = getSituation();
	for (int i = 0; i < 400; i++) {
		if (max < Qtable[s][i][turn])max = Qtable[s][i][turn];
	}

	return 1.0;
}

/*
1.盤面の初期化...ok
2.エンド終了までQテーブルを参照して投げる・s,aを保存
3.2の過程を16投目から順にQテーブルを更新
4.1~3を試合終了まで繰り返す

*/