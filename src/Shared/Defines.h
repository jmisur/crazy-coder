#ifndef _DEFINES_H_
#define _DEFINES_H_

#define MAP_WIDTH		11		
#define MAP_HEIGHT		11	
#define TILE_SIZE		40		
#define PLAYER_HEIGHT		10	
#define PLAYER_WIDTH		10		
#define PLAYER_DEADANIM_TIME	500
#define PLAYER_SPEED		4	
#define BOMB_TICK_TIME		1600
#define EXPLOSION_TIME		500
#define WAIT_TIME		15	
#define BLOCK_DEADANIM_TIME	500	
#define PACKET_SIZE		4
#define DEFAULT_PORT		54321

#if defined (__unix__)
	#define SOCKET_ERROR		-1
	#define INVALID_SOCKET		-1
#endif

enum GAME_STATE {MENU, CONNECT, NEW_ROUND, GAME, GAME_QUIT};
enum PLAYER {SERVER, CLIENT};
enum ENTITY_STATUS {ALIVE, DEAD, DEADANIM};
enum ENTITY_TYPE {NONE, SOLID, BLOCK, BOMB, EXPLOSION, BONUS_BOMB, BONUS_FLAME, BONUS_SPEED};
enum PLAYER_ACTION {IDLE, LEFT, RIGHT, UP, DOWN, PLANT, PDEAD, RDEAD};
enum PACKET {GS, P, M, PL};

#include <vector>
#include <deque>

using namespace std;
#endif
