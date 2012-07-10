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
 * @file client/Input.cc
 */

#include "stable.hh"

#include "client/Input.hh"

#include "client/Window.hh"
#include "client/NaCl.hh"

namespace oz
{
namespace client
{

Input input;

const char* Input::KEY_NAMES[] = {
  "None",

  "Drag UI windows",
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
  "Fire weapon",
  "Next weapon",

  "'Point' gesture",
  "'Fall back' gesture",
  "'Salute' gesture",
  "'Wave' gesture",
  "'Flip out' gesture",

  "Switch to selected unit",
  "Circle units",

  "Fast forward sky (cheat)",
  "Fast backward sky (cheat)",

  "Quick save",
  "Quick load",
  "Auto load",

  "Quit"
};

void Input::loadDefaultKeyMap()
{
  modifier0 = SDLK_LALT;
  modifier1 = SDLK_RALT;

  keyMap[KEY_UI_DRAG][0]            = MOD_MASK    | SDLK_LALT;
  keyMap[KEY_UI_DRAG][1]            = MOD_MASK    | SDLK_RALT;
  keyMap[KEY_UI_TOGGLE][0]          = MOD_MASK    | SDLK_TAB;

  keyMap[KEY_KP1][0]                = MOD_MASK    | SDLK_KP1;
  keyMap[KEY_KP1][1]                = MOD_MASK    | SDLK_END;
  keyMap[KEY_KP2][0]                = MOD_MASK    | SDLK_KP2;
  keyMap[KEY_KP2][1]                = MOD_MASK    | SDLK_DOWN;
  keyMap[KEY_KP3][0]                = MOD_MASK    | SDLK_KP3;
  keyMap[KEY_KP3][1]                = MOD_MASK    | SDLK_PAGEDOWN;
  keyMap[KEY_KP4][0]                = MOD_MASK    | SDLK_KP4;
  keyMap[KEY_KP4][1]                = MOD_MASK    | SDLK_LEFT;
  keyMap[KEY_KP6][0]                = MOD_MASK    | SDLK_KP6;
  keyMap[KEY_KP6][1]                = MOD_MASK    | SDLK_RIGHT;
  keyMap[KEY_KP7][0]                = MOD_MASK    | SDLK_KP7;
  keyMap[KEY_KP7][1]                = MOD_MASK    | SDLK_HOME;
  keyMap[KEY_KP8][0]                = MOD_MASK    | SDLK_KP8;
  keyMap[KEY_KP8][1]                = MOD_MASK    | SDLK_UP;
  keyMap[KEY_KP9][0]                = MOD_MASK    | SDLK_KP9;
  keyMap[KEY_KP9][1]                = MOD_MASK    | SDLK_PAGEUP;

  keyMap[KEY_NV_TOGGLE][0]          = MOD_OFF_BIT | SDLK_n;
  keyMap[KEY_BINOCULARS_TOGGLE][0]  = MOD_OFF_BIT | SDLK_b;
  keyMap[KEY_MAP_TOGGLE][0]         = MOD_OFF_BIT | SDLK_m;

  keyMap[KEY_CAMERA_TOGGLE][0]      = MOD_MASK    | SDLK_KP_ENTER;
  keyMap[KEY_CAMERA_TOGGLE][1]      = MOD_MASK    | SDLK_RETURN;
  keyMap[KEY_FREELOOK_TOGGLE][0]    = MOD_MASK    | SDLK_KP_MULTIPLY;

  keyMap[KEY_TURN_LEFT][0]          = MOD_MASK    | SDLK_q;
  keyMap[KEY_TURN_RIGHT][0]         = MOD_MASK    | SDLK_e;
  keyMap[KEY_MOVE_RIGHT][0]         = MOD_MASK    | SDLK_d;
  keyMap[KEY_MOVE_LEFT][0]          = MOD_MASK    | SDLK_a;
  keyMap[KEY_MOVE_FORWARD][0]       = MOD_MASK    | SDLK_w;
  keyMap[KEY_MOVE_BACKWARD][0]      = MOD_MASK    | SDLK_s;
  keyMap[KEY_MOVE_UP][0]            = MOD_MASK    | SDLK_SPACE;
  keyMap[KEY_MOVE_DOWN][0]          = MOD_MASK    | SDLK_LCTRL;
  keyMap[KEY_MOVE_DOWN][1]          = MOD_MASK    | SDLK_c;
  keyMap[KEY_SPEED_TOGGLE][0]       = MOD_MASK    | SDLK_LSHIFT;

  keyMap[KEY_CROUCH_TOGGLE][0]      = MOD_MASK    | SDLK_LCTRL;
  keyMap[KEY_JUMP][0]               = MOD_MASK    | SDLK_SPACE;
  keyMap[KEY_EXIT][0]               = MOD_OFF_BIT | SDLK_x;
  keyMap[KEY_EJECT][0]              = MOD_ON_BIT  | SDLK_x;
  keyMap[KEY_SUICIDE][0]            = MOD_ON_BIT  | SDLK_k;

  keyMap[KEY_GESTURE_POINT][0]      = MOD_OFF_BIT | SDLK_f;
  keyMap[KEY_GESTURE_BACK][0]       = MOD_OFF_BIT | SDLK_g;
  keyMap[KEY_GESTURE_SALUTE][0]     = MOD_OFF_BIT | SDLK_h;
  keyMap[KEY_GESTURE_WAVE][0]       = MOD_OFF_BIT | SDLK_j;
  keyMap[KEY_GESTURE_FLIP][0]       = MOD_OFF_BIT | SDLK_k;

  keyMap[KEY_SWITCH_TO_UNIT][0]     = MOD_OFF_BIT | SDLK_i;
  keyMap[KEY_CIRCLE_UNITS][0]       = MOD_OFF_BIT | SDLK_y;

  keyMap[KEY_CHEAT_SKY_FORWARD][0]  = MOD_OFF_BIT | SDLK_p;
  keyMap[KEY_CHEAT_SKY_BACKWARD][0] = MOD_OFF_BIT | SDLK_o;

  keyMap[KEY_QUICKSAVE][0]          = MOD_MASK    | SDLK_F5;
  keyMap[KEY_QUICKLOAD][0]          = MOD_MASK    | SDLK_F7;
  keyMap[KEY_AUTOLOAD][0]           = MOD_MASK    | SDLK_F8;
  keyMap[KEY_QUIT][0]               = MOD_MASK    | SDLK_F10;
  keyMap[KEY_QUIT][1]               = MOD_MASK    | SDLK_ESCAPE;
}

void Input::loadKeyMap( const JSON& keyConfig )
{
  HashString<SDLKey, 512> sdlKeyNames;

  for( int i = 0; i < SDLK_LAST; ++i ) {
    const char* name = SDL_GetKeyName( SDLKey( i ) );

    sdlKeyNames.add( name, SDLKey( i ) );
  }

  for( int i = KEY_NONE + 1; i < KEY_MAX; ++i ) {
    const JSON& keyBindings = keyConfig[ KEY_NAMES[i] ];

    int nBindings = keyBindings.length();
    if( nBindings > 2 ) {
      throw Exception( "Key '%s' has %d bindings but at most 2 supported",
                       KEY_NAMES[i], nBindings );
    }

    for( int j = 0; j < nBindings; ++j ) {
      const String& keyDesc = keyBindings[j].asString();

      if( keyDesc.isEmpty() ) {
        throw Exception( "Empty key description string for '%s'", KEY_NAMES[i] );
      }

      const SDLKey* sdlKey = sdlKeyNames.find( &keyDesc[1] );
      int mod = keyDesc[0] == '_' ? int( MOD_OFF_BIT ) :
                keyDesc[0] == '!' ? int( MOD_ON_BIT ) : int( MOD_MASK );

      if( sdlKey == null ) {
        throw Exception( "Cannot resolve SDL key name '%s'", &keyDesc[1] );
      }

      keyMap[i][j] = mod | *sdlKey;
    }
  }
}

JSON Input::keyMapToJSON() const
{
  JSON keyConfig;
  keyConfig.setObject();

  for( int i = KEY_NONE + 1; i < KEY_MAX; ++i ) {
    JSON& key = keyConfig.addArray( KEY_NAMES[i] );

    for( int j = 0; j < 2; ++j ) {
      if( keyMap[i][j] != KEY_NONE ) {
        String sdlName = SDL_GetKeyName( SDLKey( keyMap[i][j] & ~MOD_MASK ) );

        int mod = keyMap[i][j] & MOD_MASK;
        key.add( ( mod == MOD_OFF_BIT ? "_" : mod == MOD_ON_BIT ? "!" : "&" ) + sdlName );
      }
    }
  }

  return keyConfig;
}

void Input::readEvent( SDL_Event* event )
{
  switch( event->type ) {
    case SDL_MOUSEBUTTONUP: {
      currButtons &= char( ~SDL_BUTTON( event->button.button ) );
      break;
    }
    case SDL_MOUSEBUTTONDOWN: {
      buttons     |= char( SDL_BUTTON( event->button.button ) );
      currButtons |= char( SDL_BUTTON( event->button.button ) );

#if SDL_MAJOR_VERSION < 2
      if( buttons & SDL_BUTTON( SDL_BUTTON_WHEELUP ) ) {
        ++mouseW;
      }
      if( buttons & SDL_BUTTON( SDL_BUTTON_WHEELDOWN ) ) {
        --mouseW;
      }
#endif
      break;
    }
#if SDL_MAJOR_VERSION >= 2
    case SDL_MOUSEWHEEL: {
      mouseW -= event->wheel.y;
      break;
    }
#endif
    case SDL_KEYDOWN: {
      sdlKeys[event->key.keysym.sym] |= SDL_PRESSED;
      break;
    }
    default: {
      hard_assert( false );
      break;
    }
  }
}

void Input::reset()
{
#ifdef __native_client__

  NaCl::moveX = 0;
  NaCl::moveY = 0;
  NaCl::moveZ = 0;
  NaCl::moveW = 0;

#else

#if SDL_VERSION_MAJOR < 2
  SDL_WarpMouse( ushort( window.width / 2 ), ushort( window.height / 2 ) );
#else
  SDL_WarpMouseInWindow( window.descriptor, window.width / 2, window.height / 2 );
#endif
  // Suppress mouse motion event generated by SDL_WarpMouse().
  SDL_PumpEvents();
  SDL_GetRelativeMouseState( null, null );

#endif
}

void Input::prepare()
{
  mouseX     = 0;
  mouseY     = 0;
  mouseZ     = 0;
  mouseW     = 0;

  oldButtons = buttons;
  buttons    = currButtons;

  memcpy( sdlOldKeys, sdlKeys, sizeof( sdlKeys ) );
  memcpy( sdlKeys, sdlCurrKeys, sizeof( sdlKeys ) );
}

void Input::update()
{
  if( !hasFocus ) {
    return;
  }

  int inputX, inputY;

#ifdef __native_client__
  inputX = NaCl::moveX;
  inputY = NaCl::moveY;
#else
  SDL_GetRelativeMouseState( &inputX, &inputY );
#endif

  mouseX = +inputX;
  mouseY = -inputY;

  int clickedButtons = input.buttons & ~input.oldButtons;

  leftClick   = clickedButtons & Input::LEFT_BUTTON;
  middleClick = clickedButtons & Input::MIDDLE_BUTTON;
  rightClick  = clickedButtons & Input::RIGHT_BUTTON;
  wheelUp     = input.mouseW > 0;
  wheelDown   = input.mouseW < 0;

  // If input is not grabbed we must centre mouse so it cannot move out of the window.
  if( isLocked ) {
    reset();
  }

  memcpy( oldKeys, keys, sizeof( keys ) );
  memset( keys, 0, sizeof( keys ) );

  int mod = sdlKeys[modifier0] | sdlKeys[modifier1] ? int( MOD_ON_BIT ) : int( MOD_OFF_BIT );

  for( int i = 0; i < aLength( keys ); ++i ) {
    if( keyMap[i][0] & mod ) {
      keys[i] |= sdlKeys[ keyMap[i][0] & ~MOD_MASK ];
    }
    if( keyMap[i][1] & mod ) {
      keys[i] |= sdlKeys[ keyMap[i][1] & ~MOD_MASK ];
    }
  }
}

void Input::init()
{
  File configFile( config["dir.config"].asString() + "/input.json" );

  Log::print( "Initialising Input from '%s' ...", configFile.path().cstr() );

  JSON inputConfig;
  bool configExists = inputConfig.load( &configFile );

  const JSON& mouseConfig    = inputConfig["mouse"];
  const JSON& keyboardConfig = inputConfig["keyboard"];
  const JSON& keyMapConfig   = inputConfig["bindings"];

  memset( sdlKeys, 0, sizeof( sdlKeys ) );
  memset( sdlOldKeys, 0, sizeof( sdlOldKeys ) );
  sdlCurrKeys = SDL_GetKeyState( null );

  memset( keyMap, 0, sizeof( keyMap ) );

  if( configExists ) {
    loadKeyMap( keyMapConfig );
  }
  else {
    loadDefaultKeyMap();
  }

  mouseX      = 0;
  mouseY      = 0;
  mouseZ      = 0;
  mouseW      = 0;

  buttons     = 0;
  oldButtons  = 0;
  currButtons = 0;

  leftClick   = false;
  middleClick = false;
  rightClick  = false;
  wheelUp     = false;
  wheelDown   = false;

  hasFocus    = true;
  isLocked    = true;

  memset( keys, 0, sizeof( keys ) );
  memset( oldKeys, 0, sizeof( oldKeys ) );

  mouseSensH = mouseConfig["sensitivityH"].get( 0.004f );
  mouseSensV = mouseConfig["sensitivityV"].get( 0.004f );
  keySensH   = keyboardConfig["sensitivityH"].get( 0.04f );
  keySensV   = keyboardConfig["sensitivityV"].get( 0.04f );

  Log::printEnd( " OK" );
}

void Input::free()
{
  File configFile( config["dir.config"].asString() + "/input.json" );

  Log::print( "Writing current Input configuration to '%s' ...", configFile.path().cstr() );

  JSON inputConfig;
  inputConfig.setObject();

  JSON& mouseConfig    = inputConfig.addObject( "mouse" );
  JSON& keyboardConfig = inputConfig.addObject( "keyboard" );
  JSON& keyMapConfig   = inputConfig.addObject( "bindings" );

  mouseConfig.add( "sensitivityH", mouseSensH );
  mouseConfig.add( "sensitivityV", mouseSensV );
  keyboardConfig.add( "sensitivityH", keySensH );
  keyboardConfig.add( "sensitivityV", keySensV );
  keyMapConfig = keyMapToJSON();

  if( !inputConfig.save( &configFile ) ) {
    throw Exception( "Failed to write '%s'", configFile.path().cstr() );
  }

  Log::printEnd( " OK" );
}

}
}
