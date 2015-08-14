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

struct SDL_Event;

namespace oz
{

class Input
{
private:

  static const int MAX_ACTIONS = 255;

private:

  static SBitset<MAX_ACTIONS> downActions;     ///< Action for which keys were down.
  static SBitset<MAX_ACTIONS> pressedActions;  ///< Action for which keys were pressed.
  static SBitset<MAX_ACTIONS> releasedActions; ///< Action for which keys were released.

public:

  /**
   * Static class.
   */
  Input() = delete;

  /**
   * A key/button for a given action was down at some moment during the current tick.
   */
  static bool down(int action)
  {
    return downActions.get(action);
  }

  /**
   * A key/buttons for a given action changed state to down at some moment during the current tick.
   */
  static bool pressed(int action)
  {
    return pressedActions.get(action);
  }

  /**
   * A key/buttons for a given action changed state to up at some moment during the current tick.
   */
  static bool released(int action)
  {
    return releasedActions.get(action);
  }

  static void handle(SDL_Event* event);

};

}
