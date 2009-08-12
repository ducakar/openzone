/*
 *  Mouse.h
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
namespace ui
{

  class Mouse
  {
    private:

      static const int SIZE = 32;

      uint ptrTexId;
      int  maxX;
      int  maxY;

    public:

      int  x;
      int  y;
      byte buttons;
      bool doShow;

      void init( int maxX, int maxY );
      void free();

      void setBounds( int maxX, int maxY );
      void show();
      void hide();

      void update( int moveX, int moveY, byte buttons );
      void draw() const;

  };

  extern Mouse mouse;

}
}
}
