/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/modules/GalileoFrame.cpp
 */

#include "stable.hpp"

#include "client/modules/GalileoFrame.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"
#include "client/OpenGL.hpp"

#include "client/modules/GalileoModule.hpp"
#include "client/modules/QuestModule.hpp"

namespace oz
{
namespace client
{
namespace ui
{

uint GalileoFrame::loadTexture( const char* path ) const
{
  File file( path );
  if( !file.map() ) {
    throw Exception( "Failed reading galileo texture '%s'", path );
  }

  InputStream istream = file.inputStream();
  uint texId = context.readTexture( &istream );

  file.unmap();
  return texId;
}

bool GalileoFrame::onMouseEvent()
{
  return isVisible ? Frame::onMouseEvent() : false;
}

void GalileoFrame::onDraw()
{
  if( camera.state == Camera::BOT && camera.botObj != null ) {
    const Vector<int>& items = camera.botObj->items;

    for( int i = 0; i < items.length(); ++i ) {
      const Object* item = orbis.objects[ items[i] ];

      if( item != null && item->clazz->name.equals( "galileo" ) ) {
        goto galileoEnabled;
      }
    }

    isVisible = false;
    return;
  }

  galileoEnabled:;

  isVisible = true;

  if( mapTexId == 0 ) {
    mapTexId = loadTexture( "ui/galileo/" + library.terras[orbis.terra.id].name + ".ozcTex" );
  }

  float pX;
  float pY;
  float h;

  if( camera.state == Camera::BOT && camera.botObj != null ) {
    pX = camera.botObj->p.x;
    pY = camera.botObj->p.y;
    h  = camera.botObj->h;
  }
  else {
    pX = camera.p.x;
    pY = camera.p.y;
    h  = camera.h;
  }

  glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 0.6f );
  glBindTexture( GL_TEXTURE_2D, mapTexId );
  fill( 0, 0, width, height );

  glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );

  float oX      = float( x );
  float oY      = float( y );
  float fWidth  = float( width );
  float fHeight = float( height );

  const QuestFrame* questFrame = questModule.questFrame;
  if( questFrame != null && questFrame->currentQuest != -1 ) {
    const Quest& quest = questModule.quests[questFrame->currentQuest];

    glBindTexture( GL_TEXTURE_2D, markerTexId );

    float mapX = oX + ( Orbis::DIM + quest.place.x ) / ( 2.0f*Orbis::DIM ) * fWidth;
    float mapY = oY + ( Orbis::DIM + quest.place.y ) / ( 2.0f*Orbis::DIM ) * fHeight;

    tf.model = Mat44::translation( Vec3( mapX, mapY, 0.0f ) );
    tf.model.scale( Vec3( 16.0f, 16.0f, 0.0f ) );
    tf.model.translate( Vec3( -0.5f, -0.5f, 0.0f ) );
    tf.apply();

    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
  }

  glBindTexture( GL_TEXTURE_2D, arrowTexId );

  float mapX = oX + ( Orbis::DIM + pX ) / ( 2.0f*Orbis::DIM ) * fWidth;
  float mapY = oY + ( Orbis::DIM + pY ) / ( 2.0f*Orbis::DIM ) * fHeight;

  tf.model = Mat44::translation( Vec3( mapX, mapY, 0.0f ) );
  tf.model.rotateZ( h );
  tf.model.scale( Vec3( 10.0f, 10.0f, 0.0f ) );
  tf.model.translate( Vec3( -0.5f, -0.5f, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

  glBindTexture( GL_TEXTURE_2D, 0 );
}

GalileoFrame::GalileoFrame() :
    Frame( 8, -60 - Font::INFOS[Font::SMALL].height - Font::INFOS[Font::LARGE].height,
           240, 232 - Font::INFOS[Font::LARGE].height, "" ),
    mapTexId( 0 ), arrowTexId( 0 ), markerTexId( 0 ), isVisible( true )

{
  flags = PINNED_BIT;

  arrowTexId = loadTexture( "ui/galileo/arrow.ozcTex" );
  markerTexId = loadTexture( "ui/galileo/marker.ozcTex" );
}

GalileoFrame::~GalileoFrame()
{
  if( mapTexId != 0 ) {
    glDeleteTextures( 1, &mapTexId );
  }
  if( arrowTexId != 0 ) {
    glDeleteTextures( 1, &arrowTexId );
  }
  if( markerTexId != 0 ) {
    glDeleteTextures( 1, &markerTexId );
  }
}

}
}
}
