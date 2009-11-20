/*
 *  Client.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
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

    mouseXSens = config.getSet( "input.mouse.xSens", 0.2f );
    mouseYSens = config.getSet( "input.mouse.ySens", 0.2f );
    keyXSens   = config.getSet( "input.keys.xSens", 0.2f );
    keyYSens   = config.getSet( "input.keys.ySens", 0.2f );

    matrix.init();
    nirvana::nirvana.init();

    network.connect();

    Buffer buffer( 1024 * 1024 * 10 );
    String stateFile = config.get( "dir.rc", "" ) + String( "/default.ozState" );

    log.print( "Loading world stream from %s ...", stateFile.cstr() );
    if( buffer.load( stateFile ) ) {
      log.printEnd( " OK" );

      InputStream istream = buffer.inputStream();

      matrix.load( &istream );
      nirvana::nirvana.load( &istream );
    }
    else {
      log.printEnd( " Failed, starting a new world" );

      matrix.load( null );
      nirvana::nirvana.load( null );
    }

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
    synapse.commit();
    synapse.doDeletes();
    synapse.clear();

    Buffer buffer( 1024 * 1024 * 10 );
    OutputStream ostream = buffer.outputStream();
    String stateFile = config.get( "dir.rc", "" ) + String( "/default.ozState" );

    matrix.unload( &ostream );
    nirvana::nirvana.unload( &ostream );

    log.print( "Writing world stream to %s ...", stateFile.cstr() );
    buffer.write( stateFile );
    log.printEnd( " OK" );

    network.disconnect();

    nirvana::nirvana.free();
    matrix.free();

    log.unindent();
    log.println( "}" );
  }

  void Game::start() const
  {
    nirvana::nirvana.start();

    camera.p = Vec3( 52, -49, 40 );
  }

  void Game::stop() const
  {
    nirvana::nirvana.stop();
  }

  bool Game::update( int time )
  {
    // wait until nirvana thread has stopped
    SDL_SemWait( matrix.semaphore );
    assert( SDL_SemValue( matrix.semaphore ) == 0 );

    synapse.clear();

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
      camera.bot->actions = 0;

      /*
       * Camera
       */
      if( camera.isExternal && camera.isFreeLook ) {
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
      }
      else {
        camera.bot->h -= ui::mouse.overEdgeX * mouseXSens;
        camera.bot->v += ui::mouse.overEdgeY * mouseYSens;

        camera.h = camera.bot->h;
        camera.v = camera.bot->v;

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
        camera.bot->actions |= Bot::ACTION_FORWARD;
      }
      if( input.keys[SDLK_s] ) {
        camera.bot->actions |= Bot::ACTION_BACKWARD;
      }
      if( input.keys[SDLK_d] ) {
        camera.bot->actions |= Bot::ACTION_RIGHT;
      }
      if( input.keys[SDLK_a] ) {
        camera.bot->actions |= Bot::ACTION_LEFT;
      }

      /*
       * Actions
       */
      if( input.keys[SDLK_SPACE] ) {
        camera.bot->actions |= Bot::ACTION_JUMP;
      }
      if( input.keys[SDLK_LCTRL] ) {
        camera.bot->actions |= Bot::ACTION_CROUCH;
      }
      if( input.keys[SDLK_LSHIFT] && !input.oldKeys[SDLK_LSHIFT] ) {
        camera.bot->state ^= Bot::RUNNING_BIT;
      }
      if( input.keys[SDLK_z] ) {
        camera.bot->actions |= Bot::ACTION_EXIT;
      }
      if( input.keys[SDLK_x] ) {
        camera.bot->actions |= Bot::ACTION_EJECT;
      }
      if( camera.isExternal && input.keys[SDLK_LALT] && !input.oldKeys[SDLK_LALT] ) {
        camera.isFreeLook = !camera.isFreeLook;
      }
      if( input.keys[SDLK_p] && !input.oldKeys[SDLK_p] ) {
        camera.bot->state ^= Bot::STEPPING_BIT;
      }

      camera.bot->state &= ~( Bot::GESTURE0_BIT | Bot::GESTURE1_BIT | Bot::GESTURE2_BIT | Bot::GESTURE3_BIT | Bot::GESTURE4_BIT | Bot::GESTURE_ALL_BIT );
      if( input.keys[SDLK_f] ) {
        camera.bot->state |= Bot::GESTURE0_BIT;
      }
      if( input.keys[SDLK_g] ) {
        camera.bot->state |= Bot::GESTURE1_BIT;
      }
      if( input.keys[SDLK_h] ) {
        camera.bot->state |= Bot::GESTURE2_BIT;
      }
      if( input.keys[SDLK_j] ) {
        camera.bot->state |= Bot::GESTURE3_BIT;
      }
      if( input.keys[SDLK_k] ) {
        camera.bot->state |= Bot::GESTURE4_BIT;
      }
      if( input.keys[SDLK_l] ) {
        camera.bot->state |= Bot::GESTURE_ALL_BIT;
      }

      if( camera.botIndex >= 0 && input.keys[SDLK_m] && !input.oldKeys[SDLK_m] ) {
        camera.isExternal = !camera.isExternal;
        camera.h = camera.bot->h;
        camera.v = camera.bot->v;
      }

      if( state == GAME ) {
        if( ui::mouse.rightClick ) {
          camera.bot->actions |= Bot::ACTION_USE;
        }
        if( ui::mouse.wheelDown ) {
//          camera.bot->keys |= Bot::KEY_TAKE;
        }
        if( ui::mouse.wheelUp ) {
          camera.bot->actions |= Bot::ACTION_THROW;
        }
        if( ui::mouse.middleClick ) {
          camera.bot->actions |= Bot::ACTION_GRAB;
        }
      }
    }

    if( input.keys[SDLK_i] && !input.oldKeys[SDLK_i] ) {
      if( camera.botIndex < 0 ) {
        Bot *me = static_cast<Bot*>( translator.createObject( "Lord", camera.p ) );
        me->h = camera.h;
        me->v = camera.v;
        me->state |= Bot::PLAYER_BIT;

        camera.botIndex = synapse.put( me );
      }
      else {
        camera.h = camera.bot->h;
        camera.v = camera.bot->v;
        camera.botIndex = -1;
        camera.bot->kill();
      }
    }
    if( state == GAME_INTERFACE ) {
      // interface
      ui::update();
    }

    synapse.clearTickets();

    network.update();

    // remove/cut objects scheduled for removal
    synapse.commit();

    // delete models/audios of removed objects
    render.sync();
    sound.sync();

    // we can finally delete removed object after render and sound are sync'd as model/audio dtors
    // have references to the objects
    synapse.doDeletes();

    // update world
    matrix.update();

    // resume nirvana
    SDL_SemPost( nirvana::nirvana.semaphore );

    camera.update();

    return !input.keys[SDLK_ESCAPE];
  }

}
}
