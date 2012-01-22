
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

levi klik           - sproži orožje ali rotiraj predmet, ki ga držiš
desni klik          - uporabi označen predmet ali vstopi v označeno vozilo
srednji klik        - zgrabi označen predmet ali spusti predmet, ki ga držiš
kolešček gor        - vrzi predmet, ki ga držiš
kolešček dol        - pospravi izbran predmet v inventar

ALT + k             - stori bridko smert
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
desni klik          - uporabi predmet, za orožja: vzemi orožje v roke ali ga pospravi

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
Esc                 - izhod v glavni meni

LAlt + vlečenje     - vleka oken uporabniškega vmesnika

o                   - hitro vrti nebo naprej
Shift + o           - hitro vrti nebo nazaj

F5                  - hitro shranjevanje (v ~/.config/openzone/quicksave.ozState)
F7                  - hitro nalaganje (iz ~/.config/openzone/quicksave.ozState)
F8                  - naloži zadnje samodejno shranjeno stanje
                      (iz ~/.config/openzone/autosave.ozState)
F10                 - zajem zaslona (shrani se v '~/config/.openzone/screenshot DATUM ČAS.bmp')
F11                 - preklopi celozaslonski način
Ctrl + F11          - preklopi centriranje miške in sistemski kazalec (samo v okenskem načinu)
F12                 - minimiraj in zamrzni
Ctrl + F12          - končaj program

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
V distribuciji za Windows so vse potrebne knjižnice že vključene.
Na Linuxu je potrebno namestiti naslednje knjižnice/pakete:
- libpulse
- PhysFS
- SDL
- Lua
- OpenGL
- OpenAL
- FreeImage (neobvezno, za orodje ozBuild)
- libvorbis
- libmad (neobvezno, za podporo MP3)
- faad2 (neobvezno, za podporo AAC).

Odvisnosti za gradnjo
---------------------
- GNU/Linux ali podoben OS
- cmake
- GNU make
- GNU binutils
- GCC >= 4.6 ali LLVM/Clang >= 3.0
- gettext (neobvezno, za prevajanje)
- razvojni paketi vseh knjižnic iz prejšnjega razdelka.

Ukazna vrstica
--------------
openzone [-v] [-l | -i <misija>] [-t <št>] [-p <predpona>]

-v
      Zgovornejši dnevnik.

-l
      Preskoči glavni meni in naloži nazadnje samodejno shranjeno stanje.

-i <misija>
      Preskoči glavni meni in poženi misijo <misija>.
      Misije se nahajajo v <predpona>/share/openzone/lua/mission v datotekah
      poimenovanih <misija>.lua.

-t <št>
      Končaj po <št> sekundah (lahko je decimalno število) in kot naključnostno
      seme uporabi 42. Uporabno za benchmark.

-p <predpona>
      Nastavi podatkovni imenik na <predpona>/share/openzone.
      Privzeto: '/usr'.

Nastavitve
----------
Pot do nastavitvene datoteke je "$HOME/.config/openzone/client.rc" na Linuxu podobnih sistemih ter
"%APPDATA%\openzone\client.rc" na Windowsu. Celotna pot je torej (od običajni namestitvi):
- Linux:      /home/<uporabnik>/.config/openzone/client.rc
- Wine:       /home/<uporabnik>/.wine/drive_c/users/<uporabnik>/Podatki programov/openzone/client.rc
- Windows XP: C:\Documents and Settings\<uporabnik>\Application Data\openzone\client.rc
- Windows 7:  C:\Users\<uporabnik>\AppData\Roaming\openzone\client.rc

spremenljivka [tip] privzeta_vrednost
-------------------------------------
_version [string] "0.3.0"
  Različica OpenZone, za katero so pisane nastavitve. Če se različici ne ujemata, se ob izhodu
  shranijo posodobljene nastavitve. Spremenljivke, ki so v novi verziji opuščene, so odstranjene.

camera.angle [float] 80.0
  Navpični kot kamere. Vodoravni kot je prilagojen glede na razmerje stranic ločljivosti.

camera.aspect [float] 0.0
  Razmerje stranic širina/višina. Če je 0.0, je enak razmerju ločljivosti.

camera.keysXSens [float] 1.0
  Občutljivost za tipki levo/desno.

camera.keysYSens [float] 1.0
  Občutljivost za tipki gor/dol.

camera.mouseXSens [float] 1.0
  Občutljivost miške v vodoravni smeri.

camera.mouseYSens [float] 1.0
  Občutljivost miške v navpični smeri.

dir.music [string] ""
  Vrhnji imenik, ki se rekurzivno preišče za datoteke *.oga, *.ogg, *.mp3 in *.aac. Vse najdene
  datoteke so skladbe v Predvajalniku glasbe poleg glasbe, ki je priložena igri.

dir.prefix [string] Linux: "/usr", Windows: "."
  Predpona za imenike v katerih se nahajajo podatki za igro. Imenika share/openzone in share/locale
  se morata nahajati v tem imeniku.

lingua [string] ""
  Koda jezika, za katerega naj se naložijo prevodi. Ta mora ustrezati imenu katerega od podimenikov
  imenika "lingua", ki se nahaja v katerem od arhivov s podatki.

