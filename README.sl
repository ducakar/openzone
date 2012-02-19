
                                        OpenZone

                                     različica 0.3.0

Nadzor kamere
-------------
gibanje miške       - rotiraj kamero
smerne tipke        - rotiraj kamero
w/a/s/d             - pojdi naprej / levo / nazaj / desno
Space               - gor / povečaj višino
LCtrl               - dol / zmanjšaj višino
LShift              - preklopi hitro premikanje

Tab                 - preklopi način vmesnika
Numpad Enter        - preklopi med kamero FreeCam in Strategic
n                   - preklopi nočno gledanje
i                   - inkarniraj se v izbranega bota (ali pilota vozila)

Nadzor bota
-----------
gibanje miške       - glej okoli
smerne tipke        - glej okoli
w/a/s/d             - pojdi naprej / levo / nazaj / desno
LShift              - preklopi tek
LCtrl               - preklopi počep
Space               - skoči

levi klik           - sproži orožje / rotiraj predmet, ki ga držiš
desni klik          - izvedi dejanje na označenem predmetu
srednji klik        - zgrabi označen predmet / spusti predmet, ki ga držiš
kolešček gor        - vrzi predmet, ki ga držiš
kolešček dol        - spravi označen predmet v inventar

n                   - preklopi nočno gledanje (če je na voljo)
Alt + k             - stori samomor
i                   - zapusti bota

Tab                 - preklopi način uporabniškega vmesnika
Numpad Enter        - preklopi med prvoosebno in tretjeosebno kamero

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
gibanje miške       - obračaj vozilo
smerne tipke        - obračaj vozilo
w/a/s/d             - pojdi naprej / levo / nazaj / desno
Space               - gor (samo leteča vozila)
LCtrl               - dol (samo leteča vozila)

levi klik           - sproži izbrano orožje
desni klik          - izberi naslednje orožje

x                   - izstopi
Alt + x             - izskoči

Numpad *            - preklopi prosti pogled (glej okoli brez obračanja vozila)

Razno
-----
Esc                 - izhod v glavni meni

Alt + vlečenje      - vleka oken uporabniškega vmesnika

o                   - hitro vrti nebo naprej
Shift + o           - hitro vrti nebo nazaj

F5                  - hitro shranjevanje (v <config>/saves/quicksave.ozState)*
F7                  - hitro nalaganje (iz <config>/saves/quicksave.ozState)*
F8                  - naloži zadnje samodejno shranjeno stanje
                      (iz <config>/saves/autosave.ozState)*
F10                 - zajem zaslona
                      (v <config>/screenshots/screenshot DATUM ČAS.bmp)*
F11                 - preklopi celozaslonski način
Ctrl + F11          - preklopi centriranje miške in sistemski kazalec v okenskem
                      načinu
F12                 - minimiraj in zamrzni
Ctrl + F12          - končaj program

* Mesto imenika <config> pri običajni namestitvi:
- Linux:      /home/<uporabnik>/.config/openzone
- Windows XP: C:\Documents and Settings\<uporabnik>\Application Data\openzone
- Windows 7:  C:\Users\<uporabnik>\AppData\Roaming\openzone

Problemi
--------
- Prepričaj se, da imaš *NAJNOVEJŠE* gonilnike za grafično kartico.
- Na Linuxu utegne izklop namiznih učinkov precej pohitriti grafiko.
- Potrebuješ vsaj grafično kartico in gonilnik s podporo za OpenGL 2.1 in
  naslednjimi razširitvami:
  - GL_ARB_framebuffer_object in
  - GL_EXT_texture_compression_s3tc.
  Kartica s popolno podporo za DirectX 9.0c bi morala zadostovati. Na Linuxu
  pogon deluje tako na lastniških kot na odprtokodnih gonilnikih Gallium3D.
- Če uporabljaš gonilnike osnovane na Gallium3D in teksture manjkajo, nastavi
  shader.setSamplerIndices na "false" v datoteki <config>/client.rc.
- Če uporabljaš gonilnike AMD/ATI Catalyst in je teren bel/siv nastavi
  shader.setSamplerIndices na "true" v datoteki <config>/client.rc.
- Če imaš staro grafično kartico ali gonilnike AMD/ATI Catalyst in manjkajo
  modeli MD2, nastavi shader.vertexTexture na "false" v datoteki
  <config>/client.rc, sicer pa na "true" za hitrejše izrisovanje.

