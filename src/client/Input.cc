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

#include <client/Input.hh>

#include <SDL2/SDL.h>

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

  "Skip",
  "Delete",

  "Quick save",
  "Quick load",
  "Auto load",

  "Quit"
};

static SDL_Scancode modifier0;
static SDL_Scancode modifier1;

static ubyte        sdlKeys[SDL_NUM_SCANCODES];
static ubyte        sdlOldKeys[SDL_NUM_SCANCODES];
static ubyte        sdlCurrKeys[SDL_NUM_SCANCODES];

static int          keyMap[Input::KEY_MAX][2];
static bool         configExists = false;

static int mouseEventFilter(void*, SDL_Event* event)
{
  return event->type != SDL_MOUSEMOTION;
}

void Input::loadDefaultKeyMap()
{
  modifier0                         = SDL_SCANCODE_LALT;
  modifier1                         = SDL_SCANCODE_RALT;

  keyMap[KEY_UI_ALT][0]             = MOD_MASK    | SDL_SCANCODE_LALT;
  keyMap[KEY_UI_ALT][1]             = MOD_MASK    | SDL_SCANCODE_RALT;
  keyMap[KEY_UI_TOGGLE][0]          = MOD_MASK    | SDL_SCANCODE_TAB;
  keyMap[KEY_DIR_1][0]              = MOD_MASK    | SDL_SCANCODE_KP_1;

  keyMap[KEY_DIR_1][1]              = MOD_MASK    | SDL_SCANCODE_END;
  keyMap[KEY_DIR_2][0]              = MOD_MASK    | SDL_SCANCODE_KP_2;
  keyMap[KEY_DIR_2][1]              = MOD_MASK    | SDL_SCANCODE_DOWN;
  keyMap[KEY_DIR_3][0]              = MOD_MASK    | SDL_SCANCODE_KP_3;
  keyMap[KEY_DIR_3][1]              = MOD_MASK    | SDL_SCANCODE_PAGEDOWN;
  keyMap[KEY_DIR_4][0]              = MOD_MASK    | SDL_SCANCODE_KP_4;
  keyMap[KEY_DIR_4][1]              = MOD_MASK    | SDL_SCANCODE_LEFT;
  keyMap[KEY_DIR_6][0]              = MOD_MASK    | SDL_SCANCODE_KP_6;
  keyMap[KEY_DIR_6][1]              = MOD_MASK    | SDL_SCANCODE_RIGHT;
  keyMap[KEY_DIR_7][0]              = MOD_MASK    | SDL_SCANCODE_KP_7;
  keyMap[KEY_DIR_7][1]              = MOD_MASK    | SDL_SCANCODE_HOME;
  keyMap[KEY_DIR_8][0]              = MOD_MASK    | SDL_SCANCODE_KP_8;
  keyMap[KEY_DIR_8][1]              = MOD_MASK    | SDL_SCANCODE_UP;
  keyMap[KEY_DIR_9][0]              = MOD_MASK    | SDL_SCANCODE_KP_9;
  keyMap[KEY_DIR_9][1]              = MOD_MASK    | SDL_SCANCODE_PAGEUP;

  keyMap[KEY_NV_TOGGLE][0]          = MOD_OFF_BIT | SDL_SCANCODE_N;
  keyMap[KEY_BINOCULARS_TOGGLE][0]  = MOD_OFF_BIT | SDL_SCANCODE_B;
  keyMap[KEY_MAP_TOGGLE][0]         = MOD_OFF_BIT | SDL_SCANCODE_M;

  keyMap[KEY_CAMERA_TOGGLE][0]      = MOD_MASK    | SDL_SCANCODE_KP_ENTER;
  keyMap[KEY_CAMERA_TOGGLE][1]      = MOD_MASK    | SDL_SCANCODE_RETURN;
  keyMap[KEY_FREELOOK_TOGGLE][0]    = MOD_MASK    | SDL_SCANCODE_KP_MULTIPLY;

  keyMap[KEY_TURN_LEFT][0]          = MOD_MASK    | SDL_SCANCODE_Q;
  keyMap[KEY_TURN_RIGHT][0]         = MOD_MASK    | SDL_SCANCODE_E;
  keyMap[KEY_MOVE_RIGHT][0]         = MOD_MASK    | SDL_SCANCODE_D;
  keyMap[KEY_MOVE_LEFT][0]          = MOD_MASK    | SDL_SCANCODE_A;
  keyMap[KEY_MOVE_FORWARD][0]       = MOD_MASK    | SDL_SCANCODE_W;
  keyMap[KEY_MOVE_BACKWARD][0]      = MOD_MASK    | SDL_SCANCODE_S;
  keyMap[KEY_MOVE_UP][0]            = MOD_MASK    | SDL_SCANCODE_SPACE;
  keyMap[KEY_MOVE_DOWN][0]          = MOD_MASK    | SDL_SCANCODE_C;
  keyMap[KEY_MOVE_DOWN][1]          = MOD_MASK    | SDL_SCANCODE_LCTRL;
  keyMap[KEY_SPEED_TOGGLE][0]       = MOD_MASK    | SDL_SCANCODE_LSHIFT;

  keyMap[KEY_CROUCH_TOGGLE][0]      = MOD_MASK    | SDL_SCANCODE_C;
  keyMap[KEY_CROUCH_TOGGLE][1]      = MOD_MASK    | SDL_SCANCODE_LCTRL;
  keyMap[KEY_JUMP][0]               = MOD_MASK    | SDL_SCANCODE_SPACE;
  keyMap[KEY_EXIT][0]               = MOD_OFF_BIT | SDL_SCANCODE_H;
  keyMap[KEY_EJECT][0]              = MOD_ON_BIT  | SDL_SCANCODE_J;
  keyMap[KEY_SUICIDE][0]            = MOD_ON_BIT  | SDL_SCANCODE_K;

  keyMap[KEY_ACTIVATE][0]           = MOD_ON_BIT  | SDL_SCANCODE_E;
  keyMap[KEY_GRAB][0]               = MOD_ON_BIT  | SDL_SCANCODE_F;
  keyMap[KEY_THROW][0]              = MOD_ON_BIT  | SDL_SCANCODE_R;
  keyMap[KEY_PICK_UP][0]            = MOD_ON_BIT  | SDL_SCANCODE_Q;

  keyMap[KEY_WEAPON_1][0]           = MOD_OFF_BIT | SDL_SCANCODE_1;
  keyMap[KEY_WEAPON_2][0]           = MOD_OFF_BIT | SDL_SCANCODE_2;
  keyMap[KEY_WEAPON_3][0]           = MOD_OFF_BIT | SDL_SCANCODE_3;
  keyMap[KEY_WEAPON_4][0]           = MOD_OFF_BIT | SDL_SCANCODE_4;

  keyMap[KEY_GESTURE_POINT][0]      = MOD_ON_BIT  | SDL_SCANCODE_1;
  keyMap[KEY_GESTURE_BACK][0]       = MOD_ON_BIT  | SDL_SCANCODE_2;
  keyMap[KEY_GESTURE_SALUTE][0]     = MOD_ON_BIT  | SDL_SCANCODE_3;
  keyMap[KEY_GESTURE_WAVE][0]       = MOD_ON_BIT  | SDL_SCANCODE_4;
  keyMap[KEY_GESTURE_FLIP][0]       = MOD_ON_BIT  | SDL_SCANCODE_5;

  keyMap[KEY_SWITCH_TO_UNIT][0]     = MOD_OFF_BIT | SDL_SCANCODE_I;
  keyMap[KEY_CYCLE_UNITS][0]        = MOD_OFF_BIT | SDL_SCANCODE_Y;

  keyMap[KEY_GROUP_SELECT][0]       = MOD_MASK    | SDL_SCANCODE_LSHIFT;
  keyMap[KEY_GROUP_SELECT][1]       = MOD_MASK    | SDL_SCANCODE_RSHIFT;

  keyMap[KEY_CHEAT_SKY_FORWARD][0]  = MOD_OFF_BIT | SDL_SCANCODE_P;
  keyMap[KEY_CHEAT_SKY_BACKWARD][0] = MOD_OFF_BIT | SDL_SCANCODE_O;

  keyMap[KEY_SKIP][0]               = MOD_OFF_BIT | SDL_SCANCODE_SPACE;
  keyMap[KEY_DELETE][0]             = MOD_OFF_BIT | SDL_SCANCODE_DELETE;

  keyMap[KEY_QUICKSAVE][0]          = MOD_OFF_BIT | SDL_SCANCODE_F5;
  keyMap[KEY_QUICKLOAD][0]          = MOD_OFF_BIT | SDL_SCANCODE_F7;
  keyMap[KEY_AUTOLOAD][0]           = MOD_OFF_BIT | SDL_SCANCODE_F8;
  keyMap[KEY_QUIT][0]               = MOD_MASK    | SDL_SCANCODE_F10;
  keyMap[KEY_QUIT][1]               = MOD_MASK    | SDL_SCANCODE_ESCAPE;
}

