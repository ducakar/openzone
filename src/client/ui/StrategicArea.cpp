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
 * @file client/ui/StrategicArea.cpp
 */

#include "stable.hpp"

#include "client/ui/StrategicArea.hpp"

#include "matrix/Collider.hpp"

#include "client/Camera.hpp"
#include "client/Colours.hpp"
#include "client/Shape.hpp"
#include "client/OpenGL.hpp"

#include "client/ui/Keyboard.hpp"

#include <SDL/SDL_ttf.h>

namespace oz
{
namespace client
{
namespace ui
{

const float StrategicArea::TAG_CLIP_DIST      = 0.1f;
const float StrategicArea::TAG_CLAMP_LIMIT    = 1e6f;
// size in pixels
const float StrategicArea::TAG_MIN_PIXEL_SIZE = 4.0f;
// size in coefficient
const float StrategicArea::TAG_MAX_COEFF_SIZE = 4.0f;

bool StrategicArea::projectBounds( Span* span, const AABB& bb ) const
{
  Point3 corners[8] = {
    bb.p + Vec3( -bb.dim.x, -bb.dim.y, -bb.dim.z ),
    bb.p + Vec3( -bb.dim.x, -bb.dim.y, +bb.dim.z ),
    bb.p + Vec3( -bb.dim.x, +bb.dim.y, -bb.dim.z ),
    bb.p + Vec3( -bb.dim.x, +bb.dim.y, +bb.dim.z ),
    bb.p + Vec3( +bb.dim.x, -bb.dim.y, -bb.dim.z ),
    bb.p + Vec3( +bb.dim.x, -bb.dim.y, +bb.dim.z ),
    bb.p + Vec3( +bb.dim.x, +bb.dim.y, -bb.dim.z ),
    bb.p + Vec3( +bb.dim.x, +bb.dim.y, +bb.dim.z )
  };

  float minX = +Math::INF;
  float minY = +Math::INF;
  float maxX = -Math::INF;
  float maxY = -Math::INF;

  for( int i = 0; i < 8; ++i ) {
    Point3 t = camera.rotTMat * corners[i];
    float  d = max( -t.z, TAG_CLIP_DIST );
    // we have to clamp to prevent integer overflows
    float  x = clamp( ( t.x / d ) * stepPixel, -TAG_CLAMP_LIMIT, +TAG_CLAMP_LIMIT );
    float  y = clamp( ( t.y / d ) * stepPixel, -TAG_CLAMP_LIMIT, +TAG_CLAMP_LIMIT );

    minX = min( minX, x );
    minY = min( minY, y );
    maxX = max( maxX, x );
    maxY = max( maxY, y );
  }

  if( maxX - minX < TAG_MIN_PIXEL_SIZE || ( maxX - minX ) * pixelStep > TAG_MAX_COEFF_SIZE ||
      maxY - minY < TAG_MIN_PIXEL_SIZE || ( maxY - minY ) * pixelStep > TAG_MAX_COEFF_SIZE )
  {
    return false;
  }

  span->minX = camera.centreX + int( minX + 0.5f );
  span->minY = camera.centreY + int( minY + 0.5f );
  span->maxX = camera.centreX + int( maxX + 0.5f );
  span->maxY = camera.centreY + int( maxY + 0.5f );

  return true;
}

void StrategicArea::printName( int baseX, int baseY, const char* s, ... )
{
  char buffer[1024];
  va_list ap;

  va_start( ap, s );
  vsnprintf( buffer, 1024, s, ap );
  va_end( ap );
  buffer[1023] = '\0';

  if( buffer[0] == '\0' ) {
    return;
  }

  SDL_Surface* text = TTF_RenderUTF8_Blended( currentFont, buffer, Font::SDL_COLOUR_WHITE );

  int x = baseX - text->w / 2;
  int y = baseY - text->h / 2;
  int width = text->w;
  int height = text->h;

  if( x < 0 || x + width >= camera.width - 1 || y < 1 || y + height >= camera.height ) {
    SDL_FreeSurface( text );
    return;
  }

  glBindTexture( GL_TEXTURE_2D, titleTexId );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                text->pixels);

