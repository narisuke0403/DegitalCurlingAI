#include "Qlearning.h"

extern vector<unordered_map<string, int>> situation;


//使い方
//引数に、現在のノード、行動した後のノード、とった行動　をいれると、現在のノードのQ値を更新する
void Qlearning(Node *now,Node const next,int indexP,int indexS,int angle) {
	float alpha = 0.5; //どのくらい学習結果を反映させるか 0:一切反映させない　1:更新値自体が学習値になる
	float lamda = 1; //割引率、のちに関数にして対応することも可
	float score = 0;

	int next_max_score = -1;
	for (int i = 0; i < 43200; i++) {
		if (next_max_score == -1 || next.Qtable[next_max_score] < next.Qtable[i]) {
			next_max_score = i;
		}
	}
	now->Qtable[(indexP * 16 + indexS) * 2 + angle] = (1 - alpha) * now->Qtable[(indexP * 16 + indexS) * 2 + angle] + alpha * lamda * (score + next.Qtable[next_max_score]);
}