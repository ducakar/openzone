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
#include "Render.h"
#include "Sound.h"

#include "Camera.h"

namespace oz
{
namespace client
{

  Game game;

  const float Game::FREECAM_SLOW_SPEED = 0.05f;
  const float Game::FREECAM_FAST_SPEED = 0.50f;

  bool Game::init()
  {
    fastMove = false;

    log.println( "Initializing Game {" );
    log.indent();

    mouseXSens = config.get( "input.mouse.xSens", 0.2f );
    mouseYSens = config.get( "input.mouse.ySens", 0.2f );
    keyXSens   = config.get( "input.keys.xSens", 0.2f );
    keyYSens   = config.get( "input.keys.ySens", 0.2f );

    network.connect();
    matrix.load();
    nirvana.load();

    camera.botIndex = -1;
    camera.bot = null;
    camera.p = Vec3( 50, -50, 30 );

    log.unindent();
    log.println( "}" );
    return true;
  }

  void Game::free() const
  {
    log.println( "Shutting down Game {" );
    log.indent();

    // remove myself
    if( camera.botIndex >= 0 ) {
      synapse.remove( camera.bot );
    }
    synapse.commitAll();

    network.disconnect();
    nirvana.free();
    matrix.free();

    log.unindent();
    log.println( "}" );
  }

  void Game::start() const
  {
    nirvana.start();
  }

  void Game::stop() const
  {
    nirvana.stop();
  }

  bool Game::update( int time )
  {
    timer.update( time );

    nirvana.requestSuspend = true;

    if( input.keys[SDLK_TAB] && !input.oldKeys[SDLK_TAB] ) {
      if( state == GAME ) {
        state = GAME_INTERFACE;
        ui::mouse.show();
      }
      else {
        state = GAME;
        ui::mouse.hide();
      }
    }

    if( camera.bot == null ) {
      /*
       * Camera
       */
      camera.h -= ui::mouse.overEdgeX * mouseXSens;
      camera.v += ui::mouse.overEdgeY * mouseYSens;

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
      if( input.keys[SDLK_LSHIFT] && !input.oldKeys[SDLK_LSHIFT] ) {
        fastMove = !fastMove;
      }

      float speed = fastMove ? FREECAM_FAST_SPEED : FREECAM_SLOW_SPEED;

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
      camera.bot->keys = 0;

      /*
       * Camera
       */
      if( camera.bot->state & Bot::FREELOOK_BIT ) {
        camera.h -= ui::mouse.overEdgeX * mouseXSens;
        camera.v += ui::mouse.overEdgeY * mouseYSens;

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

        camera.bot->h -= ui::mouse.overEdgeX * mouseXSens;
        camera.bot->v += ui::mouse.overEdgeY * mouseYSens;

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
      if( input.keys[SDLK_m] && !input.oldKeys[SDLK_m] ) {
        camera.isThirdPerson = !camera.isThirdPerson;
      }

      if( ui::mouse.rightClick ) {
        camera.bot->keys |= Bot::KEY_USE;
      }
      if( ui::mouse.rightClick ) {
        camera.bot->keys |= Bot::KEY_GRAB;
      }
    }

    if( input.keys[SDLK_i] && !input.oldKeys[SDLK_i] ) {
      if( camera.botIndex < 0 ) {
        Bot *me = (Bot*) translator.createObject( "Lord", camera.p );
        me->h = camera.h;
        me->v = camera.v;

        camera.botIndex = synapse.put( me );
      }
      else {
        camera.h = camera.bot->h;
        camera.v = camera.bot->v;
        camera.botIndex = -1;
        synapse.remove( camera.bot );
      }
    }
    if( state == GAME_INTERFACE ) {
      // interface
      ui::update();
    }

    synapse.clearTickets();

    SDL_SemWait( matrix.semaphore );

    network.update();
    matrix.update();
    synapse.commitPlus();

    return !input.keys[SDLK_ESCAPE];
  }

  void Game::sync() const
  {
    render.sync();
    sound.sync();

    synapse.commitMinus();
    synapse.clearPending();

    nirvana.sync();
    nirvana.requestSuspend = false;
    SDL_SemPost( nirvana.semaphore );

    camera.update();
  }

}
}
