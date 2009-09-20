/*
 *  Client.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{
namespace client
{

  class Game
  {
    public:

      struct Input
      {
        ubyte *keys;
        ubyte oldKeys[SDLK_LAST];
      };

      enum State
      {
        GAME,
        GAME_INTERFACE,
        MENU
      };

    private:

      static const float FREECAM_SLOW_SPEED;
      static const float FREECAM_FAST_SPEED;

      float mouseXSens;
      float mouseYSens;

      float keyXSens;
      float keyYSens;

      float moveStep;
      float runStep;

      bool  fastMove;

    public:

      Input input;
      State state;

      bool init();
      void free() const;

      void start() const;
      void stop() const;

      bool update( int time );

  };

  extern Game game;

}
}