void Input::loadKeyMap(const Json& keyConfig)
{
  modifier0 = SDL_GetScancodeFromName(keyConfig["modifier0"].get(""));
  modifier1 = SDL_GetScancodeFromName(keyConfig["modifier1"].get(""));

  for (int i = KEY_NONE + 1; i < KEY_MAX; ++i) {
    const Json& keyBindings = keyConfig[KEY_NAMES[i]];

    int nBindings = keyBindings.length();
    if (nBindings > 2) {
      OZ_ERROR("Key '%s' has %d bindings but at most 2 supported", KEY_NAMES[i], nBindings);
    }

    for (int j = 0; j < nBindings; ++j) {
      String keyDesc = keyBindings[j].get("?");

      if (keyDesc.isEmpty()) {
        OZ_ERROR("Empty key description string for '%s'", KEY_NAMES[i]);
      }

      int mod = keyDesc[0] == '_' ? MOD_OFF_BIT : keyDesc[0] == '^' ? MOD_ON_BIT : MOD_MASK;

      SDL_Scancode sdlKey = SDL_GetScancodeFromName(&keyDesc[1]);
      if (sdlKey == SDL_SCANCODE_UNKNOWN) {
        OZ_ERROR("Cannot resolve SDL key name '%s'", &keyDesc[1]);
      }

      keyMap[i][j] = mod | sdlKey;
    }
  }
}

