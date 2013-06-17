#ifndef _GFX_H_
#define _GFX_H_

#include "../SDL/include/SDL.h"
#include "../SDL/include/SDL_image.h"
#include "SFont.h"

//SDL_Surface *scr;
//dat to do classy!!!!!!!!!!!!!!11
bool gfxInit(int w, int h, bool fullscreen);
void gfxSetResolution(int w, int h, bool fullscreen);

class gfxFont{
	public:
		~gfxFont(){if(m_font)SFont_FreeFont(m_font);};

		bool init(const char *filename);
		void draw(int x, int y, const char *s);
		void drawf(int x, int y, char *s, ...);

		void drawCentered(int y, const char *text);


		int getHeight(){return SFont_TextHeight(m_font);};
		int getWidth(const char *text){return SFont_TextWidth(m_font, text);};

	private:
		SFont_Font *m_font;
};

#endif
