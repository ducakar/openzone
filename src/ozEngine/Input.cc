/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "Input.hh"

#include <SDL2/SDL.h>

namespace oz
{

namespace
{

// Char type is used to map actions, so no more than 256 are possible.
static_assert(Input::MAX_ACTIONS <= 256, "MAX_ACTIONS must be <= 256");

ubyte                       keyMaps[4][SDL_NUM_SCANCODES];
ubyte                       buttonMaps[4][8];
SBitset<Input::MAX_ACTIONS> previousActions;
SBitset<Input::MAX_ACTIONS> currentActions;
Input::ModKey               modKey = Input::NONE;

inline void updateModKey(uint16 modMask)
{
  modKey = modMask & KMOD_ALT     ? Input::ALT
           : modMask & KMOD_CTRL  ? Input::CONTROL
           : modMask & KMOD_SHIFT ? Input::SHIFT
                                  : Input::NONE;
}

}

SBitset<Input::MAX_ACTIONS> Input::downActions_;
SBitset<Input::MAX_ACTIONS> Input::pressedActions_;
SBitset<Input::MAX_ACTIONS> Input::releasedActions_;
Input::Mouse                Input::mouse;
Input::Config               Input::config = {
  {
    Vec3(0.04f, 0.04f, 0.04f),
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UNKNOWN
  },
  {
    Vec4(0.003f, 0.003f, 0.003f, 0.003f),
    0.3f,
    false
  }
};

void Input::processEvent(const SDL_Event* event)
{
  switch (event->type) {
    case SDL_MOUSEMOTION: {
      mouse.x   = float(event->motion.x);
      mouse.y   = float(event->motion.y);
      mouse.dx += float(event->motion.xrel) * config.mouse.sensitivity.x;
      mouse.dy += float(event->motion.yrel) * config.mouse.sensitivity.y;
      break;
    }
    case SDL_MOUSEWHEEL: {
      mouse.dz += float(event->wheel.x) * config.mouse.sensitivity.z;
      mouse.dw += float(event->wheel.y) * config.mouse.sensitivity.w;
      break;
    }
    case SDL_MOUSEBUTTONDOWN: {
      int code   = event->button.button & 0x07;
      int action = buttonMaps[modKey][code];

      if (action >= 0) {
        currentActions.set(action);
      }
      break;
    }
    case SDL_MOUSEBUTTONUP: {
      int code = event->button.button & 0x07;

      for (const auto& buttonMap : buttonMaps) {
        int action = buttonMap[code];
        if (action >= 0) {
          currentActions.clear(action);
        }
      }
      break;
    }
    case SDL_KEYDOWN: {
      updateModKey(event->key.keysym.mod);

      SDL_Scancode code   = event->key.keysym.scancode;
      int          action = keyMaps[modKey][code];

      if (action >= 0) {
        currentActions.set(action);
      }
      break;
    }
    case SDL_KEYUP: {
      updateModKey(event->key.keysym.mod);

      SDL_Scancode code = event->key.keysym.scancode;

      for (const auto& keyMap : keyMaps) {
        int action = keyMap[code];
        if (action >= 0) {
          currentActions.clear(action);
        }
      }
      break;
    }
  }
}

bool Input::loadConfig(const File& file)
{
  config = Config();

  Json rootJson(file);

  if (rootJson.isNull()) {
    return false;
  }

  const Json& keyboardJson = rootJson["keyboard"];
  const Json& mouseJson    = rootJson["mouse"];

  config.keyboard.sensitivity = keyboardJson["sensitivity"].get(config.keyboard.sensitivity);
  config.keyboard.leftKey     = keyboardJson["leftKey"].get(config.keyboard.leftKey);
  config.keyboard.rightKey    = keyboardJson["rightKey"].get(config.keyboard.rightKey);
  config.keyboard.downKey     = keyboardJson["downKey"].get(config.keyboard.downKey);
  config.keyboard.upKey       = keyboardJson["upKey"].get(config.keyboard.upKey);
  config.keyboard.inKey       = keyboardJson["inKey"].get(config.keyboard.inKey);
  config.keyboard.outKey      = keyboardJson["outKey"].get(config.keyboard.outKey);

  config.mouse.sensitivity    = mouseJson["sensitivity"].get(config.mouse.sensitivity);
  config.mouse.smoothing      = mouseJson["smoothing"].get(config.mouse.smoothing);
  config.mouse.isRaw          = mouseJson["isRaw"].get(config.mouse.isRaw);

  return true;
}

bool Input::saveConfig(const File& file)
{
  Json keyboardJson = Json::OBJECT;
  Json mouseJson    = Json::OBJECT;
  Json rootJson     = {keyboardJson, mouseJson};

  keyboardJson.add("sensitivity", config.keyboard.sensitivity);
  keyboardJson.add("leftKey", config.keyboard.leftKey);
  keyboardJson.add("rightKey", config.keyboard.rightKey);
  keyboardJson.add("downKey", config.keyboard.downKey);
  keyboardJson.add("upKey", config.keyboard.upKey);
  keyboardJson.add("inKey", config.keyboard.inKey);
  keyboardJson.add("outKey", config.keyboard.outKey);

  mouseJson.add("sensitivity", config.mouse.sensitivity);
  mouseJson.add("smoothing", config.mouse.smoothing);
  mouseJson.add("isRaw", config.mouse.isRaw);

  return rootJson.save(file);
}

}
