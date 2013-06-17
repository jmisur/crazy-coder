/*
	nazov: Crazy Coder
	platforma: Unix/Win32
	autor: Juraj Misur
	email: misur@matfyz.cz
	web: http://misur.matfyz.cz/mff/prg020
	org: MFF UK in Prague, winter semester 2005/2006
*/

#include "Network.h"
#include "../SDL/include/SDL_image.h"
#include "../SDL/include/SDL.h"
#include "../Shared/Gfx.h"
#include "../Shared/CSprite.h"
#include "../Shared/Defines.h"
#include "../Shared/CBaseEntity.h"
#include "CBlock.h"
#include "CBomb.h"
#include "CBonus.h"
#include "CExplosion.h"
#include "CMap.h"
#include "CPlayer.h"
#include "../Shared/CTile.h"
#if defined (_WIN32)
	#include <time.h>
#endif


bool debug;
extern SDL_Surface *scr;
extern SOCKET startupServerForListening(unsigned short port);
extern SOCKET startupClient(unsigned short port, const char* serverName);
extern void shutdownClient(SOCKET &remoteSocket);
extern void shutdownServer(SOCKET &socket);

Uint8		*keystates;
SDL_Surface	*scr;	

CSprite		spr_tile;
CSprite		spr_player[4];	
CSprite		spr_bomb;
CSprite		spr_background;
CSprite		spr_explosion;
CSprite		spr_block;
CSprite		spr_bonus;
CPlayer		player[4];
CPlayer		*myplayer, *remoteplayer;
CMap		map;

vector<CBomb>					vecBombs;
vector<CBomb>::iterator			itBomb;
vector<CExplosion>				vecExplosions;
vector<CExplosion>::iterator	itExplosion;
vector<CBlock>					vecBlocks;
vector<CBlock>::iterator		itBlock;
vector<CBonus>					vecBonuses;	
vector<CBonus>::iterator		itBonus;
deque<CPacket>					deqPackets;
deque<CPacket>::iterator		itPacket;

SOCKET remoteSocket, mySocket;
bool server, multi;
SFont_Font* Font; 
int port;
char ip[16];
GAME_STATE gameState = MENU;
bool netThreadActive = false;
bool netThreadDied = false;

#if defined (_WIN32)
	HANDLE mutexHandle;
	HANDLE threadHandle;
#elif defined (__unix__)
	pthread_mutex_t mutexHandle;
	pthread_t threadHandle;
#endif

int DrawWorld();
int GameCoreSingle();
void Disconnected();
void DrawRound();
int GameCoreMulti();
int Clear();
void Text(int a, int b, char *s);
int NewRound();
void Menu();
void DrawIP();
void DrawPort(char *port);
void GetIP();
int ConvertPort(char *p);
void GetPort();
void NetThread();
void NetThreadEnd();
void Connect();
int InitGame();
int InitRound();
int DrawWins();
void ResetWins();
void Game();
int main(int argc, char *argv[]);

// draws everything
int DrawWorld()
{
	//vykreslim pozadie
	spr_background.Draw();

	//vykreslim mapu
	map.Draw();

	//vykreslim bomby, ak nejake su
	if (!vecBombs.empty())
		for(itBomb = vecBombs.begin(); itBomb != vecBombs.end(); itBomb++) itBomb->Draw();

	//vykreslim explozie, ak nejake su
	if (!vecExplosions.empty())
		for(itExplosion = vecExplosions.begin(); itExplosion != vecExplosions.end(); itExplosion++) itExplosion->Draw();

	//vykreslim bonusy, ak nejake su
	if(!vecBonuses.empty())
			for(itBonus = vecBonuses.begin(); itBonus != vecBonuses.end(); itBonus++) itBonus->Draw();
	if(!vecBlocks.empty())
	{
		for(itBlock = vecBlocks.begin(); itBlock != vecBlocks.end(); itBlock++)
		{
			if (itBlock->Status() == ALIVE)	itBlock->Draw();
			else if(itBlock->Status() == DEADANIM) itBlock->DeadAnim();
		}
	}
	//vykreslim playerov
	if (player[0].Status() == ALIVE) player[0].Draw();
	else if (player[0].Status() == DEADANIM) player[0].DeadAnim();
	if (player[1].Status() == ALIVE) player[1].Draw();
	else if (player[1].Status() == DEADANIM) player[1].DeadAnim();		 

	return 0;
}

