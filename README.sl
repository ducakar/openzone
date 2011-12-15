
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
q                   - zarotiraj predmet, ki ga trenutno držiš
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
Esc                 - izhod v glavni meni

LAlt + vlečenje     - vleka oken uporabniškega vmesnika

o                   - hitro vrti nebo naprej
Shift + o           - hitro vrti nebo nazaj

F5                  - hitro shranjevanje (v ~/.config/openzone/quicksave.ozState)
F7                  - hitro nalaganje (iz ~/.config/openzone/quicksave.ozState)
F8                  - naloži zadnje samodejno shranjeno stanje
                      (iz ~/.config/openzone/autosave.ozState)
F10                 - zajem zaslona (shrani se v '~/config/.openzone/screenshot DATUM ČAS.bmp')
Ctrl + F11          - preklopi celozaslonski način
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

Ukazna vrstica
--------------
openzone [-v] [-l | -i <function>] [-t <št>] [-p <predpona>]

-v
      Zgovornejši dnevnik.

-l
      Preskoči glavni meni in naloži nazadnje samodejno shranjeno stanje.

-i <misija>
      Preskoči glavni meni in poženi misijo <misija>.
      Misije se nahajajo v <predpona>/share/openzone/lua/mission in v datotekah
      poimenovanih <misija>.lua.

-t <št>
      Končaj po <št> sekundah (lahko je decimalno število). Za potrebe benchmarka.

-p <predpona>
      Nastavi podatkovni imenik na <predpona>/share/openzone in imenik lokalizacije na
      <predpona>/share/locale.
      Privzeto: '/usr'.

Nastavitve
----------
Pot do nastavitvene datoteke je "$HOME/.config/openzone/client.rc" na Linuxu podobnih sistemih ter
"%APPDATA%\OpenZone\client.rc" na Windowsu. Celotna pot je torej (od običajni namestitvi):
- Linux:      /home/<uporabnik>/.config/openzone/client.rc
- Wine:       /home/<uporabnik>/.wine/drive_c/users/<uporabnik>/Podatki programov/client.rc
- Windows XP: C:\Documents and Settings\<uporabnik>\Application Data\OpenZone\client.rc
- Windows 7:  C:\Users\<uporabnik>\AppData\Roaming\OpenZone\client.rc

spremenljivka [tip] privzeta_vrednost
-------------------------------------
_version [string] "0.3.0"
  Version of OpenZone for which the settings file was created. The only effect of this setting is
  that on version mismatch, OpenZone dumps its configuration on exit and rewrites the old file.
  Deprecated variables are not removed during that process.

camera.angle [float] 80.0
  Vertical camera angle in degrees. Horizontal angle is calculated from aspect ratio.

camera.aspect [float] 0.0
  Aspect ratio width/height. 0.0 means it is calculated from the current screen resolution.

camera.keysXSens [float] 2.0
  Key sensitivity for up/down arrow keys.

camera.keysYSens [float] 2.0
  Key sensitivity for left/right arrow keys.

camera.mouseXSens [float] 0.005
  Mouse sensitivity for X axis when rotating camera.

camera.mouseYSens [float] 0.005
  Mouse sensitivity for X axis when rotating camera.

dir.music [string] ""
  Top directory that will be recursively searched for *.oga, *.ogg and (if compiled with support for
  non-free technologies) *.mp3. All found tracks will be accessible from Music Player along the
  original tracks included in game data.

dir.prefix [string] Linux default: "/usr", Windows default: "."
  Prefix path to game directory structure. The share/openzone and share/locale directories must be
  located inside the prefix directory.

locale.messages [string] ""
  Which locale should be used for translations. Empty string means the system locale is used.
  On Linux, run "locale -a" to see a list of installed/generated locales on your system.
  Currently there are only Slovene and English translations.

modules.profile.playerName [string] "<user>"
  Name of the player. Username of the current user is used as default.

