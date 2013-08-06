/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

namespace oz
{
namespace client
{

const char* const Input::KEY_NAMES[] = {
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

  "Quick save",
  "Quick load",
  "Save layout",
  "Load layout",
  "Auto load",

  "Quit"
};

#if SDL_MAJOR_VERSION < 2
const char* const Input::BACKEND = "SDL1";
#else
const char* const Input::BACKEND = "SDL2";
#endif

void Input::loadDefaultKeyMap()
{
#if SDL_MAJOR_VERSION < 2

  modifier0                         = SDLK_LALT;
  modifier1                         = SDLK_RALT;

  keyMap[KEY_UI_ALT][0]             = MOD_MASK    | SDLK_LALT;
  keyMap[KEY_UI_ALT][1]             = MOD_MASK    | SDLK_RALT;
  keyMap[KEY_UI_TOGGLE][0]          = MOD_MASK    | SDLK_TAB;

  keyMap[KEY_DIR_1][0]              = MOD_MASK    | SDLK_KP1;
  keyMap[KEY_DIR_1][1]              = MOD_MASK    | SDLK_END;
  keyMap[KEY_DIR_2][0]              = MOD_MASK    | SDLK_KP2;
  keyMap[KEY_DIR_2][1]              = MOD_MASK    | SDLK_DOWN;
  keyMap[KEY_DIR_3][0]              = MOD_MASK    | SDLK_KP3;
  keyMap[KEY_DIR_3][1]              = MOD_MASK    | SDLK_PAGEDOWN;
  keyMap[KEY_DIR_4][0]              = MOD_MASK    | SDLK_KP4;
  keyMap[KEY_DIR_4][1]              = MOD_MASK    | SDLK_LEFT;
  keyMap[KEY_DIR_6][0]              = MOD_MASK    | SDLK_KP6;
  keyMap[KEY_DIR_6][1]              = MOD_MASK    | SDLK_RIGHT;
  keyMap[KEY_DIR_7][0]              = MOD_MASK    | SDLK_KP7;
  keyMap[KEY_DIR_7][1]              = MOD_MASK    | SDLK_HOME;
  keyMap[KEY_DIR_8][0]              = MOD_MASK    | SDLK_KP8;
  keyMap[KEY_DIR_8][1]              = MOD_MASK    | SDLK_UP;
  keyMap[KEY_DIR_9][0]              = MOD_MASK    | SDLK_KP9;
  keyMap[KEY_DIR_9][1]              = MOD_MASK    | SDLK_PAGEUP;

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
  keyMap[KEY_MOVE_DOWN][0]          = MOD_MASK    | SDLK_c;
  keyMap[KEY_SPEED_TOGGLE][0]       = MOD_MASK    | SDLK_LSHIFT;

  keyMap[KEY_CROUCH_TOGGLE][0]      = MOD_MASK    | SDLK_c;
  keyMap[KEY_CROUCH_TOGGLE][1]      = MOD_MASK    | SDLK_LCTRL;
  keyMap[KEY_JUMP][0]               = MOD_MASK    | SDLK_SPACE;
  keyMap[KEY_EXIT][0]               = MOD_OFF_BIT | SDLK_x;
  keyMap[KEY_EJECT][0]              = MOD_ON_BIT  | SDLK_x;
  keyMap[KEY_SUICIDE][0]            = MOD_ON_BIT  | SDLK_k;

  keyMap[KEY_ACTIVATE][0]           = MOD_ON_BIT  | SDLK_e;
  keyMap[KEY_GRAB][0]               = MOD_ON_BIT  | SDLK_f;
  keyMap[KEY_THROW][0]              = MOD_ON_BIT  | SDLK_r;
  keyMap[KEY_PICK_UP][0]            = MOD_ON_BIT  | SDLK_q;

  keyMap[KEY_GESTURE_POINT][0]      = MOD_OFF_BIT | SDLK_g;
  keyMap[KEY_GESTURE_BACK][0]       = MOD_OFF_BIT | SDLK_h;
  keyMap[KEY_GESTURE_SALUTE][0]     = MOD_OFF_BIT | SDLK_j;
  keyMap[KEY_GESTURE_WAVE][0]       = MOD_OFF_BIT | SDLK_k;
  keyMap[KEY_GESTURE_FLIP][0]       = MOD_OFF_BIT | SDLK_l;

  keyMap[KEY_SWITCH_TO_UNIT][0]     = MOD_OFF_BIT | SDLK_i;
  keyMap[KEY_CYCLE_UNITS][0]        = MOD_OFF_BIT | SDLK_y;

  keyMap[KEY_GROUP_SELECT][0]       = MOD_MASK    | SDLK_LSHIFT;
  keyMap[KEY_GROUP_SELECT][1]       = MOD_MASK    | SDLK_RSHIFT;

  keyMap[KEY_CHEAT_SKY_FORWARD][0]  = MOD_OFF_BIT | SDLK_p;
  keyMap[KEY_CHEAT_SKY_BACKWARD][0] = MOD_OFF_BIT | SDLK_o;

  keyMap[KEY_QUICKSAVE][0]          = MOD_OFF_BIT | SDLK_F5;
  keyMap[KEY_QUICKLOAD][0]          = MOD_OFF_BIT | SDLK_F7;
  keyMap[KEY_AUTOLOAD][0]           = MOD_OFF_BIT | SDLK_F8;
  keyMap[KEY_SAVE_LAYOUT][0]        = MOD_ON_BIT  | SDLK_F5;
  keyMap[KEY_LOAD_LAYOUT][0]        = MOD_ON_BIT  | SDLK_F7;
  keyMap[KEY_QUIT][0]               = MOD_MASK    | SDLK_F10;
  keyMap[KEY_QUIT][1]               = MOD_MASK    | SDLK_ESCAPE;

#else

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
  keyMap[KEY_SPEED_TOGGLE][0]       = MOD_MASK    | SDL_SCANCODE_LSHIFT;

  keyMap[KEY_CROUCH_TOGGLE][0]      = MOD_MASK    | SDL_SCANCODE_C;
  keyMap[KEY_CROUCH_TOGGLE][1]      = MOD_MASK    | SDL_SCANCODE_LCTRL;
  keyMap[KEY_JUMP][0]               = MOD_MASK    | SDL_SCANCODE_SPACE;
  keyMap[KEY_EXIT][0]               = MOD_OFF_BIT | SDL_SCANCODE_X;
  keyMap[KEY_EJECT][0]              = MOD_ON_BIT  | SDL_SCANCODE_X;
  keyMap[KEY_SUICIDE][0]            = MOD_ON_BIT  | SDL_SCANCODE_K;

  keyMap[KEY_ACTIVATE][0]           = MOD_ON_BIT  | SDL_SCANCODE_E;
  keyMap[KEY_GRAB][0]               = MOD_ON_BIT  | SDL_SCANCODE_F;
  keyMap[KEY_THROW][0]              = MOD_ON_BIT  | SDL_SCANCODE_R;
  keyMap[KEY_PICK_UP][0]            = MOD_ON_BIT  | SDL_SCANCODE_Q;

  keyMap[KEY_GESTURE_POINT][0]      = MOD_OFF_BIT | SDL_SCANCODE_G;
  keyMap[KEY_GESTURE_BACK][0]       = MOD_OFF_BIT | SDL_SCANCODE_H;
  keyMap[KEY_GESTURE_SALUTE][0]     = MOD_OFF_BIT | SDL_SCANCODE_J;
  keyMap[KEY_GESTURE_WAVE][0]       = MOD_OFF_BIT | SDL_SCANCODE_K;
  keyMap[KEY_GESTURE_FLIP][0]       = MOD_OFF_BIT | SDL_SCANCODE_L;

  keyMap[KEY_SWITCH_TO_UNIT][0]     = MOD_OFF_BIT | SDL_SCANCODE_I;
  keyMap[KEY_CYCLE_UNITS][0]        = MOD_OFF_BIT | SDL_SCANCODE_Y;

  keyMap[KEY_GROUP_SELECT][0]       = MOD_MASK    | SDL_SCANCODE_LSHIFT;
  keyMap[KEY_GROUP_SELECT][1]       = MOD_MASK    | SDL_SCANCODE_RSHIFT;

  keyMap[KEY_CHEAT_SKY_FORWARD][0]  = MOD_OFF_BIT | SDL_SCANCODE_P;
  keyMap[KEY_CHEAT_SKY_BACKWARD][0] = MOD_OFF_BIT | SDL_SCANCODE_O;

  keyMap[KEY_QUICKSAVE][0]          = MOD_OFF_BIT | SDL_SCANCODE_F5;
  keyMap[KEY_QUICKLOAD][0]          = MOD_OFF_BIT | SDL_SCANCODE_F7;
  keyMap[KEY_AUTOLOAD][0]           = MOD_OFF_BIT | SDL_SCANCODE_F8;
  keyMap[KEY_SAVE_LAYOUT][0]        = MOD_ON_BIT  | SDL_SCANCODE_F5;
  keyMap[KEY_LOAD_LAYOUT][0]        = MOD_ON_BIT  | SDL_SCANCODE_F7;
  keyMap[KEY_QUIT][0]               = MOD_MASK    | SDL_SCANCODE_F10;
  keyMap[KEY_QUIT][1]               = MOD_MASK    | SDL_SCANCODE_ESCAPE;

#endif
}

void Input::loadKeyMap( const JSON& keyConfig )
{
#if SDL_MAJOR_VERSION < 2

  HashMap<String, SDLKey, 512> sdlKeyNames;

  for( int i = 0; i < SDLK_LAST; ++i ) {
    const char* name = SDL_GetKeyName( SDLKey( i ) );

    sdlKeyNames.add( name, SDLKey( i ) );
  }

  const SDLKey* pModifier0 = sdlKeyNames.find( keyConfig["modifier0"].get( "" ) );
  const SDLKey* pModifier1 = sdlKeyNames.find( keyConfig["modifier1"].get( "" ) );

  modifier0 = pModifier0 == nullptr ? SDLK_UNKNOWN : *pModifier0;
  modifier1 = pModifier1 == nullptr ? SDLK_UNKNOWN : *pModifier1;

#else

  modifier0 = SDL_GetScancodeFromName( keyConfig["modifier0"].get( "" ) );
  modifier1 = SDL_GetScancodeFromName( keyConfig["modifier1"].get( "" ) );

#endif

  for( int i = KEY_NONE + 1; i < KEY_MAX; ++i ) {
    const JSON& keyBindings = keyConfig[ KEY_NAMES[i] ];

    int nBindings = keyBindings.length();
    if( nBindings > 2 ) {
      OZ_ERROR( "Key '%s' has %d bindings but at most 2 supported", KEY_NAMES[i], nBindings );
    }

    for( int j = 0; j < nBindings; ++j ) {
      const String& keyDesc = keyBindings[j].asString();

      if( keyDesc.isEmpty() ) {
        OZ_ERROR( "Empty key description string for '%s'", KEY_NAMES[i] );
      }

      int mod = keyDesc[0] == '_' ? int( MOD_OFF_BIT ) :
                keyDesc[0] == '^' ? int( MOD_ON_BIT ) :
                                    int( MOD_MASK );

#if SDL_MAJOR_VERSION < 2

      const SDLKey* sdlKey = sdlKeyNames.find( &keyDesc[1] );
      if( sdlKey == nullptr ) {
        OZ_ERROR( "Cannot resolve SDL key name '%s'", &keyDesc[1] );
      }

      keyMap[i][j] = mod | *sdlKey;

#else

      SDL_Scancode sdlKey = SDL_GetScancodeFromName( &keyDesc[1] );

      keyMap[i][j] = mod | sdlKey;

#endif
    }
  }
}

JSON Input::keyMapToJSON() const
{
  JSON keyConfig( JSON::OBJECT );

#if SDL_MAJOR_VERSION < 2
  keyConfig.add( "modifier0", SDL_GetKeyName( modifier0 ) );
  keyConfig.add( "modifier1", SDL_GetKeyName( modifier1 ) );
#else
  keyConfig.add( "modifier0", SDL_GetScancodeName( modifier0 ) );
  keyConfig.add( "modifier1", SDL_GetScancodeName( modifier1 ) );
#endif

  for( int i = KEY_NONE + 1; i < KEY_MAX; ++i ) {
    JSON& key = keyConfig.add( KEY_NAMES[i], JSON::ARRAY );

    for( int j = 0; j < 2; ++j ) {
      if( keyMap[i][j] != KEY_NONE ) {
        int mod  = keyMap[i][j] & MOD_MASK;
        int code = keyMap[i][j] & ~MOD_MASK ;

#if SDL_MAJOR_VERSION < 2
        String sdlName = SDL_GetKeyName( SDLKey( code ) );
#else
        String sdlName = SDL_GetScancodeName( SDL_Scancode( code ) );
#endif

        key.add( ( mod == MOD_OFF_BIT ? "_" : mod == MOD_ON_BIT ? "^" : "&" ) + sdlName );
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
      mouseZ += float( event->wheel.x );
      mouseW += float( event->wheel.y );
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
    default: {
      hard_assert( false );
      break;
    }
  }
}

void Input::reset()
{
  Window::warpMouse();

  mouseX         = 0.0f;
  mouseY         = 0.0f;
  mouseZ         = 0.0f;
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

  mSet( sdlKeys, 0, sizeof( sdlKeys ) );
  mSet( sdlOldKeys, 0, sizeof( sdlOldKeys ) );
  mSet( sdlCurrKeys, 0, sizeof( sdlCurrKeys ) );

  isKeyPressed   = false;
  isKeyReleased  = false;
}

void Input::prepare()
{
  mouseX         = 0.0f;
  mouseY         = 0.0f;
  mouseZ         = 0.0f;
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

  mCopy( sdlOldKeys, sdlKeys, sizeof( sdlKeys ) );
  mCopy( sdlKeys, sdlCurrKeys, sizeof( sdlKeys ) );

  isKeyPressed  = false;
  isKeyReleased = false;
}

void Input::update()
{
  if( !Window::hasFocus() ) {
    return;
  }

#if defined( __native_client__ )

  mouseX = +Pepper::moveX;
  mouseY = -Pepper::moveY;

#else

  int dx, dy;
  SDL_GetRelativeMouseState( &dx, &dy );

  mouseX = +float( dx );
  mouseY = -float( dy );

# ifndef _WIN32
  if( Window::hasGrab() ) {
    // Compensate lack of mouse acceleration when receiving raw (non-accelerated) mouse input. This
    // code is not based on actual code from X.Org, but experimentally tuned to match default X
    // server mouse acceleration as closely as possible.
    float move2  = max( mouseX*mouseX + mouseY*mouseY - mouseAccelThreshold, 0.0f );
    float move   = Math::fastSqrt( move2 );
    float factor = min( mouseAccelC0 + mouseAccelC1 * move + mouseAccelC2 * move2, mouseMaxAccel );

    mouseX *= factor;
    mouseY *= factor;
    mouseW *= mouseWheelStep;
  }
# endif
#endif

  int pressedButtons  = input.buttons & ~input.oldButtons;
  int releasedButtons = ~input.buttons & input.oldButtons;

  leftPressed    = pressedButtons  & Input::LEFT_BUTTON;
  leftReleased   = releasedButtons & Input::LEFT_BUTTON;
  middlePressed  = pressedButtons  & Input::MIDDLE_BUTTON;
  middleReleased = releasedButtons & Input::MIDDLE_BUTTON;
  rightPressed   = pressedButtons  & Input::RIGHT_BUTTON;
  rightReleased  = releasedButtons & Input::RIGHT_BUTTON;
  wheelUp        = input.mouseW > 0;
  wheelDown      = input.mouseW < 0;

  Window::warpMouse();

  mCopy( oldKeys, keys, sizeof( keys ) );
  mSet( keys, 0, sizeof( keys ) );

  int mod = sdlKeys[modifier0] | sdlKeys[modifier1] ? int( MOD_ON_BIT ) : int( MOD_OFF_BIT );

  for( int i = 0; i < aLength( keys ); ++i ) {
    if( keyMap[i][0] & mod ) {
      keys[i] |= sdlKeys[ keyMap[i][0] & ~MOD_MASK ];
    }
    if( keyMap[i][1] & mod ) {
      keys[i] |= sdlKeys[ keyMap[i][1] & ~MOD_MASK ];
    }
  }

  lookX = 0.0f;
  lookY = 0.0f;
  moveX = 0.0f;
  moveY = 0.0f;

  if( keys[Input::KEY_DIR_1] | keys[Input::KEY_DIR_4] | keys[Input::KEY_DIR_7] ) {
    lookX += keySensX;
  }
  if( keys[Input::KEY_DIR_3] | keys[Input::KEY_DIR_6] | keys[Input::KEY_DIR_9] ) {
    lookX -= keySensX;
  }
  if( keys[Input::KEY_DIR_1] | keys[Input::KEY_DIR_2] | keys[Input::KEY_DIR_3] ) {
    lookY -= keySensY;
  }
  if( keys[Input::KEY_DIR_7] | keys[Input::KEY_DIR_8] | keys[Input::KEY_DIR_9] ) {
    lookY += keySensY;
  }

  if( input.keys[Input::KEY_MOVE_FORWARD] ) {
    moveY += 1.0f;
  }
  if( input.keys[Input::KEY_MOVE_BACKWARD] ) {
    moveY -= 1.0f;
  }
  if( input.keys[Input::KEY_MOVE_RIGHT] ) {
    moveX += 1.0f;
  }
  if( input.keys[Input::KEY_MOVE_LEFT] ) {
    moveX -= 1.0f;
  }
}

void Input::init()
{
  File configFile( config["dir.config"].asString() + "/input.json" );

  Log::print( "Initialising Input from '%s' ...", configFile.path().cstr() );

  JSON inputConfig;
  configExists = inputConfig.load( configFile );

  if( !String::equals( inputConfig["_version"].get( "" ), OZ_VERSION ) ) {
    configExists = false;
    inputConfig = JSON();
  }

  if( !String::equals( inputConfig["_backend"].get( "" ), BACKEND ) ) {
    configExists = false;
    inputConfig = JSON();
  }

  const JSON& mouseConfig    = inputConfig["mouse"];
  const JSON& keyboardConfig = inputConfig["keyboard"];
  const JSON& keyMapConfig   = inputConfig["bindings"];

  mSet( sdlKeys, 0, sizeof( sdlKeys ) );
  mSet( sdlOldKeys, 0, sizeof( sdlOldKeys ) );
  mSet( sdlCurrKeys, 0, sizeof( sdlCurrKeys ) );

  mSet( keyMap, 0, sizeof( keyMap ) );

  if( configExists ) {
    loadKeyMap( keyMapConfig );
  }
  else {
    Log::printRaw( " Non-existent or invalid, loading defaults ..." );

    loadDefaultKeyMap();
  }

  mouseX         = 0.0f;
  mouseY         = 0.0f;
  mouseZ         = 0.0f;
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

  mSet( keys, 0, sizeof( keys ) );
  mSet( oldKeys, 0, sizeof( oldKeys ) );

  isKeyPressed        = false;
  isKeyReleased       = false;

  mouseSensX          = mouseConfig["sensitivity.x"].get( 0.004f );
  mouseSensY          = mouseConfig["sensitivity.y"].get( 0.004f );
  mouseSensZ          = mouseConfig["sensitivity.z"].get( 2.0f );
  mouseSensW          = mouseConfig["sensitivity.w"].get( 2.0f );

  mouseAccelThreshold = mouseConfig["acceleration.threshold"].get( 0.0f );
  mouseMaxAccel       = mouseConfig["acceleration.max"].get( 2.0f );
  mouseAccelC0        = mouseConfig["acceleration.c0"].get( 1.0f );
  mouseAccelC1        = mouseConfig["acceleration.c1"].get( 0.0f );
  mouseAccelC2        = mouseConfig["acceleration.c2"].get( 0.0004f );
  mouseWheelStep      = mouseConfig["wheelStep"].get( 3.0f );

  keySensX            = keyboardConfig["sensitivity.x"].get( 0.04f );
  keySensY            = keyboardConfig["sensitivity.y"].get( 0.04f );

#if SDL_MAJOR_VERSION < 2
  SDL_ShowCursor( false );
#else
  SDL_SetRelativeMouseMode( SDL_TRUE );
#endif

  inputConfig.clear( true );

  Log::printEnd( " OK" );
}

void Input::destroy()
{
  if( configExists ) {
    return;
  }

  File configFile( config["dir.config"].asString() + "/input.json" );

  Log::print( "Writing Input configuration to '%s' ...", configFile.path().cstr() );

  JSON inputConfig( JSON::OBJECT );

  inputConfig.add( "_version", OZ_VERSION );
  inputConfig.add( "_backend", BACKEND );

  JSON& mouseConfig    = inputConfig.add( "mouse", JSON::OBJECT );
  JSON& keyboardConfig = inputConfig.add( "keyboard", JSON::OBJECT );
  JSON& keyMapConfig   = inputConfig.add( "bindings", JSON::OBJECT );

  mouseConfig.add( "sensitivity.x",          mouseSensX );
  mouseConfig.add( "sensitivity.y",          mouseSensY );
  mouseConfig.add( "sensitivity.z",          mouseSensZ );
  mouseConfig.add( "sensitivity.w",          mouseSensW );
  mouseConfig.add( "acceleration.threshold", mouseAccelThreshold );
  mouseConfig.add( "acceleration.max",       mouseMaxAccel );
  mouseConfig.add( "acceleration.c0",        mouseAccelC0 );
  mouseConfig.add( "acceleration.c1",        mouseAccelC1 );
  mouseConfig.add( "acceleration.c2",        mouseAccelC2 );
  mouseConfig.add( "wheelStep",              mouseWheelStep );

  keyboardConfig.add( "sensitivity.x",       keySensX );
  keyboardConfig.add( "sensitivity.y",       keySensY );
  keyMapConfig = keyMapToJSON();

  if( !inputConfig.save( configFile ) ) {
    OZ_ERROR( "Failed to write '%s'", configFile.path().cstr() );
  }

  Log::printEnd( " OK" );
}

Input input;

}
}