// single player game
// checking for player action, bomb ticks, random bonuses, etc.
int GameCoreSingle()
{
	ENTITY_TYPE pom;
	bool done = false;
	int	frame, wait, ran;
	SDL_Event event;

	while (!done)
	{
		frame = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event)) 
		{
			switch(event.type)
			{
			case SDL_QUIT: gameState = GAME_QUIT; return 0;
			}
		}

		if(keystates[SDLK_ESCAPE])
		{
			gameState = MENU;
			return 0;
		}
		
		// checking if some explosion died, then erased from vector and map
		//if (debug) printf("Explosions think.\n");		
		itExplosion = vecExplosions.begin();
		while (1)
		{	
			if (itExplosion == vecExplosions.end()) break;
			if (itExplosion->think()) itExplosion++;
			else 
			{
				map.PopExplosion(itExplosion->GetMapX(), itExplosion->GetMapY());
				itExplosion = vecExplosions.erase(itExplosion);
			}	
		}
	
		// checking if some block died, then erased from map, vector, created new random
		// bonus and put into map and vector

		//if (debug) printf("Blocks think.\n");		
		itBlock = vecBlocks.begin();
		while (1)
		{
			if (itBlock == vecBlocks.end()) break;
			if (itBlock->Status() == DEAD) 
			{
				map.SetNotSolid(itBlock->GetMapX(),itBlock->GetMapY());
				
				ran = rand()%20;
				if(debug) printf("%d", ran);
				if(ran >= 5) 
				{
					if(ran < 10) pom = BONUS_BOMB;
					else if (ran < 15) pom = BONUS_FLAME;
					else if (ran < 20) pom = BONUS_SPEED;
					CBonus tmp(itBlock->GetX(), itBlock->GetY(), pom);
					tmp.SetSprite(&spr_bonus);
					vecBonuses.push_back(tmp);
					map.PushBonus(itBlock->GetMapX(), itBlock->GetMapY(), pom);
				}
				itBlock = vecBlocks.erase(itBlock);	
			}
			else if (itBlock->Status() == ALIVE) {itBlock->Think(); itBlock++;}
			else break;
		}

		// checking if some bonus died
		itBonus = vecBonuses.begin();
		while(1)
		{
			if (itBonus == vecBonuses.end()) break;
			//if (debug) printf("mam bonus");
			if (itBonus->Status() == DEAD) {itBonus = vecBonuses.erase(itBonus); }
			else if (itBonus->Status() == ALIVE) {itBonus->Think();itBonus++;}
		}

		// checking if some bomb exploded, then make explosions
		//if (debug) printf("Bomb think.\n");		
		itBomb= vecBombs.begin(); 
		while (1)
		{	
			if (itBomb== vecBombs.end()) break;
			if (itBomb->think()) itBomb++;
			else 
			{
				map.PopBomb(itBomb->GetMapX(), itBomb->GetMapY());
				itBomb->MakeExplosions();
				itBomb= vecBombs.erase(itBomb);	
			}
		}

		// players can think! :)
		//if (debug) printf("Playes think.\n");		
		if (player[0].Status() == ALIVE) player[0].Think();
		else if (player[0].Status() == DEADANIM) player[0].DeadAnim();

		if (player[1].Status() == ALIVE) player[1].Think();
		else if (player[1].Status() == DEADANIM) player[1].DeadAnim();

		// some player status checks
		if ((player[0].Status() == DEAD) && ((player[1].Status() == DEAD) || (player[1].Status() == DEADANIM)))
		{
			DrawRound();
			if (debug) printf("\nDrawRound");
			gameState = NEW_ROUND;
			return 0;
		}
		else if ((player[1].Status() == DEAD) && ((player[0].Status() == DEAD) || (player[0].Status() == DEADANIM)))
		{
			DrawRound();
			if (debug) printf("\nDrawRound");
			gameState = NEW_ROUND;
			return 0;
		}
		else if (player[0].Status() == DEAD)
		{ 
			player[1].wins++; 
			gameState = NEW_ROUND;
			return 0; 
		}
		else if (player[1].Status() == DEAD)
		{
			player[0].wins++; 
			gameState = NEW_ROUND;
			return 0;  
		}
		
		DrawWorld();
	
		SDL_Flip(scr);

		wait = WAIT_TIME - (SDL_GetTicks() - frame);
		if (wait > 10) SDL_Delay(wait); else SDL_Delay(10);
	}

	return 0;
}



// text to show when player disconnected
void Disconnected()
{
	Clear();
	Text(0, 0, "Disconnected.");
	SDL_Delay(1000);
}
// text to show when server bind failed
void BindFailed()
{
	Clear();
	Text(0, 0, "Bind failed.");
	SDL_Delay(1000);
}

// text to show when error with map loading
void MapError()
{
	Clear();
	Text(0, 0, "Cannot load gfx/map01.map.");
	SDL_Delay(1000);
}

// text to show when client was unable to find server
void CannotFindServer()
{
	Clear();
	Text(0, 0, "Cannot find server.");
	SDL_Delay(1000);
}


// texto to show when both players died simultaneously
void DrawRound()
{
	Clear();
	Text(0, 0, "Draw round.");
	SDL_Delay(1000);
}