Json Input::keyMapToJson() const
{
  Json keyConfig(Json::OBJECT);

  keyConfig.add("modifier0", SDL_GetScancodeName(modifier0));
  keyConfig.add("modifier1", SDL_GetScancodeName(modifier1));

  for (int i = KEY_NONE + 1; i < KEY_MAX; ++i) {
    Json& key = keyConfig.add(KEY_NAMES[i], Json::ARRAY);

    for (int j = 0; j < 2; ++j) {
      if (keyMap[i][j] != KEY_NONE) {
        int mod  = keyMap[i][j] & MOD_MASK;
        int code = keyMap[i][j] & ~MOD_MASK ;

        String sdlName = SDL_GetScancodeName(SDL_Scancode(code));
        key.add((mod == MOD_OFF_BIT ? "_" : mod == MOD_ON_BIT ? "^" : "&") + sdlName);
      }
    }
  }

  return keyConfig;
}

void Input::readEvent(SDL_Event* event)
{
  switch (event->type) {
    case SDL_MOUSEMOTION: {
      mouseX += float(event->motion.xrel);
      mouseY -= float(event->motion.yrel);
      break;
    }
    case SDL_MOUSEBUTTONUP: {
      currButtons &= char(~SDL_BUTTON(event->button.button));
      break;
    }
    case SDL_MOUSEBUTTONDOWN: {
      buttons     |= char(SDL_BUTTON(event->button.button));
      currButtons |= char(SDL_BUTTON(event->button.button));
      break;
    }
    case SDL_MOUSEWHEEL: {
      mouseW += float(event->wheel.y);
      break;
    }
    case SDL_KEYUP: {
      sdlCurrKeys[event->key.keysym.scancode] = false;
      isKeyReleased = true;
      break;
    }
    case SDL_KEYDOWN: {
      sdlKeys[event->key.keysym.scancode]     = true;
      sdlCurrKeys[event->key.keysym.scancode] = true;
      isKeyPressed = true;
      break;
    }
  }
}

