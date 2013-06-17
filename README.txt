===================
Crazy Coder v0.5
Juraj Misur
MFF UK v Prahe 2005
misur@matfyz.cz
http://misur.matfyz.cz/mff/swi015/
===================

-------------------
       UNIX:
-------------------

Vsetko sa odohrava v adresari bin/

!!!POZOR!!!
Je potreba mat nainstalovane SDL (najlepsie s SDL_image)
libSDL_image pre solaris a linux je predkompilovany v bin,
kedze nie je sucast standardnej SDL distribucie
http://www.libsdl.org

Instalacia:
prikazom make

Editor:
Ak si chcete vytvorit vlastnu mapku, prikaz ./Editor
Ak chete vytvorit random mapku, spustite ./Editor --random
To vytvori novu random mapku, ktoru si dalej mozete menit v Editore
Povodna mapka je v maps/map01.map.backup

Hra:
Hru spustite prikazom ./Game
Ak chete debug vystup, ./Game --debug
Ak sa nenahra nijaka mapka, ./Editor --random, alebo prepisat backup mapku 
na map01.map
Ak hrate po sieti mali by ste mat rovnake mapky :))

Ovladanie:
Cisla 1, 2, 3 ovladanie v menu
ESC vyjde z menu/hry
Single:
    Modry hrac - sipky pohyb a RCTRL strielanie
    Cerveny - WSAD a LCTRL
Mutli:
    Obidve postavicky sipky a RCTRL

Editor:
    lava mys - zamiena debnicku/stenu/prazdne policko
    prava mys - zamiena priechodnost/nepriechodnost 
	(ak je na policku vlavo hore cerveny stvorcek, je stena nepriechodna)
    ESC - vypne editor a ulozi mapku (takze si staru radsej zalohujte >:-)

-------------------
      WINDOWS:
-------------------

To iste ako na Unixe, SDL je prilozene

Instalacia:
Otvorit CrazyCoder.sln vo Visual Studiu .NET 2003 a buildnut

Hra:
Game.exe

Editor:
Editor.exe