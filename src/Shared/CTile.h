#ifndef _CTILE_H_
#define _CTILE_H_

#include "CBaseEntity.h"

class CTile: public CBaseEntity
{
public:
	//CSprite	*spr;	//sprite to Draw
	CTile();
	//void Draw(int x, int y);
	bool solid;	//is the tile solid
	bool isBomb;
	bool isExplosion;
	bool isBlock;
	ENTITY_TYPE isBonus;
};

#endif
