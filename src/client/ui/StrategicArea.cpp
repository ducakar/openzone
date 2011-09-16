/*
 *  StrategicArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/StrategicArea.hpp"

#include "matrix/Collider.hpp"

#include "client/Camera.hpp"
#include "client/Colours.hpp"
#include "client/Shape.hpp"

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

    SDL_Surface* text = TTF_RenderUTF8_Blended( currentFont, buffer, SDL_COLOUR_WHITE );

    textWidth = text->w;

    int x = baseX - text->w / 2;
    int y = baseY - text->h / 2;

    if( x < 0 || x + text->w >= camera.width - 1 || y < 1 || y + text->h >= camera.height ) {
      textWidth = 0;
      SDL_FreeSurface( text );
      return;
    }

    uint textTexId;
    glGenTextures( 1, &textTexId );
    glBindTexture( GL_TEXTURE_2D, textTexId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, text->w, text->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                  text->pixels);

    glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 1.0f );
    shape.fill( x + 1, y - 1, text->w, text->h );
    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
    shape.fill( x, y, text->w, text->h );

    glBindTexture( GL_TEXTURE_2D, 0 );

    glDeleteTextures( 1, &textTexId );
    SDL_FreeSurface( text );
  }

  void StrategicArea::drawHoveredRect( const Span& span )
  {
    const ObjectClass *clazz = hovered->clazz;

    float minX = float( span.minX );
    float maxX = float( span.maxX );
    float maxY = float( span.maxY );

    String description;
    if( hovered->flags & Object::BOT_BIT ) {
      const Bot* bot = static_cast<const Bot*>( hovered );

      description = bot->name.isEmpty() ?
          clazz->description :
          bot->name + " (" + clazz->description + ")";
    }
    else {
      description = clazz->description;
    }

    printName( ( span.minX + span.maxX ) / 2, ( span.maxY + 18 ), "%s", description.cstr() );

    float life = ( hovered->flags & Object::BOT_BIT ) ?
        ( hovered->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f ) :
        hovered->life / clazz->life;

    life = max( life, 0.0f );

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

  void StrategicArea::drawTaggedRect( const Object* obj, const Span& span )
  {
    float minX = float( span.minX );
    float maxX = float( span.maxX );
    float minY = float( span.minY );
    float maxY = float( span.maxY );

    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
    shape.tag( minX, minY, maxX, maxY );

    if( obj != hovered ) {
      const ObjectClass *clazz = obj->clazz;
      float life = ( obj->flags & Object::BOT_BIT ) ?
          max( 0.0f, ( obj->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f ) ) :
          obj->life / clazz->life;
      float barWidth = maxX - minX + 2.0f;
      float lifeWidth = life * barWidth;
      float lifeWidthLeft = barWidth - lifeWidth;

      hard_assert( 0.0f <= life && life <= 1.0f );

      glUniform4f( param.oz_Colour, 1.0f - life, life, 0.0f, 0.5f );
      shape.fill( minX - 1.0f, maxY + 3.0f, lifeWidth, 6.0f );
      glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 0.2f );
      shape.fill( minX - 1.0f + lifeWidth, maxY + 3.0f, lifeWidthLeft, 6.0f );

      glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 0.5f );
      shape.rect( minX - 2.0f, maxY + 2.0f, barWidth + 2.0f, 8.0f );

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
    }
  }

  void StrategicArea::onUpdate()
  {
    for( int i = 0; i < tagged.length(); ) {
      const Dynamic* dyn = static_cast<const Dynamic*>( orbis.objects[ tagged[i] ] );

      if( dyn == null || ( ( dyn->flags & Object::DYNAMIC_BIT ) && dyn->parent != -1 ) ) {
        tagged.removeUO( i );
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
    hovered = collider.hit.obj;

    if( hovered != null ) {
      camera.setTagged( null );

      if( ui::mouse.leftClick ) {
        tagged.clear();
        tagged.add( hovered->index );
      }
    }
    else {
      camera.setTagged( null );

      if( ui::mouse.leftClick ) {
        tagged.clear();
      }
    }

    return true;
  }

  void StrategicArea::onDraw()
  {
    Span span;

    if( hovered != null ) {
      if( projectBounds( &span, *hovered + ( Point3::ORIGIN - camera.p ) ) ) {
        drawHoveredRect( span );
      }
    }

    hovered = null;

    foreach( objIndex, tagged.citer() ) {
      const Object* obj = orbis.objects[*objIndex];

      if( obj != null ) {
        AABB bb = *obj + ( Point3::ORIGIN - camera.p );
        if( bb.p * camera.at >= TAG_CLIP_DIST ) {
          if( projectBounds( &span, bb ) ) {
            drawTaggedRect( obj, span );
          }
        }
      }
    }
  }

  StrategicArea::StrategicArea() : Area( camera.width, camera.height ), hovered( null )
  {
    flags = IGNORE_BIT | HIDDEN_BIT | UPDATE_BIT | PINNED_BIT;
    setFont( Font::SANS );

    pixelStep   = camera.coeff / float( camera.height / 2 );
    stepPixel   = 1.0f / pixelStep;
  }

  StrategicArea::~StrategicArea()
  {}

}
}
}
