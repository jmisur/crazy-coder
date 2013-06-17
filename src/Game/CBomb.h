#ifndef _CBOMB_H_
#define _CBOMB_H_

#include "../Shared/CBaseEntity.h"
#include "CPlayer.h"

class CBomb: public CBaseEntity
{
	public:
		CBomb();
		CBomb(int xpos, int ypos, int fl, CPlayer *pl);
		~CBomb();
		bool think();
		void Draw();
		void boom();
		int GetFlameLength(){return flameLength;};
		void MakeExplosions();

	private:
		int flameLength;
		int tick;
		int frame;
		CPlayer *player;
		int animFrame;
};

#endif