  SDL_FreeSurface( text );

  glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 1.0f );
  shape.fillInv( x + 1, y - 1, width, height );
  glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
  shape.fillInv( x, y, width, height );

  glBindTexture( GL_TEXTURE_2D, 0 );
}

void StrategicArea::drawHoveredRect( const Span& span )
{
  float minX = float( span.minX );
  float maxX = float( span.maxX );
  float maxY = float( span.maxY );

  float life = 1.0f;

  if( hoveredStr != null ) {
    if( !Math::isInfFM( hoveredStr->life ) ) {
      life = hoveredStr->life / hoveredStr->bsp->life;
    }

    const String& title = library.bspClasses[hoveredStr->id].title;
    printName( ( span.minX + span.maxX ) / 2, ( span.maxY + 18 ), "%s", title.cstr() );
  }
  else {
    const ObjectClass *clazz = hoveredObj->clazz;

    const Bot* bot = static_cast<const Bot*>( hoveredObj );
    String title = ( hoveredObj->flags & Object::BOT_BIT ) && !bot->name.isEmpty() ?
        bot->name + " (" + clazz->title + ")" : clazz->title;

    printName( ( span.minX + span.maxX ) / 2, ( span.maxY + 18 ), "%s", title.cstr() );

    if( !Math::isInfFM( hoveredObj->life ) ) {
      life = ( hoveredObj->flags & Object::BOT_BIT ) ?
          max( 0.0f, ( hoveredObj->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f ) ) :
          hoveredObj->life / clazz->life;
    }
  }

  float barWidth = maxX - minX + 2.0f;
  float lifeWidth = life * barWidth;
  float lifeWidthLeft = barWidth - lifeWidth;

  glUniform4f( param.oz_Colour, 1.0f - life, life, 0.0f, 1.0f );
  shape.fill( minX - 1.0f, maxY + 3.0f, lifeWidth, 6.0f );
  glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 0.2f );
  shape.fill( minX - 1.0f + lifeWidth, maxY + 3.0f, lifeWidthLeft, 6.0f );

  glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
  shape.rect( minX - 2.0f, maxY + 2.0f, barWidth + 2.0f, 8.0f );
}

void StrategicArea::drawTaggedRect( const Struct* str, const Object* obj, const Span& span )
{
  float minX = float( span.minX );
  float maxX = float( span.maxX );
  float minY = float( span.minY );
  float maxY = float( span.maxY );

  float life = 1.0f;

  if( str != null ) {
    float maxLife = str->bsp->life;

    if( !Math::isInfFM( str->life ) ) {
      life = str->life / maxLife;
    }
  }
  else {
    float maxLife = obj->clazz->life;

    if( !Math::isInfFM( maxLife ) ) {
      life = ( obj->flags & Object::BOT_BIT ) ?
          max( 0.0f, ( obj->life - maxLife / 2.0f ) / ( maxLife / 2.0f ) ) :
          obj->life / maxLife;
    }
  }

  float barWidth = maxX - minX + 2.0f;
  float lifeWidth = life * barWidth;
  float lifeWidthLeft = barWidth - lifeWidth;

  hard_assert( 0.0f <= life && life <= 1.0f );

  glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
  shape.tag( minX, minY, maxX, maxY );

  glUniform4f( param.oz_Colour, 1.0f - life, life, 0.0f, 0.5f );
  shape.fill( minX - 1.0f, maxY + 3.0f, lifeWidth, 6.0f );
  glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 0.2f );
  shape.fill( minX - 1.0f + lifeWidth, maxY + 3.0f, lifeWidthLeft, 6.0f );

  glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 0.5f );
  shape.rect( minX - 2.0f, maxY + 2.0f, barWidth + 2.0f, 8.0f );
}

