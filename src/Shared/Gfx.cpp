#include "Gfx.h"
#include "Defines.h"

extern SDL_Surface *scr;
extern bool debug;

#define GFX_BPP		0
#define GFX_FLAGS	SDL_SWSURFACE

bool gfxInit(int w, int h, bool fullscreen)
{
	
	if (debug) printf("SDL Initialization:\n");
	
	if( SDL_Init(SDL_INIT_VIDEO ) < 0 ) {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    atexit(SDL_Quit);
    
    if(fullscreen) scr = SDL_SetVideoMode(w, h, GFX_BPP, GFX_FLAGS | SDL_FULLSCREEN);
	else scr = SDL_SetVideoMode(w, h, GFX_BPP, GFX_FLAGS);

    if ( scr == NULL ) {
		fprintf(stderr, "Couldn't set video mode %dx%d: %s\n", w, h, SDL_GetError());
		return false;
    }
    
	if (debug) printf("Running @ %dx%d %dbpp (done)\n", w, h, scr->format->BitsPerPixel);	
	
	return true;
}

void gfxSetResolution(int w, int h, bool fullscreen)
{
	Uint32 flags = GFX_FLAGS;
	
	if(fullscreen) flags |= SDL_FULLSCREEN;
	scr = SDL_SetVideoMode(w, h, GFX_BPP, flags);
}