// multiplayer game core
// same as single, plus netcode
int GameCoreMulti()
{
	if (debug) printf("\nBEGIN GameCoreMulti()");
	
	CPacket a;
	int frame, wait;
	SDL_Event event;
	PLAYER_ACTION move, plant;
	char buf[PACKET_SIZE];
	bool done = false, serverMoved = false, clientMoved = false;

#if defined (__unix__)
	pthread_mutex_init(&mutexHandle, 0);
#endif

	while (!done)
	{
		// if gameState is MENU, then NetThread died, so we must escape too
		if (gameState == MENU)
		{
			if (debug) printf("\nGameCoreMulti(): Connection terminated.");
			if (server) shutdownServer(remoteSocket);
			else shutdownClient(remoteSocket);
			NetThreadEnd();
			Disconnected();
			return 0;
		}
		//if (debug) printf("\nGameCore B");
		frame = SDL_GetTicks();
		move = plant = IDLE;
		serverMoved = clientMoved = false;

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT: gameState = GAME_QUIT; return 0;
			}
		}

		while(1)
		{
			//checking for local player moves
			if(keystates[SDLK_ESCAPE])  
			{
				if (debug) printf("\nGameCoreNet: ESCAPE");
				if (server) shutdownServer(remoteSocket);
				else shutdownClient(remoteSocket);
				NetThreadEnd();
				gameState = MENU; 
				return 0;
			}
			if(keystates[SDLK_LEFT])	{move = LEFT; break;}
			if(keystates[SDLK_RIGHT])	{move = RIGHT; break;}
			if(keystates[SDLK_UP])		{move = UP; break;}
			if(keystates[SDLK_DOWN])	{move = DOWN; break;}
			
			break;
		}

		if(keystates[SDLK_RCTRL]) plant = PLANT;
		//if (debug) printf("\nMOVE: %d, PLANT: %d", buf[0], buf[1]);
		
		// explosion check
		//if (debug) printf("Explosions think.\n");		
		itExplosion = vecExplosions.begin();
		while (1)
		{	
			if (itExplosion == vecExplosions.end()) break;
			if (itExplosion->think()) itExplosion++;
			else 
			{
				map.PopExplosion(itExplosion->GetMapX(), itExplosion->GetMapY());
				itExplosion = vecExplosions.erase(itExplosion);
			}	
		}
	
		// block check
		//if (debug) printf("Blocks think.\n");		
		itBlock = vecBlocks.begin();
		while (1)
		{
			if (itBlock == vecBlocks.end()) break;
			if (itBlock->Status() == DEAD) 
			{
				map.SetNotSolid(itBlock->GetMapX(), itBlock->GetMapY());
					CBonus tmp(itBlock->GetX(), itBlock->GetY(), BONUS_BOMB);
					tmp.SetSprite(&spr_bonus);
					vecBonuses.push_back(tmp);
					map.PushBonus(itBlock->GetMapX(), itBlock->GetMapY(), BONUS_BOMB);
				//}
				//else pom = NONE;
				itBlock = vecBlocks.erase(itBlock);	
			}
			else if (itBlock->Status() == ALIVE) {itBlock->Think(); itBlock++;}
			else break;
		}

		// bonus check
		itBonus = vecBonuses.begin();
		while(1)
		{
			if (itBonus == vecBonuses.end()) break;
			//if (debug) printf("mam bonus");
			if (itBonus->Status() == DEAD) {itBonus = vecBonuses.erase(itBonus); }
			else if (itBonus->Status() == ALIVE) {itBonus->Think(); itBonus++;}
		}

		// bomb check
		//if (debug) printf("Bomb think.\n");		
		itBomb= vecBombs.begin(); 
		while (1)
		{	
			if (itBomb== vecBombs.end()) break;
			if (itBomb->think()) itBomb++;
			else 
			{
				map.PopBomb(itBomb->GetMapX(), itBomb->GetMapY());
				itBomb->MakeExplosions();
				itBomb= vecBombs.erase(itBomb);	
			}
		}
	
	if (server)
		{
			buf[GS] = GAME; buf[P] = SERVER; buf[M] = move; buf[PL] = plant; 
			a.buf[GS] = GAME; a.buf[P] = CLIENT; a.buf[M] = a.buf[PL] = IDLE; 
			
			// if we're server, check our moves, then send it to remote player (if we're not idle)
			if (myplayer->Status() == ALIVE) myplayer->NetThink(buf[M], buf[PL]);
			else if (myplayer->Status() == DEADANIM) myplayer->DeadAnim();
			
			if ((buf[M] != IDLE) || (buf[PL] != IDLE)) 
			{
				send(remoteSocket, buf, PACKET_SIZE, 0);
				//printf("\nMY PACKET: GS(%d), P(%d), M(%d), PL(%d)", buf[GS], buf[P], buf[M], buf[PL]);	
			}

			#if defined (_WIN32)
				WaitForSingleObject(mutexHandle, INFINITE);
			#elif defined (__unix__)
				pthread_mutex_lock(&mutexHandle);
			#endif

			// check if some packets arrived from remote player
			if(!deqPackets.empty())
			{
				while(!deqPackets.empty())
				{
					a = deqPackets.front();
					deqPackets.pop_front();

					//if (debug) printf("\n\t\t\t\tYOUR PACKET: GS(%d), P(%d), M(%d), PL(%d)", a.buf[GS], a.buf[P], a.buf[M], a.buf[PL]);
					// if packet arrived, move with remote player
					if (remoteplayer->Status() == ALIVE) remoteplayer->NetThink(a.buf[M], a.buf[PL]);	
					else if (remoteplayer->Status() == DEADANIM) remoteplayer->DeadAnim();

					send(remoteSocket, a.buf, PACKET_SIZE, 0);
				}
			}
			else
			{
				if (remoteplayer->Status() == ALIVE) remoteplayer->NetThink(IDLE, IDLE);	
				else if (remoteplayer->Status() == DEADANIM) remoteplayer->DeadAnim();				
			}

			#if defined (_WIN32)
				ReleaseMutex(mutexHandle);
			#elif defined (__unix__)
				pthread_mutex_unlock(&mutexHandle);
			#endif

			// some player status checks
			if ((myplayer->Status() == DEAD) && ((remoteplayer->Status() == DEAD) || (remoteplayer->Status() == DEADANIM)))
			{
				DrawRound();
				if (debug) printf("\nDrawRound");
				gameState = NEW_ROUND;
				return 0;
			}
			else if ((remoteplayer->Status() == DEAD) && ((myplayer->Status() == DEAD) || (myplayer->Status() == DEADANIM)))
			{
				DrawRound();
				if (debug) printf("\nDrawRound");
				gameState = NEW_ROUND;
				return 0;
			}
			else if (myplayer->Status() == DEAD)
			{ 
				remoteplayer->wins++; 
				gameState = NEW_ROUND;
				return 0; 
			}
			else if (remoteplayer->Status() == DEAD)
			{
				myplayer->wins++; 
				gameState = NEW_ROUND;
				return 0; 
			}
		}

		// if we're client, we just send our moves to server and wait for packets to arrive (but don't move!),
		else
		{
			
			buf[GS] = GAME; buf[P] = CLIENT; buf[M] = move; buf[PL] = plant; 
			a.buf[GS] = GAME; a.buf[P] = CLIENT; a.buf[M] = a.buf[PL] = IDLE; 
			
			if ((buf[M] != IDLE) || (buf[PL] != IDLE)) 
			{
				send(remoteSocket, buf, PACKET_SIZE, 0);
				//if (debug) printf("\nMY PACKET: GS(%d), P(%d), M(%d), PL(%d)", buf[GS], buf[P], buf[M], buf[PL]);	
			}	

			#if defined (_WIN32)
				WaitForSingleObject(mutexHandle, INFINITE);
			#elif defined (__unix__)
				pthread_mutex_lock(&mutexHandle);
			#endif

			// if some packets arrived, we move with us (server commited our move) and with remote player
			if (!deqPackets.empty())
			{
				while (!deqPackets.empty())
				{
					a = deqPackets.front();
					deqPackets.pop_front();

					//if (debug) printf("\n\t\t\t\tYOUR PACKET: GS(%d), P(%d), M(%d), PL(%d)", a.buf[GS], a.buf[P], a.buf[M], a.buf[PL]);	
					
					if (a.buf[P] == SERVER)	
					{
						if (remoteplayer->Status() == ALIVE) remoteplayer->NetThink(a.buf[M], a.buf[PL]);	
						else if (remoteplayer->Status() == DEADANIM) remoteplayer->DeadAnim();
						serverMoved = true;
					}
					if (a.buf[P] == CLIENT)	
					{
						if (myplayer->Status() == ALIVE) myplayer->NetThink(a.buf[M], a.buf[PL]);	
						else if (myplayer->Status() == DEADANIM) myplayer->DeadAnim();
						clientMoved = true;
					}
				}
			}
			// no packets, just ckeck player status but don't move
			else
			{
				if (remoteplayer->Status() == ALIVE) remoteplayer->NetThink(IDLE, IDLE);	
				else if (remoteplayer->Status() == DEADANIM) remoteplayer->DeadAnim();
				if (myplayer->Status() == ALIVE) myplayer->NetThink(IDLE, IDLE);	
				else if (myplayer->Status() == DEADANIM) myplayer->DeadAnim();
				serverMoved = clientMoved = true;
			}

			#if defined (_WIN32)
				ReleaseMutex(mutexHandle);
			#elif defined (__unix__)
				pthread_mutex_unlock(&mutexHandle);
			#endif
			
			// other possibilities
			if (!serverMoved)
			{
				if (remoteplayer->Status() == ALIVE) remoteplayer->NetThink(IDLE, IDLE);	
				else if (remoteplayer->Status() == DEADANIM) remoteplayer->DeadAnim();
			}
			if (!clientMoved)
			{
				if (myplayer->Status() == ALIVE) myplayer->NetThink(IDLE, IDLE);	
				else if (myplayer->Status() == DEADANIM) myplayer->DeadAnim();
			}
			if ((myplayer->Status() == DEAD) && ((remoteplayer->Status() == DEAD) || (remoteplayer->Status() == DEADANIM)))
			{
				DrawRound();
				if (debug) printf("\nDrawRound");
				gameState = NEW_ROUND;
				return 0;
			}
			else if ((remoteplayer->Status() == DEAD) && ((myplayer->Status() == DEAD) || (myplayer->Status() == DEADANIM)))
			{
				DrawRound();
				if (debug) printf("\nDrawRound");
				gameState = NEW_ROUND;
				return 0;
			}
			else if (myplayer->Status() == DEAD)
			{ 
				remoteplayer->wins++; 
				gameState = NEW_ROUND;
				return 0; 
			}
			else if (remoteplayer->Status() == DEAD)
			{
				myplayer->wins++; 
				gameState = NEW_ROUND;
				return 0; 
			}
		}
		DrawWorld();
	
		SDL_Flip(scr);

		wait = WAIT_TIME - (SDL_GetTicks() - frame);
		if (wait > 10) SDL_Delay(wait); else SDL_Delay(10);
	}

	#if defined (__unix__)
		pthread_mutex_destroy(&mutexHandle);
	#endif

	if (debug) printf("\nEND GameCoreMulti()");
	return 0;
}

