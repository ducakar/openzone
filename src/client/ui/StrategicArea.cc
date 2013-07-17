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
 * @file client/ui/StrategicArea.cc
 */

#include <client/ui/StrategicArea.hh>

#include <client/Shader.hh>
#include <client/Shape.hh>
#include <client/Camera.hh>
#include <client/Input.hh>

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
  Vec3 p = bb.p - camera.p;
  Vec3 corners[8] = {
    p + Vec3( -bb.dim.x, -bb.dim.y, -bb.dim.z ),
    p + Vec3( -bb.dim.x, -bb.dim.y, +bb.dim.z ),
    p + Vec3( -bb.dim.x, +bb.dim.y, -bb.dim.z ),
    p + Vec3( -bb.dim.x, +bb.dim.y, +bb.dim.z ),
    p + Vec3( +bb.dim.x, -bb.dim.y, -bb.dim.z ),
    p + Vec3( +bb.dim.x, -bb.dim.y, +bb.dim.z ),
    p + Vec3( +bb.dim.x, +bb.dim.y, -bb.dim.z ),
    p + Vec3( +bb.dim.x, +bb.dim.y, +bb.dim.z )
  };

  float minX = +Math::INF;
  float minY = +Math::INF;
  float maxX = -Math::INF;
  float maxY = -Math::INF;

  for( int i = 0; i < 8; ++i ) {
    Vec3  t = camera.rotTMat * corners[i];
    float d = max( -t.z, TAG_CLIP_DIST );
    // we have to clamp to prevent integer overflows
    float x = clamp( ( t.x / d ) * stepPixel, -TAG_CLAMP_LIMIT, +TAG_CLAMP_LIMIT );
    float y = clamp( ( t.y / d ) * stepPixel, -TAG_CLAMP_LIMIT, +TAG_CLAMP_LIMIT );

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

  span->minX = camera.centreX + Math::lround( minX );
  span->minY = camera.centreY + Math::lround( minY );
  span->maxX = camera.centreX + Math::lround( maxX );
  span->maxY = camera.centreY + Math::lround( maxY );

  return true;
}

void StrategicArea::drawHoveredRect( const Span& span, const Struct* str, const Entity* ent,
                                     const Object* obj )
{
  float minX = float( span.minX );
  float maxX = float( span.maxX );
  float maxY = float( span.maxY );

  float life = 1.0f;

  if( str != nullptr ) {
    int labelX = ( span.minX + span.maxX ) / 2;
    int labelY = span.maxY + 12;

    if( ent != nullptr ) {
      int entIndex = str->index * Struct::MAX_ENTITIES + int( ent - str->entities.begin() );

      if( entIndex != cachedEntityIndex ) {
        const String& title = ent->clazz->title;

        cachedStructIndex = -1;
        cachedEntityIndex = entIndex;
        cachedObjectIndex = -1;

        unitName.set( labelX, labelY, "%s", title.cstr() );
      }
      else {
        unitName.setPosition( labelX, labelY );
      }

      unitName.draw( this );
    }
    else {
      if( str->index != cachedStructIndex ) {
        const String& title = str->bsp->title;

        cachedStructIndex = str->index;
        cachedEntityIndex = -1;
        cachedObjectIndex = -1;

        unitName.set( labelX, labelY, "%s", title.cstr() );
      }
      else {
        unitName.setPosition( labelX, labelY );
      }

      life = str->life / str->bsp->life;

      unitName.draw( this );
    }
  }
  else {
    const Bot*         bot   = static_cast<const Bot*>( obj );
    const ObjectClass* clazz = obj->clazz;

    int labelX = ( span.minX + span.maxX ) / 2;
    int labelY = span.maxY + 12;

    if( obj->index != cachedObjectIndex ) {
      String title = ( obj->flags & Object::BOT_BIT ) && !bot->name.isEmpty() ?
                     bot->name + " (" + clazz->title + ")" : clazz->title;

      cachedStructIndex = -1;
      cachedEntityIndex = -1;
      cachedObjectIndex = obj->index;

      unitName.set( labelX, labelY, "%s", title.cstr() );
    }
    else {
      unitName.setPosition( labelX, labelY );
    }

    life = obj->flags & Object::BOT_BIT ?
           max( 0.0f, ( obj->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f ) ) :
           obj->life / clazz->life;

    unitName.draw( this );
  }

  if( ent == nullptr ) {
    float barWidth = maxX - minX + 2.0f;
    float lifeWidth = life * barWidth;
    float lifeWidthLeft = barWidth - lifeWidth;

    shape.colour( 1.0f - life, life, 0.0f, 0.8f );
    shape.fill( minX - 1.0f, maxY + 3.0f, lifeWidth, 6.0f );

    shape.colour( 0.0f, 0.0f, 0.0f, 0.15f );
    shape.fill( minX - 1.0f + lifeWidth, maxY + 3.0f, lifeWidthLeft, 6.0f );

    shape.colour( 1.0f, 1.0f, 1.0f, 0.8f );
    shape.rect( minX - 2.0f, maxY + 2.0f, barWidth + 2.0f, 8.0f );
  }
}

