/*
 *  Game.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{
namespace server
{

  class Game
  {
    public:

      bool init();
      void start();

      void update();

      void stop();
      void free();

  };

  extern Game game;

}
}
