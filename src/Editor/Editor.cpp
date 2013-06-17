#include <stdio.h>
#include "../Shared/Gfx.h"	
#include "../Shared/Defines.h"
#include "../Shared/CTile.h"
#include "../Shared/CSprite.h"
#include <stdlib.h>
#include <time.h>

SDL_Surface	*scr;
SDL_Event event;
CSprite spr_tile;
CSprite spr_block;
CSprite spr_background;
CSprite spr_solid;
bool done = false;
bool solidset = false;
CSprite *sprset = NULL;
CTile tiles[MAP_WIDTH][MAP_HEIGHT];
bool debug;

int RandomMap()
{
	srand((unsigned)time(NULL));
	int i, j;
	FILE *outp;
	char solid[2] = {'0', '1'};
	char sprite[3] = {'0', '1', '2'};
	char sol;
	int ran = 0;
	
	if ((outp = fopen("maps/map01.map", "wt+")) == NULL) {printf("Can't create/open file 'maps/map01.map'."); return -1;}

	
	for(i = 0; i < MAP_WIDTH; i++) {fwrite(&solid[1], sizeof(char), 1, outp); fwrite(&sprite[1], sizeof(char), 1, outp);}
	fwrite(&solid[1], sizeof(char), 1, outp); fwrite(&sprite[1], sizeof(char), 1, outp);
	for(i = 0; i < MAP_WIDTH - 2; i++) {fwrite(&solid[0], sizeof(char), 1, outp); fwrite(&sprite[0], sizeof(char), 1, outp);}
	fwrite(&solid[1], sizeof(char), 1, outp); fwrite(&sprite[1], sizeof(char), 1, outp);
	for(j = 0; j < MAP_HEIGHT - 4; j++)
	{
		fwrite(&solid[1], sizeof(char), 1, outp);
		fwrite(&sprite[1], sizeof(char), 1, outp);		
		fwrite(&solid[0], sizeof(char), 1, outp); 
		fwrite(&sprite[0], sizeof(char), 1, outp);
		for(i = 0; i < MAP_WIDTH - 4; i++)
		{
			if ((ran = rand()%3) == 0) sol = '0';
			else sol = '1';
			fwrite(&sol, sizeof(char), 1, outp);
			fwrite(&sprite[ran], sizeof(char), 1, outp);
		}
		fwrite(&solid[0], sizeof(char), 1, outp); 
		fwrite(&sprite[0], sizeof(char), 1, outp);
		fwrite(&solid[1], sizeof(char), 1, outp);
		fwrite(&sprite[1], sizeof(char), 1, outp);
	}
	fwrite(&solid[1], sizeof(char), 1, outp); fwrite(&sprite[1], sizeof(char), 1, outp);
	for(i = 0; i < MAP_WIDTH - 2; i++) {fwrite(&solid[0], sizeof(char), 1, outp); fwrite(&sprite[0], sizeof(char), 1, outp);}
	fwrite(&solid[1], sizeof(char), 1, outp); fwrite(&sprite[1], sizeof(char), 1, outp);
	for(i = 0; i < MAP_WIDTH - 2; i++) {fwrite(&solid[1], sizeof(char), 1, outp); fwrite(&sprite[1], sizeof(char), 1, outp);}

	fclose(outp);

return 0;
}

//main main main
int main(int argc, char *argv[])
{
	if (argc == 2) {if (!strcmp(argv[1], "--debug")) debug = true;}
	if (argc == 2) {if (!strcmp(argv[1], "--random")) RandomMap(); return 0;}
	
	char solid[2] = {'0', '1'};
	char sprite[3] = {'0', '1', '2'};
	char r;
	int i, j, t;
	bool error = false;

	//init some stuff
	gfxInit(440,440, false);
	if (spr_tile.Init("gfx/tile.bmp", 1) == false) error = true;
	if (spr_block.Init("gfx/block.bmp", 4) == false) error = true;
	if (spr_background.Init("gfx/bg.bmp", 1) == false) error = true;
	if (spr_solid.Init("gfx/solid.bmp", 1) == false) error = true;

	//read the map file
	FILE *mapfile;
			
	if ((mapfile = fopen("maps/map01.map", "rt")) == NULL) {fprintf(stderr, "Cannot open maps/map01.map."); return -1;}

	for(j = 0; j < MAP_WIDTH; j++)
	{
		for(i = 0; i < MAP_HEIGHT; i++)
		{
			//read solid
			fread(&r, sizeof(char), 1, mapfile);
			if (r == '0') tiles[i][j].solid = false;
			else tiles[i][j].solid = true;
			
			//read tile
			fread(&r, sizeof(char), 1, mapfile);
			if(r == '1') tiles[i][j].SetSprite(&spr_tile);
			if(r == '2') tiles[i][j].SetSprite(&spr_block);
		}
	}
	fclose(mapfile);

	//printf("\nentering game loop...\n") ;
	while (!done){

		//handle messages
			while(SDL_PollEvent(&event))
			{
				switch(event.type){
					case SDL_QUIT:
						done = true;
					break;

					case SDL_KEYDOWN:
						if (event.key.keysym.sym == SDLK_ESCAPE)
							done = true;

					break;

					case SDL_MOUSEBUTTONDOWN:
						if(event.button.button == SDL_BUTTON_LEFT)
						{
							CTile &t = tiles[event.button.x / TILE_SIZE][event.button.y / TILE_SIZE];
							
							if(t.GetSprite() == NULL)
								sprset = &spr_tile;
							else if(t.GetSprite() == &spr_tile)
								sprset = &spr_block;
							else sprset = NULL;
							t.SetSprite(sprset);
						}
						else if(event.button.button == SDL_BUTTON_RIGHT)
						{
							CTile &t = tiles[event.button.x / TILE_SIZE][event.button.y / TILE_SIZE];
							solidset = !t.solid;
							t.solid = solidset;
						}
					break;

					case SDL_MOUSEMOTION:
						if(event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT))
						{
							CTile &t = tiles[event.motion.x / TILE_SIZE][event.motion.y / TILE_SIZE];
							t.SetSprite(sprset);
						}
						else if(event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT))
						{
							CTile &t = tiles[event.motion.x / TILE_SIZE][event.motion.y / TILE_SIZE];
							t.solid = solidset;
						}
					break;

					default:
					break;
				}
			}


		//draw the map
		spr_background.Draw(0,0);
		for(i = 0; i < MAP_WIDTH; i++)
		{
			for(j = 0; j < MAP_HEIGHT; j++)
			{
				if(tiles[i][j].GetSprite() != NULL)
					tiles[i][j].GetSprite()->Draw(i*TILE_SIZE, j*TILE_SIZE);
				if(tiles[i][j].solid)
					spr_solid.Draw(i*TILE_SIZE,j*TILE_SIZE);
			}
		}
	
		SDL_Flip(scr);
	}

	//write the map

	
	FILE *outp;

	outp = fopen("maps/map01.map", "wt");

	for(j = 0; j < MAP_HEIGHT; j++)
	{
		for(i = 0; i < MAP_WIDTH; i++)
		{
			//write solid
			if (tiles[i][j].solid) fwrite(&solid[1], sizeof(char), 1, outp);
			else fwrite(&solid[0], sizeof(char), 1, outp);

			//write tile
			if(tiles[i][j].GetSprite() == &spr_tile)
				t = 1;				
			else if(tiles[i][j].GetSprite() == &spr_block)
				t = 2;				
			else
				t = 0;

			fwrite(&sprite[t], sizeof(char), 1, outp);

		}
	}
	fclose(outp);


	return 0;
}
