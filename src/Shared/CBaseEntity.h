#ifndef _CBASEENTITY_H_
#define _CBASEENTITY_H_

#include "Defines.h"
#include "Gfx.h"
#include "CSprite.h"

class CBaseEntity
{
public:
	CBaseEntity();
	CBaseEntity(int posX, int posY);
	virtual ~CBaseEntity();
	virtual ENTITY_STATUS Think();
	ENTITY_STATUS Status();
	CSprite *GetSprite();
	void SetSprite(CSprite *tmp);
	int GetMapX();
	int GetMapY();
	int GetX();
	int GetY();


protected:
	CSprite *spr;
	int x, y;
	int w, h;
	int mapX, mapY;
	ENTITY_STATUS status;
	int animFrame;
	int animTick;
};

#endif
