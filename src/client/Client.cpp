/*
 *  Client.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "Client.hpp"

#include "matrix/Timer.hpp"
#include "matrix/Bot.hpp"
#include "matrix/Matrix.hpp"
#include "nirvana/Nirvana.hpp"

#include "Camera.hpp"

namespace oz
{
namespace client
{

  Input input;
  Client client;

  bool Client::init()
  {
    logFile.println( "Setting game variables" );
    sscanf( config["input.mouse.xSens"], "%f", &settings.mouseXSens );
    sscanf( config["input.mouse.ySens"], "%f", &settings.mouseYSens );
    sscanf( config["input.keys.xSens"], "%f", &settings.keyXSens );
    sscanf( config["input.keys.ySens"], "%f", &settings.keyYSens );

    matrix.load();
    nirvana.load();

    return true;
  }

  void Client::start()
  {
    logFile.print( "Starting Nirvana thread ..." );
    nirvana.start();
    logFile.printRaw( " OK\n" );
  }

  bool Client::update( int time )
  {
    nirvana.requestSuspend = true;

    camera.player = (Bot*) world.objects[0];

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
    logFile.printRaw( " OK\n" );
  }

  void Client::free()
  {
    logFile.print( "Shutting down Nirvana ..." );
    logFile.indent();

    nirvana.free();

    logFile.unindent();
    logFile.printRaw( " OK\n" );

    logFile.print( "Shutting down Matrix ..." );
    logFile.indent();

    matrix.free();

    logFile.unindent();
    logFile.printRaw( " OK\n" );
  }

}
}
