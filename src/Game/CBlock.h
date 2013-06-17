#ifndef _CBLOCK_H_
#define _CBLOCK_H_

#include "../Shared/Defines.h"
#include "../Shared/CBaseEntity.h"

class CBlock: public CBaseEntity
{
public:
	CBlock(int posX, int posY);
	~CBlock();
	ENTITY_STATUS Status();
	void DeadAnim();
	void Draw();
	ENTITY_STATUS Think();
private:
	//int x, y, mapX, mapY;
	//ENTITY_STATUS status;
	int deadFrame, deadTick;
};

#endif