// clear the srcreen
int Clear()
{
	spr_background.Draw();
	SDL_Flip(scr);

	return 0;
}

// write some text
void Text(int a, int b, char *s)
{
	SFont_Write (scr, Font, a, b, s);
	SDL_Flip(scr);
}

// new round
int NewRound()
{
	if (debug) printf("\n\tBEGIN NewRound(%d)", multi);
	
	if (InitRound() == -1) { gameState = MENU; MapError(); return -1;}

	fflush(stdout);

	SDL_Event event;
	CPacket a;
	int frame, wait;
	bool done = false;
	
	// if multiplayer, separate server and client behaviour
	if (multi)
	{
		Clear();
		Text(0, 0, "New Round");
		DrawWins();
		
		// if server, we are able to start the game with enter, and send packet to client "let's go"
		if(server)
		{
			Text(0, 180, "PRESS ENTER");
			
			while(!done)
			{
				frame = SDL_GetTicks();

				while(SDL_PollEvent(&event))
				{
					switch(event.type)
					{
					case SDL_QUIT: gameState = GAME_QUIT; return 0;
					}
				}
						
				if (keystates[SDLK_ESCAPE]) 
				{
					if (debug) printf("\n\tNewRound(): ESCAPE");
					shutdownServer(remoteSocket);
					NetThreadEnd();
					gameState = MENU;
					done = true;
				}
				else if (keystates[SDLK_RETURN] || keystates[SDLK_KP_ENTER]) 
				{
					if (debug) printf("\n\tNewRound(): ENTER"); 
					a.Set(GAME, SERVER, IDLE, IDLE);
					send(remoteSocket, a.buf, PACKET_SIZE, 0);
					send(remoteSocket, a.buf, PACKET_SIZE, 0);
					send(remoteSocket, a.buf, PACKET_SIZE, 0);
					send(remoteSocket, a.buf, PACKET_SIZE, 0);
					gameState = GAME;
					done = true;
					
				}
				else if (gameState == MENU)
				{
					if (debug) printf("\n\tNewRound(): Remote escape");
					shutdownServer(remoteSocket);
					NetThreadEnd();
					Disconnected();
					done = true;
				}

				wait = WAIT_TIME - (SDL_GetTicks() - frame);
				if (wait > 10) SDL_Delay(wait); else SDL_Delay(10);
			}
		}
		// client just waits for starting packets from server, NetThread put gameState to GAME if packet arrives
		else
		{
			Text(0, 180, "Waiting for start...");

			while(!done)
			{
				frame = SDL_GetTicks();

				while (SDL_PollEvent(&event))
				{
					switch(event.type)
					{
					case SDL_QUIT: gameState = GAME_QUIT; return 0;
					}
				}
						
				if (keystates[SDLK_ESCAPE]) 
				{
					if (debug) printf("\n\tNewRound(): ESCAPE");
					shutdownClient(remoteSocket);
					NetThreadEnd();
					gameState = MENU;
					done = true;
				}
				else if (gameState == GAME)
				{
					if (debug) printf("\n\tNewRound(): Remote enter");
					a.Set(GAME, CLIENT, IDLE, IDLE);
					send(remoteSocket, a.buf, PACKET_SIZE, 0);
					send(remoteSocket, a.buf, PACKET_SIZE, 0);
					send(remoteSocket, a.buf, PACKET_SIZE, 0);
					send(remoteSocket, a.buf, PACKET_SIZE, 0);
					done = true;
				}
				else if (gameState == MENU)
				{
					if (debug) printf("\n\tNewRound(): Remote escape");
					shutdownClient(remoteSocket);
					NetThreadEnd();
					Disconnected();
					done = true;
				}

				wait = WAIT_TIME - (SDL_GetTicks() - frame);
				if (wait > 10) SDL_Delay(wait); else SDL_Delay(10);
			}
		}
	}
	// if single game, just waiting for enter
	else
	{
		Clear();
		Text(0, 0, "New Round");
		DrawWins();

		Text(0, 180, "PRESS ENTER");
		
		while(!done)
		{
			frame = SDL_GetTicks();

			while(SDL_PollEvent(&event))
			{
				switch(event.type)
				{
				case SDL_QUIT: gameState = GAME_QUIT; return 0;
				}
			}

			if (keystates[SDLK_RETURN]) 
			{
				if (debug) printf("\n\tNewRound(): ENTER"); 
				gameState = GAME;
				done =  true;
			}
			if (keystates[SDLK_KP_ENTER]) 
			{
				if (debug) printf("\n\tNewRound(): ENTER"); 
				gameState = GAME;
				done =  true;
			}
			else if (keystates[SDLK_ESCAPE]) 
			{
				if (debug) printf("\n\tNewRound(): ESCAPE");
				gameState = MENU;
				done = true;

			}

			wait = WAIT_TIME - (SDL_GetTicks() - frame);
			if (wait > 10) SDL_Delay(wait); else SDL_Delay(10);
		}		
	}

	if (debug) printf("\n\tEND NewRound()");

	return 0;
}