Odvisnosti
----------
Binarni paketi bodisi že vsebujejo potrebne knjižnice ali ji imajo navedene
navedene kot odvisnost.

Sicer so za poganjanje potrebne naslednje knjižnice:
- libpulse
- PhysFS
- SDL
- Lua
- OpenGL
- OpenAL
- FreeImage (neobvezno, za orodje ozBuild)
- libvorbis
- libmad (neobvezno, za predvananje formata MP3)
- faad2 (neobvezno, za predvajanje formata AAC).

Ukazna vrstica
--------------
openzone [-v] [-l | -i <misija>] [-t <št>] [-p <predpona>]

-v
      Zgovornejši dnevnik.

-l
      Preskoči glavni meni in naloži nazadnje samodejno shranjeno stanje.

-i <misija>
      Preskoči glavni meni in poženi misijo <misija>.
      Misije se nahajajo v <predpona>/share/openzone/lua/mission.

-t <št>
      Končaj po <št> sekundah (lahko je decimalno število) in kot naključnostno
      seme uporabi 42. Uporabno za benchmark.

-p <predpona>
      Nastavi podatkovni imenik na <predpona>/share/openzone.
      Privzeto: '/usr'.

Nastavitve
----------
Pot do nastavitvene datoteke je "$HOME/.config/openzone/client.rc" na Linuxu
podobnih sistemih ter "%APPDATA%\openzone\client.rc" na Windowsu. Ob običajni
namesitvi je ta pot torej:
- Linux: /home/<uporabnik>/.config/openzone/client.rc
- WinXP: C:\Documents and Settings\<uporabnik>\Application Data\openzone\client.rc
- Win7:  C:\Users\<uporabnik>\AppData\Roaming\openzone\client.rc

spremenljivka [tip] privzeta_vrednost
-------------------------------------
_version [string] "0.3.0"
  Različica OpenZone, za katero so pisane nastavitve. Če se različici ne
  ujemata, se ob izhodu shranijo posodobljene nastavitve. Spremenljivke, ki so v
  novi verziji opuščene, so odstranjene.

camera.angle [float] 80.0
  Navpični kot kamere. Vodoravni kot je prilagojen glede na razmerje stranic
  ločljivosti.

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
  Vrhnji imenik, ki se rekurzivno preišče za datoteke *.oga, *.ogg, *.mp3 in
  *.aac. Vse najdene datoteke so skladbe v Predvajalniku glasbe poleg glasbe, ki
  je priložena igri.

dir.prefix [string] Linux: "/usr", Windows: "."
  Predpona za imenike v katerih se nahajajo podatki za igro. Imenik
  share/openzone se morat nahajati v tem imeniku.

lingua [string] ""
  Koda jezika, za katerega naj se naložijo prevodi. Ta mora ustrezati imenu
  katerega od podimenikov imenika "lingua", ki se nahaja v katerem od arhivov s
  podatki.

mouse.accelFactor [float] 0.04
  Običajno operacijsko sistem pospeši miško, preden OpenZone prebere položaj
  le-te, z izjemo okolja X11, če aplikacija teče v celozaslonskem načinu. V tem
  primeru skuša OpenZone oponašati pospešitev.

profile.playerName [string] "<user>"
  Ime igralca. Privzeto se uporabi uporabniško ime z veliko začetnico.

  render.deferred [bool] false
  Uporabi odloženo senčene (deferred shading). Trenutno še ni implementirano.
  Ta nastavitev se upošteva le, če je vklopljeno upodabljanje v medpomnilnik.

render.lowDetail [bool] false
  Za izboljšanje hitrosti upodabljanja uporabi preprostejše senčenje (brez
  odseva gladkih površin in izseva) ter ne izrisuj zvezd na nebu.

  render.offscreen [bool] true
  Vklopi upodabljanje v medpomnilnik (offscreen rendering) namesto neposredno na
  zaslon. To omogoči nekatere napredne zmožnosti, kot je odloženo senčenje
  (deferred shading) in postprocesiranje. Omogoča tudi, da se svet upodablja na
  ločljivosti različni od zaslonske.

