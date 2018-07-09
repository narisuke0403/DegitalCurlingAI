
#ifndef DLLAPI
#define DLLAPI extern "C" __declspec(dllexport)
#endif

//! state of the game
typedef struct _GAMESTATE{
	int		ShotNum;		// number of Shot
							// if ShotNum = n, next Shot is (n+1)th shot in this End

	int		CurEnd;			// (number of current end) - 1
	int		LastEnd;		// number of final End
	int		Score[10];		// score of each End (if Score < 0: First player in 1st End scored)
	bool	WhiteToMove;	// Which player will shot next
							// if WhiteToMove = 0: First player in 1st End will shot next, 
                            //  else (WhiteToMove = 1) : Second player will shot next

	float	body[16][2];	// body[n][0] : x of cordinate of n th stone
							// body[n][1] : y of cordinate of n th stone

} GAMESTATE, *PGAMESTATE;

// Shot
//! position on a Sheet of Curling
typedef struct _ShotPos{
	float x;
	float y;
	bool angle;

} SHOTPOS, PSHOTPOS;

//! vector of shot
typedef struct _Shotvec{
	float x;
	float y;
	bool angle;

} SHOTVEC, PSHOTVEC;

//! functions from CurlingSimulator.dll
//  if you don't need these functions, please delete below
/*
DLLAPI int Simulation( GAMESTATE *pGameState, SHOTVEC Shot, float Rand, SHOTVEC *lpResShot, int LoopCount );
DLLAPI int CreateShot( SHOTPOS ShotPos, SHOTVEC *lpResShotvec );
DLLAPI int CreateHitShot( SHOTPOS Shot, float Power, SHOTVEC *lpResShot );
DLLAPI int GetScore( GAMESTATE *pGameState );
*/
typedef int (*SIMULATION_FUNC)( GAMESTATE *pGameState, SHOTVEC Shot, float Rand, SHOTVEC *lpResShot, int LoopCount );
typedef int (*CREATESHOT_FUNC)( SHOTPOS ShotPos, SHOTVEC *lpResShotvec );
typedef int (*CREATEHITSHOT_FUNC)( SHOTPOS Shot, float Power, SHOTVEC *lpResShot );
typedef int (*GETSCORE_FUNC)( GAMESTATE *pGameState );