// main menu and options
void Menu()
{
	if (debug) printf("\nBEGIN Menu()");
	SDL_Event event;
	int frame, wait;
	bool done = false;
	
	ResetWins();
	Clear();
	Text(0, 0, "Crazy Coder Menu:");
	Text(0, 60, "(1) LOCAL: 2 players");
	Text(0, 90, "(2) LAN: server");
	Text(0, 120, "(3) LAN: client");

	keystates[SDLK_ESCAPE] = 0;

	while(!done)
	{
		frame = SDL_GetTicks();
		
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT: gameState = GAME_QUIT; return;
			}
		}

		if (keystates[SDLK_1] || keystates[SDLK_KP1]) 
		{
			if (debug) printf("\nMenu(): LOCAL");
			multi = false; 
			gameState = NEW_ROUND;
			done = true;
		}
		else if (keystates[SDLK_2] || keystates[SDLK_KP2]) 
		{
			if (debug) printf("\nMenu(): LAN SERVER");
			gameState = CONNECT;
			multi = server = done = true; 
		}
		else if (keystates[SDLK_3] || keystates[SDLK_KP3]) 
		{
			if (debug) printf("\nMenu(): LAN CLIENT");
			gameState = CONNECT;
			multi = done = true; 
			server = false; 
		}
		else if (keystates[SDLK_ESCAPE]) 
		{
			if (debug) printf("\nMenu(): ESCAPE");
			gameState = GAME_QUIT;
			done = true;
		}
	
		wait = WAIT_TIME - (SDL_GetTicks() - frame);
		if (wait > 10) SDL_Delay(wait); else SDL_Delay(10);
	}

	if (debug) printf("\nEND: Menu()");
	return;
}

// draws an IP address while filling in
void DrawIP()
{
	Clear();
	Text(0, 0, "Enter IP:");
	Text(0, 30, ip);
	Text(0, 410, "empty IP = localhost");

	return;
}

//draws port while filling in
void DrawPort(char *port)
{
	Clear();
	Text(0, 0, "Enter Port:");
	Text(0, 30, port);
	Text(0, 410, "empty Port = 54321");

	return;
}

// reads an IP address
void GetIP()
{
	if (debug) printf("\n\t\tBEGIN GetIP");
	int i = 0, frame, wait, num[4] = {0}, nump = 0;
	ip[i] = '\0';
	bool done = false;
	SDL_Event event;
	
	DrawIP();
		
	while(!done)
	{
		frame = SDL_GetTicks();

		if (SDL_PollEvent(&event))
		{
			
			switch(event.type)
			{
			case SDL_QUIT: gameState = GAME_QUIT; return;
			}
			
			if(event.key.state == SDL_PRESSED)
			{
				if (i < 15)
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE: gameState = MENU; done = true; break;
						case SDLK_RETURN: done = true; break;
						case SDLK_KP_ENTER: done = true; break;
						case SDLK_BACKSPACE: 
							if (i > 0) 
							{
								if (ip[--i] == '.') {ip[i] = '\0'; nump--;}
								else {ip[i] = '\0'; num[nump]--;}
								DrawIP();
							}
							break;
							
						case SDLK_1: /*if (num[nump] < 3) */{ip[i] = '1'; num[nump]++;} break;
						case SDLK_2: /*if (num[nump] < 3) */{ip[i] = '2'; num[nump]++;} break; 
						case SDLK_3: /*if ((num[nump] == 1) || (num[nump] == 2)) */{ip[i] = '3'; num[nump]++;} break;
						case SDLK_4: /*if ((num[nump] == 1) || (num[nump] == 2)) */{ip[i] = '4'; num[nump]++;} break;
						case SDLK_5: /*if ((num[nump] == 1) || (num[nump] == 2)) */{ip[i] = '5'; num[nump]++;} break;
						case SDLK_6: /*if (((num[nump] == 1) || (num[nump] == 2)) && (ip[i-1] < 53))*/ {ip[i] = '6'; num[nump]++;} break;
						case SDLK_7: /*if (((num[nump] == 1) || (num[nump] == 2)) && (ip[i-1] < 53))*/ {ip[i] = '7'; num[nump]++;} break;
						case SDLK_8: /*if (((num[nump] == 1) || (num[nump] == 2)) && (ip[i-1] < 53))*/ {ip[i] = '8'; num[nump]++;} break;
						case SDLK_9: /*if (((num[nump] == 1) || (num[nump] == 2)) && (ip[i-1] < 53))*/ {ip[i] = '9'; num[nump]++;} break;
						case SDLK_0: /*if ((num[nump] == 1) || (num[nump] == 2)) */{ip[i] = '0'; num[nump]++;} break;
						case SDLK_KP1: /*if (num[nump] < 3)*/ {ip[i] = '1'; num[nump]++;} break;
						case SDLK_KP2: /*if (num[nump] < 3)*/ {ip[i] = '2'; num[nump]++;} break; 
						case SDLK_KP3: /*if ((num[nump] == 1) || (num[nump] == 2)) */{ip[i] = '3'; num[nump]++;} break;
						case SDLK_KP4: /*if ((num[nump] == 1) || (num[nump] == 2)) */{ip[i] = '4'; num[nump]++;} break;
						case SDLK_KP5: /*if ((num[nump] == 1) || (num[nump] == 2)) */{ip[i] = '5'; num[nump]++;} break;
						case SDLK_KP6: /*if ((num[nump] == 2) && (ip[i-1] < 53)) */{ip[i] = '6'; num[nump]++;} break;
						case SDLK_KP7: /*if ((num[nump] == 2) && (ip[i-1] < 53))*/ {ip[i] = '7'; num[nump]++;} break;
						case SDLK_KP8: /*if ((num[nump] == 2) && (ip[i-1] < 53))*/ {ip[i] = '8'; num[nump]++;} break;
						case SDLK_KP9: /*if ((num[nump] == 2) && (ip[i-1] < 53))*/ {ip[i] = '9'; num[nump]++;} break;
						case SDLK_KP0: /*if ((num[nump] == 1) || (num[nump] == 2)) */{ip[i] = '0'; num[nump]++;} break;
						case '.': /*if ((num[nump] != 0) && (nump < 3))*/ {ip[i] = '.'; nump++;} break;
						case SDLK_KP_PERIOD: /*if ((num[nump] != 0) && (nump < 3))*/ {ip[i] = '.'; nump++;} break;
						default: break;
					}
					if (ip[i] != '\0') {ip[++i] = '\0'; DrawIP();}
				}
				else
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE: gameState = MENU; done = true; break;
						case SDLK_RETURN: done = true; break;
						case SDLK_KP_ENTER: done = true; break;
						case SDLK_BACKSPACE: 	
							if (ip[--i] == '.') {ip[i] = '\0'; nump--;}
							else {ip[i] = '\0'; num[nump]--;}
							DrawIP(); break;
						default: break;							
					}
				}
			}
		}
		
		wait = WAIT_TIME - (SDL_GetTicks() - frame);
		if (wait > 10) SDL_Delay(wait); else SDL_Delay(10);
	}
	
	if (i == 0) strcpy(ip, "127.0.0.1");
		
	if (debug) printf("\n\t\tEND GetIP(%s)", ip);
	
	return;
}

