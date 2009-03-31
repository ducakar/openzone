/*
 *  Client.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Client.h"

#include "matrix/Timer.h"
#include "matrix/Bot.h"
#include "matrix/Matrix.h"
#include "nirvana/Nirvana.h"

#include "Camera.h"

namespace oz
{
namespace client
{

  Input input;
  Client client;

  bool Client::init()
  {
    settings.mouseXSens = config.get( "input.mouse.xSens", 0.2f );
    settings.mouseYSens = config.get( "input.mouse.ySens", 0.2f );
    settings.keyXSens   = config.get( "input.keys.xSens", 0.2f );
    settings.keyYSens   = config.get( "input.keys.ySens", 0.2f );

    matrix.load();
    nirvana.load();

    camera.player = (Bot*) world.objects[0];
    camera.oldP = camera.player->p;

    return true;
  }

  void Client::start()
  {
    logFile.print( "Starting Nirvana thread ..." );
    nirvana.start();
    logFile.printEnd( " OK" );
  }

  bool Client::update( int time )
  {
    nirvana.requestSuspend = true;

    camera.player->h -= input.mouse.x * settings.mouseXSens;
    camera.player->v -= input.mouse.y * settings.mouseYSens;

    if( input.keys[SDLK_UP] ) {
      camera.player->v += settings.keyXSens * time;
    }
    if( input.keys[SDLK_DOWN] ) {
      camera.player->v -= settings.keyXSens * time;
    }
    if( input.keys[SDLK_RIGHT] ) {
      camera.player->h -= settings.keyYSens * time;
    }
    if( input.keys[SDLK_LEFT] ) {
      camera.player->h += settings.keyYSens * time;
    }

    if( input.keys[SDLK_w] ) {
      camera.player->keys |= Bot::KEY_FORWARD;
    }
    if( input.keys[SDLK_s] ) {
      camera.player->keys |= Bot::KEY_BACKWARD;
    }
    if( input.keys[SDLK_d] ) {
      camera.player->keys |= Bot::KEY_RIGHT;
    }
    if( input.keys[SDLK_a] ) {
      camera.player->keys |= Bot::KEY_LEFT;
    }

    if( input.keys[SDLK_SPACE] ) {
      camera.player->keys |= Bot::KEY_JUMP;
    }
    if( input.keys[SDLK_LCTRL] ) {
      camera.player->keys |= Bot::KEY_CROUCH;
    }
    if( input.keys[SDLK_LALT] ) {
      camera.player->keys |= Bot::KEY_RUN;
    }

    if( input.keys[SDLK_l] ) {
      camera.player->keys |= Bot::KEY_NOCLIP;
    }

    if( input.keys[SDLK_g] ) {
      camera.player->keys |= Bot::KEY_GESTURE0;
    }
    if( input.keys[SDLK_h] ) {
      camera.player->keys |= Bot::KEY_GESTURE1;
    }
    if( input.keys[SDLK_LSHIFT] ) {
      camera.player->keys |= Bot::KEY_STEP;
    }

    if( ( input.mouse.b == SDL_BUTTON_LEFT ) ) {
      camera.player->keys |= Bot::KEY_FIRE;
    }
    if( input.keys[SDLK_f] || ( input.mouse.b == SDL_BUTTON_RIGHT ) ) {
      camera.player->keys |= Bot::KEY_USE;
    }

    SDL_SemWait( matrix.semaphore );

    timer.update( time );
    matrix.update();

    SDL_SemPost( nirvana.semaphore );

    camera.update();

    return !input.keys[SDLK_ESCAPE];
  }

  void Client::stop()
  {
    logFile.print( "Stopping Nirvana thread ..." );
    nirvana.stop();
    logFile.printEnd( " OK" );
  }

  void Client::free()
  {
    logFile.print( "Shutting down Nirvana ..." );
    logFile.indent();

    nirvana.free();

    logFile.unindent();
    logFile.printEnd( " OK" );

    logFile.print( "Shutting down Matrix ..." );
    logFile.indent();

    matrix.free();

    logFile.unindent();
    logFile.printEnd( " OK" );
  }

}
}
