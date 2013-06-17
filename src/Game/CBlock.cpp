#include "CBlock.h"
#include "CMap.h"
#include "CBonus.h"

extern CMap map;
extern vector<CBonus> vecBonuses;
extern CSprite spr_bonus;

CBlock::CBlock(int posX, int posY)
{
	
	x = posX*TILE_SIZE;
	y = posY*TILE_SIZE;
	mapX = posX;
	mapY = posY;
	status = ALIVE;
	deadFrame = deadTick = 0;
	//printf("%d %d %d %d", x, y, mapX, mapY);
}

CBlock::~CBlock()
{

}

ENTITY_STATUS CBlock::Status()
{
	return status;
}

ENTITY_STATUS CBlock::Think()
{
	if (!map.IsBlock(mapX, mapY)) {status = DEADANIM; deadFrame = SDL_GetTicks();}
	return status;
}

void CBlock::DeadAnim()
{
	deadTick += SDL_GetTicks() - deadFrame;
	deadFrame = SDL_GetTicks();
	if (deadTick >= BLOCK_DEADANIM_TIME) 
	{
		status = DEAD; 

	}
	else 
	{
		if (deadTick < BLOCK_DEADANIM_TIME/3) spr->Draw(x, y, 1);
		else if (deadTick < BLOCK_DEADANIM_TIME/3*2) spr->Draw(x, y, 2);
		else if (deadTick < BLOCK_DEADANIM_TIME) spr->Draw(x, y, 3);
	}
}

void CBlock::Draw()
{
	spr->Draw(x, y,0);
}
