/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <stable.hh>
#include <client/ui/GalileoFrame.hh>

#include <client/Shape.hh>
#include <client/Camera.hh>
#include <client/Context.hh>
#include <client/QuestList.hh>
#include <client/OpenGL.hh>

#include <client/ui/Style.hh>

namespace oz
{
namespace client
{
namespace ui
{

uint GalileoFrame::loadTexture( const char* path ) const
{
  PFile file( path );

  Buffer buffer = file.read();
  if( buffer.isEmpty() ) {
    OZ_ERROR( "Failed reading galileo texture '%s'", path );
  }

  InputStream istream = buffer.inputStream();
  uint texId = context.readTextureLayer( &istream );

  return texId;
}

void GalileoFrame::onReposition()
{
  int maxSize     = camera.height < camera.width ? camera.height - 64 : camera.width - 64;

  normalX         = defaultX < 0 ? parent->x + parent->width - normalWidth + defaultX :
                                   parent->x + defaultX;
  normalY         = defaultY < 0 ? parent->y + parent->height - normalHeight + defaultY :
                                   parent->y + defaultY;

  maximisedX      = camera.centreX - maxSize / 2;
  maximisedY      = camera.centreY - maxSize / 2;
  maximisedWidth  = maxSize;
  maximisedHeight = maxSize;

  setMaximised( isMaximised );
}

void GalileoFrame::onUpdate()
{
  const Bot* bot = camera.botObj;

  if( orbis.terra.id < 0 || ( camera.state == Camera::UNIT && ( bot == nullptr ||
        ( bot->state & Bot::DEAD_BIT ) || !bot->hasAttribute( ObjectClass::GALILEO_BIT ) ) ) )
  {
    if( !( flags & HIDDEN_BIT ) ) {
      setMaximised( false );
      show( false );
    }
  }
  else {
    if( flags & HIDDEN_BIT ) {
      show( true );
    }
  }
}

void GalileoFrame::onDraw()
{
  if( mapTexId == 0 ) {
    mapTexId = loadTexture( "terra/" + liber.terrae[orbis.terra.id].name + ".ozcTex" );
  }

  float pX = camera.p.x;
  float pY = camera.p.y;
  float h  = camera.botObj == nullptr ? camera.strategic.h : camera.botObj->h;

  shape.colour( colour );
  glBindTexture( GL_TEXTURE_2D, mapTexId );
  shape.fill( x, y, width, height );
  shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );

  float oX      = float( x );
  float oY      = float( y );
  float fWidth  = float( width );
  float fHeight = float( height );

  if( questList.activeQuest >= 0 ) {
    const Quest& quest = questList.quests[questList.activeQuest];

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

  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
}

GalileoFrame::GalileoFrame() :
  Frame( 240, 232 - HEADER_SIZE, "" ),
  mapTexId( 0 ), arrowTexId( 0 ), markerTexId( 0 ), colour( style.colours.galileoNormal ),
  isMaximised( false )
{
  flags = PINNED_BIT | UPDATE_BIT;

  arrowTexId = loadTexture( "ui/icon/arrow.ozIcon" );
  markerTexId = loadTexture( "ui/icon/marker.ozIcon" );

  normalWidth  = width;
  normalHeight = height;
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

void GalileoFrame::setMaximised( bool doMaximise )
{
  isMaximised = doMaximise;

  if( doMaximise ) {
    x      = maximisedX;
    y      = maximisedY;
    width  = maximisedWidth;
    height = maximisedHeight;
    colour = style.colours.galileoMaximised;

    raise();
  }
  else {
    x      = normalX;
    y      = normalY;
    width  = normalWidth;
    height = normalHeight;
    colour = style.colours.galileoNormal;
  }
}

}
}
}
