#include "CSprite.h"
#include "Defines.h"
#include "../SDL/include/SDL_image.h"
extern bool debug;
extern SDL_Surface *scr;

CSprite::CSprite()
{
	rect.x = 0;
	rect.y = 0;
	rect.w = 0;
	rect.h = 0;
	surface = NULL;
	frames = 1;
}

CSprite::~CSprite()
{
	if(surface)
	{
		SDL_FreeSurface(surface);
		surface = NULL;
	}
}

bool CSprite::Init(const char *filename, Uint8 r, Uint8 g, Uint8 b, int fr)
{
	if(surface != NULL)
	{
		SDL_FreeSurface(surface);
		surface = NULL;
	}

	rwop = SDL_RWFromFile(filename, "rb");

	surface = IMG_LoadBMP_RW(rwop);

    	if (surface == NULL) 
	{
		fprintf(stderr, "ERROR: IMG_LoadBMP: %s\n", SDL_GetError());
        return false;
    }

	if( SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(surface->format, r, g, b)) < 0)
	{
		fprintf(stderr, "ERROR: SDL_SetColorKey: %s\n", SDL_GetError());
		return false;
	}

	SDL_Surface *temp;

	if( (temp = SDL_DisplayFormat(surface)) == NULL){
		fprintf(stderr, "ERROR: SDL_DisplayFormat: %s\n", SDL_GetError());
		return false;
	}

	SDL_FreeSurface(surface);
	surface = temp;
	rect.w = surface->w/fr;
	rect.h = surface->h;
	if (debug) printf("Loaded %s\n", filename);
	frames = fr;
	return true;
}

bool CSprite::Init(const char *filename, int fr) 
{
	if(surface != NULL)
	{
		SDL_FreeSurface(surface);
		surface = NULL;
	}

	rwop = SDL_RWFromFile(filename, "rb");

	surface = IMG_LoadBMP_RW(rwop);

    if (surface == NULL) 
	{
		fprintf(stderr, "ERROR: IMG_LoadBMP: %s\n", SDL_GetError());
        return false;
    }

	SDL_Surface *temp;

	if((temp = SDL_DisplayFormat(surface)) == NULL)
	{
		fprintf(stderr, "ERROR: SDL_DisplayFormat: %s\n", SDL_GetError());
		return false;
	}

	SDL_FreeSurface(surface);
	surface = temp;
	rect.w = surface->w/fr;
	rect.h = surface->h;
	if (debug) printf("Loaded %s\n", filename);
	frames = fr;
	return true;
}

bool CSprite::Draw(int x, int y, int frame)
{
	rect.x = x;
	rect.y = y;

	SDL_Rect src;

	src.x = frame*TILE_SIZE;
	src.y = 0;
	src.w = rect.w;
	src.h = rect.h;

	//if (frame == 1) if (debug) printf("src.x %d, src.y %d, src.w %d, src.h %d\nrect.x %d, rect.y %d, rect.w %d, rect.y %d\n\n", src.x, src.y, src.w, src.h, rect.x, rect.y, rect.w, rect.h);

	if(SDL_BlitSurface(surface, &src, scr, &rect) < 0)
	{
		fprintf(stderr, "ERROR: SDL_BlitSurface: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

int CSprite::GetHeight()
{
	return surface->h;
}

int CSprite::GetWidth()
{
	return surface->w;
}

SDL_Surface* CSprite::GetSurface()
{
	return surface;
}
