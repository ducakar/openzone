/*
 *  ui.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "ui.h"

namespace oz
{
namespace client
{
namespace ui
{

  Area root;

  void draw()
  {
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, root.width, 0.0, root.height, 0.0, 1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    root.draw();
  }

  void init( int screenX, int screenY )
  {
    root = Area( 0, 0, screenX, screenY );
    font.init();
  }

  void free()
  {
    font.free();
  }

}
}
}
