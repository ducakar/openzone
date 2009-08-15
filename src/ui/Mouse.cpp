/*
 *  Mouse.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Mouse.h"

#include "client/Context.h"

namespace oz
{
namespace client
{
namespace ui
{

  Mouse mouse;

  void Mouse::init( int maxX, int maxY )
  {
    doShow = false;
    setBounds( maxX, maxY );

    ptrTexId = context.loadTexture( "base/left_ptr.png",
                                    false, GL_NEAREST, GL_NEAREST );
  }

  void Mouse::free()
  {
    context.freeTexture( ptrTexId );
  }

  void Mouse::setBounds( int maxX_, int maxY_ )
  {
    maxX = maxX_;
    maxY = maxY_;
  }

  void Mouse::show()
  {
    doShow = true;
    x = maxX / 2;
    y = maxY / 2;
  }

  void Mouse::hide()
  {
    doShow = false;
  }

  void Mouse::update( int moveX, int moveY, byte buttons_ )
  {
    x += moveX;
    y += moveY;

    x = bound( x, 0, maxX - 1 );
    y = bound( y, 0, maxY - 1 );

    buttons = buttons_;
  }

  void Mouse::draw() const
  {
    if( doShow ) {
      glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, ptrTexId );
      glBegin( GL_QUADS );
        glTexCoord2i( 0, 1 );
        glVertex2i( x       , y + 1 - SIZE );
        glTexCoord2i( 1, 1 );
        glVertex2i( x + SIZE, y + 1 - SIZE );
        glTexCoord2i( 1, 0 );
        glVertex2i( x + SIZE, y + 1        );
        glTexCoord2i( 0, 0 );
        glVertex2i( x       , y + 1        );
      glEnd();
      glDisable( GL_TEXTURE_2D );
    }
  }

}
}
}