void StrategicArea::onUpdate()
{
  for( int i = 0; i < taggedStrs.length(); ) {
    const Struct* str = orbis.structs[ taggedStrs[i] ];

    if( str == null ) {
      taggedStrs.removeUO( i );
    }
    else {
      ++i;
    }
  }

  for( int i = 0; i < taggedObjs.length(); ) {
    const Object* obj = orbis.objects[ taggedObjs[i] ];

    if( obj == null || obj->cell == null ) {
      taggedObjs.removeUO( i );
    }
    else {
      ++i;
    }
  }
}

bool StrategicArea::onMouseEvent()
{
  Vec3 at = Vec3( float( mouse.x - camera.centreX ) * pixelStep * 100.0f,
                  float( mouse.y - camera.centreY ) * pixelStep * 100.0f,
                  -100.0f );

  at = camera.rotMat * at;
  collider.mask = ~0;
  collider.translate( camera.p, at );
  collider.mask = Object::SOLID_BIT;

  hoveredStr = collider.hit.str;
  hoveredObj = collider.hit.obj;

  hard_assert( hoveredStr == null || hoveredObj == null );

  if( hoveredStr != null ) {
    if( ui::mouse.leftClick ) {
      taggedStrs.clear();
      taggedObjs.clear();
      taggedStrs.add( hoveredStr->index );
    }
  }
  else if( hoveredObj != null ) {
    if( ui::mouse.leftClick ) {
      taggedStrs.clear();
      taggedObjs.clear();
      taggedObjs.add( hoveredObj->index );
    }
  }
  else {
    if( ui::mouse.leftClick ) {
      taggedStrs.clear();
      taggedObjs.clear();
    }
  }

  return true;
}

void StrategicArea::onDraw()
{
  Span span;

  if( hoveredStr != null ) {
    if( projectBounds( &span, hoveredStr->toAABB() + ( Point3::ORIGIN - camera.p ) ) ) {
      drawHoveredRect( span );
    }
  }
  if( hoveredObj != null ) {
    if( projectBounds( &span, *hoveredObj + ( Point3::ORIGIN - camera.p ) ) ) {
      drawHoveredRect( span );
    }
  }

  hoveredStr = null;
  hoveredObj = null;

  for( int i = 0; i < taggedStrs.length(); ++i ) {
    const Struct* str = orbis.structs[ taggedStrs[i] ];

    if( str != null ) {
      AABB bb = str->toAABB() + ( Point3::ORIGIN - camera.p );
      if( bb.p * camera.at >= TAG_CLIP_DIST ) {
        if( projectBounds( &span, bb ) ) {
          drawTaggedRect( str, null, span );
        }
      }
    }
  }

  for( int i = 0; i < taggedObjs.length(); ++i ) {
    const Object* obj = orbis.objects[ taggedObjs[i] ];

    if( obj != null ) {
      AABB bb = *obj + ( Point3::ORIGIN - camera.p );
      if( bb.p * camera.at >= TAG_CLIP_DIST ) {
        if( projectBounds( &span, bb ) ) {
          drawTaggedRect( null, obj, span );
        }
      }
    }
  }
}

StrategicArea::StrategicArea() : Area( camera.width, camera.height ), hoveredObj( null )
{
  flags = IGNORE_BIT | HIDDEN_BIT | UPDATE_BIT | PINNED_BIT;

  glGenTextures( 1, &titleTexId );
  glBindTexture( GL_TEXTURE_2D, titleTexId );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glBindTexture( GL_TEXTURE_2D, 0 );

  pixelStep = camera.coeff / float( camera.height / 2 );
  stepPixel = 1.0f / pixelStep;
}

StrategicArea::~StrategicArea()
{
  glDeleteTextures( 1, &titleTexId );
}

}
}
}