// converts port from string to int
int ConvertPort(char *p)
{
	int i = 0, port = 0;
	
	port = p[i++] - 48;

	while((p[i]) || (i > 5)) port = port*10 + (p[i++] - 48);
	
	return port;
}

// reads port number
void GetPort()
{
	if (debug) printf("\n\t\tBEGIN GetPort()");

	int i = 0, frame, wait;
	char pom[6];
	pom[i] = '\0';
	bool done = false;
	SDL_Event event;
	
	DrawPort(NULL);
		
	while(!done)
	{
		frame = SDL_GetTicks();

		if(SDL_PollEvent(&event))
		{
		
			switch(event.type)
			{
			case SDL_QUIT: gameState = GAME_QUIT; return;
			default: break;
			}
		
			if(event.key.state == SDL_PRESSED)
			{
				if (i < 5)
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE: gameState = MENU; done = true; break;
						case SDLK_RETURN: done = true; break;
						case SDLK_KP_ENTER: done = true; break;
						case SDLK_BACKSPACE: if (i > 0) pom[--i] = '\0'; else pom[i] = '\0'; DrawPort(pom); break;
						case SDLK_1: pom[i] = '1'; break;
						case SDLK_2: pom[i] = '2'; break; 
						case SDLK_3: pom[i] = '3'; break;
						case SDLK_4: pom[i] = '4'; break;
						case SDLK_5: pom[i] = '5'; break;
						case SDLK_6: pom[i] = '6'; break;
						case SDLK_7: pom[i] = '7'; break;
						case SDLK_8: pom[i] = '8'; break;
						case SDLK_9: pom[i] = '9'; break;
						case SDLK_0: pom[i] = '0'; break;
						case SDLK_KP1: pom[i] = '1'; break;
						case SDLK_KP2: pom[i] = '2'; break; 
						case SDLK_KP3: pom[i] = '3'; break;
						case SDLK_KP4: pom[i] = '4'; break;
						case SDLK_KP5: pom[i] = '5'; break;
						case SDLK_KP6: pom[i] = '6'; break;
						case SDLK_KP7: pom[i] = '7'; break;
						case SDLK_KP8: pom[i] = '8'; break;
						case SDLK_KP9: pom[i] = '9'; break;
						case SDLK_KP0: pom[i] = '0'; break;
						default: break;
					}
					if (pom[i] != '\0') {pom[++i] = '\0'; DrawPort(pom);}
				}
				else
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE: gameState = MENU; done = true; break;
						case SDLK_RETURN: done = true; break;
						case SDLK_KP_ENTER: done = true; break;
						case SDLK_BACKSPACE: if (i > 0) pom[--i] = '\0'; DrawPort(pom); break;
						default: break;							
					}
				}
			}
		}

		wait = WAIT_TIME - (SDL_GetTicks() - frame);
		if (wait > 10) SDL_Delay(wait); else SDL_Delay(10);
	}
	
	if (i == 0) port = DEFAULT_PORT;
	else port = ConvertPort(pom);

	if (debug) printf("\n\t\tEND: GetPort(%s)", pom);

	return;
}

// NetThread, separate headers in Win32 and Unix
// depending on gameState, handles network
#if defined (_WIN32)
	void NetThread()
#elif defined (__unix__)
	void * NetThread(void *ptr)
