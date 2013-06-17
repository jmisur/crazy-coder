#ifndef _CBONUS_H_
#define _CBONUS_H_

#include "../Shared/CBaseEntity.h"

class CBonus: public CBaseEntity
{
public:
	CBonus(int posX, int posY, ENTITY_TYPE b);
	~CBonus();
	ENTITY_STATUS Status(){return status;};
	//void DeadAnim();
	void Draw();
	ENTITY_STATUS Think();
private:
	int x, y, mapX, mapY;
	ENTITY_STATUS status;
	int deadFrame, deadTick;
	ENTITY_TYPE type;
};

#endif
