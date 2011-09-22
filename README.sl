
                                        OpenZone

                                     različica 0.1.3

Nadzor kamere
-------------
gibanje miške       - sukaj kamero (kadar si v vmesniškem načinu, potisni kazalec v rob zaslona)
smerne tipke        - sukaj kamero
w/a/s/d             - pojdi naprej/levo/nazaj/desno
Space               - gor / povečaj višino
LCtrl               - dol / zmanjšaj višino
LShift              - preklopi hitro premikanje
Tab                 - preklopi način vmesnika
Numpad Enter        - preklopi med kamero FreeCam in Strategic
i                   - inkarniraj se v izbranega bota (deluje tudi ko je bot v vozilu)

Nadzor bota
-----------
gibanje miške       - glej okoli (kadar si v vmesniškem načinu, potisni kazalec v rob zaslona)
smerne tipke        - glej okoli
w/a/s/d             - pojdi naprej/levo/nazaj/desno
LShift              - preklopi tek
LCtrl               - preklopi počep
Space               - skoči

levi klik           - sproži orožje
desni klik          - uporabi označen predmet ali vstopi v označeno vozilo
srednji klik        - zgrabi/spusti označen predmet ali vzemi označen predmet iz inventarja, kadar
                      klikneš na predmet v inventarju
kolešček gor        - vrzi predmet, ki ga trenutno držiš
kolešček dol         - pospravi izbran predmet v inventar

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
Space               - gor (samo leteča vozila)
LCtrl               - dol (samo leteča vozila)

levi klik           - sproži izbrano orožje
desni klik          - izberi naslednje orožje

x                   - izstopi
j                   - izskoči

Razno
-----
o                   - prevrti nebo naprej za 1/8 dneva

Alt + vlečenje      - vleka oken

F5                  - hitro shranjevanje (v ~/.config/openzone/quicksave.ozState)
F7                  - hitro nalaganje (iz ~/.config/openzone/quicksave.ozState)
F7                  - naloži zadnje avtomatično shranjeno stanje
                      (iz ~/.config/openzone/autosave.ozState)
F11                 - zajem zaslona (shrani se v ~/config/.openzone/screenshot DATUM ČAS.bmp)
F12                 - minimiraj in zamrzni
Shift + F12         - preklopi celozaslonski način
Ctrl + F12          - prisilni izhod

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
- Če ni nobene tarče, potem sledi poveljniškemu droidu, če je kakšen v bližini, sicer išči tarčo.

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
- Tečeš lahko le, če ne čepiš in ne nosiš nobenega predmeta.
- Če je vozilo uničeno, samodejno izvrže pilota.
- Za nekaj zabave ustvari droida (ne poveljniškega droida) z menija Ustvari.
- Za še nekaj več zabave, daj tega droida v tank. (Inkarniraj se v droida, vkrcaj se v tank in
  odinkarniraj. Umetna inteligenca bo prevzela droida, ki bo raztural.)

Ukazna vrstica
--------------
openzone [--help] [--load | -l | --no-load | -L]
         [--save | -s | --no-save | -S] [(--time | -t) <št>]
         [(--prefix | -p) <predpona>]

--help
      Prikaže to sporočilo s pomočjo.

-l, --load
-L, --no-load
      Vklopi ali izklopi samodejno nalaganje iz ~/.config/openzone/default.ozState med zagonom.
      To preglasi nastavitev 'autoload'.

-s, --save
-S, --no-save
      Vklopi ali izklopi samodejno shranjevanje iz ~/.config/openzone/default.ozState ob izhodu.
      To preglasi nastavitev 'autosave'.

-t <št>, --time <št>
      Končaj po <št> sekundah (lahko je decimalno število). Za potrebe benchmarka.

-p <predpona>, --prefix <predpona>
      Nastavi podatkovni imenik na <predpona>/share/openzone in imenik lokalizacije na
      <predpona>/share/locale.

Opombe
------
- Med prvim zagonom se ustvari nastavitvena datoteka ~/.config/openzone/client.rc
  (na Windowsu: %APPDATA%\OpenZone\client.rc) s privzetimi nastavitvami. Uredi jo po želji.
- %APPDATA% ponavadi pomeni C:\Documents and Settings\uporabnik\Application Data na Windowsu XP ali
  C:\Users\uporabnik\AppData\Roaming na Windowsu 7.

Problemi
--------
- Prepričaj se, da imaš *NAJNOVEJŠE* gonilnike za grafično kartico.
- Potrebuješ vsaj grafično kartico in gonilnik s podporo za OpenGL 2.1 in naslednjimi razširitvami:
  - GL_ARB_framebuffer_object.
  Kartica s popolno podporo za DirectX 9.0c bi morala zadostovati, zagotovo pa zadostuje kartica s
  podporo za DirectX 10.
  Na Linuxu pogon deluje tako na lastniških kot na odprtokodnih gonilnikih Gallium3D.
- Če uporabljate gonilnike osnovane na Gallium3D in teksture (večinoma) manjkajo, nastavite
  shader.setSamplerIndices na "false" v datoteki client.rc.
- Če uporabljate gonilnike AMD/ATI Catalyst in je teren bel/siv nastavite shader.setSamplerIndices
  na "true" v datoteki client.rc.
- Če imate staro grafično kartico ali gonilnike AMD/ATI Catalyst in manjkajo modeli MD2, nastavite
  shader.vertexTexture na "false" v datoteki client.rc.
- Za hitrejše izrisovanje lahko omogočite animacijo MD2 modelov na senčilnikih, če nastavite
  shader.vertexTexture na "true" v client.rc.

Odvisnosti
----------
- pthreads (na Linuxu že vgrajeno v glibc)
- libintl (na Linuxu že vgrajeno v glibc)
- SDL
- SDL_image (potrebno le za ozPrebuild, vsaj s podporo za PNG in JPEG)
- SDL_ttf
- SDL_net (potrebno le za večigralstvo, ki pa še ne deluje)
- OpenGL
- OpenAL
- libvorbis
- Lua

Na Linuxu morate namestiti pakete, ki vsebujejo zgornje knjižnice.
V distribuciji za Windows so vse potrebne knjižnice že vključene.

Odvisnosti za gradnjo
---------------------
- osnovni sistem GNU (bash, coreutils, grep, sed ...)
- cmake
- GNU make
- GNU binutils
- GCC >= 4.5 ali LLVM/Clang >= 2.9
- gettext
- razvojni paketi knjižnic iz prejšnjega razdelka

Licenca
-------
Copyright (C) 2002-2011  Davorin Učakar <davorin.ucakar@gmail.com>

Ta program je POPOLNOMA BREZ GARANCIJE.
To je prosto programje, ki se lahko distribuira po določenimi pogoji;
za podrobnosti glej datoteko COPYING.

Podatki (vse pod imenikom 'share/openzone') in priložene knjižnice ('*.dll' po imenikom
'bin') so delo različnih avtorjev. Za podrobnosti glej pripadajoče datoteke *README in *COPYING.
