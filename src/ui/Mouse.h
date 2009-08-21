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

      uint ptrTexId;
      int  size;
      int  hotspotX;
      int  hotspotY;

      int  maxX;
      int  maxY;

    public:

      // read-write
      int  moveX;
      int  moveY;
      byte newButtons;

      // read-only
      int  x;
      int  y;
      byte b;

      int  overEdgeX;
      int  overEdgeY;

      bool leftClick;
      bool rightClick;
      bool middleClick;
      bool wheelUp;
      bool wheelDown;

      bool doShow;

      void init( int maxX, int maxY );
      void free();

      void setBounds( int maxX, int maxY );
      void show();
      void hide();

      void update();
      void draw() const;

  };

  extern Mouse mouse;

}
}
}
