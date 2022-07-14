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

/**
 * @file ozEngine/Input.hh
 */

#pragma once

#include "common.hh"

union SDL_Event;

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
  static constexpr int MAX_ACTIONS = 256;

public:

  /**
   * Modifier key.
   */
  enum ModKey
  {
    NONE,    ///< None.
    SHIFT,   ///< (Left or right) Shift key.
    CONTROL, ///< (Left or right) Control key.
    ALT      ///< (Left or right) Alt or AltGr key.
  };

  /**
   * Key-to-action binding.
   */
  struct Binding
  {
    int    key;    ///< Primary key binding.
    int    altKey; ///< Alternate key binding.
    int    mod;    ///< ModKey bitmask for primary binding.
    int    altMod; ///< ModKey bitmask for alternate binding.
    int    button; ///< Mouse button (1 - left, 2 - right, 3 - middle/wheel).
    int    action; ///< User's action id.
    String name;   ///< Action name.
  };

  /**
   * Input devices' settings.
   */
  struct Config
  {
    /**
     * Keyboard settings.
     */
    struct Keyboard
    {
      Vec3 sensitivity; ///< Sensitivity for movement along X, Y and Z axes.
      int  leftKey;     ///< Left key.
      int  rightKey;    ///< Right key.
      int  downKey;     ///< Down key.
      int  upKey;       ///< Up key.
      int  inKey;       ///< Forward / zoom in key.
      int  outKey;      ///< Backward / zoom out key.
    };

    /**
     * Mouse settings.
     */
    struct Mouse
    {
      Vec4  sensitivity; ///< Sensitivity for (X, Y, horizontal when, vertical wheel).
      float smoothing;   ///< Input smoothing for movement and mouse wheel.
      bool  isRaw;       ///< Whether the input is raw i.e. not accelerated by the OS.
    };

    Keyboard keyboard; ///< Keyboard settings.
    Mouse    mouse;    ///< Mouse settings.
  };

  /**
   * Mouse state.
   */
  struct Mouse
  {
    float x;              ///< X-position in pixels.
    float y;              ///< Y-position in pixels.

    float dx;             ///< X-move in pixels in the last update.
    float dy;             ///< Y-move in pixels in the last update.
    float dz;             ///< Horizontal wheel scroll in pixels in the last update.
    float dw;             ///< Vertical wheel scroll in pixels in the last update.

    bool  leftDown;       ///< Left button was down during the last update.
    bool  leftPressed;    ///< Left button pressed in the last update.
    bool  leftReleased;   ///< Left button released in the last update.

    bool  rightDown;      ///< Right button was down during the last update.
    bool  rightPressed;   ///< Right button pressed in the last update.
    bool  rightReleased;  ///< Right button released in the last update.

    bool  middleDown;     ///< Middle button was down during the last update.
    bool  middlePressed;  ///< Middle button pressed in the last update.
    bool  middleReleased; ///< Middle button released in the last update.
  };

private:

  static SBitset<MAX_ACTIONS> downActions_;     ///< Action for which keys were down.
  static SBitset<MAX_ACTIONS> pressedActions_;  ///< Action for which keys were pressed.
  static SBitset<MAX_ACTIONS> releasedActions_; ///< Action for which keys were released.

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
    return downActions_.get(action);
  }

  /**
   * A key/buttons for a given action was up in the previous tick and is now down.
   */
  static bool pressed(int action)
  {
    return pressedActions_.get(action);
  }

  /**
   * A key/buttons for a given action was down in the previous tick in is now up.
   */
  static bool released(int action)
  {
    return releasedActions_.get(action);
  }

  /**
   * Process an input-related SDL event.
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

};

}
