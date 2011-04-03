
                                        OpenZone

                                     različica 0.2.1

Nadzor kamere
-------------
gibanje miške       - sukaj kamero (kadar si v vmesniškem načinu, potisni kazalec v rob zaslona)
smerne tipke        - sukaj kamero
w/a/s/d             - pojdi naprej/levo/nazaj/desno
Space               - gor / povečaj višino
Ctrl                - dol / zmanjšaj višino
Tab                 - preklopi način vmesnika
Numpad Enter        - preklopi med kamero FreeCam in Strategic
i                   - inkarniraj se v izbranega bota (deluje tudi ko je bot v vozilu)

Nadzor bota
-----------
gibanje miške       - glej okoli (kadar si v vmesniškem načinu, potisni kazalec v rob zaslona)
smerne tipke        - glej okoli
w/a/s/d             - pojdi naprej/levo/nazaj/desno
Shift               - preklopi tek
Ctrl                - preklopi počep
Space               - skoči

left click          - sproži orožje
right click         - uporabi označen predmet ali vstopi v označeno vozilo
middle click        - zgrabi/spusti označen predmet ali vzemi označen predmet iz inventarja, kadar
                      klikneš na predmet v inventarju
wheel up            - vrzi predmet, ki ga trenutno držiš
wheel down          - pospravi izbran predmet v inventar

m                   - stori samomor
i                   - zapusti bota
p                   - preklopi zmožnost stopanja čez manjše ovire, npr. hoja po stopnicah

Tab                 - preklopi način uporabniškega vmesnika
Numpad Enter        - preklopi med prvoosebno in tretjeosebno kamero
Numpad *            - preklopi proti pogled (glej okoli, brez da bi obračal bota)

f                   - kretnja "kazanje"
g                   - kretnja "nazaj"
h                   - kretnja "pozdrav"
j                   - kretnja "pomahaj"
k                   - kretnja "ponori"
l                   - predvajaj vse animacije

Nadzor vozil
------------
gibanje miške       - glej okoli (kadar si v vmesniškem načinu, potisni kazalec v rob zaslona)
smerne tipke        - glej okoli
w/a/s/d             - pojdi naprej/levo/nazaj/desno
Space               - gor (lebdeča in zračna vozila)
Ctrl                - dol (zračna vozila)

left click          - sproži izbrano orožje
right click         - izberi naslednje orožje

x                   - izstopi
j                   - izskoči

Razno
-----
o                   - prevrti nebo naprej za 1/8 dneva

Levi Alt            - tišči za vleko oken

F11                 - zajem zaslona (shrani se v ~/config/.openzone/screenshot DATUM ČAS.bmp)
F12                 - minimiraj in zamrzni (ne dela na Windows)

Esc                 - končaj program

Vedenja umetne inteligence
--------------------------
beastie (beštija)
- Nakljušno teči in skači naokoli.
prey (škrat)
- Beži pred vsemi razen pred drugim škratom.
predator (vitez)
- Išči škrate in teči proti njim.
droid (droid)
- Če vidiš kogarkoli, ki ni droid ali poveljniški droid, ga ulovi in ustreli.
- Če ni nobene tarče, potem sledi povelniškemu droidu, če je kakšen v bližini, sicer išči tarčo.

Nasveti
-------
- Skočiš lahko višje, če med skokom na hitro počepneš in od-počepneš.
- Klikni orožje v inventarju z desnim gumbom, da ga vzameš v roke oziroma pospraviš nazaj.
- Če želiš dvigniti predmet, morajo biti tvoje roke prazne (brez orožja), ne smeš plavati in predmet
  ne sme biti pretežak. Če ne stojiš dovolj blizu predmeta, ti lahko zdrsne z rok takoj zatem, ko
  ga pograbiš. Prav tako utegne zdrsniti z rok, če ga od zgoraj zadane drug predmet.
- Da se povzpneš po lestvi, glej gor oziroma dol po lestvi. Če se pomakneš stran od lestve, padeš
  dol.
- Pod vodo se lahko pomikaš nekoliko hitreje, če hodiš po dnu. Pomikaj se v dno, da to dosežeš.
- Če je vezilo uničeno, samodejno izvrže pilota.
- Za nekaj zabave ustvari droida (ne poveljniškega droida) z menija Ustvari.
- Za še nekaj več zabave, daj tega droida v tank. (Inkarniraj se v droida, vkrcaj se v tank in
  odinkarniraj. Umetna inteligenca bo prevzela droida, ki bo raztural.)

Ukazna vrstica
--------------
openzone [MOŽNOSTI]

--help
      Prikaže to sporočilo s pomočjo.

--load, --no-load
-l, -L
      Vklopi ali izklopi samodejno nalaganje iz ~/.config/openzone/default.ozState med zagonom.
      To preglasi nastavitev 'autoload'.

--save, --no-save
-s, -S
      Vklopi ali izklopi samodejno sharnjevanje iz ~/.config/openzone/default.ozState ob izhodu.
      To preglasi nastavitev 'autosave'.

--time št
-t št
      Končaj po št sekundah (lahko je decimalno število). Za potrebe benchmarka.

Opombe
------
- Med prvim zagonom se ustvari nastavitvena datoteka ~/.config/openzone/client.rc
  (na Windowsu: %APPDATA%\OpenZone\client.rc) s privzetimi nastavitvami. Uredi jo po želji.
- %APPDATA% ponavadi pomeni C:\Documents and Settings\uporabnik\Application Data na Windowsu XP ali
  C:\Users\uporabnik\AppData\Roaming na Windowsu 7

Problemi
--------
- Prepričaj se, da imaš *NAJNOVEJŠE* gonilnike za grafično kartico.
- Potrebuješ vsaj grafično kartico in gonolnik s podporo za OpenGL 2.1 in naslednje razširitve:
  - GL_ARB_vertex_array_object,
  - GL_ARB_framebuffer_object,
  - GL_ARB_texture_float,
  - GL_EXT_texture_compression_s3tc,
  - senčilni jezik GLSL 1.30 in
  - vertex texture fetch (neobvezno).
  Kartica s popolno podporo za DirectX 9.0c bi morala zadostovati.

Odvisnosti
----------
- SDL
- SDL_image (potrebno le za prebuild, potrebuje vsaj podporo za PNG in JPEG)
- SDL_ttf
- SDL_net (neobvezno)
- OpenGL (glave MesaGL)
- OpenAL
- FreeALUT
- libvorbis
- Lua

Na Linuxu morate namestiti pakete, ki vsebujejo zgornje knjižnice.
V distribuciji za Windows so vse potrebne knjižnice že vključene.

Licenca
-------
Copyright (C) 2002-2011  Davorin Učakar <davorin.ucakar@gmail.com>

Ta program je POPOLNOMA BREZ GARANCIJE.
To je prosto programje, ki se lahko distribuira po določenimi pogoji;
za podrobnosti glej datoteko COPYING.

Podatki (vse pod direktorijem 'share/openzone') in priložene knjižnice ('*.dll' po direktorijem
'bin') so delo različnih avtorjev. Za podrobnosti glej pripadajoče datoteke *README in *COPYING.
