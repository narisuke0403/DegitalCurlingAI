/*
float Qlearning(const GAMESTATE* const gs, float Qtable[3000][400][6]);
float Qlearning(const GAMESTATE* const gs, float Qtable[3000][400][6]) {//Qtable[s][a][t]

	return 1.0;
}
*/
/*
1.盤面の初期化...ok
2.エンド終了までQテーブルを参照して投げる・s,aを保存
3.2の過程を16投目から順にQテーブルを更新
4.1~3を試合終了まで繰り返す

*/