mouse.accelFactor [float] 0.05
  Only used while in fullscreen mode on X11. Usually OS applies mouse acceleration before OpenZone
  reads the mouse input. The only exception is when running in fullscreen mode on X11 server, when
  OpenZone receives unaccelerated mouse input. In that case it tries to emulate X11 mouse
  acceleration, since mouse cursor moves pretty slow otherwise.

render.deferred [bool] false
  Enable deferred shading. Not implemented.
  This option has effect only if offscreen rendering is enabled.

render.offscreen [bool] true
  Enable rendering into offscreen buffers instead directly to the screen. This enables some advanced
  effects like deferred shading, postprocess effects. It also enables that the world and UI are
  rendered at resolutions that differ from the screen resolution.

render.postprocess [bool] true
  When enabled, world is rendered into an offscreen buffer. The image is postprocessed and then
  rendered to the screen. This technique only applies for the world, UI is always rendered directly.
  The offscreen buffer may use different resolution than the screen.

render.scale [float] 1.0
  Scale of resolution of offscreen buffer where world is rendered to.
  This option has effect only if offscreen rendering is enabled.

render.showAim [bool] false
  Draw a small green box at the point you are currenty aiming at. It is intended for testing
  purposes, mostly for testing collision detection.

render.showBounds [bool] false
  Show AABBs of various objects. Green for solid objects, blue-grey for non-solid objects,
  blue for structures and purple for structure entities.

render.visibilityRange [float] 300.0
  Visibility range.

screen.bpp [int] 0
  Desired bits per pixel for the screen mode. This setting is not necessarily obeyed. If zero, SDL
  chooses on its own.

screen.full [bool] false
  Start in fullscreen mode.

screen.height [int] 0
  Vertical screen resolution. Zero equals desktop resolution.

screen.leaveScreensaver [bool] true
  Do not disable screensaver. Usually there is no need to explicitly disable screensaver, since
  most system do this on their own when running an fullscreen application and player moves the mouse
  and/or presses the keys most of the time.
  You can turn this to false if you want to disable screensaver anyways. Note that screensaver may
  remain turned off if the application is not terminated properly.

screen.nvVSync [bool] true
  Try to use vertical synchronisation for Nvidia cards on Linux (sets __SYNC_TO_VBLANK=1).

screen.width [int] 0
  Horizontal screen resolution. Zero equals desktop resolution.

seed [int|string] "TIME"
  Seed for random generator. Integer number or "TIME" string to use the current Unix time as the
  seed.

shader.setSamplerIndices [bool] false
  Set indices for texture samplers. It set, breaks textures on Gallium3D driver. If not set, breaks
  textures on older ATI/AMD Catalyst drivers.

shader.vertexTexture [bool] false
  Use vertex texture fetch feature to perform MD2 model animation in vertex shader. Should work
  on GeForce 6 or newer or Radeon 2xxx or newer. Does not work with Catalyst drivers.

sound.device [string] ""
  Sound device to pass to OpenAL. Empty string to let OpenAL choose on its own.

sound.volume [float] 1.0
  Sound volume factor. 1.0 means full (original) intensity.

ui.aspect [float] 0.0
  Aspect ratio for UI. If 0.0, aspect is the same as resolution and bots axes are scaled by ui.scale
  factor. Otherwise, height is scaled by ui.scale and width is height scaled by ui.aspect.

ui.scale [float] 1.0
  Scale for UI resolution. 1.0 means UI is rendered with the screen resolution.

ui.showBuild [bool] false
  Show a windows with buttons to add various objects into the world. For development purpuses.

ui.showDebug [bool] false
  Show a window that shows coordinates, orientation and other data about camera, controlled bot,
  tagged object etc. For development purposes.

Avtorske pravice za pogon OpenZone
----------------------------------
Copyright (C) 2002-2011  Davorin Učakar. Vse pravice pridržane.
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
