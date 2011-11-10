
                                        OpenZone

                                     različica 0.3.0

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
srednji klik        - zgrabi/spusti označen predmet
kolešček gor        - vrzi predmet, ki ga trenutno držiš
kolešček dol        - pospravi izbran predmet v inventar

ALT + m             - stori samomor
ALT + p             - preklopi zmožnost stopanja čez manjše ovire, npr. hoja po stopnicah
i                   - zapusti bota

Tab                 - preklopi način uporabniškega vmesnika
Numpad Enter        - preklopi med prvoosebno in tretjeosebno kamero
Numpad *            - preklopi proti pogled (glej okoli, brez da bi obračal bota)

f                   - kretnja "kazanje"
g                   - kretnja "nazaj"
h                   - kretnja "pozdrav"
j                   - kretnja "pomahaj"
k                   - kretnja "ponori"
l                   - predvajaj vse animacije

Inventar
--------
levi klik           - odvrzi predmet ali prestavi predmet v drug inventar, kadar imaš odprti dve
                      okni z inventarjem
srednji klik        - zgrabi predmet iz inventarja
desni klik          - uporabi predmet, za orožja: vzemi orožje v roke oziroma ga pospravi

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
ALT + x             - izskoči

Razno
-----
o                   - hitro vrti nebo naprej
Shift + o           - hitro vrti nebo nazaj

LAlt + vlečenje     - vleka oken

F5                  - hitro shranjevanje (v ~/.config/openzone/quicksave.ozState)
F7                  - hitro nalaganje (iz ~/.config/openzone/quicksave.ozState)
F8                  - naloži zadnje samodejno shranjeno stanje
                      (iz ~/.config/openzone/autosave.ozState)
F10                 - zajem zaslona (shrani se v '~/config/.openzone/screenshot DATUM ČAS.bmp')
F11                 - preklopi celozaslonski način
Ctrl + F12          - minimiraj in zamrzni
Ctrl + F12          - prisilni izhod

Esc                 - končaj program

Ukazna vrstica
--------------
openzone [--help] [(--load | -l) | (--init | -i) <function>]
         [(--time | -t) <št>] [(--prefix | -p) <predpona>]

--help
      Prikaže to sporočilo s pomočjo.

-l, --load
      Preskoči glavni meni in naloži samodejno shranjeno stanje.
      Samodejno shranjeno stanje se nahaja v ~/.config/openzone/autosave.ozState.

-i <misija>, --init <misija>
      Preskoči glavni meni in poženi misijo <misija>.
      Misije se nahajajo v <predpona>/share/openzone/lua/mission in v datotekah
      poimenovanih <misija>.lua.

-t <št>, --time <št>
      Končaj po <št> sekundah (lahko je decimalno število). Za potrebe benchmarka.

-p <predpona>, --prefix <predpona>
      Nastavi podatkovni imenik na <predpona>/share/openzone in imenik lokalizacije na
      <predpona>/share/locale.

Opombe
------
- Med prvim zagonom se ustvari nastavitvena datoteka ~/.config/openzone/client.rc
  (na Windowsu: %APPDATA%\OpenZone\client.rc) s privzetimi nastavitvami. Uredi jo po želji.
- %APPDATA% ponavadi pomeni 'C:\Documents and Settings\uporabnik\Application Data' na Windowsu XP
  ali C:\Users\uporabnik\AppData\Roaming na Windowsu 7.

Problemi
--------
- Prepričaj se, da imaš *NAJNOVEJŠE* gonilnike za grafično kartico.
- Na Linuxu utegne izklop namiznih učinkov pohitriti grafiko, kadar ne poganjate v celozaslonskem
  načinu.
- Potrebuješ vsaj grafično kartico in gonilnik s podporo za OpenGL 2.1 in naslednjimi razširitvami:
  - GL_ARB_framebuffer_object in
  - GL_EXT_texture_compression_s3tc.
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
- OpenGL
- OpenAL
- libvorbis
- Lua

Na Linuxu morate namestiti pakete, ki vsebujejo zgornje knjižnice.
V distribuciji za Windows so vse potrebne knjižnice že vključene.

Odvisnosti za gradnjo
---------------------
- osnovni sistem GNU (bash, coreutils, grep, sed ...)
- gettext
- cmake
- GNU make
- GNU binutils
- GCC >= 4.6 ali LLVM/Clang >= 3.0
- razvojni paketi knjižnic iz prejšnjega razdelka

Avtorske pravice za pogon OpenZone
----------------------------------
Copyright (C) 2002-2011  Davorin Učakar. Vse pravice pridržane.
Avtor:   Davorin Učakar
Kontakt: davorin.ucakar@gmail.com

Ta program je POPOLNOMA BREZ GARANCIJE.
To je prosto programje pod licenco GNU GPLv3, ki se lahko razpečuje pod določenimi pogoji;
za podrobnosti glejte datoteko COPYING.

Avtorske pravice za podatke
---------------------------
Podatki (vse pod imenikom 'share/openzone') in priložene knjižnice ('*.dll' pod imenikom 'bin') so
delo različnih avtorjev. Za podrobnosti glejte pripadajoče datoteke *README in *COPYING.
