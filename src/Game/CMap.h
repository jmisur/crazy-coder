#ifndef _CMAP_H_
#define _CMAP_H_

#include "../Shared/CTile.h"

class CMap{
	public:
		int LoadMap(const char *file);		//loads the map from a file
		CSprite *GetTileSprite(int x, int y) {return tiles[x][y].GetSprite();}

		void Draw();
		bool IsBomb(int x, int y){return tiles[x][y].isBomb;};
		void PushBomb(int x, int y){tiles[x][y].isBomb = true; tiles[x][y].solid = true;};
		void PopBomb(int x, int y){tiles[x][y].isBomb = false; tiles[x][y].solid = false;};
		//void Push
		bool IsExplosion(int x, int y){return tiles[x][y].isExplosion;};
		void PushExplosion(int x, int y){tiles[x][y].isExplosion = true;};
		void PopExplosion(int x, int y){tiles[x][y].isExplosion = false;};

		bool IsBlock(int x, int y) {return tiles[x][y].isBlock;};
		void PushBlock(int x, int y) {tiles[x][y].isBlock = true; tiles[x][y].solid = true;};
		void PopBlock(int x, int y) {tiles[x][y].isBlock = false;};

		ENTITY_TYPE GetBonus(int x, int y){return tiles[x][y].isBonus;};
		void PushBonus(int x, int y, ENTITY_TYPE b){tiles[x][y].isBonus = b;};
		void PopBonus(int x, int y){tiles[x][y].isBonus = NONE;};

		void SetNotSolid(int x, int y) {tiles[x][y].solid = false;};
		bool IsSolid(int x, int y){return tiles[x][y].solid;}

	//private:
		CTile tiles[MAP_WIDTH][MAP_HEIGHT];
};

#endif
