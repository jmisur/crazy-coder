#ifndef _CPLAYER_H_
#define _CPLAYER_H_

#include "../Shared/CBaseEntity.h"

class CPlayer: public CBaseEntity
{
	public:
		ENTITY_STATUS Think();			//handle input, collision detection, ...
		void Draw();
		void DeadAnim();
		void NetThink(char move, char plant);
		/*void NetThinkchar status, char move, char plant, char bonus);*/
		ENTITY_STATUS Status(){return status;};
		//bool Check();
		CPlayer();
		void HitByFlame();
		void Init();
		void SetControls(SDLKey left, SDLKey right, SDLKey up, SDLKey down, SDLKey bomb);
		void SetPosition(int posX, int posY){x = posX; y = posY;};
	//	bool collision_ver(int x, int y, int &tilecoordx);		//tertikal line from [x,y] to [x,y+height]
	//	bool collision_hor(int x, int y, int &tilecoordy);		//horizontal line from [x,y] to [x+width, y]
		void BombExploded(){plantedBombs--;};
		int wins;
		ENTITY_STATUS status;

	private:
		SDLKey key[6];
		int x, y;				//x, y coordinate (top left of the player rectangle)
		int h, w;			//height, width
		int velx, vely;	
		int speed; //velocity on x, y axis

		int flameLength;
		PLAYER_ACTION dir;		//player facing right? -> graphics
		bool dead;
		int deadFrame;
		int deadTick;
		int totalBombs;
		int plantedBombs;
		//int wins;
};

#endif
