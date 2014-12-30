/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/Input.cc
 */

#include <client/Input.hh>

#include <SDL.h>

// Choose keycode for appropriate SDL version and prepend typical prefix.
#if SDL_MAJOR_VERSION < 2
# define OZ_SDL_KEY(sdl1, sdl2) SDLK_##sdl1
#else
# define OZ_SDL_KEY(sdl1, sdl2) SDL_SCANCODE_##sdl2
#endif

namespace oz
{
namespace client
{

static const char* const KEY_NAMES[] = {
  "None",

  "Alternate UI action",
  "Toggle UI",

  "Look left down",
  "Look down",
  "Look right down",
  "Look left",
  "Look right",
  "Look left up",
  "Look up",
  "Look right up",

  "Toggle night vision",
  "Toggle binoculars",
  "Toggle map size",

  "Toggle camera mode",
  "Toggle freelook",

  "Turn left",
  "Turn right",
  "Move right",
  "Move left",
  "Move forward",
  "Move backward",
  "Move up",
  "Move down",
  "Toggle speed",

  "Toggle crouch",
  "Jump",
  "Exit vehicle",
  "Eject",
  "Commit suicide",
  "Activate",
  "Grab",
  "Throw",
  "Pick up",
  "Fire weapon",
  "Next vehicle weapon",

  "Weapon 1",
  "Weapon 2",
  "Weapon 3",
  "Weapon 4",

  "'Point' gesture",
  "'Fall back' gesture",
  "'Salute' gesture",
  "'Wave' gesture",
  "'Flip out' gesture",

  "Switch to selected unit",
  "Cycle switchable units",

  "Group selection",

  "Fast forward sky (cheat)",
  "Fast backward sky (cheat)",

  "Delete",

  "Quick save",
  "Quick load",
  "Auto load",

  "Quit"
};

#if SDL_MAJOR_VERSION < 2

static const char* const BACKEND = "SDL1";

static SDLKey            modifier0;
static SDLKey            modifier1;

static ubyte             sdlKeys[SDLK_LAST];
static ubyte             sdlOldKeys[SDLK_LAST];
static ubyte             sdlCurrKeys[SDLK_LAST];

#else

static const char* const BACKEND = "SDL2";

static SDL_Scancode      modifier0;
static SDL_Scancode      modifier1;

static ubyte             sdlKeys[SDL_NUM_SCANCODES];
static ubyte             sdlOldKeys[SDL_NUM_SCANCODES];
static ubyte             sdlCurrKeys[SDL_NUM_SCANCODES];

#endif

static int               keyMap[Input::KEY_MAX][2];
static bool              configExists = false;

void Input::loadDefaultKeyMap()
{
  modifier0                         = OZ_SDL_KEY(LALT, LALT);
  modifier1                         = OZ_SDL_KEY(RALT, RALT);

  keyMap[KEY_UI_ALT][0]             = MOD_MASK    | OZ_SDL_KEY(LALT,        LALT       );
  keyMap[KEY_UI_ALT][1]             = MOD_MASK    | OZ_SDL_KEY(RALT,        RALT       );
  keyMap[KEY_UI_TOGGLE][0]          = MOD_MASK    | OZ_SDL_KEY(TAB,         TAB        );

  keyMap[KEY_DIR_1][0]              = MOD_MASK    | OZ_SDL_KEY(KP1,         KP_1       );
  keyMap[KEY_DIR_1][1]              = MOD_MASK    | OZ_SDL_KEY(END,         END        );
  keyMap[KEY_DIR_2][0]              = MOD_MASK    | OZ_SDL_KEY(KP2,         KP_2       );
  keyMap[KEY_DIR_2][1]              = MOD_MASK    | OZ_SDL_KEY(DOWN,        DOWN       );
  keyMap[KEY_DIR_3][0]              = MOD_MASK    | OZ_SDL_KEY(KP3,         KP_3       );
  keyMap[KEY_DIR_3][1]              = MOD_MASK    | OZ_SDL_KEY(PAGEDOWN,    PAGEDOWN   );
  keyMap[KEY_DIR_4][0]              = MOD_MASK    | OZ_SDL_KEY(KP4,         KP_4       );
  keyMap[KEY_DIR_4][1]              = MOD_MASK    | OZ_SDL_KEY(LEFT,        LEFT       );
  keyMap[KEY_DIR_6][0]              = MOD_MASK    | OZ_SDL_KEY(KP6,         KP_6       );
  keyMap[KEY_DIR_6][1]              = MOD_MASK    | OZ_SDL_KEY(RIGHT,       RIGHT      );
  keyMap[KEY_DIR_7][0]              = MOD_MASK    | OZ_SDL_KEY(KP7,         KP_7       );
  keyMap[KEY_DIR_7][1]              = MOD_MASK    | OZ_SDL_KEY(HOME,        HOME       );
  keyMap[KEY_DIR_8][0]              = MOD_MASK    | OZ_SDL_KEY(KP8,         KP_8       );
  keyMap[KEY_DIR_8][1]              = MOD_MASK    | OZ_SDL_KEY(UP,          UP         );
  keyMap[KEY_DIR_9][0]              = MOD_MASK    | OZ_SDL_KEY(KP9,         KP_9       );
  keyMap[KEY_DIR_9][1]              = MOD_MASK    | OZ_SDL_KEY(PAGEUP,      PAGEUP     );

  keyMap[KEY_NV_TOGGLE][0]          = MOD_OFF_BIT | OZ_SDL_KEY(n,           N          );
  keyMap[KEY_BINOCULARS_TOGGLE][0]  = MOD_OFF_BIT | OZ_SDL_KEY(b,           B          );
  keyMap[KEY_MAP_TOGGLE][0]         = MOD_OFF_BIT | OZ_SDL_KEY(m,           M          );

  keyMap[KEY_CAMERA_TOGGLE][0]      = MOD_MASK    | OZ_SDL_KEY(KP_ENTER,    KP_ENTER   );
  keyMap[KEY_CAMERA_TOGGLE][1]      = MOD_MASK    | OZ_SDL_KEY(RETURN,      RETURN     );
  keyMap[KEY_FREELOOK_TOGGLE][0]    = MOD_MASK    | OZ_SDL_KEY(KP_MULTIPLY, KP_MULTIPLY);

  keyMap[KEY_TURN_LEFT][0]          = MOD_MASK    | OZ_SDL_KEY(q,           Q          );
  keyMap[KEY_TURN_RIGHT][0]         = MOD_MASK    | OZ_SDL_KEY(e,           E          );
  keyMap[KEY_MOVE_RIGHT][0]         = MOD_MASK    | OZ_SDL_KEY(d,           D          );
  keyMap[KEY_MOVE_LEFT][0]          = MOD_MASK    | OZ_SDL_KEY(a,           A          );
  keyMap[KEY_MOVE_FORWARD][0]       = MOD_MASK    | OZ_SDL_KEY(w,           W          );
  keyMap[KEY_MOVE_BACKWARD][0]      = MOD_MASK    | OZ_SDL_KEY(s,           S          );
  keyMap[KEY_MOVE_UP][0]            = MOD_MASK    | OZ_SDL_KEY(SPACE,       SPACE      );
  keyMap[KEY_MOVE_DOWN][0]          = MOD_MASK    | OZ_SDL_KEY(c,           C          );
  keyMap[KEY_MOVE_DOWN][1]          = MOD_MASK    | OZ_SDL_KEY(LCTRL,       LCTRL      );
  keyMap[KEY_SPEED_TOGGLE][0]       = MOD_MASK    | OZ_SDL_KEY(LSHIFT,      LSHIFT     );

  keyMap[KEY_CROUCH_TOGGLE][0]      = MOD_MASK    | OZ_SDL_KEY(c,           C          );
  keyMap[KEY_CROUCH_TOGGLE][1]      = MOD_MASK    | OZ_SDL_KEY(LCTRL,       LCTRL      );
  keyMap[KEY_JUMP][0]               = MOD_MASK    | OZ_SDL_KEY(SPACE,       SPACE      );
  keyMap[KEY_EXIT][0]               = MOD_OFF_BIT | OZ_SDL_KEY(x,           X          );
  keyMap[KEY_EJECT][0]              = MOD_ON_BIT  | OZ_SDL_KEY(x,           X          );
  keyMap[KEY_SUICIDE][0]            = MOD_ON_BIT  | OZ_SDL_KEY(k,           K          );

  keyMap[KEY_ACTIVATE][0]           = MOD_ON_BIT  | OZ_SDL_KEY(e,           E          );
  keyMap[KEY_GRAB][0]               = MOD_ON_BIT  | OZ_SDL_KEY(f,           F          );
  keyMap[KEY_THROW][0]              = MOD_ON_BIT  | OZ_SDL_KEY(r,           R          );
  keyMap[KEY_PICK_UP][0]            = MOD_ON_BIT  | OZ_SDL_KEY(q,           Q          );

  keyMap[KEY_WEAPON_1][0]           = MOD_OFF_BIT | OZ_SDL_KEY(1,           1          );
  keyMap[KEY_WEAPON_2][0]           = MOD_OFF_BIT | OZ_SDL_KEY(2,           2          );
  keyMap[KEY_WEAPON_3][0]           = MOD_OFF_BIT | OZ_SDL_KEY(3,           3          );
  keyMap[KEY_WEAPON_4][0]           = MOD_OFF_BIT | OZ_SDL_KEY(4,           4          );

  keyMap[KEY_GESTURE_POINT][0]      = MOD_OFF_BIT | OZ_SDL_KEY(g,           G          );
  keyMap[KEY_GESTURE_BACK][0]       = MOD_OFF_BIT | OZ_SDL_KEY(h,           H          );
  keyMap[KEY_GESTURE_SALUTE][0]     = MOD_OFF_BIT | OZ_SDL_KEY(j,           J          );
  keyMap[KEY_GESTURE_WAVE][0]       = MOD_OFF_BIT | OZ_SDL_KEY(k,           K          );
  keyMap[KEY_GESTURE_FLIP][0]       = MOD_OFF_BIT | OZ_SDL_KEY(l,           L          );

  keyMap[KEY_SWITCH_TO_UNIT][0]     = MOD_OFF_BIT | OZ_SDL_KEY(i,           I          );
  keyMap[KEY_CYCLE_UNITS][0]        = MOD_OFF_BIT | OZ_SDL_KEY(y,           Y          );

  keyMap[KEY_GROUP_SELECT][0]       = MOD_MASK    | OZ_SDL_KEY(LSHIFT,      LSHIFT     );
  keyMap[KEY_GROUP_SELECT][1]       = MOD_MASK    | OZ_SDL_KEY(RSHIFT,      RSHIFT     );

  keyMap[KEY_CHEAT_SKY_FORWARD][0]  = MOD_OFF_BIT | OZ_SDL_KEY(p,           P          );
  keyMap[KEY_CHEAT_SKY_BACKWARD][0] = MOD_OFF_BIT | OZ_SDL_KEY(o,           O          );

  keyMap[KEY_DELETE][0]             = MOD_OFF_BIT | OZ_SDL_KEY(DELETE,      DELETE     );

  keyMap[KEY_QUICKSAVE][0]          = MOD_OFF_BIT | OZ_SDL_KEY(F5,          F5         );
  keyMap[KEY_QUICKLOAD][0]          = MOD_OFF_BIT | OZ_SDL_KEY(F7,          F7         );
  keyMap[KEY_AUTOLOAD][0]           = MOD_OFF_BIT | OZ_SDL_KEY(F8,          F8         );
  keyMap[KEY_QUIT][0]               = MOD_MASK    | OZ_SDL_KEY(F10,         F10        );
  keyMap[KEY_QUIT][1]               = MOD_MASK    | OZ_SDL_KEY(ESCAPE,      ESCAPE     );
}

void Input::loadKeyMap(const Json& keyConfig)
{
#if SDL_MAJOR_VERSION < 2

  HashMap<String, SDLKey> sdlKeyNames;

  for (int i = 0; i < SDLK_LAST; ++i) {
    const char* name = SDL_GetKeyName(SDLKey(i));

    sdlKeyNames.add(name, SDLKey(i));
  }

  const SDLKey* pModifier0 = sdlKeyNames.find(keyConfig["modifier0"].get(""));
  const SDLKey* pModifier1 = sdlKeyNames.find(keyConfig["modifier1"].get(""));

  modifier0 = pModifier0 == nullptr ? SDLK_UNKNOWN : *pModifier0;
  modifier1 = pModifier1 == nullptr ? SDLK_UNKNOWN : *pModifier1;

#else

  modifier0 = SDL_GetScancodeFromName(keyConfig["modifier0"].get(""));
  modifier1 = SDL_GetScancodeFromName(keyConfig["modifier1"].get(""));

#endif

  for (int i = KEY_NONE + 1; i < KEY_MAX; ++i) {
    const Json& keyBindings = keyConfig[KEY_NAMES[i]];

    int nBindings = keyBindings.length();
    if (nBindings > 2) {
      OZ_ERROR("Key '%s' has %d bindings but at most 2 supported", KEY_NAMES[i], nBindings);
    }

    for (int j = 0; j < nBindings; ++j) {
      const String& keyDesc = keyBindings[j].get("?");

      if (keyDesc.isEmpty()) {
        OZ_ERROR("Empty key description string for '%s'", KEY_NAMES[i]);
      }

      int mod = keyDesc[0] == '_' ? int(MOD_OFF_BIT) :
                keyDesc[0] == '^' ? int(MOD_ON_BIT) :
                                    int(MOD_MASK);

#if SDL_MAJOR_VERSION < 2

      const SDLKey* sdlKey = sdlKeyNames.find(&keyDesc[1]);
      if (sdlKey == nullptr) {
        OZ_ERROR("Cannot resolve SDL key name '%s'", &keyDesc[1]);
      }

      keyMap[i][j] = mod | *sdlKey;

#else

      SDL_Scancode sdlKey = SDL_GetScancodeFromName(&keyDesc[1]);
      if (sdlKey == SDL_SCANCODE_UNKNOWN) {
        OZ_ERROR("Cannot resolve SDL key name '%s'", &keyDesc[1]);
      }

      keyMap[i][j] = mod | sdlKey;

#endif
    }
  }
}

Json Input::keyMapToJson() const
{
  Json keyConfig(Json::OBJECT);

#if SDL_MAJOR_VERSION < 2
  keyConfig.add("modifier0", SDL_GetKeyName(modifier0));
  keyConfig.add("modifier1", SDL_GetKeyName(modifier1));
#else
  keyConfig.add("modifier0", SDL_GetScancodeName(modifier0));
  keyConfig.add("modifier1", SDL_GetScancodeName(modifier1));
#endif

  for (int i = KEY_NONE + 1; i < KEY_MAX; ++i) {
    Json& key = keyConfig.add(KEY_NAMES[i], Json::ARRAY);

    for (int j = 0; j < 2; ++j) {
      if (keyMap[i][j] != KEY_NONE) {
        int mod  = keyMap[i][j] & MOD_MASK;
        int code = keyMap[i][j] & ~MOD_MASK ;

#if SDL_MAJOR_VERSION < 2
        String sdlName = SDL_GetKeyName(SDLKey(code));
#else
        String sdlName = SDL_GetScancodeName(SDL_Scancode(code));
#endif

        key.add((mod == MOD_OFF_BIT ? "_" : mod == MOD_ON_BIT ? "^" : "&") + sdlName);
      }
    }
  }

  return keyConfig;
}

void Input::readEvent(SDL_Event* event)
{
  switch (event->type) {
    case SDL_MOUSEBUTTONUP: {
      currButtons &= char(~SDL_BUTTON(event->button.button));
      break;
    }
    case SDL_MOUSEBUTTONDOWN: {
      buttons     |= char(SDL_BUTTON(event->button.button));
      currButtons |= char(SDL_BUTTON(event->button.button));
      break;
    }
#if SDL_MAJOR_VERSION >= 2
    case SDL_MOUSEWHEEL: {
      mouseW += float(event->wheel.y);
      break;
    }
#endif
    case SDL_KEYUP: {
#if SDL_MAJOR_VERSION < 2
      sdlCurrKeys[event->key.keysym.sym] = false;
#else
      sdlCurrKeys[event->key.keysym.scancode] = false;
#endif
      isKeyReleased = true;
      break;
    }
    case SDL_KEYDOWN: {
#if SDL_MAJOR_VERSION < 2
      sdlKeys[event->key.keysym.sym]     = true;
      sdlCurrKeys[event->key.keysym.sym] = true;
#else
      sdlKeys[event->key.keysym.scancode]     = true;
      sdlCurrKeys[event->key.keysym.scancode] = true;
#endif
      isKeyPressed = true;
      break;
    }
  }
}

void Input::reset()
{
  Window::warpMouse();

  oldMouseX      = 0.0f;
  oldMouseY      = 0.0f;
  oldMouseW      = 0.0f;

  mouseX         = 0.0f;
  mouseY         = 0.0f;
  mouseW         = 0.0f;

  buttons        = 0;
  oldButtons     = 0;
  currButtons    = 0;

  leftPressed    = false;
  leftReleased   = false;
  middlePressed  = false;
  middleReleased = false;
  rightPressed   = false;
  rightReleased  = false;
  wheelUp        = false;
  wheelDown      = false;

  lookX          = 0.0f;
  lookY          = 0.0f;
  moveX          = 0.0f;
  moveY          = 0.0f;

  mSet(sdlKeys, 0, sizeof(sdlKeys));
  mSet(sdlOldKeys, 0, sizeof(sdlOldKeys));
  mSet(sdlCurrKeys, 0, sizeof(sdlCurrKeys));

  isKeyPressed   = false;
  isKeyReleased  = false;
}

void Input::prepare()
{
  mouseX         = 0.0f;
  mouseY         = 0.0f;
  mouseW         = 0.0f;

  oldButtons     = buttons;
  buttons        = currButtons;

  leftPressed    = false;
  leftReleased   = false;
  middlePressed  = false;
  middleReleased = false;
  rightPressed   = false;
  rightReleased  = false;
  wheelUp        = false;
  wheelDown      = false;

  lookX          = 0.0f;
  lookY          = 0.0f;
  moveX          = 0.0f;
  moveY          = 0.0f;

  mCopy(sdlOldKeys, sdlKeys, sizeof(sdlKeys));
  mCopy(sdlKeys, sdlCurrKeys, sizeof(sdlKeys));

  isKeyPressed  = false;
  isKeyReleased = false;
}

void Input::update()
{
  if (!Window::hasFocus()) {
    return;
  }

#if defined(__native_client__)

  float newMouseX = +Pepper::moveX;
  float newMouseY = -Pepper::moveY;
  float newMouseW = +Pepper::moveW;

  Pepper::moveX = 0;
  Pepper::moveY = 0;
  Pepper::moveW = 0;

#else

  int dx, dy;
  SDL_GetRelativeMouseState(&dx, &dy);

  float newMouseX = +float(dx);
  float newMouseY = -float(dy);
  float newMouseW = mouseW;

#endif

  mouseX = Math::mix(newMouseX, oldMouseX, mouseSmoothing);
  mouseY = Math::mix(newMouseY, oldMouseY, mouseSmoothing);
  mouseW = Math::mix(newMouseW, oldMouseW, mouseSmoothing);

  oldMouseX = newMouseX;
  oldMouseY = newMouseY;
  oldMouseW = newMouseW;

  int pressedButtons  = input.buttons & ~input.oldButtons;
  int releasedButtons = ~input.buttons & input.oldButtons;

  leftPressed    = pressedButtons  & Input::LEFT_BUTTON;
  leftReleased   = releasedButtons & Input::LEFT_BUTTON;
  middlePressed  = pressedButtons  & Input::MIDDLE_BUTTON;
  middleReleased = releasedButtons & Input::MIDDLE_BUTTON;
  rightPressed   = pressedButtons  & Input::RIGHT_BUTTON;
  rightReleased  = releasedButtons & Input::RIGHT_BUTTON;
  wheelUp        = mouseW > 0.0f;
  wheelDown      = mouseW < 0.0f;

  mCopy(oldKeys, keys, sizeof(keys));
  mSet(keys, 0, sizeof(keys));

  int mod = sdlKeys[modifier0] | sdlKeys[modifier1] ? int(MOD_ON_BIT) : int(MOD_OFF_BIT);

  for (int i = 0; i < aLength(keys); ++i) {
    if (keyMap[i][0] & mod) {
      keys[i] |= sdlKeys[keyMap[i][0] & ~MOD_MASK];
    }
    if (keyMap[i][1] & mod) {
      keys[i] |= sdlKeys[keyMap[i][1] & ~MOD_MASK];
    }
  }

  lookX = 0.0f;
  lookY = 0.0f;
  moveX = 0.0f;
  moveY = 0.0f;

  if (keys[Input::KEY_DIR_1] | keys[Input::KEY_DIR_4] | keys[Input::KEY_DIR_7]) {
    lookX += keySensX;
  }
  if (keys[Input::KEY_DIR_3] | keys[Input::KEY_DIR_6] | keys[Input::KEY_DIR_9]) {
    lookX -= keySensX;
  }
  if (keys[Input::KEY_DIR_1] | keys[Input::KEY_DIR_2] | keys[Input::KEY_DIR_3]) {
    lookY -= keySensY;
  }
  if (keys[Input::KEY_DIR_7] | keys[Input::KEY_DIR_8] | keys[Input::KEY_DIR_9]) {
    lookY += keySensY;
  }

  if (input.keys[Input::KEY_MOVE_FORWARD]) {
    moveY += 1.0f;
  }
  if (input.keys[Input::KEY_MOVE_BACKWARD]) {
    moveY -= 1.0f;
  }
  if (input.keys[Input::KEY_MOVE_RIGHT]) {
    moveX += 1.0f;
  }
  if (input.keys[Input::KEY_MOVE_LEFT]) {
    moveX -= 1.0f;
  }
}

void Input::init()
{
  File configFile = config["dir.config"].get(File::CONFIG) + "/input.json";

  Log::print("Initialising Input from '%s' ...", configFile.path().cstr());

  Json inputConfig;
  configExists = inputConfig.load(configFile);

  if (!String::equals(inputConfig["_version"].get(""), OZ_VERSION)) {
    configExists = false;
    inputConfig = Json::NIL;
  }

  if (!String::equals(inputConfig["_backend"].get(""), BACKEND)) {
    configExists = false;
    inputConfig = Json::NIL;
  }

  const Json& mouseConfig    = inputConfig["mouse"];
  const Json& keyboardConfig = inputConfig["keyboard"];
  const Json& keyMapConfig   = inputConfig["bindings"];

  mSet(sdlKeys, 0, sizeof(sdlKeys));
  mSet(sdlOldKeys, 0, sizeof(sdlOldKeys));
  mSet(sdlCurrKeys, 0, sizeof(sdlCurrKeys));

  mSet(keyMap, 0, sizeof(keyMap));

  if (keyMapConfig.isNull()) {
    loadDefaultKeyMap();
  }
  else {
    loadKeyMap(keyMapConfig);
  }

  oldMouseX      = 0.0f;
  oldMouseY      = 0.0f;
  oldMouseW      = 0.0f;

  mouseX         = 0.0f;
  mouseY         = 0.0f;
  mouseW         = 0.0f;

  buttons        = 0;
  oldButtons     = 0;
  currButtons    = 0;

  leftPressed    = false;
  leftReleased   = false;
  middlePressed  = false;
  middleReleased = false;
  rightPressed   = false;
  rightReleased  = false;
  wheelUp        = false;
  wheelDown      = false;

  lookX          = 0.0f;
  lookY          = 0.0f;
  moveX          = 0.0f;
  moveY          = 0.0f;

  isKeyPressed   = false;
  isKeyReleased  = false;

  mouseSensX     = mouseConfig["sensitivity.x"].get(0.003f);
  mouseSensY     = mouseConfig["sensitivity.y"].get(0.003f);
  mouseSensW     = mouseConfig["sensitivity.w"].get(3.0f);
  mouseSmoothing = mouseConfig["smoothing"].get(true) ? 0.5f : 0.0f;

  keySensX       = keyboardConfig["sensitivity.x"].get(0.04f);
  keySensY       = keyboardConfig["sensitivity.y"].get(0.04f);

  mSet(keys, 0, sizeof(keys));
  mSet(oldKeys, 0, sizeof(oldKeys));

#if SDL_MAJOR_VERSION < 2
  SDL_ShowCursor(false);
#else
  SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1");
  SDL_SetRelativeMouseMode(SDL_TRUE);
#endif

  inputConfig.clear(true);

  Log::printEnd(" OK");
}

void Input::destroy()
{
  if (configExists) {
    return;
  }

  File configFile = config["dir.config"].get(File::CONFIG) + "/input.json";

  Log::print("Writing Input configuration to '%s' ...", configFile.path().cstr());

  Json inputConfig(Json::OBJECT);

  inputConfig.add("_version", OZ_VERSION);
  inputConfig.add("_backend", BACKEND);

  Json& mouseConfig = inputConfig.add("mouse", Json::OBJECT);
  mouseConfig.add("sensitivity.x", mouseSensX);
  mouseConfig.add("sensitivity.y", mouseSensY);
  mouseConfig.add("sensitivity.w", mouseSensW);
  mouseConfig.add("smoothing", mouseSmoothing != 0.0f);

  Json& keyboardConfig = inputConfig.add("keyboard", Json::OBJECT);
  keyboardConfig.add("sensitivity.x", keySensX);
  keyboardConfig.add("sensitivity.y", keySensY);

  Json& keyMapConfig = inputConfig.add("bindings", Json::OBJECT);
  keyMapConfig = keyMapToJson();

  if (!inputConfig.save(configFile, CONFIG_FORMAT)) {
    OZ_ERROR("Failed to write '%s'", configFile.path().cstr());
  }

  Log::printEnd(" OK");
}

Input input;

}
}
