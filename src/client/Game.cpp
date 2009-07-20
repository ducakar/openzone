/*
 *  Client.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Game.h"

#include "matrix/Timer.h"
#include "matrix/Bot.h"
#include "matrix/BotClass.h"
#include "matrix/Matrix.h"
#include "nirvana/Nirvana.h"
#include "Network.h"

#include "Camera.h"

namespace oz
{
namespace client
{

  Game game;

  const float Game::FREECAM_SLOW_SPEED = 0.04f;
  const float Game::FREECAM_FAST_SPEED = 0.40f;

  bool Game::init()
  {
    logFile.println( "Initializing Game {" );
    logFile.indent();

    mouseXSens = config.get( "input.mouse.xSens", 0.2f );
    mouseYSens = config.get( "input.mouse.ySens", 0.2f );
    keyXSens   = config.get( "input.keys.xSens", 0.2f );
    keyYSens   = config.get( "input.keys.ySens", 0.2f );

    network.connect();
    matrix.load();
    nirvana.load();

    camera.botIndex = -1;
    camera.bot = null;
    camera.p = Vec3( 40, -50, 80 );

    logFile.unindent();
    logFile.println( "}" );
    return true;
  }

  void Game::start()
  {
    nirvana.start();
  }

  bool Game::update( int time )
  {
    nirvana.requestSuspend = true;

    if( camera.bot == null ) {
      /*
       * Camera
       */
      camera.h -= input.mouse.x * mouseXSens;
      camera.v -= input.mouse.y * mouseYSens;

      if( input.keys[SDLK_UP] ) {
        camera.v += keyXSens * time;
      }
      if( input.keys[SDLK_DOWN] ) {
        camera.v -= keyXSens * time;
      }
      if( input.keys[SDLK_RIGHT] ) {
        camera.h -= keyYSens * time;
      }
      if( input.keys[SDLK_LEFT] ) {
        camera.h += keyYSens * time;
      }

      camera.update();

      /*
       * Movement
       */
      float speed = input.keys[SDLK_LSHIFT] ? FREECAM_FAST_SPEED : FREECAM_SLOW_SPEED;

      if( input.keys[SDLK_w] ) {
        camera.p += camera.at * speed;
      }
      if( input.keys[SDLK_s] ) {
        camera.p -= camera.at * speed;
      }
      if( input.keys[SDLK_d] ) {
        camera.p += camera.right * speed;
      }
      if( input.keys[SDLK_a] ) {
        camera.p -= camera.right * speed;
      }
      if( input.keys[SDLK_SPACE] ) {
        camera.p.z += speed;
      }
      if( input.keys[SDLK_LCTRL] ) {
        camera.p.z -= speed;
      }
    }
    else {
      /*
       * Camera
       */
      if( camera.bot->state & Bot::FREELOOK_BIT ) {
        camera.h -= input.mouse.x * mouseXSens;
        camera.v -= input.mouse.y * mouseYSens;

        BotClass *clazz = (BotClass*) camera.bot->type;
        camera.h = bound( camera.h, clazz->lookLimitHMin, clazz->lookLimitHMax );
        camera.v = bound( camera.v, clazz->lookLimitVMin, clazz->lookLimitVMax );

        if( input.keys[SDLK_UP] ) {
          camera.v += keyXSens * time;
        }
        if( input.keys[SDLK_DOWN] ) {
          camera.v -= keyXSens * time;
        }
        if( input.keys[SDLK_RIGHT] ) {
          camera.h -= keyYSens * time;
        }
        if( input.keys[SDLK_LEFT] ) {
          camera.h += keyYSens * time;
        }
      }
      else {
        camera.h = 0.0f;
        camera.v = 0.0f;

        camera.bot->h -= input.mouse.x * mouseXSens;
        camera.bot->v -= input.mouse.y * mouseYSens;

        if( input.keys[SDLK_UP] ) {
          camera.bot->v += keyXSens * time;
        }
        if( input.keys[SDLK_DOWN] ) {
          camera.bot->v -= keyXSens * time;
        }
        if( input.keys[SDLK_RIGHT] ) {
          camera.bot->h -= keyYSens * time;
        }
        if( input.keys[SDLK_LEFT] ) {
          camera.bot->h += keyYSens * time;
        }
      }

      /*
       * Movement
       */
      if( input.keys[SDLK_w] ) {
        camera.bot->keys |= Bot::KEY_FORWARD;
      }
      if( input.keys[SDLK_s] ) {
        camera.bot->keys |= Bot::KEY_BACKWARD;
      }
      if( input.keys[SDLK_d] ) {
        camera.bot->keys |= Bot::KEY_RIGHT;
      }
      if( input.keys[SDLK_a] ) {
        camera.bot->keys |= Bot::KEY_LEFT;
      }

      /*
       * Actions
       */
      if( input.keys[SDLK_SPACE] ) {
        camera.bot->keys |= Bot::KEY_JUMP;
      }
      if( input.keys[SDLK_LCTRL] ) {
        camera.bot->keys |= Bot::KEY_CROUCH;
      }
      if( input.keys[SDLK_LSHIFT] ) {
        camera.bot->keys |= Bot::KEY_RUN;
      }
      if( input.keys[SDLK_LALT] ) {
        camera.bot->keys |= Bot::KEY_FREELOOK;
      }

      if( input.keys[SDLK_g] ) {
        camera.bot->keys |= Bot::KEY_GESTURE0;
      }
      if( input.keys[SDLK_h] ) {
        camera.bot->keys |= Bot::KEY_GESTURE1;
      }
      if( input.keys[SDLK_p] ) {
        camera.bot->keys |= Bot::KEY_STEP;
      }
      if( input.keys[SDLK_l] ) {
        camera.bot->keys |= Bot::KEY_NOCLIP;
      }

      if( input.mouse.b & SDL_BUTTON_LEFT ) {
        camera.bot->keys |= Bot::KEY_FIRE;
      }
      if( input.keys[SDLK_f] || ( input.mouse.b == SDL_BUTTON_RIGHT ) ) {
        camera.bot->keys |= Bot::KEY_USE;
      }
    }

    if( input.mouse.b & SDL_BUTTON_LEFT ) {
      camera.botIndex = ~camera.botIndex;
    }

    SDL_SemWait( matrix.semaphore );

    timer.update( time );
    network.update();
    matrix.update();

    camera.update();

    SDL_SemPost( nirvana.semaphore );

    return !input.keys[SDLK_ESCAPE];
  }

  void Game::stop()
  {
    nirvana.stop();
  }

  void Game::free()
  {
    logFile.println( "Shutting down Game {" );
    logFile.indent();

    network.disconnect();
    nirvana.free();
    matrix.free();

    logFile.unindent();
    logFile.println( "}" );
  }

}
}
