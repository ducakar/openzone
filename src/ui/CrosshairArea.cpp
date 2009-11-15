/*
 *  CrosshairArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "CrosshairArea.h"

#include "client/Context.h"
#include "client/Camera.h"

#include <GL/gl.h>

namespace oz
{
namespace client
{
namespace ui
{

  CrosshairArea::CrosshairArea( int size ) : Area( size, size ), dim( size / 3 ),
      showUse( false ), showMount( false )
  {
    crossTexId = context.loadTexture( "ui/crosshair.png", false, GL_LINEAR, GL_LINEAR );
    useTexId   = context.loadTexture( "ui/use.png", false, GL_LINEAR, GL_LINEAR );
    mountTexId = context.loadTexture( "ui/mount.png", false, GL_LINEAR, GL_LINEAR );
    grabTexId  = context.loadTexture( "ui/grab.png", false, GL_LINEAR, GL_LINEAR );
  }

  CrosshairArea::~CrosshairArea()
  {
    context.freeTexture( crossTexId );
    context.freeTexture( useTexId );
    context.freeTexture( mountTexId );
    context.freeTexture( grabTexId );
  }

  void CrosshairArea::draw()
  {
    if( !mouse.doShow ) {
      glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
      glEnable( GL_TEXTURE_2D );

      if( !camera.isThirdPerson ) {
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
      }

      if( showUse ) {
        glBindTexture( GL_TEXTURE_2D, useTexId );
        glBegin( GL_QUADS );
          glTexCoord2i( 0, 1 );
          glVertex2i( x + 2*dim, y +   dim );
          glTexCoord2i( 1, 1 );
          glVertex2i( x + 3*dim, y +   dim );
          glTexCoord2i( 1, 0 );
          glVertex2i( x + 3*dim, y + 2*dim );
          glTexCoord2i( 0, 0 );
          glVertex2i( x + 2*dim, y + 2*dim );
        glEnd();
      }

      if( showMount ) {
        glBindTexture( GL_TEXTURE_2D, mountTexId );
        glBegin( GL_QUADS );
          glTexCoord2i( 0, 1 );
          glVertex2i( x        , y +   dim );
          glTexCoord2i( 1, 1 );
          glVertex2i( x +   dim, y +   dim );
          glTexCoord2i( 1, 0 );
          glVertex2i( x +   dim, y + 2*dim );
          glTexCoord2i( 0, 0 );
          glVertex2i( x        , y + 2*dim );
        glEnd();
      }

      if( camera.bot != null && camera.bot->grabObjIndex >= 0 ) {
        glBindTexture( GL_TEXTURE_2D, grabTexId );
        glBegin( GL_QUADS );
          glTexCoord2i( 0, 1 );
          glVertex2i( x +   dim, y         );
          glTexCoord2i( 1, 1 );
          glVertex2i( x + 2*dim, y         );
          glTexCoord2i( 1, 0 );
          glVertex2i( x + 2*dim, y +   dim );
          glTexCoord2i( 0, 0 );
          glVertex2i( x +   dim, y +   dim );
        glEnd();
      }

      glDisable( GL_TEXTURE_2D );
    }
  }

}
}
}
