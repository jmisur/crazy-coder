#include "CExplosion.h"
#include "../SDL/include/SDL.h"

CExplosion::CExplosion(int xpos, int ypos)
{
	//flameLength = fl;
	tick = 0;//BOMB_TICK_TIME;
	frame = SDL_GetTicks();
	x = xpos;
	y = ypos;
	mapX = x/TILE_SIZE;
	mapY = y/TILE_SIZE;
	//active = true;
}

CExplosion::~CExplosion()
{
	
}

bool CExplosion::think()
{
	//if ((tick <= 0) && (tick > -3000)) boom();
	tick += SDL_GetTicks() - frame;
	frame = SDL_GetTicks();
	//printf("%d\n", frame);
	if (tick > EXPLOSION_TIME) return false;
	else return true;
}

void CExplosion::Draw()
{
	if (tick <= EXPLOSION_TIME/4) spr->Draw(x, y, 0);
	else if (tick <= EXPLOSION_TIME/2) spr->Draw(x, y, 1);
	else if (tick <= EXPLOSION_TIME/4*3) spr->Draw(x, y, 2);
	else if (tick <= EXPLOSION_TIME) spr->Draw(x, y, 3);
}
