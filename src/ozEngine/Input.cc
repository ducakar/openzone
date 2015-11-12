/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

#ifdef __native_client__
# include <ppapi/cpp/completion_callback.h>
# include <ppapi_simple/ps.h>
# include <ppapi_simple/ps_interface.h>
#endif

namespace oz
{

// Char type is used to map actions, so no more than 256 are possible.
static_assert(Input::MAX_ACTIONS <= 128, "MAX_ACTIONS must be <= 128");

#ifdef __native_client__
static const PPB_MouseLock*        ppbMouseLock       = nullptr;
static Semaphore                   mouseLockSemaphore;
#endif
static byte                        keyMaps[4][SDL_NUM_SCANCODES];
static byte                        buttonMaps[4][8];
static SBitset<Input::MAX_ACTIONS> previousActions;
static SBitset<Input::MAX_ACTIONS> currentActions;
static Input::ModKey               modKey = Input::NONE;

static inline void updateModKey(ushort modMask)
{
  modKey = modMask & KMOD_ALT ? Input::ALT :
           modMask & KMOD_CTRL ? Input::CONTROL :
           modMask & KMOD_SHIFT ? Input::SHIFT : Input::NONE;
}

SBitset<Input::MAX_ACTIONS> Input::downActions;
SBitset<Input::MAX_ACTIONS> Input::pressedActions;
SBitset<Input::MAX_ACTIONS> Input::releasedActions;
bool                        Input::inputGrab;
Input::Mouse                Input::mouse;
Input::Config               Input::config;

void Input::setGrab(bool grab)
{
#ifdef __native_client__

  if (grab && !inputGrab) {
    MainCall() += []
    {
      ppbMouseLock->LockMouse(PSGetInstanceId(), PP_MakeCompletionCallback([](void*, int result)
      {
        inputGrab = result == PP_OK;
        mouseLockSemaphore.post();
      },
      nullptr));
    };
    mouseLockSemaphore.wait();
  }
  else if (!grab && inputGrab) {
    MainCall() << []
    {
      ppbMouseLock->UnlockMouse(PSGetInstanceId());
      inputGrab = false;
    };
  }

#else

  inputGrab = SDL_SetRelativeMouseMode(SDL_bool(grab)) == 0 && grab;

#endif
}

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

      for (int i = 0; i < 4; ++i) {
        int action = buttonMaps[i][code];
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

      for (int i = 0; i < 4; ++i) {
        int action = keyMaps[i][code];
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

  Json root(file);

  if (root.isNull()) {
    return false;
  }

  Json keyboard = root["keyboard"];
  Json mouse    = root["mouse"];

  config.keyboard.sensitivity = keyboard["sensitivity"].get(config.keyboard.sensitivity);
  config.keyboard.leftKey     = keyboard["leftKey"].get(config.keyboard.leftKey);
  config.keyboard.rightKey    = keyboard["rightKey"].get(config.keyboard.rightKey);
  config.keyboard.downKey     = keyboard["downKey"].get(config.keyboard.downKey);
  config.keyboard.upKey       = keyboard["upKey"].get(config.keyboard.upKey);
  config.keyboard.inKey       = keyboard["inKey"].get(config.keyboard.inKey);
  config.keyboard.outKey      = keyboard["outKey"].get(config.keyboard.outKey);

  config.mouse.sensitivity    = mouse["sensitivity"].get(config.mouse.sensitivity);
  config.mouse.smoothing      = mouse["smoothing"].get(config.mouse.smoothing);
  config.mouse.isRaw          = mouse["isRaw"].get(config.mouse.isRaw);

  return true;
}

bool Input::saveConfig(const File& file)
{
  Json keyboard = Json::OBJECT;
  Json mouse    = Json::OBJECT;
  Json root     = { keyboard, mouse };

  keyboard.add("sensitivity", config.keyboard.sensitivity);
  keyboard.add("leftKey", config.keyboard.leftKey);
  keyboard.add("rightKey", config.keyboard.rightKey);
  keyboard.add("downKey", config.keyboard.downKey);
  keyboard.add("upKey", config.keyboard.upKey);
  keyboard.add("inKey", config.keyboard.inKey);
  keyboard.add("outKey", config.keyboard.outKey);

  mouse.add("sensitivity", config.mouse.sensitivity);
  mouse.add("smoothing", config.mouse.smoothing);
  mouse.add("isRaw", config.mouse.isRaw);

  return root.save(file);
}

void Input::init()
{
#ifdef __native_client__
  ppbMouseLock = static_cast<const PPB_MouseLock*>(PSGetInterface(PPB_MOUSELOCK_INTERFACE));
#endif
}

}
