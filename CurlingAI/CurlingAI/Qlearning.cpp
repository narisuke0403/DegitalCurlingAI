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
1.�Ֆʂ̏�����...ok
2.�G���h�I���܂�Q�e�[�u�����Q�Ƃ��ē�����Es,a��ۑ�
3.2�̉ߒ���16���ڂ��珇��Q�e�[�u�����X�V
4.1~3�������I���܂ŌJ��Ԃ�

*/