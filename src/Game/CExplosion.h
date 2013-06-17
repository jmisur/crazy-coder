#ifndef _CEXPLOSION_H_
#define _CEXPLOSION_H_

#include "../Shared/CBaseEntity.h"

class CExplosion: public CBaseEntity
{
public:
	CExplosion(int x, int y/*, int fl*/);
	~CExplosion();
	bool think();
	void Draw();
	int GetMapX(){return mapX;};
	int GetMapY(){return mapY;};
private:
	int x, y, mapX, mapY;
	int tick;
	int frame;
	//int flameLength;
};

#endif
