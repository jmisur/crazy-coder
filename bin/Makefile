GCC_FLAGS = -c -O0 -Wall
LD_FLAGS =  -L../ -lSDL_image -lSDL -lSDLmain -lstdc++ -lnsl -lpng -ljpeg
GAME_PATH = ../../src/Game/
EDITOR_PATH = ../../src/Editor/
SHARED_PATH = ../../src/Shared/
OBJ_PATH = obj/
GAME = CBlock.o CBomb.o CBonus.o CExplosion.o Client.o CMap.o CPlayer.o Game.o Server.o
SHARED = CBaseEntity.o CSprite.o CTile.o Gfx.o SFont.o
EDITOR = Editor.o
CHECK=`if [ $$TYPEOS = "Solaris" ]; then echo -lsocket; fi`
CHECKLIBSOL=`if [ $$TYPEOS = "Solaris" ]; then cp libSDL_image.a.solaris libSDL_image.a; fi`
CHECKLIBLIN=`if [ $$TYPEOS = "Linux" ]; then cp libSDL_image.a.linux libSDL_image.a; fi`

all: Game Editor

clean: 
	rm -f Game Editor
	rm -r -f obj/
	@echo "Clean OK";
	
Game: GameC SharedC GameL
	@echo "Game OK";

GameL: 
	$(CHECKLIBSOL) 
	$(CHECKLIBLIN)
	cd obj && gcc -o ../Game $(GAME) $(SHARED) $(LD_FLAGS) $(CHECK)
	
Editor: EditorC SharedC EditorL
	@echo "Editor OK";

EditorL: 
	$(CHECKLIBSOL) 
	$(CHECKLIBLIN)
	cd obj && gcc -o ../Editor $(EDITOR) $(SHARED) $(LD_FLAGS)
	
GameC: OBJ
	cd obj && gcc $(GCC_FLAGS) $(GAME_PATH)*.cpp

EditorC: OBJ
	cd obj && gcc $(GCC_FLAGS) $(EDITOR_PATH)*.cpp

SharedC: OBJ
	cd obj && gcc $(GCC_FLAGS) $(SHARED_PATH)*.cpp $(SHARED_PATH)*.c	
OBJ:
	if [ ! -d obj ]; then mkdir obj; fi
