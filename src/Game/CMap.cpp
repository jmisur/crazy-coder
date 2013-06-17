#include "CMap.h"
#include "../Shared/Defines.h"
#include "CBlock.h"

extern vector<CBlock> vecBlocks;
extern CSprite spr_tile;
extern CSprite spr_block;

int CMap::LoadMap(const char *file)
{
	FILE *mapfile;
	int i, j;
	char r;

	if((mapfile = fopen(file, "rt")) == NULL) {fprintf(stderr, "Cannot load map %s.", file); return -1;}

	for(j = 0; j < MAP_HEIGHT; j++)
	{
		for(i = 0; i < MAP_WIDTH; i++)
		{			
			//read solid
			fread(&r, sizeof(char), 1, mapfile);
			if (r == '0') tiles[i][j].solid = false;
			else tiles[i][j].solid = true;
			
			//read tile
			fread(&r, sizeof(char), 1, mapfile);
			if (r == '1')	
			{
				tiles[i][j].SetSprite(&spr_tile); 
				tiles[i][j].isBlock = false;
			}
			if (r == '2') 
			{
				CBlock tmp(i, j);
				tmp.SetSprite(&spr_block);
				vecBlocks.push_back(tmp); 
				tiles[i][j].isBlock = true;
			}
			tiles[i][j].isBomb = false;
			tiles[i][j].isExplosion = false;
			tiles[i][j].isBonus = NONE;
		}
	}
	fclose(mapfile);

	return 0;
}


void CMap::Draw(){
	int i, j;

	for(i = 0; i < MAP_WIDTH; i++){
		for(j = 0; j < MAP_HEIGHT; j++){
			if(tiles[i][j].GetSprite() != NULL)
				tiles[i][j].GetSprite()->Draw(i*TILE_SIZE, j*TILE_SIZE,0);
		}
	}
}
