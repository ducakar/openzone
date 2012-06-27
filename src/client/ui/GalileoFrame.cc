/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file client/ui/GalileoFrame.cc
 */

#include "stable.hh"

#include "client/ui/GalileoFrame.hh"

#include "client/Camera.hh"
#include "client/Context.hh"
#include "client/QuestList.hh"
#include "client/OpenGL.hh"

#include "client/ui/QuestFrame.hh"

namespace oz
{
namespace client
{
namespace ui
{

uint GalileoFrame::loadTexture( const char* path ) const
{
  PFile file( path );
  if( !file.map() ) {
    throw Exception( "Failed reading galileo texture '%s'", path );
  }

  InputStream istream = file.inputStream();
  uint texId = context.readTextureLayer( &istream, path );

  file.unmap();
  return texId;
}

bool GalileoFrame::onMouseEvent()
{
  return isVisible ? Frame::onMouseEvent() : false;
}

void GalileoFrame::onDraw()
{
  if( orbis.terra.id < 0 || ( camera.state == Camera::UNIT && camera.botObj != null &&
      !camera.botObj->hasAttribute( ObjectClass::GALILEO_BIT ) ) )
  {
    isVisible = false;
    setMaximised( false );
    return;
  }

  isVisible = true;

  if( mapTexId == 0 ) {
    mapTexId = loadTexture( "terra/" + library.terrae[orbis.terra.id].name + ".ozcTex" );
  }

  float pX = camera.p.x;
  float pY = camera.p.y;
  float h  = camera.botObj == null ? camera.strategic.h : camera.botObj->h;

  shader.colour( Vec4( 1.0f, 1.0f, 1.0f, isMaximised ? 0.8f : 0.6f ) );
  glBindTexture( GL_TEXTURE_2D, mapTexId );
  fill( 0, 0, width, height );
  shader.colour( Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

  float oX      = float( x );
  float oY      = float( y );
  float fWidth  = float( width );
  float fHeight = float( height );

  if( questFrame != null && questFrame->currentQuest >= 0 ) {
    const Quest& quest = questList.quests[questFrame->currentQuest];

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

GalileoFrame::GalileoFrame( const QuestFrame* questFrame_ ) :
  Frame( 8, -8, 240, 232 - Font::INFOS[Font::LARGE].height, "" ),
  questFrame( questFrame_ ), mapTexId( 0 ), arrowTexId( 0 ), markerTexId( 0 ),
  isVisible( true ), isMaximised( false )
{
  flags = PINNED_BIT;

  arrowTexId = loadTexture( "ui/icon/arrow.ozIcon" );
  markerTexId = loadTexture( "ui/icon/marker.ozIcon" );

  normalX      = x;
  normalY      = y;
  normalWidth  = width;
  normalHeight = height;

  int maxSize = camera.width - 2 * ( width + 8 );
  maxSize = camera.height < maxSize ? camera.height - 64 : maxSize - 64;

  maximisedX      = camera.centreX - maxSize / 2;
  maximisedY      = camera.centreY - maxSize / 2;
  maximisedWidth  = maxSize;
  maximisedHeight = maxSize;
}

GalileoFrame::~GalileoFrame()
{
  OZ_MAIN_CALL( this, {
    if( _this->mapTexId != 0 ) {
      glDeleteTextures( 1, &_this->mapTexId );
    }
    if( _this->arrowTexId != 0 ) {
      glDeleteTextures( 1, &_this->arrowTexId );
    }
    if( _this->markerTexId != 0 ) {
      glDeleteTextures( 1, &_this->markerTexId );
    }
  } )
}

void GalileoFrame::setMaximised( bool doMaximise )
{
  isMaximised = doMaximise;

  if( doMaximise ) {
    x      = maximisedX;
    y      = maximisedY;
    width  = maximisedWidth;
    height = maximisedHeight;

    raise();
  }
  else {
    x      = normalX;
    y      = normalY;
    width  = normalWidth;
    height = normalHeight;
  }
}

}
}
}
