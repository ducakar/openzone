/*
 *  HudArea.cc
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hh"

#include "ui/HudArea.hh"

#include "matrix/BotClass.hh"
#include "client/Camera.hh"
#include "client/Context.hh"

#include <SDL_opengl.h>

namespace oz
{
namespace client
{
namespace ui
{

  HudArea::HudArea() : Area( camera.width, camera.height )
  {
    flags |= IGNORE_BIT;
    setFont( TITLE );

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
    healthBarY = crossIconY + ICON_SIZE;
    descTextX  = width / 2;
    descTextY  = crossIconY + ICON_SIZE + 36;
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
    if( camera.tagged != -1 && camera.state != Camera::STRATEGIC ) {
      const Object* obj = camera.taggedObj;
      const ObjectClass* clazz = obj->type;
      float life = ( obj->flags & Object::BOT_BIT ) ?
          ( obj->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f ) :
          obj->life / clazz->life;
      int lifeWidth = int( life * float( ICON_SIZE + 14 ) );

      glColor4f( 1.0f - life, life, 0.0f, 0.6f );
      fill( healthBarX + 1, healthBarY + 11, lifeWidth, 10 );

      glColor4f( 1.0f, 1.0f, 1.0f, 0.6f );
      rect( healthBarX, healthBarY + 10, ICON_SIZE + 16, 12 );

      String description;
      if( obj->flags & Object::BOT_BIT ) {
        const Bot* bot = static_cast<const Bot*>( obj );

        description = bot->name.isEmpty() ?
            clazz->description :
            bot->name + " (" + clazz->description + ")";
      }
      else {
        description = clazz->description;
      }

      setFontColor( 0x00, 0x00, 0x00 );
      printCentered( descTextX + 1, descTextY - 1, "%s", description.cstr() );
      setFontColor( 0xff, 0xff, 0xff );
      printCentered( descTextX + 0, descTextY + 0, "%s", description.cstr() );
    }

    if( camera.bot != -1 ) {
      const BotClass* clazz = static_cast<const BotClass*>( camera.botObj->type );

      float life         = ( camera.botObj->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f );
      int   lifeWidth    = max( int( life * 188.0f ), 0 );
      float stamina      = camera.botObj->stamina / clazz->stamina;
      int   staminaWidth = max( int( stamina * 188.0f ), 0 );

      glColor4f( 1.0f - life, life, 0.0f, 0.6f );
      fill( -199, 35, lifeWidth, 14 );
      glColor4f( 0.7f - 0.7f * stamina, 0.3f, 0.5f + 0.5f * stamina, 0.6f );
      fill( -199, 11, staminaWidth, 14 );

      glColor4f( 1.0f, 1.0f, 1.0f, 0.6f );
      rect( -200, 34, 190, 16 );
      rect( -200, 10, 190, 16 );

      glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
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

      if( camera.botObj->parent == -1 ) {
        if( camera.tagged != -1 ) {
          if( camera.taggedObj->flags & Object::VEHICLE_BIT ) {
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
            if( camera.taggedObj->flags & Object::USE_FUNC_BIT ) {
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
            if( camera.taggedObj->flags & Object::ITEM_BIT ) {
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

        if( camera.botObj->grabObj != -1 ) {
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
    else if( camera.state != Camera::STRATEGIC ) {
      glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
      glEnable( GL_TEXTURE_2D );

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
