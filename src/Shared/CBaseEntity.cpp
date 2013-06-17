#include "CBaseEntity.h"

#include "../SDL/include/SDL.h"

CBaseEntity::CBaseEntity()
{
	x = 0;
	y = 0;
	w = 0;
	h = 0;
	mapX = 0;
	mapY = 0;
	status = ALIVE;
	animFrame = 0;
	animTick = 0;
	spr = NULL;
}

CBaseEntity::CBaseEntity(int posX, int posY)
{
	x = posX;
	y = posY;
	w = 0;
	h = 0;
	mapX = posX / TILE_SIZE;
	mapY = posY / TILE_SIZE;
	status = ALIVE;
	animFrame = 0;
	animTick = 0;
	spr = NULL;
}

CBaseEntity::~CBaseEntity()
{

}

ENTITY_STATUS CBaseEntity::Think()
{
	return status;
}

ENTITY_STATUS CBaseEntity::Status()
{
	return status;
}

CSprite *CBaseEntity::GetSprite()
{
	return spr;
}

void CBaseEntity::SetSprite(CSprite *tmp)
{
	spr = tmp;
}

int CBaseEntity::GetMapX()
{
	return mapX;
}

int CBaseEntity::GetMapY()
{
	return mapY;
}

int CBaseEntity::GetX()
{
	return x;
}

int CBaseEntity::GetY()
{
	return y;
}
