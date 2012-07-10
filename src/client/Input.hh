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

    static const int MOD_OFF_BIT   = 0x10000000;
    static const int MOD_ON_BIT    = 0x20000000;
    static const int MOD_MASK      = 0x30000000;

    enum Key
    {
      KEY_NONE,

      KEY_UI_DRAG,
      KEY_UI_TOGGLE,

      KEY_KP1,
      KEY_KP2,
      KEY_KP3,
      KEY_KP4,
      KEY_KP6,
      KEY_KP7,
      KEY_KP8,
      KEY_KP9,

      KEY_NV_TOGGLE,
      KEY_BINOCULARS_TOGGLE,
      KEY_MAP_TOGGLE,

      KEY_CAMERA_TOGGLE,
      KEY_FREELOOK_TOGGLE,

      KEY_TURN_LEFT,
      KEY_TURN_RIGHT,
      KEY_MOVE_RIGHT,
      KEY_MOVE_LEFT,
      KEY_MOVE_FORWARD,
      KEY_MOVE_BACKWARD,
      KEY_MOVE_UP,
      KEY_MOVE_DOWN,
      KEY_SPEED_TOGGLE,

      KEY_CROUCH_TOGGLE,
      KEY_JUMP,
      KEY_EXIT,
      KEY_EJECT,
      KEY_SUICIDE,
      KEY_FIRE,
      KEY_NEXT_WEAPON,

      KEY_GESTURE_POINT,
      KEY_GESTURE_BACK,
      KEY_GESTURE_SALUTE,
      KEY_GESTURE_WAVE,
      KEY_GESTURE_FLIP,

      KEY_SWITCH_TO_UNIT,
      KEY_CIRCLE_UNITS,

      KEY_CHEAT_SKY_FORWARD,
      KEY_CHEAT_SKY_BACKWARD,

      KEY_QUICKSAVE,
      KEY_QUICKLOAD,
      KEY_AUTOLOAD,
      KEY_QUIT,

      KEY_MAX
    };

  private:

    static const char* KEY_NAMES[KEY_MAX];

    SDLKey modifier0;
    SDLKey modifier1;

    ubyte* sdlCurrKeys;
    ubyte  sdlKeys[SDLK_LAST];
    ubyte  sdlOldKeys[SDLK_LAST];

    int    keyMap[KEY_MAX][2];

  public:

    int    mouseX;
    int    mouseY;
    int    mouseZ;
    int    mouseW;

    char   buttons;
    char   oldButtons;
    char   currButtons;

    bool   leftClick;
    bool   rightClick;
    bool   middleClick;
    bool   wheelUp;
    bool   wheelDown;

    bool   hasFocus;
    bool   isLocked;

    bool   keys[KEY_MAX];
    bool   oldKeys[KEY_MAX];

    float  mouseSensH;
    float  mouseSensV;
    float  keySensH;
    float  keySensV;

  private:

    void loadDefaultKeyMap();
    void loadKeyMap( const JSON& keyConfig );
    JSON keyMapToJSON() const;

  public:

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
