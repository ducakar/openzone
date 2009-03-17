/*
 *  Skybox.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#define SKY_DIST   1.0f

namespace oz
{
namespace client
{

  class Skybox
  {
    private:

      uint list;

      void drawUpper( uint top, uint front, uint left, uint back, uint right );

    public:

      void init( uint top, uint front, uint left, uint back, uint right );
      void init( uint top, uint front, uint left, uint back, uint right, uint bottom );
      void draw();

  };

}
}
