#include "CPlayer.h"
#include "CMap.h"
#include "CBomb.h"

extern CMap map;
extern Uint8 *keystates;
extern vector<CBomb> vecBombs;
extern bool debug;

CPlayer::CPlayer(){
	x		= 100;			//start coordinates
	y		= 100;

	h		= PLAYER_HEIGHT;	//save player height and width
	w		= PLAYER_WIDTH;
	status = ALIVE;
	velx	= 0;			//speed
	vely	= 0;
	speed = 0;

	flameLength = 3;
	//lockjump = false;		//player may jump
	dir = DOWN;			//player looks right
	totalBombs = 1;
	plantedBombs = 0;
	wins = 0;
}

void CPlayer::Init()
{
	if(wins > 99) wins = 0;
	status = ALIVE;
	velx	= 0;			//speed
	vely	= 0;
	speed = 0;
	totalBombs = 1; 
	plantedBombs = 0; 	
	flameLength = 3;
};

void CPlayer::Draw()
{
	//crap, change planned in jnrdev #4
	switch (dir)
	{
	case LEFT : {spr->Draw(x - 10, y - 40, 0); break;}
	case RIGHT	 : {spr->Draw(x - 10, y - 40, 1); break;}
	case UP : {spr->Draw(x - 10, y - 40, 2); break;}
	case DOWN : {spr->Draw(x - 10, y - 40, 3); break;}
	default: break;
	}
}

ENTITY_STATUS CPlayer::Think(){
	if (map.GetBonus(x/TILE_SIZE, y/TILE_SIZE)) 
	{
		if(map.GetBonus(x/TILE_SIZE, y/TILE_SIZE) == BONUS_BOMB) totalBombs++;
		if(map.GetBonus(x/TILE_SIZE, y/TILE_SIZE) == BONUS_SPEED) speed++;
		if(map.GetBonus(x/TILE_SIZE, y/TILE_SIZE) == BONUS_FLAME) flameLength++;

		map.PopBonus(x/TILE_SIZE, y/TILE_SIZE);
	}
	velx = 0;	//don't move left / right by default
	vely = 0;
	
	if(keystates[key[RIGHT]]){
		velx = PLAYER_SPEED + speed/2;		//move right
		dir = RIGHT;		//player graphic is facing right
	}
	if(keystates[key[LEFT]]){
		velx = -PLAYER_SPEED - speed/2;		//move left
		dir = LEFT;
	}
	if(keystates[key[UP]]){		//if the player isn't jumping already
		vely = -PLAYER_SPEED - speed/2;		//jump!
		dir = UP;
	}
	if(keystates[key[DOWN]]){	//if the player isn't jumping already
		vely = PLAYER_SPEED + speed/2;		//jump!
		dir = DOWN;
	}
	if(keystates[key[PLANT]]){
		if (!map.IsBomb(x/TILE_SIZE, y/TILE_SIZE))
		{
			if (plantedBombs < totalBombs)
			{
				plantedBombs++;
				vecBombs.push_back(CBomb(x - x%TILE_SIZE, y - y%TILE_SIZE, flameLength, this));
				map.PushBomb(x/TILE_SIZE, y/TILE_SIZE);
			}
		}
	}

	//check for collisions with the map
	if (velx > 0)
	{
		int pos = (x + w) / TILE_SIZE;
		int posNew = (x + w + velx) / TILE_SIZE;
		if (posNew != pos)
		{
			if (map.IsSolid(posNew, y/TILE_SIZE) || 
				map.IsSolid(posNew, (y + h - 1)/TILE_SIZE))
				x = (x + velx) / TILE_SIZE * TILE_SIZE + 29;
			else x += velx;
		}
		else x += velx;
	}		 
	else if (velx < 0)
	{	
		int pos = x / TILE_SIZE;
		int posNew = (x + velx) / TILE_SIZE;
		if (posNew != pos)
		{
			if (map.IsSolid(posNew, y/TILE_SIZE) || 
				map.IsSolid(posNew, (y + h - 1)/TILE_SIZE))
				x = (x + velx) / TILE_SIZE * TILE_SIZE + TILE_SIZE;
			else x += velx;
		}
		else x += velx;
	}
	if (vely > 0)
	{	
		int pos = (y + h) / TILE_SIZE;
		int posNew = (y + h + vely) / TILE_SIZE;
		if (posNew != pos)
		{
			if (map.IsSolid(x/TILE_SIZE, posNew) || 
				map.IsSolid((x + w - 1)/TILE_SIZE , posNew))
				y = (y + vely) / TILE_SIZE * TILE_SIZE + 29;
			else y += vely;
		}
		else y += vely;
	}		 
	else if (vely < 0)
	{	
		int pos = (y) / TILE_SIZE;
		int posNew = (y + vely) / TILE_SIZE;
		if (posNew != pos)
		{
			if (map.IsSolid(x/TILE_SIZE, posNew) || 
				map.IsSolid((x + w - 1)/TILE_SIZE , posNew))
				y = (y + vely) / TILE_SIZE * TILE_SIZE + TILE_SIZE;
			else y += vely;
		}
		else y += vely;
	}



	if (map.IsExplosion(x/TILE_SIZE, y/TILE_SIZE))
	{
		status = DEADANIM;
		deadFrame = SDL_GetTicks();
		deadTick = 0;
	}

	return status;
}

