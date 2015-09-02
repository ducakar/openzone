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

/**
 * @file ozEngine/Input.hh
 */

#pragma once

#include "common.hh"

#include <SDL.h>

namespace oz
{

/**
 * Input reading and mapping class.
 *
 * To avoid missing key/button presses that where bot pressed and released during the just passed
 * tick, `down()`, `leftDown` etc. don't represent snapshot of the current state but whether keys/
 * buttons were down at some moment in the passed tick.
 */
class Input
{
public:

  /// Maximum number of actions.
  static const int MAX_ACTIONS = 128;

public:

  enum ModKey
  {
    NONE    = 0, ///< None.
    SHIFT   = 1, ///< (Left or right) Shift key.
    CONTROL = 2, ///< (Left or right) Control key.
    ALT     = 4  ///< (Left or right) Alt or Alt Gr key.
  };

  struct Binding
  {
    SDL_Scancode key;    ///< Primary key binding.
    SDL_Scancode altKey; ///< Alternate key binding.
    int          mod;    ///< ModKey bitmask for primary binding.
    int          altMod; ///< ModKey bitmask for alternate binding.
    int          button; ///< Mouse button (1 - left, 2 - right, 3 - middle/wheel).
    int          action; ///< User's action id.
    String       name;   ///< Action name.
  };

  struct Config
  {
    struct Keyboard
    {
      Vec3         sensitivity = Vec3(0.04f, 0.04f, 0.04f);
      SDL_Scancode leftKey     = SDL_SCANCODE_LEFT;
      SDL_Scancode rightKey    = SDL_SCANCODE_RIGHT;
      SDL_Scancode downKey     = SDL_SCANCODE_DOWN;
      SDL_Scancode upKey       = SDL_SCANCODE_UP;
      SDL_Scancode inKey       = SDL_SCANCODE_UNKNOWN;
      SDL_Scancode outKey      = SDL_SCANCODE_UNKNOWN;
    };

    struct Mouse
    {
      Vec4  sensitivity = Vec4(0.003f, 0.003f, 0.003f, 0.003f);
      float smoothing   = 0.3f;
      bool  isRaw       = false;
    };

    Keyboard keyboard;
    Mouse    mouse;
  };

  struct Mouse
  {
    float x;
    float y;

    float dx;
    float dy;
    float dz;
    float dw;

    bool  leftDown;
    bool  leftPressed;
    bool  leftReleased;

    bool  rightDown;
    bool  rightPressed;
    bool  rightReleased;

    bool  middleDown;
    bool  middlePressed;
    bool  middleReleased;
  };

private:

  static SBitset<MAX_ACTIONS> downActions;     ///< Action for which keys were down.
  static SBitset<MAX_ACTIONS> pressedActions;  ///< Action for which keys were pressed.
  static SBitset<MAX_ACTIONS> releasedActions; ///< Action for which keys were released.
  static bool                 inputGrab;       ///< True if in relative mouse mode.

public:

  static Mouse  mouse;  ///< Mouse state.
  static Config config; ///< Current configuration.

public:

  /**
   * Static class.
   */
  Input() = delete;

  /**
   * A key/button for a given action is down in the current tick.
   */
  static bool down(int action)
  {
    return downActions.get(action);
  }

  /**
   * A key/buttons for a given action was up in the previous tick and is now down.
   */
  static bool pressed(int action)
  {
    return pressedActions.get(action);
  }

  /**
   * A key/buttons for a given action was down in the previous tick in is now up.
   */
  static bool released(int action)
  {
    return releasedActions.get(action);
  }

  /**
   * True iff the window has input grab.
   */
  static bool hasGrab()
  {
    return inputGrab;
  }

  /**
   * Toggle input grab.
   */
  static void setGrab(bool grab);

  /**
   * Process an input-replated SDL event.
   */
  static void processEvent(const SDL_Event* event);

  /**
   * Clear current and set new key/button bindings.
   */
  static void setBindings(const Binding* bindings);

  /**
   * Load configuration from a file.
   */
  static bool loadConfig(const File& file);

  /**
   * Save configuration to a file.
   */
  static bool saveConfig(const File& file);

  /**
   * Initialise PPAPI interfaces.
   */
  static void init();

};

}
