#ifndef _CSPRITE_H_
#define _CSPRITE_H_

#include "Gfx.h"

class CSprite
{
public:
	CSprite();
	~CSprite();
	bool Init(const char *filename, Uint8 r, Uint8 g, Uint8 b, int fr);
	bool Init(const char *filename, int fr);						
	bool Draw(int x = 0, int y = 0, int frame = 0);
	int GetWidth();
	int GetHeight();
	SDL_Surface *GetSurface();

private:
	int frames;
	SDL_Surface *surface;
	SDL_Rect rect;
	SDL_RWops *rwop;
};

#endif
