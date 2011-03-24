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
#include "matrix/VehicleClass.hpp"

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

  void HudArea::drawBotCrosshair()
  {
    const Bot*      bot   = camera.botObj;
    const BotClass* clazz = static_cast<const BotClass*>( camera.botObj->clazz );

    glUniform1i( param.oz_IsTextureEnabled, true );
    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );

    if( !camera.isExternal ) {
      glBindTexture( GL_TEXTURE_2D, crossTexId );
      shape.fill( crossIconX, crossIconY, ICON_SIZE, ICON_SIZE );
    }

    glUniform1i( param.oz_IsTextureEnabled, false );

    if( bot->parent == -1 && camera.tagged != -1 ) {
      const Object* tagged = camera.taggedObj;
      const ObjectClass* taggedClazz = tagged->clazz;

      // it might happen that bot itself is tagged object for a frame when switching from freecam
      // into a bot
      if( tagged != null && ( tagged->flags & Object::SOLID_BIT ) && tagged != bot ) {
        float life = ( tagged->flags & Object::BOT_BIT ) ?
            ( tagged->life - taggedClazz->life / 2.0f ) / ( taggedClazz->life / 2.0f ) :
            tagged->life / taggedClazz->life;
        int lifeWidth = int( life * float( ICON_SIZE + 14 ) );

        life = max( life, 0.0f );

        glUniform4f( param.oz_Colour, 1.0f - life, life, 0.0f, 0.6f );
        fill( healthBarX + 1, healthBarY + 11, lifeWidth, 10 );

        glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 0.8f );
        rect( healthBarX, healthBarY + 10, ICON_SIZE + 16, 12 );

        String description;
        if( tagged->flags & Object::BOT_BIT ) {
          const Bot* bot = static_cast<const Bot*>( tagged );

          description = bot->name.isEmpty() ?
              taggedClazz->description :
              bot->name + " (" + taggedClazz->description + ")";
        }
        else {
          description = taggedClazz->description;
        }

        printCentred( descTextX, descTextY, "%s", description.cstr() );

        glUniform1i( param.oz_IsTextureEnabled, true );
        glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );

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
            // if it is not a bot that is holding us
            ( !( tagged->flags & Object::BOT_BIT ) || ( taggedBot->grabObj == -1 ) ) &&
            taggedDyn->mass <= clazz->grabMass && bot->lower != camera.tagged )
        {
          glBindTexture( GL_TEXTURE_2D, liftTexId );
          shape.fill( liftIconX, liftIconY, ICON_SIZE, ICON_SIZE );
        }
        if( camera.botObj->grabObj != -1 ) {
          glBindTexture( GL_TEXTURE_2D, grabTexId );
          shape.fill( grabIconX, grabIconY, ICON_SIZE, ICON_SIZE );
        }

        glUniform1i( param.oz_IsTextureEnabled, false );
      }
    }
  }

  void HudArea::drawBotStatus()
  {
    const Bot*      bot   = camera.botObj;
    const BotClass* clazz = static_cast<const BotClass*>( camera.botObj->clazz );

    float life         = ( bot->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f );
    int   lifeWidth    = max( int( life * 198.0f ), 0 );
    float stamina      = bot->stamina / clazz->stamina;
    int   staminaWidth = max( int( stamina * 198.0f ), 0 );

    glUniform4f( param.oz_Colour, 1.0f - life, life, 0.0f, 0.6f );
    fill( 9, 31, lifeWidth, 12 );
    glUniform4f( param.oz_Colour, 0.7f - 0.7f * stamina, 0.3f, 0.5f + 0.5f * stamina, 0.6f );
    fill( 9, 9, staminaWidth, 12 );

    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 0.6f );
    rect( 8, 30, 200, 14 );
    rect( 8, 8, 200, 14 );

    if( bot->parent != -1 && orbis.objects[bot->parent] != null ) {
      const Vehicle*      vehicle = static_cast<const Vehicle*>( orbis.objects[bot->parent] );
      const VehicleClass* clazz   = static_cast<const VehicleClass*>( vehicle->clazz );

      float life      = vehicle->life / clazz->life;
      int   lifeWidth = max( int( life * 188.0f ), 0 );

      glUniform4f( param.oz_Colour, 1.0f - life, life, 0.0f, 0.6f );
      fill( -199, 31, lifeWidth, 10 );
//         glUniform4f( param.oz_Colour, 0.7f - 0.7f * stamina, 0.3f, 0.5f + 0.5f * stamina, 0.6f );
//         fill( 11, 11, staminaWidth, 10 );

      glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 0.6f );
      rect( -200, 30, 190, 12 );
//         rect( 10, 10, 190, 12 );
    }
  }

  void HudArea::drawVehicleStatus()
  {}

  void HudArea::onDraw()
  {
    const Bot* bot = camera.botObj;

    if( camera.state == Camera::BOT && camera.bot != -1 ) {
      hard_assert( bot != null );

      drawBotCrosshair();
      drawBotStatus();

      if( bot->parent != -1 ) {
        drawVehicleStatus();
      }
    }

    drawChildren();
  }

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

}
}
}