modules.profile.playerName [string] "<user>"
  Ime igralca. Privzeto se uporabi uporabniško ime z veliko začetnico.

mouse.accelFactor [float] 0.04
  Običajno operacijsko sistem pospeši miško, preden OpenZone prebere položaj le-te, z izjemo okolja
  X11, če aplikacija teče v celozaslonskem načinu. V tem primeru skuša OpenZone emulirati
  pospešitev, saj je v nasprotnem primeru miškin kazalec prepočasen.

render.deferred [bool] false
  Uporabi odloženo senčene (deferred shading). Trenutno še ni zares implementirano.
  Ta nastavitev se upošteva le, če je vklopljeno upodabljanje v medpomnilnik.

render.offscreen [bool] true
  Vklopi upodabljanje v medpomnilnik (offscreen rendering) namesto neposredno na zaslon. To omogoči
  nekatere napredne zmožnosti, kot je odloženo senčenje (deferred shading) in postprocesiranje.
  Omogoča tudi, da se svet upodablja na ločljivosti različni od zaslonske.

render.postprocess [bool] true
  Vključi postprocesiranje. Deluje le, če je vklopljen render.offscreen.

render.scale [float] 1.0
  Povečava medpomnilnika za upodabljanje glede na zaslonsko ločljivost.

render.showAim [bool] false
  Prikaže majhen zelen kvadratek v smeri, v katero gleda kamera. To je namenjeno preverjanju zaznave
  trkov.

render.showBounds [bool] false
  Prikaže mejne kvadre predmetov. Za trdne predmete so zelene barve, za (za trke) prosojne predmete
  sive, za zgradbe modre in za premične dele zgradb redeče.

render.visibilityRange [float] 300.0
  Razdalja vidljivosti.

screen.full [bool] false
  Poženi v celozaslonskem načinu.

screen.height [int] 0
  Navpična ločljivost zaslona. Če je 0, se uporabi ločljivost namizja.

screen.disableScreensaver [bool] false
  Običajno izklop ohranjevalnika zaslona ni potreben, saj večina sistemov prepreči poganjanje
  ohranjevalnika zaslona, če teče celozaslonska aplikacija, pa tudi igralec med igranjem večino časa
  uporablja miško in tipkovnico.

  Če ohranjevalnik zalona želite kljub vsemu izključiti, nastavite to nastavitev na true. Vendar
  pozor: če se aplikacija ne zaključi pravilno, ohranjevalnik zaslona lahko ostane izklopljen, prav
  tako lahko preneha delovati samodejna odsotnost v različnih aplikacijah.

screen.vsync [bool] true
  Skušaj vklopiti sinhronizacijo upodabljanja z osveževanjem zaslona (vertical synchronisation).

screen.width [int] 0
  Vodoravna ločljivost zaslona. Če je 0, se uporabi ločljivost namizja.

seed [int | string] "TIME"
  Seme za generator naključnih števil. Lahko je celo število ali, če naj se uporabi trenutni čas,
  "TIME".

shader.setSamplerIndices [bool] false
  Nastavi indekse samplerjev tekstur. Če je vklopljeno, teksture prenehajo delovati na gonilnikih
  Gallium3D, pa je je izklopljeno, teksture ne delujejo pravilno na starejših gonilnikih ATI/AMD
  Catalyst.

shader.vertexTexture [bool] false
  Omogoči teksture za senčilnike oglišč, s katerimi se lahko animira površine na senčilnikih.
  Potreben je vsaj GeForce 6 ali Radeon HD 2xxx. Ne deluje na gonilnikih ATI/AMD Catalyst.

sound.device [string] ""
  Zvočna naprava, ki naj jo uporabi OpenAL. Če je niz prazen, OpenAL uporabi privzeto napravo.

sound.volume [float] 1.0
  Glasnost. 1.0 pomeni izvirno glasnost.

ui.aspect [float] 0.0
  Razmerje uporabniškega vmesnika. Če je 0.0 se določi iz razmerja ločljivosti.

ui.scale [float] 1.0
  Pomanjšava uporabniškega vmesnika.

ui.showBuild [bool] false
  Prikaži okno z gumbi za dodajanje raznih predmetov v svet. Za namene razvoja.

ui.showDebug [bool] false
  Prikaži okno, ki prikazuje koordinate, usmeritev in druge podatke o kameri, nadzorovanem botu,
  označenem predmetu ... Za namene razvoje.

Avtorske pravice za pogon OpenZone
----------------------------------
Copyright © 2002-2012 Davorin Učakar. Vse pravice pridržane.
Avtor:   Davorin Učakar
Kontakt: davorin.ucakar@gmail.com

Ta program je POPOLNOMA BREZ GARANCIJE.
To je prosto programje pod licenco GNU GPLv3, ki se lahko razpečuje pod določenimi pogoji;
za podrobnosti glejte datoteko COPYING.

Knjižnica liboz samo se lahko razpečuje tudi pod licenco X11;
za podrobnosti glejte datoteko etc/COPYING.liboz.

Avtorske pravice za podatke
---------------------------
Podatki (vse pod imenikom 'share/openzone') in priložene knjižnice ('*.dll' pod imenikom 'bin') so
delo različnih avtorjev. Za podrobnosti glejte pripadajoče datoteke *README in *COPYING.