#endif
{
	if (debug) printf("\n\t\t\t\tBEGIN NetThread()");
	netThreadActive = true;

	CPacket a;
	#if defined (_WIN32)
		int rec = 0;
	#elif defined (__unix__)
		ssize_t rec = 0;
	#endif

	bool done = false;

	while (!done)
	{
	//printf("x");
		// if we're connecting
		if (gameState == CONNECT)
		{
			// server waits for client
			if (server)
			{
				if ((remoteSocket = accept(mySocket, 0, 0)) == INVALID_SOCKET)
				{
					if (debug) printf("\n\t\t\t\tNetThread(): Server: accept(): INVALID_SOCKET");
					gameState = MENU;
					done = true;
				}
				else
				{
					if (debug) printf("\n\t\t\t\tNetThread(): Connected.");
					gameState = NEW_ROUND;
				}
			}
			// client 
			else
			{	
				struct hostent *host_entry;

				if ((host_entry = gethostbyname(ip)) == NULL) 
				{
					if (debug) printf("\n\t\t\t\tNetThread(): Client: gethostbyname(): Could not find host.");
					gameState = MENU;
					done = true;
				}
				// client tries to connect to server
				else
				{
					struct sockaddr_in server;

					server.sin_family = AF_INET;
					server.sin_port = htons(port);
					server.sin_addr.s_addr = *(unsigned long*) host_entry->h_addr;
					
					if ((rec = connect(remoteSocket, (sockaddr*)&server, sizeof(server))) == SOCKET_ERROR) 
					{
						if (debug) printf("\n\t\t\t\tNetThread(): Client: connect(%s, %d): Error connecting to server.", ip, port);
						gameState = MENU;
						done = true;
					}
					/*#ifdef __unix__
					else if (rec == 0)
					{
						if (debug) printf("\n\t\t\t\tNetThread(): Disconnected.");
						gameState = MENU;
						done = true;
					}
					#endif*/
					else 
					{
						if (debug) printf("\n\t\t\t\tNetThread(): Connected.");
						gameState = NEW_ROUND;
					}
				}
			}
		}
		// if we're about to start a new round
		else if (gameState == NEW_ROUND)			
		{
			// if I'm server just check if player didn't disconnect
			//printf("\nKOTVIM NA NEW_ROUND THREAD");
			if (server)
			{
				if ((rec = recv(remoteSocket, a.buf, PACKET_SIZE, 0)) == SOCKET_ERROR)
				{
					if (debug) printf("\n\t\t\t\tNetThread(): SOCKET_ERROR");
					gameState = MENU;
					done = true;
				}
				else if (rec == 0)
				{
					if (debug) printf("\n\t\t\t\tNetThread(): Client disconnected.");
					gameState = MENU;
					done = true;
				}
			}
			// if I'm client, check if server didn't disconnect and if arrived packets to start the game
			else
			{
				if ((rec = recv(remoteSocket, a.buf, PACKET_SIZE, 0)) == SOCKET_ERROR)
				{
					if (debug) printf("\n\t\t\t\tNetThread(): SOCKET_ERROR");
					gameState = MENU;
					done = true;
				}
				else if (rec == 0)
				{
					if (debug) printf("\n\t\t\t\tNetThread(): Server disconnected.");
					gameState = MENU;
					done = true;
				}
				else
				{
					if (debug) printf("\n\t\t\t\tNetThread(): received packet(%d, %d, %d, %d)", a.buf[0], a.buf[1], a.buf[2], a.buf[3]);
					gameState = GAME;
				}
			}
			//printf("\nODKOTVIM NA NEW_ROUND THREAD");
		}
		// if we're playing, just check if remote player didn't disconnect and if some packet arrives
		// if yes, put it to vector
		else if (gameState == GAME)
		{
			//printf("\nKOTVIM NA GAME THREAD");
			//fflush(stdout);
			if ((rec = recv(remoteSocket, a.buf, PACKET_SIZE, 0)) == SOCKET_ERROR)
			{
				if (debug) printf("\n\t\t\t\tNetThread(): SOCKET_ERROR");
				gameState = MENU;
				done = true;
			}
			else if (rec == 0)
			{
				if (debug) printf("\n\t\t\t\tNetThread(): Disconnected.");
				gameState = MENU;
				done = true;
			}
			else
			{
				if (debug) printf("\n\t\t\t\tNetThread(): received packet(%d, %d, %d, %d)", a.buf[0], a.buf[1], a.buf[2], a.buf[3]);
				#if defined (_WIN32)
					WaitForSingleObject(mutexHandle, INFINITE);
				#elif defined (__unix__)
					pthread_mutex_lock(&mutexHandle);
				#endif

				deqPackets.push_back(a);

				#if defined (_WIN32)
					ReleaseMutex(mutexHandle);
				#elif defined (__unix__)
					pthread_mutex_unlock(&mutexHandle);
				#endif
			}
			//printf("\nODKOTVIM NA GAME THREAD");
			//fflush(stdout);
		}
	}

	if (debug) printf("\n\t\t\t\tEND: NetThread()");
	fflush(stdout);

	netThreadActive = false;
	netThreadDied = true;

	#if defined (_WIN32)
		return;
	#elif defined (__unix__)
		pthread_exit(NULL);
	#endif
}

// waiting for NetThread until it dies
void NetThreadEnd()
{
	while(netThreadActive) {}
	netThreadDied = false;
}

// starting up connection
void Connect()
{
	if (debug) printf("\n\tBEGIN Connect(%d)", server);
	int frame, wait;
	char *thread_message = "NetThread";
	bool done = false;//, flag = false;
	SDL_Event event;
//	CPacket a;
	//printf("%d", netThreadDied);

	// fires up server, checks for errors, listens for clients
	if (server) 
	{
		myplayer = &player[0];
		remoteplayer = &player[1];

		GetPort();
		if (gameState == MENU)
		{
			if (debug) printf("\n\tConnect(): Server: Escape from GetPort()");
		}
		else if (gameState == GAME_QUIT)
		{
			return;
		}
		else
		{
			Clear();
			Text(0, 0, "Waiting for client...");

			mySocket = startupServerForListening(port);
			if (mySocket == -1) 
			{
				if (debug) printf("\nConnect(): Error starting up server");
				gameState = MENU;
				done = true;
			}
			else if (mySocket == -2) 
			{
				if (debug) printf("\nConnect(): Bind failed.");
				fflush(stdout);
				gameState = MENU;
				BindFailed();
				done = true;
			}
			else
			{
				#if defined (_WIN32)
					threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NetThread, NULL, 0, NULL);
				#elif defined (__unix__)
					pthread_create(&threadHandle, NULL, NetThread, (void *)thread_message);
				#endif				
				
				while (1) 
				{
					//printf("n");
					if (netThreadActive) break;
					if (netThreadDied) 
					{
						NetThreadEnd();
						if (debug) printf("\nNetThreadDied."); 
						CannotFindServer(); 
						gameState = MENU;
						return;
					}
				}
				if (debug) printf("\n\tConnect(): NetThread started.");
				fflush(stdout);

				while (!done)
				{
					frame = SDL_GetTicks();

					while (SDL_PollEvent(&event))
					{
						switch(event.type)
						{
						case SDL_QUIT: gameState = GAME_QUIT; return;
						}
					}

					if (keystates[SDLK_ESCAPE]) 
					{
						if (debug) printf("\n\tConnect(): Server: ESCAPE");
						fflush(stdout);
						shutdownServer(mySocket);
						NetThreadEnd();
						gameState = MENU;
						done = true;
					}
					else 
					{	
						if (gameState == NEW_ROUND)
						{
							if (debug) printf("\n\tConnect(): Server: Connected.");
							fflush(stdout);
							done = true;
						}
						else if (gameState == MENU)
						{
							if (debug) printf("\n\tConnect(): Server: Connection error.");
							fflush(stdout);
							shutdownServer(remoteSocket);
							NetThreadEnd();
							done = true;
						}
					}

					wait = WAIT_TIME - (SDL_GetTicks() - frame);
					if (wait > 10) SDL_Delay(wait); else SDL_Delay(10);
				}
			}
		}
	}
	// fires up client, checks for errors, getip, getport and try to connect
	else
	{
		myplayer = &player[1];
		remoteplayer = &player[0];

		GetIP();
		if (gameState == MENU)
		{
			if (debug) printf("\n\tConnect(): Escape from GetIP().");
		}
		else if (gameState == GAME_QUIT)
		{
			return;
		}
		else
		{
			GetPort();
			if (gameState == MENU)
			{
				if (debug) printf("\n\tConnect(): Escape from GetPort().");	
			}
			else if (gameState == GAME_QUIT)
			{
				return;
			}
			else
			{
				Clear();
				Text(0, 0, "Connecting to server...");

				remoteSocket = startupClient(port, ip);

				if (remoteSocket == -1)
				{
					if (debug) printf("\nConnect(): Error starting up client");
					fflush(stdout);
					gameState = MENU;
					done = true;
				}
				else
				{

					#if defined (_WIN32)
						threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NetThread, NULL, 0, NULL);
					#elif defined (__unix__)
						pthread_create(&threadHandle, NULL, NetThread, (void *)thread_message);
					#endif	
	
					while (1) 
					{
						//printf(".");
						if (netThreadActive) break;
						if (netThreadDied) 
						{
							netThreadDied = false; 
							netThreadActive = false; 
							if (debug) printf("\nNetThreadDied."); 
							CannotFindServer(); 
							return;
						}
					}
					if (debug) printf("\nNetThread started.");
					fflush(stdout);

					while (!done)
					{
						frame = SDL_GetTicks();
						
						while (SDL_PollEvent(&event))
						{
							switch(event.type)
							{
							case SDL_QUIT: gameState = GAME_QUIT; return;
							}
						}

						if (keystates[SDLK_ESCAPE]) 
						{
							if (debug) printf("\n\tConnect(): Client: ESCAPE");
							fflush(stdout);
							shutdownClient(remoteSocket);
							NetThreadEnd();
							gameState = MENU;
							done = true;
						}
						else
						{
							if (gameState == MENU)
							{
								if (debug) printf("\n\tConnect(): Client: Connection error.");
								fflush(stdout);
								shutdownClient(remoteSocket);
								NetThreadEnd();
								done = true;
								Clear();
								Text(0, 0, "Could not connect.");
								SDL_Delay(1000);
							}
							else if (gameState == NEW_ROUND)
							{
								if (debug) printf("\n\tConnect(): Client: Connected.");
								fflush(stdout);
								done = true;
							}
						}
					
						wait = WAIT_TIME - (SDL_GetTicks() - frame);
						if (wait > 10) SDL_Delay(wait); else SDL_Delay(10);
					}
				}
			}
		}
	}

	if (debug) printf("\n\tEND: Connect(%d)", gameState);
	fflush(stdout);
	return;
}

