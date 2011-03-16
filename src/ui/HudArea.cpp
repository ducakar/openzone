/*
 *  HudArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/HudArea.hpp"

#include "matrix/BotClass.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"
#include "client/Shape.hpp"

#include <GL/gl.h>

namespace oz
{
namespace client
{
namespace ui
{

  HudArea::HudArea() : Area( camera.width, camera.height )
  {
    flags |= IGNORE_BIT;
    setFont( Font::TITLE );

    crossTexId = context.loadTexture( "ui/crosshair.ozcTex" );
    useTexId   = context.loadTexture( "ui/use.ozcTex" );
    mountTexId = context.loadTexture( "ui/mount.ozcTex" );
    takeTexId  = context.loadTexture( "ui/take.ozcTex" );
    liftTexId  = context.loadTexture( "ui/lift.ozcTex" );
    grabTexId  = context.loadTexture( "ui/grab.ozcTex" );

    crossIconX = ( width - ICON_SIZE ) / 2;
    crossIconY = ( height - ICON_SIZE ) / 2;
    useIconX   = crossIconX + ICON_SIZE;
    useIconY   = crossIconY;
    mountIconX = crossIconX + ICON_SIZE;
    mountIconY = crossIconY;
    takeIconX  = crossIconX - ICON_SIZE;
    takeIconY  = crossIconY;
    liftIconX  = crossIconX;
    liftIconY  = crossIconY - ICON_SIZE;
    grabIconX  = crossIconX;
    grabIconY  = crossIconY - ICON_SIZE;
    healthBarX = crossIconX - 8;
    healthBarY = crossIconY + ICON_SIZE;
    descTextX  = width / 2;
    descTextY  = crossIconY + ICON_SIZE + 36;
  }

  HudArea::~HudArea()
  {
    glDeleteTextures( 1, &crossTexId );
    glDeleteTextures( 1, &useTexId );
    glDeleteTextures( 1, &mountTexId );
    glDeleteTextures( 1, &takeTexId );
    glDeleteTextures( 1, &liftTexId );
    glDeleteTextures( 1, &grabTexId );
  }

  void HudArea::onDraw()
  {
    if( camera.tagged != -1 && camera.state != Camera::STRATEGIC ) {
      const Object* obj = camera.taggedObj;
      const ObjectClass* clazz = obj->clazz;
      float life = ( obj->flags & Object::BOT_BIT ) ?
          ( obj->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f ) :
          obj->life / clazz->life;
      int lifeWidth = int( life * float( ICON_SIZE + 14 ) );

      life = max( life, 0.0f );

      glUniform4f( param.oz_Colour, 1.0f - life, life, 0.0f, 0.6f );
      fill( healthBarX + 1, healthBarY + 11, lifeWidth, 10 );

      glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 0.8f );
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

      setFontColour( 0x00, 0x00, 0x00 );
      printCentred( descTextX + 1, descTextY - 1, "%s", description.cstr() );
      setFontColour( 0xff, 0xff, 0xff );
      printCentred( descTextX + 0, descTextY + 0, "%s", description.cstr() );
    }

    if( camera.bot != -1 ) {
      const Bot*      bot   = camera.botObj;
      const BotClass* clazz = static_cast<const BotClass*>( camera.botObj->clazz );

      float life         = ( bot->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f );
      int   lifeWidth    = max( int( life * 188.0f ), 0 );
      float stamina      = bot->stamina / clazz->stamina;
      int   staminaWidth = max( int( stamina * 188.0f ), 0 );

      glUniform4f( param.oz_Colour, 1.0f - life, life, 0.0f, 0.6f );
      fill( -199, 35, lifeWidth, 14 );
      glUniform4f( param.oz_Colour, 0.7f - 0.7f * stamina, 0.3f, 0.5f + 0.5f * stamina, 0.6f );
      fill( -199, 11, staminaWidth, 14 );

      glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 0.6f );
      rect( -200, 34, 190, 16 );
      rect( -200, 10, 190, 16 );

      glUniform1i( param.oz_IsTextureEnabled, true );

      if( !camera.isExternal ) {
        glBindTexture( GL_TEXTURE_2D, crossTexId );
        shape.fill( crossIconX, crossIconY, ICON_SIZE, ICON_SIZE );
      }

      if( bot->parent == -1 ) {
        const Object* tagged = camera.taggedObj;

        // it might happen that bot itself is tagged object for a frame when switching from freecam
        // into a bot
        if( tagged != null && ( tagged->flags & Object::SOLID_BIT ) && tagged != bot ) {
          if( tagged->flags & Object::VEHICLE_BIT ) {
            glBindTexture( GL_TEXTURE_2D, mountTexId );
            shape.fill( mountIconX, mountIconY, ICON_SIZE, ICON_SIZE );
          }
          else if( tagged->flags & Object::USE_FUNC_BIT ) {
            glBindTexture( GL_TEXTURE_2D, useTexId );
            shape.fill( useIconX, useIconY, ICON_SIZE, ICON_SIZE );
          }
          if( tagged->flags & Object::ITEM_BIT ) {
            glBindTexture( GL_TEXTURE_2D, takeTexId );
            shape.fill( takeIconX, takeIconY, ICON_SIZE, ICON_SIZE );
          }

          const Dynamic* taggedDyn = static_cast<const Dynamic*>( tagged );
          const Bot* taggedBot = static_cast<const Bot*>( tagged );

          if( bot->grabObj == -1 && bot->weaponItem == -1 && ( tagged->flags & Object::DYNAMIC_BIT ) &&
              // not swimming
              ( ( bot->flags & Object::ON_FLOOR_BIT ) || bot->lower != -1 || bot->depth <= bot->dim.z ) &&
              ( !( tagged->flags & Object::BOT_BIT ) || ( taggedBot->grabObj == -1 ) ) )
          {
            if( taggedDyn->mass <= clazz->grabMass && bot->lower != camera.tagged ) {
              glBindTexture( GL_TEXTURE_2D, liftTexId );
              shape.fill( liftIconX, liftIconY, ICON_SIZE, ICON_SIZE );
            }
          }
          if( camera.botObj->grabObj != -1 ) {
            glBindTexture( GL_TEXTURE_2D, grabTexId );
            shape.fill( grabIconX, grabIconY, ICON_SIZE, ICON_SIZE );
          }
        }
      }
      glUniform1i( param.oz_IsTextureEnabled, false );
    }

    drawChildren();
  }

}
}
}