void Input::reset()
{
  Window::warpMouse();

  SDL_PumpEvents();
  SDL_FilterEvents(mouseEventFilter, nullptr);

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

  memset(sdlKeys, 0, sizeof(sdlKeys));
  memset(sdlOldKeys, 0, sizeof(sdlOldKeys));
  memset(sdlCurrKeys, 0, sizeof(sdlCurrKeys));

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

  memcpy(sdlOldKeys, sdlKeys, sizeof(sdlKeys));
  memcpy(sdlKeys, sdlCurrKeys, sizeof(sdlKeys));

  isKeyPressed  = false;
  isKeyReleased = false;
}

void Input::update()
{
  if (!Window::hasFocus()) {
    return;
  }

  mouseX = Math::mix(mouseX, oldMouseX, mouseSmoothing);
  mouseY = Math::mix(mouseY, oldMouseY, mouseSmoothing);

  oldMouseX = mouseX;
  oldMouseY = mouseY;

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

  memcpy(oldKeys, keys, sizeof(keys));
  memset(keys, 0, sizeof(keys));

  int mod = sdlKeys[modifier0] | sdlKeys[modifier1] ? MOD_ON_BIT : MOD_OFF_BIT;

  for (int i = 0; i < Arrays::length(keys); ++i) {
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

  Log::print("Initialising Input from '%s' ...", configFile.c());

  Json inputConfig;
  configExists = inputConfig.load(configFile);

  if (!String::equals(inputConfig["_version"].get(""), OZ_VERSION)) {
    configExists = false;
    inputConfig = Json::NIL;
  }

  const Json& mouseConfig    = inputConfig["mouse"];
  const Json& keyboardConfig = inputConfig["keyboard"];
  const Json& keyMapConfig   = inputConfig["bindings"];

  memset(sdlKeys, 0, sizeof(sdlKeys));
  memset(sdlOldKeys, 0, sizeof(sdlOldKeys));
  memset(sdlCurrKeys, 0, sizeof(sdlCurrKeys));

  memset(keyMap, 0, sizeof(keyMap));

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
  mouseSmoothing = mouseConfig["smoothing"].get(0.3f);
  mouseRawInput  = mouseConfig["rawInput"].get(false);

  keySensX       = keyboardConfig["sensitivity.x"].get(0.04f);
  keySensY       = keyboardConfig["sensitivity.y"].get(0.04f);

  memset(keys, 0, sizeof(keys));
  memset(oldKeys, 0, sizeof(oldKeys));

  SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, mouseRawInput ? "0" : "1");

  inputConfig.clear(true);

  Log::printEnd(" OK");
}

void Input::destroy()
{
  File configFile = config["dir.config"].get(File::CONFIG) + "/input.json";

  Log::print("Writing Input configuration to '%s' ...", configFile.c());

  Json inputConfig(Json::OBJECT);

  inputConfig.add("_version", OZ_VERSION);

  Json& mouseConfig = inputConfig.add("mouse", Json::OBJECT);
  OZ_CONF_ADD(mouseConfig, "sensitivity.x", mouseSensX, "Mouse X sensitivity. [0.003]");
  OZ_CONF_ADD(mouseConfig, "sensitivity.y", mouseSensY, "Mouse Y sensitivity. [0.003]");
  OZ_CONF_ADD(mouseConfig, "sensitivity.w", mouseSensW, "Mouse wheel sensitivity. [3.0]");
  OZ_CONF_ADD(mouseConfig, "smoothing", mouseSmoothing, "Interpolation factor between the current "
              "input (0.0) and the previous value (1.0). [0.3]");
  OZ_CONF_ADD(mouseConfig, "rawInput", mouseRawInput, "Use raw input, not accelerated or adjusted "
              "by the OS. [false]");

  Json& keyboardConfig = inputConfig.add("keyboard", Json::OBJECT);
  OZ_CONF_ADD(keyboardConfig, "sensitivity.x", keySensX, "Left/right key sensitivity when rotating "
              "camera.");
  OZ_CONF_ADD(keyboardConfig, "sensitivity.y", keySensY, "Up/down key sensitivity when rotating "
              "camera.");

  Json& keyMapConfig = inputConfig.add("bindings", Json::OBJECT);
  keyMapConfig = keyMapToJson();

  if (!inputConfig.save(configFile, CONFIG_FORMAT)) {
    OZ_ERROR("Failed to write '%s'", configFile.c());
  }

  Log::printEnd(" OK");
}

Input input;

}
}
