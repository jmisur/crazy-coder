#include "CBonus.h"
#include "CMap.h"

extern CMap map;

CBonus::CBonus(int posX, int posY, ENTITY_TYPE b)
{
	
	//tick = 0;//BOMB_TICK_TIME;
	//frame = SDL_GetTicks();
	x = posX;
	y = posY;
	mapX = x/TILE_SIZE;
	mapY = y/TILE_SIZE;
	type = b;
	status = ALIVE;
	//active = true;
}


CBonus::~CBonus()
{

}

void CBonus::Draw()
{
	//printf("kreslim bonus\n");
	if (type == BONUS_BOMB) spr->Draw(x, y, 0);
	else if (type == BONUS_SPEED) spr->Draw(x, y, 1);
	else if (type == BONUS_FLAME) spr->Draw(x, y, 2);
}

ENTITY_STATUS CBonus::Think()
{
	if (!map.GetBonus(mapX, mapY)) 
	{
		status = DEAD;
	}
	return status;
}
