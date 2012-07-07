/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file client/Input.hh
 */

#pragma once

#include "client/common.hh"

namespace oz
{
namespace client
{

class Input
{
  public:

    static const int LEFT_BUTTON   = 0x01;
    static const int MIDDLE_BUTTON = 0x02;
    static const int RIGHT_BUTTON  = 0x04;

    enum Key
    {
      KEY_KP_1,
      KEY_KP_2,
      KEY_KP_3,
      KEY_KP_4,
      KEY_KP_6,
      KEY_KP_7,
      KEY_KP_8,
      KEY_KP_9,
      KEY_FREELOOK_TOGGLE,
      KEY_CAMERA_TOGGLE,
      KEY_TURN_RIGHT,
      KEY_TURN_LEFT,
      KEY_STRAFE_LEFT,
      KEY_STRAFE_RIGHT,
      KEY_FORWARD,
      KEY_BACKWARD,
      KEY_UP,
      KEY_DOWN,
      KEY_WALK_TOGGLE,
      KEY_CROUCH_TOGGLE,
      KEY_JUMP,
      KEY_UI_TOGGLE,

      KEY_MAX
    };

  private:

    ubyte* currKeys;

  public:

    int   mouseX;
    int   mouseY;
    int   mouseZ;
    int   mouseW;

    char  buttons;
    char  oldButtons;
    char  currButtons;

    bool  leftClick;
    bool  rightClick;
    bool  middleClick;
    bool  wheelUp;
    bool  wheelDown;

    bool  hasFocus;
    bool  isLocked;

    ubyte keys[SDLK_LAST];
    ubyte oldKeys[SDLK_LAST];

    void readEvent( SDL_Event* event );

    void reset();
    void prepare();
    void update();

    void init();
    void free();

};

extern Input input;

}
}
