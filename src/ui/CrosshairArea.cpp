/*
 *  CrosshairArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "CrosshairArea.h"

#include "client/Context.h"
#include "client/Camera.h"

namespace oz
{
namespace client
{
namespace ui
{

  CrosshairArea::CrosshairArea( int size ) : Area( size, size )
  {
    dim = size / 3;
    crossTexId = context.loadTexture( "ui/crosshair.png", false, GL_LINEAR, GL_LINEAR );
    grabTexId = context.loadTexture( "ui/grab.png", false, GL_LINEAR, GL_LINEAR );
  }

  CrosshairArea::~CrosshairArea()
  {
    context.freeTexture( crossTexId );
    context.freeTexture( grabTexId );
  }

  void CrosshairArea::draw()
  {
    if( !mouse.doShow && !camera.isThirdPerson ) {
      glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
      glEnable( GL_TEXTURE_2D );

      glBindTexture( GL_TEXTURE_2D, crossTexId );
      glBegin( GL_QUADS );
        glTexCoord2i( 0, 1 );
        glVertex2i( x +   dim, y +   dim );
        glTexCoord2i( 1, 1 );
        glVertex2i( x + 2*dim, y +   dim );
        glTexCoord2i( 1, 0 );
        glVertex2i( x + 2*dim, y + 2*dim );
        glTexCoord2i( 0, 0 );
        glVertex2i( x +   dim, y + 2*dim );
      glEnd();

      if( camera.bot != null && camera.bot->grabObjIndex >= 0 ) {
        glBindTexture( GL_TEXTURE_2D, grabTexId );
        glBegin( GL_QUADS );
          glTexCoord2i( 0, 1 );
          glVertex2i( x +   dim, y );
          glTexCoord2i( 1, 1 );
          glVertex2i( x + 2*dim, y );
          glTexCoord2i( 1, 0 );
          glVertex2i( x + 2*dim, y + dim );
          glTexCoord2i( 0, 0 );
          glVertex2i( x +   dim, y + dim );
        glEnd();
      }

      glDisable( GL_TEXTURE_2D );
    }
  }

}
}
}
