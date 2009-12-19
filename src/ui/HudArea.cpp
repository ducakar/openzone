/*
 *  HudArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "HudArea.h"

#include "matrix/BotClass.h"
#include "client/Camera.h"
#include "client/Context.h"

#include <GL/glu.h>

namespace oz
{
namespace client
{
namespace ui
{

  HudArea::HudArea( int width, int height ) : Area( width, height )
  {
    crossTexId = context.loadTexture( "ui/crosshair.png", false, GL_LINEAR, GL_LINEAR );
    useTexId   = context.loadTexture( "ui/use.png", false, GL_LINEAR, GL_LINEAR );
    takeTexId  = context.loadTexture( "ui/take.png", false, GL_LINEAR, GL_LINEAR );
    mountTexId = context.loadTexture( "ui/mount.png", false, GL_LINEAR, GL_LINEAR );
    grabTexId  = context.loadTexture( "ui/grab.png", false, GL_LINEAR, GL_LINEAR );

    crossIconX = ( width - ICON_SIZE ) / 2;
    crossIconY = ( height - ICON_SIZE ) / 2;
    useIconX   = crossIconX + ICON_SIZE;
    useIconY   = crossIconY;
    takeIconX  = crossIconX + ICON_SIZE;
    takeIconY  = crossIconY - ICON_SIZE;
    mountIconX = crossIconX - ICON_SIZE;
    mountIconY = crossIconY;
    grabIconX  = crossIconX;
    grabIconY  = crossIconY - ICON_SIZE;
    healthBarX = crossIconX - 8;
    healthBarY = crossIconY + ICON_SIZE + 8;
  }

  HudArea::~HudArea()
  {
    context.freeTexture( crossTexId );
    context.freeTexture( useTexId );
    context.freeTexture( takeTexId );
    context.freeTexture( mountTexId );
    context.freeTexture( grabTexId );
  }

  void HudArea::onDraw()
  {
    // tagged object pointer
    Object *obj = null;

    if( taggedObj != -1 ) {
      obj = world.objects[taggedObj];

      const ObjectClass *clazz = obj->type;
      float life = ( obj->flags & Object::BOT_BIT ) ?
          ( obj->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f ) :
          obj->life / clazz->life;
      int lifeWidth = life * ( ICON_SIZE + 14 );

      glColor4f( 1.0f - life, life, 0.0f, 0.6f );
      fill( healthBarX + 1, healthBarY + 11, lifeWidth, 10 );

      glColor4f( 1.0f, 1.0f, 1.0f, 0.6f );
      rect( healthBarX, healthBarY + 10, ICON_SIZE + 16, 12 );
    }

    if( camera.botIndex != -1 ) {
      const BotClass *clazz = static_cast<const BotClass*>( camera.bot->type );

      float life         = ( camera.bot->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f );
      int   lifeWidth    = max<int>( life * 188.0f, 0 );
      float stamina      = camera.bot->stamina / clazz->stamina;
      int   staminaWidth = max<int>( stamina * 188.0f, 0 );

      glColor4f( 1.0f - life, life, 0.0f, 0.6f );
      fill( -199, 35, lifeWidth, 14 );
      glColor4f( 0.7f - 0.7f * stamina, 0.3f, 0.5f + 0.5f * stamina, 0.6f );
      fill( -199, 11, staminaWidth, 14 );

      glColor4f( 1.0f, 1.0f, 1.0f, 0.6f );
      rect( -200, 34, 190, 16 );
      rect( -200, 10, 190, 16 );

      glEnable( GL_TEXTURE_2D );

      if( !camera.isExternal ) {
        glBindTexture( GL_TEXTURE_2D, crossTexId );
        glBegin( GL_QUADS );
          glTexCoord2i( 0, 1 );
          glVertex2i( crossIconX, crossIconY );
          glTexCoord2i( 1, 1 );
          glVertex2i( crossIconX + ICON_SIZE, crossIconY );
          glTexCoord2i( 1, 0 );
          glVertex2i( crossIconX + ICON_SIZE, crossIconY + ICON_SIZE );
          glTexCoord2i( 0, 0 );
          glVertex2i( crossIconX, crossIconY + ICON_SIZE );
        glEnd();
      }

      if( camera.bot->parent == -1 ) {
        if( obj != null ) {
          if( obj->flags & Object::VEHICLE_BIT ) {
            glBindTexture( GL_TEXTURE_2D, mountTexId );
            glBegin( GL_QUADS );
              glTexCoord2i( 0, 1 );
              glVertex2i( mountIconX, mountIconY );
              glTexCoord2i( 1, 1 );
              glVertex2i( mountIconX + ICON_SIZE, mountIconY );
              glTexCoord2i( 1, 0 );
              glVertex2i( mountIconX + ICON_SIZE, mountIconY + ICON_SIZE );
              glTexCoord2i( 0, 0 );
              glVertex2i( mountIconX, mountIconY + ICON_SIZE );
            glEnd();
          }
          else {
            if( obj->flags & Object::USE_FUNC_BIT ) {
              glBindTexture( GL_TEXTURE_2D, useTexId );
              glBegin( GL_QUADS );
                glTexCoord2i( 0, 1 );
                glVertex2i( useIconX, useIconY );
                glTexCoord2i( 1, 1 );
                glVertex2i( useIconX + ICON_SIZE, useIconY );
                glTexCoord2i( 1, 0 );
                glVertex2i( useIconX + ICON_SIZE, useIconY + ICON_SIZE );
                glTexCoord2i( 0, 0 );
                glVertex2i( useIconX, useIconY + ICON_SIZE );
              glEnd();
            }
            if( obj->flags & Object::ITEM_BIT ) {
              glBindTexture( GL_TEXTURE_2D, takeTexId );
              glBegin( GL_QUADS );
                glTexCoord2i( 0, 1 );
                glVertex2i( takeIconX, takeIconY );
                glTexCoord2i( 1, 1 );
                glVertex2i( takeIconX + ICON_SIZE, takeIconY );
                glTexCoord2i( 1, 0 );
                glVertex2i( takeIconX + ICON_SIZE, takeIconY + ICON_SIZE );
                glTexCoord2i( 0, 0 );
                glVertex2i( takeIconX, takeIconY + ICON_SIZE );
              glEnd();
            }
          }
        }

        if( camera.bot->grabObj != -1 ) {
          glBindTexture( GL_TEXTURE_2D, grabTexId );
          glBegin( GL_QUADS );
            glTexCoord2i( 0, 1 );
            glVertex2i( grabIconX, grabIconY );
            glTexCoord2i( 1, 1 );
            glVertex2i( grabIconX + ICON_SIZE, grabIconY );
            glTexCoord2i( 1, 0 );
            glVertex2i( grabIconX + ICON_SIZE, grabIconY + ICON_SIZE );
            glTexCoord2i( 0, 0 );
            glVertex2i( grabIconX, grabIconY + ICON_SIZE );
          glEnd();
        }
      }

      glDisable( GL_TEXTURE_2D );
    }
    else {
      glEnable( GL_TEXTURE_2D );

      if( taggedObj == -1 ) {
        glColor4f( 1.0f, 1.0f, 1.0f, 0.6f );
      }

      glBindTexture( GL_TEXTURE_2D, crossTexId );
      glBegin( GL_QUADS );
        glTexCoord2i( 0, 1 );
        glVertex2i( crossIconX, crossIconY );
        glTexCoord2i( 1, 1 );
        glVertex2i( crossIconX + ICON_SIZE, crossIconY );
        glTexCoord2i( 1, 0 );
        glVertex2i( crossIconX + ICON_SIZE, crossIconY + ICON_SIZE );
        glTexCoord2i( 0, 0 );
        glVertex2i( crossIconX, crossIconY + ICON_SIZE );
      glEnd();

      glDisable( GL_TEXTURE_2D );
    }

    drawChildren();
  }

}
}
}