// reads images, sets some inits...
int InitGame()
{
	bool error = false;
	srand((unsigned)time(NULL));
	gfxInit(440,440, false);
	keystates = SDL_GetKeyState(0);
	if (!spr_tile.Init("gfx/tile.bmp",1)) error = true;
	if (!spr_player[0].Init("gfx/player_blue.bmp", 255,255,255,8)) error = true;
	if (!spr_player[1].Init("gfx/player_red.bmp", 255,255,255,8)) error = true;
	if (!spr_bomb.Init("gfx/bomb.bmp", 255,0,255, 2)) error = true;
	if (!spr_background.Init("gfx/bg.bmp",1)) error = true;
	if (!spr_explosion.Init("gfx/explosion.bmp",255,0,255,4)) error = true;
	if (!spr_block.Init("gfx/block.bmp", 4)) error = true;
	if (!spr_bonus.Init("gfx/bonuses.bmp",3)) error = true;
	player[0].SetControls(SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_RCTRL);
	player[1].SetControls(SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_LCTRL);
	player[0].SetSprite(&spr_player[0]);
	player[1].SetSprite(&spr_player[1]);
	if ((Font = SFont_InitFont(IMG_Load("gfx/font.png"))) == NULL ) {fprintf(stderr, "Cannot open font gfx/font.png.");error = true;}
	
	if (error) return -1;

	if (debug) printf("\nGame started") ;

	return 0;
}

// erases temporary vectors, inits player
int InitRound()
{
	player[0].Init();
	player[1].Init();
	vecBonuses.erase(vecBonuses.begin(), vecBonuses.end());
	vecBlocks.erase(vecBlocks.begin(), vecBlocks.end());
	vecExplosions.erase(vecExplosions.begin(), vecExplosions.end());
	vecBombs.erase(vecBombs.begin(), vecBombs.end());
	if (map.LoadMap("maps/map01.map") == -1) {MapError(); Clear(); return -1;}
	player[0].SetPosition(TILE_SIZE, TILE_SIZE);
	player[1].SetPosition(TILE_SIZE*9, TILE_SIZE*9);

	return 0;
}

// some text to show about players
int DrawWins()
{
	char num[3];
	int wins = 0, i = 0;

	Text(0, 60, "Score");
	Text(0, 90, "Blue player:");
	wins = player[0].wins;
	//if (debug) printf("\nwins: %d", wins);
	
	if (wins < 10) {num[0] = wins + 48; num[1] = '\0';}
	else {num[1] = wins%10 + 48; wins/=10;num[0] = wins%10 + 48; num[2] = '\0';}	

	//if (debug) printf("\nnum: %s", num);
	Text(240, 90, num);

	Text(0, 120, "Red player:");
	wins = 0;
	num[0] = '\0';
	i = 0;
	wins = player[1].wins;
	//if (debug) printf("\nwins: %d", wins);
	
	if (wins < 10) {num[0] = wins + 48; num[1] = '\0';}
	else {num[1] = wins%10 + 48; wins/=10;num[0] = wins%10 + 48; num[2] = '\0';}	
	
	//if (debug) printf("\nnum: %s", num);
	Text(240, 120, num);
	return 0;
}

void ResetWins()
{
	player[0].wins = 0;
	player[1].wins = 0;
}

// decides what to do
void Game()
{
	if (multi) GameCoreMulti();
	else GameCoreSingle();
}

// depending on gameState, calls procedures
int main(int argc, char *argv[])
{
	bool done = false;
	int frame, wait;
	if (InitGame() == -1) return -1;
	if (argc == 2) if (!strcmp("--debug", argv[1])) debug = true;
	

	while(!done)
	{
		frame = SDL_GetTicks();	
	
		switch(gameState)
		{
		case MENU: Menu(); break;
		case CONNECT: Connect(); break;
		case NEW_ROUND: NewRound(); break;
		case GAME: Game(); break;
		case GAME_QUIT: done = true; break;
		default: break;
		}

		wait = WAIT_TIME - (SDL_GetTicks() - frame);
		if (wait > 10) SDL_Delay(wait); else SDL_Delay(10);
	}

	SFont_FreeFont(Font);
	if (debug) printf("\nGame ended.\n");

	return 0;
}