render.offscreenFilte [NEAREST|LINEAR] LINEAR
  Določi kateri filter za raztegovanje/krčenje teksture se uporabi pri izrisu
  vsebine medpomnilnika pri vklopljenem render.offscreen. Če je LINEAR se
  vrednosti pikslov interpolirajo (zamegljena slika), če pa NEAREST, se izbere
  vrednost najbližjega piksla (kockasta slika).
  Ta nastavitev nima učinka, če je velikost medpomnilnika za upodabljanje enaka
  zaslonski ločljivosti. Ponavadi je boljše nastavitev LINEAR, razen če je
  zaslonska ločljivost večkratnik velikosti medpomnilnika (z drugimi besedami,
  če je 1.0 / render.scale celo število), bo za marsikoga boljša nastavitev
  NEAREST.

render.postprocess [bool] true
  Vključi postprocesiranje. Deluje le, če je vklopljen render.offscreen.

render.scale [float] 1.0
  Povečava medpomnilnika za upodabljanje glede na zaslonsko ločljivost.

render.showAim [bool] false
  Prikaže majhen zelen kvadratek v smeri, v katero gleda kamera. To je namenjeno
  preverjanju zaznave trkov.

render.showBounds [bool] false
  Prikaže mejne kvadre predmetov. Za trdne predmete so zelene barve, za
  (za trke) prosojne predmete sive, za zgradbe modre in za premične dele zgradb
  rdeče.

render.visibilityRange [float] 300.0
  Razdalja vidljivosti.

render.vsync [bool] true
  Skušaj vklopiti sinhronizacijo upodabljanja z osveževanjem zaslona
  (vertical synchronisation).

seed [int | string] "TIME"
  Seme za generator naključnih števil. Lahko je celo število ali, če naj se
  uporabi trenutni čas, "TIME".

shader.setSamplerIndices [bool] false
  Nastavi indekse samplerjev tekstur. Če je vklopljeno, teksture prenehajo
  delovati na gonilnikih Gallium3D, pa je je izklopljeno, teksture ne delujejo
  pravilno na starejših gonilnikih ATI/AMD Catalyst.

shader.vertexTexture [bool] false
  Omogoči teksture za senčilnike oglišč, s katerimi se lahko animira površine na
  senčilnikih. Potreben je vsaj GeForce 6 ali Radeon HD 2xxx. Ne deluje na
  gonilnikih ATI/AMD Catalyst.

sound.device [string] ""
  Zvočna naprava, ki naj jo uporabi OpenAL. Če je niz prazen, se izbira prepusti
  OpenAL.

sound.volume [float] 1.0
  Glasnost. 1.0 pomeni izvirno glasnost.

ui.aspect [float] 0.0
  Razmerje uporabniškega vmesnika. Če je 0.0 se določi iz razmerja ločljivosti.

ui.scale [float] 1.0
  Pomanjšava uporabniškega vmesnika.

ui.showBuild [bool] false
  Prikaži okno z gumbi za dodajanje raznih predmetov v svet. Za namene razvoja.

ui.showDebug [bool] false
  Prikaži okno, ki prikazuje koordinate, usmeritev in druge podatke o kameri,
  nadzorovanem botu, označenem predmetu ... Za namene razvoje.

ui.showFPS [bool] false
  Prikazuj število sličic na sekundo. Ker se svet simulira na 60 Hz, število
  sličic na sekundo ne more preseči te vrednosti.

window.fullscreen [bool] false
  Poženi v celozaslonskem načinu.

window.height [int] 0
  Navpična ločljivost zaslona. Če je 0, se uporabi ločljivost namizja.

window.width [int] 0
  Vodoravna ločljivost zaslona. Če je 0, se uporabi ločljivost namizja.

Avtorske pravice za pogon OpenZone
----------------------------------
Copyright © 2002-2012 Davorin Učakar
Avtor:   Davorin Učakar
Kontakt: davorin.ucakar@gmail.com

Ta program je POPOLNOMA BREZ GARANCIJE.
To je prosto programje pod licenco GNU GPLv3, ki se lahko razpečuje pod določenimi pogoji;
za podrobnosti glejte datoteko COPYING.

Knjižnico liboz samo se lahko razpečuje tudi pod licenco X11;
za podrobnosti glejte datoteko etc/liboz/COPYING.

Avtorske pravice za podatke
---------------------------
Podatki (vse pod imenikom 'share/openzone') in priložene knjižnice ('*.dll' pod
imenikom 'bin') so delo različnih avtorjev. Za podrobnosti glejte pripadajoče
datoteke README* in COPYING* (v arhivih ZIP).