void StrategicArea::drawTaggedRect( const Span& span, const Struct* str, const Object* obj,
                                    bool isHovered )
{
  float minX = float( span.minX );
  float maxX = float( span.maxX );
  float minY = float( span.minY );
  float maxY = float( span.maxY );

  if( isHovered ) {
    shape.colour( 1.0f, 1.0f, 1.0f, 0.8f );
    shape.tag( minX, minY, maxX, maxY );
  }
  else {
    float life = 1.0f;

    if( str != nullptr ) {
      float maxLife = str->bsp->life;

      life = str->life / maxLife;
    }
    else {
      float maxLife = obj->clazz->life;

      life = obj->flags & Object::BOT_BIT ?
             max( 0.0f, ( obj->life - maxLife / 2.0f ) / ( maxLife / 2.0f ) ) :
             obj->life / maxLife;
    }

    float barWidth = maxX - minX + 2.0f;
    float lifeWidth = life * barWidth;
    float lifeWidthLeft = barWidth - lifeWidth;

    hard_assert( 0.0f <= life && life <= 1.0f );

    shape.colour( 1.0f - life, life, 0.0f, 0.5f );
    shape.fill( minX - 1.0f, maxY + 3.0f, lifeWidth, 6.0f );

    shape.colour( 0.0f, 0.0f, 0.0f, 0.1f );
    shape.fill( minX - 1.0f + lifeWidth, maxY + 3.0f, lifeWidthLeft, 6.0f );

    shape.colour( 1.0f, 1.0f, 1.0f, 0.8f );
    shape.rect( minX - 2.0f, maxY + 2.0f, barWidth + 2.0f, 8.0f );
    shape.tag( minX, minY, maxX, maxY );
  }
}

void StrategicArea::onVisibilityChange( bool )
{
  taggedStrs.clear();
  taggedObjs.clear();

  hoverStr = -1;
  hoverObj = -1;

  cachedStructIndex = -1;
  cachedEntityIndex = -1;
  cachedObjectIndex = -1;
}

void StrategicArea::onReposition()
{
  width     = camera.width;
  height    = camera.height;

  pixelStep = camera.coeff / float( camera.height / 2 );
  stepPixel = 1.0f / pixelStep;
}

void StrategicArea::onUpdate()
{
  for( int i = 0; i < taggedStrs.length(); ) {
    const Struct* str = orbis.str( taggedStrs[i] );

    if( str == nullptr ) {
      taggedStrs.eraseUnordered( i );
    }
    else {
      ++i;
    }
  }

  for( int i = 0; i < taggedObjs.length(); ) {
    const Object* obj = orbis.obj( taggedObjs[i] );

    if( obj == nullptr || obj->cell == nullptr ) {
      taggedObjs.eraseUnordered( i );
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

  const Struct* str = collider.hit.str;
  const Entity* ent = collider.hit.entity;
  const Object* obj = collider.hit.obj;

  if( input.leftClick && !input.keys[Input::KEY_UI_ALT] ) {
    taggedStrs.clear();
    taggedObjs.clear();
  }

  if( str != nullptr ) {
    hoverStr = str->index;

    if( ent != nullptr ) {
      hoverEnt = str->index * Struct::MAX_ENTITIES + int( ent - str->entities.begin() );
    }

    if( input.leftClick ) {
      int index = taggedStrs.index( hoverStr );

      if( index >= 0 ) {
        taggedStrs.eraseUnordered( index );
      }
      else {
        taggedStrs.add( hoverStr );
      }
    }
  }
  else if( obj != nullptr ) {
    hoverObj = obj->index;

    if( input.leftClick ) {
      int index = taggedObjs.index( hoverObj );

      if( index >= 0 ) {
        taggedObjs.eraseUnordered( index );
      }
      else {
        taggedObjs.add( hoverObj );
      }
    }
  }
  return true;
}

void StrategicArea::onDraw()
{
  const Struct* str = orbis.str( hoverStr );
  const Entity* ent = orbis.ent( hoverEnt );
  const Object* obj = orbis.obj( hoverObj );

  Span span;

  if( obj != nullptr ) {
    if( projectBounds( &span, *obj ) ) {
      drawHoveredRect( span, nullptr, nullptr, obj );
    }
  }
  else if( ent != nullptr ) {
    str = ent->str;

    if( projectBounds( &span, str->toAbsoluteCS( *ent->clazz + ent->offset ).toAABB() ) ) {
      drawHoveredRect( span, str, ent, nullptr );
    }
  }
  else if( str != nullptr ) {
    if( projectBounds( &span, str->toAABB() ) ) {
      drawHoveredRect( span, str, nullptr, nullptr );
    }
  }
  else {
    cachedStructIndex = -1;
    cachedEntityIndex = -1;
    cachedObjectIndex = -1;
  }

  for( int i = 0; i < taggedStrs.length(); ++i ) {
    const Struct* str = orbis.str( taggedStrs[i] );

    if( str != nullptr ) {
      if( ( str->p - camera.p ) * camera.at >= TAG_CLIP_DIST ) {
        if( projectBounds( &span, str->toAABB() ) ) {
          drawTaggedRect( span, str, nullptr, taggedStrs[i] == hoverStr );
        }
      }
    }
  }

  for( int i = 0; i < taggedObjs.length(); ++i ) {
    const Object* obj = orbis.obj( taggedObjs[i] );

    if( obj != nullptr ) {
      if( ( obj->p - camera.p ) * camera.at >= TAG_CLIP_DIST ) {
        if( projectBounds( &span, *obj ) ) {
          drawTaggedRect( span, nullptr, obj, taggedObjs[i] == hoverObj );
        }
      }
    }
  }

  hoverStr = -1;
  hoverEnt = -1;
  hoverObj = -1;
}

StrategicArea::StrategicArea() :
  Area( camera.width, camera.height ),
  unitName( 0, 0, ALIGN_HCENTRE, Font::SANS, " " ),
  cachedStructIndex( -1 ), cachedEntityIndex( -1 ), cachedObjectIndex( -1 ),
  hoverStr( -1 ), hoverEnt( -1 ), hoverObj( -1 )
{
  flags = UPDATE_BIT | PINNED_BIT;
}

}
}
}
