#include "Qlearning.h"

extern vector<unordered_map<string, int>> situation;


//�g����
//�����ɁA���݂̃m�[�h�A�s��������̃m�[�h�A�Ƃ����s���@�������ƁA���݂̃m�[�h��Q�l���X�V����
float Qlearning(Node* now, Node next, int indexP, int indexS, int angle) {
	float alpha = 0.5; //�ǂ̂��炢�w�K���ʂ𔽉f�����邩 0:��ؔ��f�����Ȃ��@1:�X�V�l���̂��w�K�l�ɂȂ�
	float lamda = 1; //�������A�̂��Ɋ֐��ɂ��đΉ����邱�Ƃ���
	//float score = 0;
	int score = 0;// now->gsNode->Score[now->gsNode->CurEnd];
	for (int i = 0; i < stateNum * 16 * 2;i++) {
		score += next.Qtable[i];
	}
	score = score / (stateNum * 16 * 2);

	int next_max_score = -100;
	for (int i = 0; i < 1315*16*2; i++) {
		if (next_max_score == -1 || next.Qtable[next_max_score] < next.Qtable[i]) {
			next_max_score = i;
		}
	}
	return (1 - alpha) * now->Qtable[((indexP + 1) * indexS - 1) * 2 + angle] + alpha * lamda * (score + next.Qtable[next_max_score]);
}