void CPlayer::NetThink(char move, char plant)
{
	if (map.GetBonus(x/TILE_SIZE, y/TILE_SIZE)) 
	{
		if(map.GetBonus(x/TILE_SIZE, y/TILE_SIZE) == BONUS_BOMB)  totalBombs++;
		if(map.GetBonus(x/TILE_SIZE, y/TILE_SIZE) == BONUS_SPEED) speed++;
		if(map.GetBonus(x/TILE_SIZE, y/TILE_SIZE) == BONUS_FLAME) flameLength++;

		map.PopBonus(x/TILE_SIZE, y/TILE_SIZE);
	}

	velx = 0;	//don't move left / right by default
	vely = 0;
	//printf("su tu");
	if(move == RIGHT){
		velx = PLAYER_SPEED + speed/2;		//move right
		dir = RIGHT;		//player graphic is facing right
	}
	if(move == LEFT){
		velx = -PLAYER_SPEED - speed/2;		//move left
		dir = LEFT;
	}
	if(move == UP){		//if the player isn't jumping already
		vely = -PLAYER_SPEED - speed/2;		//jump!
		dir = UP;
	}
	if(move == DOWN){	//if the player isn't jumping already
		vely = PLAYER_SPEED + speed/2;		//jump!
		dir = DOWN;
	}
	if(plant == PLANT){
		if (!map.IsBomb(x/TILE_SIZE, y/TILE_SIZE))
		{
			if (plantedBombs < totalBombs)
			{
				plantedBombs++;
				vecBombs.push_back(CBomb(x - x%TILE_SIZE, y - y%TILE_SIZE, flameLength, this));
				map.PushBomb(x/TILE_SIZE, y/TILE_SIZE);
			}
		}
	}

	//check for collisions with the map
	if (velx > 0)
	{
		int pos = (x + w) / TILE_SIZE;
		int posNew = (x + w + velx) / TILE_SIZE;
		if (posNew != pos)
		{
			if (map.IsSolid(posNew, y/TILE_SIZE) || 
				map.IsSolid(posNew, (y + h - 1)/TILE_SIZE))
				x = (x + velx) / TILE_SIZE * TILE_SIZE + 29;
			else x += velx;
		}
		else x += velx;
	}		 
	else if (velx < 0)
	{	
		int pos = x / TILE_SIZE;
		int posNew = (x + velx) / TILE_SIZE;
		if (posNew != pos)
		{
			if (map.IsSolid(posNew, y/TILE_SIZE) || 
				map.IsSolid(posNew, (y + h - 1)/TILE_SIZE))
				x = (x + velx) / TILE_SIZE * TILE_SIZE + TILE_SIZE;
			else x += velx;
		}
		else x += velx;
	}
	if (vely > 0)
	{	
		int pos = (y + h) / TILE_SIZE;
		int posNew = (y + h + vely) / TILE_SIZE;
		if (posNew != pos)
		{
			if (map.IsSolid(x/TILE_SIZE, posNew) || 
				map.IsSolid((x + w - 1)/TILE_SIZE , posNew))
				y = (y + vely) / TILE_SIZE * TILE_SIZE + 29;
			else y += vely;
		}
		else y += vely;
	}		 
	else if (vely < 0)
	{	
		int pos = (y) / TILE_SIZE;
		int posNew = (y + vely) / TILE_SIZE;
		if (posNew != pos)
		{
			if (map.IsSolid(x/TILE_SIZE, posNew) || 
				map.IsSolid((x + w - 1)/TILE_SIZE , posNew))
				y = (y + vely) / TILE_SIZE * TILE_SIZE + TILE_SIZE;
			else y += vely;
		}
		else y += vely;
	}



	if (map.IsExplosion(x/TILE_SIZE, y/TILE_SIZE))
	{
		status = DEADANIM;
		deadFrame = SDL_GetTicks();
		deadTick = 0;
	}
}


void CPlayer::SetControls(SDLKey left, SDLKey right, SDLKey up, SDLKey down, SDLKey bomb)
{
	key[LEFT] = left;
	key[RIGHT] = right;
	key[UP] = up;
	key[DOWN] = down;
	key[PLANT] = bomb;
}
void CPlayer::DeadAnim()
{
	if (deadTick >= PLAYER_DEADANIM_TIME) 
	{
		if (debug) printf("\nPlayer dead."); 
		status = DEAD;
	}
	else 
	{
		deadTick += SDL_GetTicks() - deadFrame;
		deadFrame = SDL_GetTicks();
		//printf("\n%d", deadTick);
		if (deadTick < PLAYER_DEADANIM_TIME/4) spr->Draw(x - 10, y - 40, 4);
		else if (deadTick < PLAYER_DEADANIM_TIME/4*2) spr->Draw(x - 10, y - 40, 5);
		else if (deadTick < PLAYER_DEADANIM_TIME/4*3) spr->Draw(x - 10, y - 40, 6);
		else if (deadTick < PLAYER_DEADANIM_TIME) spr->Draw(x - 10, y - 40, 7);
	}
}
