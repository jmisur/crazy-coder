#include "CBomb.h"
//#include "CPlayer.h"
#include "CMap.h"
#include "CExplosion.h"

extern CMap map;
extern CSprite spr_bomb;
extern CSprite spr_explosion;

extern vector<CExplosion> vecExplosions;

CBomb::CBomb(int xpos, int ypos, int fl, CPlayer *pl)
{
	//printf("%d %d", xpos, ypos);
	flameLength = fl;
	tick = 0;//BOMB_TICK_TIME;
	frame = SDL_GetTicks();
	x = xpos;
	y = ypos;
	mapX = x/TILE_SIZE;
	mapY = y/TILE_SIZE;
	player = pl;
	animFrame = 0;
	SetSprite(&spr_bomb);
	//active = true;
}

CBomb::~CBomb()
{

}

bool CBomb::think()
{
	//MALO BY TO BYT NEZAVISLE KAZDA BOMBA!!!!!!!!!!!!!!!!!!!!!!!!1
	if (!map.IsBomb(mapX, mapY)) return false;
	tick += SDL_GetTicks() - frame;
	frame = SDL_GetTicks();
	if (tick >= BOMB_TICK_TIME) return false;
	else return true;
}

void CBomb::Draw()
{
	if(((frame/100)%10) > 5) animFrame = 0;
	else animFrame = 1;
	spr->Draw(x, y, animFrame);
}

void CBomb::MakeExplosions()
{
	player->BombExploded();
	int exX = x - x%20, exY = y - y%20;
	if (!map.IsExplosion(mapX, mapY))
	{
		map.PushExplosion(mapX, mapY);
		CExplosion tmp(exX, exY);
		tmp.SetSprite(&spr_explosion);
		vecExplosions.push_back(tmp);
	}
	int i=1;
	while ((i < flameLength) && ((mapX + i) < MAP_WIDTH))
	{
		if (map.IsSolid(mapX + i, mapY)) 
		{
				if (map.IsBlock(mapX + i, mapY)) map.PopBlock(mapX + i, mapY);
				if (map.IsBomb(mapX + i, mapY)) map.PopBomb(mapX + i, mapY);
				break;
		}
		//if (map.IsBlock(mapX + i, mapY)) map.PopBlock(mapX + i, mapY);

		if (!map.IsExplosion(mapX + i, mapY))
		{
			map.PushExplosion(mapX + i, mapY);
			CExplosion tmp(exX + i*TILE_SIZE, exY);
			tmp.SetSprite(&spr_explosion);
			vecExplosions.push_back(tmp);
		}
		if (map.GetBonus(mapX + i, mapY)) {map.PopBonus(mapX + i, mapY); break;}
		i++;
	}
	i=1;
	while ((i < flameLength) && ((mapX - i) > 0))
	{
		if (map.IsSolid(mapX - i, mapY)) 
		{
			if (map.IsBlock(mapX - i, mapY)) {map.PopBlock(mapX - i, mapY);}
			if (map.IsBomb(mapX - i, mapY)) map.PopBomb(mapX - i, mapY);
			break;
		}

		if (!map.IsExplosion(mapX - i, mapY))
		{
			map.PushExplosion(mapX - i, mapY);
			CExplosion tmp(exX - i*TILE_SIZE, exY);
			tmp.SetSprite(&spr_explosion);
			vecExplosions.push_back(tmp);
		}
		if (map.GetBonus(mapX - i, mapY)) {map.PopBonus(mapX - i, mapY); break;}
		i++;
	}
	i=1;
	while ((i < flameLength) && ((mapY + i) < MAP_HEIGHT))
	{
		if (map.IsSolid(mapX, mapY + i)) 
		{
			if (map.IsBlock(mapX, mapY + i)) {map.PopBlock(mapX, mapY + i);}
			if (map.IsBomb(mapX, mapY + i)) map.PopBomb(mapX, mapY + i);			
			break;
		}

		if (!map.IsExplosion(mapX, mapY + i))
		{
			map.PushExplosion(mapX, mapY + i);
			CExplosion tmp(exX , exY + i*TILE_SIZE);
			tmp.SetSprite(&spr_explosion);
			vecExplosions.push_back(tmp);
		}
		if (map.GetBonus(mapX , mapY + i)) {map.PopBonus(mapX, mapY + i); break;}
		i++;
	}
	i=1;
	while ((i < flameLength) && ((mapY - i) > 0))
	{
		if (map.IsSolid(mapX, mapY - i)) 
		{
			if (map.IsBlock(mapX, mapY - i)) {map.PopBlock(mapX, mapY - i);}
			if (map.IsBomb(mapX, mapY - i)) map.PopBomb(mapX, mapY - i);	
			break;
		}
		if (!map.IsExplosion(mapX, mapY - i))
		{
			map.PushExplosion(mapX, mapY - i);
			CExplosion tmp(exX , exY - i*TILE_SIZE);
			tmp.SetSprite(&spr_explosion);
			vecExplosions.push_back(tmp);
		}
		if (map.GetBonus(mapX, mapY - i)) {map.PopBonus(mapX, mapY - i); break;}
		i++;
	}
}
