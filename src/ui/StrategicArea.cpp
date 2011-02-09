/*
 *  StrategicArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/StrategicArea.hpp"

#include "matrix/Collider.hpp"

#include "client/Camera.hpp"
#include "client/Colours.hpp"
#include "ui/Keyboard.hpp"

#include <SDL_ttf.h>
#include <SDL_opengl.h>

namespace oz
{
namespace client
{
namespace ui
{

  const float StrategicArea::TAG_CLIP_DIST = 0.1f;

  Pair<int> StrategicArea::project( const Point3& p ) const
  {
    Point3 t = camera.rotTMat * p;

    t.x = Math::round( ( t.x / t.y ) * stepPixel );
    t.z = Math::round( ( t.z / t.y ) * stepPixel );

    return Pair<int>( camera.centreX + int( t.x ), camera.centreY + int( t.z ) );
  }

  Span StrategicArea::projectBounds( const AABB& bb ) const
  {
    Span span;
    Pair<int> t[8];

    t[0] = project( bb.p + Vec3( -bb.dim.x, -bb.dim.y, -bb.dim.z ) );
    t[1] = project( bb.p + Vec3( +bb.dim.x, -bb.dim.y, -bb.dim.z ) );
    t[2] = project( bb.p + Vec3( -bb.dim.x, +bb.dim.y, -bb.dim.z ) );
    t[3] = project( bb.p + Vec3( +bb.dim.x, +bb.dim.y, -bb.dim.z ) );
    t[4] = project( bb.p + Vec3( -bb.dim.x, -bb.dim.y, +bb.dim.z ) );
    t[5] = project( bb.p + Vec3( +bb.dim.x, -bb.dim.y, +bb.dim.z ) );
    t[6] = project( bb.p + Vec3( -bb.dim.x, +bb.dim.y, +bb.dim.z ) );
    t[7] = project( bb.p + Vec3( +bb.dim.x, +bb.dim.y, +bb.dim.z ) );

    span.minX = t[0].x;
    span.maxX = t[0].x;
    span.minY = t[0].y;
    span.maxY = t[0].y;

    for( int i = 0; i < 8; ++i ) {
      span.minX = min( t[i].x, span.minX );
      span.maxX = max( t[i].x, span.maxX );
      span.minY = min( t[i].y, span.minY );
      span.maxY = max( t[i].y, span.maxY );
    }

    return span;
  }

  void StrategicArea::fillRect( float x, float y, float width, float height )
  {
    glRectf( x, y, x + width, y + height );
  }

  void StrategicArea::drawRect( float x, float y, float width, float height )
  {
    glBegin( GL_LINE_LOOP );
      glVertex2f( x         + 0.5f, y          + 0.5f );
      glVertex2f( x + width - 0.5f, y          + 0.5f );
      glVertex2f( x + width - 0.5f, y + height - 0.5f );
      glVertex2f( x         + 0.5f, y + height - 0.5f );
    glEnd();
  }

  void StrategicArea::printName( int baseX, int baseY, const char* s, ... )
  {
    char buffer[1024];
    va_list ap;

    va_start( ap, s );
    vsnprintf( buffer, 1024, s, ap );
    va_end( ap );
    buffer[1023] = '\0';

    setFontColour( 0x00, 0x00, 0x00 );
    SDL_Surface* text = TTF_RenderUTF8_Blended( currentFont, buffer, fontColour );

    // flip
    uint* pixels = reinterpret_cast<uint*>( text->pixels );
    for( int i = 0; i < text->h / 2; ++i ) {
      for( int j = 0; j < text->w; ++j ) {
        swap( pixels[i * text->w + j], pixels[( text->h - i - 1 ) * text->w + j] );
      }
    }

    int x = baseX - text->w / 2;
    int y = baseY - text->h / 2;

    if( x < 0 || x + text->w >= camera.width - 1 || y < 1 || y + text->h >= camera.height ) {
      textWidth = 0;
      SDL_FreeSurface( text );
      return;
    }

    glRasterPos2i( x + 1, y - 1 );
    glDrawPixels( text->w, text->h, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    // make white
    int size = text->w * text->h;
    for( int i = 0; i < size; ++i ) {
      pixels[i] |= 0x00ffffff;
    }

    glRasterPos2i( x, y );
    glDrawPixels( text->w, text->h, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    textWidth = text->w;
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
        Math::max( 0.0f, ( hovered->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f ) ) :
        hovered->life / clazz->life;
    float barWidth = maxX - minX + 2.0f;
    float lifeWidth = life * barWidth;
    float lifeWidthLeft = barWidth - lifeWidth;

    hard_assert( 0.0f <= life && life <= 1.0f );

    glColor4f( 1.0f - life, life, 0.0f, 1.0f );
    fillRect( minX - 1.0f, maxY + 3.0f, lifeWidth, 6.0f );
    glColor4f( 0.0f, 0.0f, 0.0f, 0.2f );
    fillRect( minX - 1.0f + lifeWidth, maxY + 3.0f, lifeWidthLeft, 6.0f );

    glColor4fv( Colours::WHITE );
    drawRect( minX - 2.0f, maxY + 2.0f, barWidth + 2.0f, 8.0f );
  }

  void StrategicArea::drawTaggedRect( const Object* obj, const Span& span )
  {
    float minX = float( span.minX );
    float maxX = float( span.maxX );
    float minY = float( span.minY );
    float maxY = float( span.maxY );

    if( obj != hovered ) {
      const ObjectClass *clazz = obj->clazz;
      float life = ( obj->flags & Object::BOT_BIT ) ?
          Math::max( 0.0f, ( obj->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f ) ) :
          obj->life / clazz->life;
      float barWidth = maxX - minX + 2.0f;
      float lifeWidth = life * barWidth;
      float lifeWidthLeft = barWidth - lifeWidth;

      hard_assert( 0.0f <= life && life <= 1.0f );

      glColor4f( 1.0f - life, life, 0.0f, 0.5f );
      fillRect( minX - 1.0f, maxY + 3.0f, lifeWidth, 6.0f );
      glColor4f( 0.0f, 0.0f, 0.0f, 0.2f );
      fillRect( minX - 1.0f + lifeWidth, maxY + 3.0f, lifeWidthLeft, 6.0f );

      glColor4f( 1.0f, 1.0f, 1.0f, 0.5f );
      drawRect( minX - 2.0f, maxY + 2.0f, barWidth + 2.0f, 8.0f );

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

    glColor4fv( Colours::WHITE );
    glBegin( GL_LINES );
      glVertex2f( minX - 1.5f, minY - 1.5f );
      glVertex2f( minX - 1.5f, minY + 3.5f );
      glVertex2f( minX - 0.5f, minY - 1.5f );
      glVertex2f( minX + 3.5f, minY - 1.5f );

      glVertex2f( maxX + 1.5f, minY - 1.5f );
      glVertex2f( maxX - 3.5f, minY - 1.5f );
      glVertex2f( maxX + 1.5f, minY - 0.5f );
      glVertex2f( maxX + 1.5f, minY + 3.5f );

      glVertex2f( maxX + 1.5f, maxY + 1.5f );
      glVertex2f( maxX - 3.5f, maxY + 1.5f );
      glVertex2f( maxX + 1.5f, maxY + 0.5f );
      glVertex2f( maxX + 1.5f, maxY - 3.5f );

      glVertex2f( minX - 1.5f, maxY + 1.5f );
      glVertex2f( minX + 3.5f, maxY + 1.5f );
      glVertex2f( minX - 1.5f, maxY + 0.5f );
      glVertex2f( minX - 1.5f, maxY - 3.5f );
    glEnd();
  }

  StrategicArea::StrategicArea() : Area( camera.width, camera.height ), leftClick( false )
  {
    flags |= IGNORE_BIT | HIDDEN_BIT | UPDATE_FUNC_BIT;
    setFont( Font::Type::SANS );

    pixelStep = Math::tan( Math::rad( camera.angle / 2.0f ) ) / float( camera.height / 2 );
    stepPixel = 1.0f / pixelStep;
  }

  StrategicArea::~StrategicArea()
  {}

  void StrategicArea::onUpdate()
  {
    if( camera.state != Camera::State::STRATEGIC ) {
      show( false );
      tagged.clear();
      leftClick = false;
      return;
    }
  }

  bool StrategicArea::onMouseEvent()
  {
    leftClick |= mouse.leftClick;
    return true;
  }

  void StrategicArea::onDraw()
  {
    Vec3 at = Vec3( float( mouse.x - camera.centreX ) * pixelStep * 100.0f,
                    100.0f,
                    float( mouse.y - camera.centreY ) * pixelStep * 100.0f );

    at = camera.rotMat * at;
    collider.mask = ~0;
    collider.translate( camera.p, at );
    collider.mask = Object::SOLID_BIT;
    hovered = collider.hit.obj;

    if( hovered != null ) {
      Span span = projectBounds( *hovered + ( Point3::ORIGIN - camera.p ) );
      drawHoveredRect( span );
      camera.setTagged( hovered );

      if( leftClick ) {
        tagged.clear();
        tagged.add( hovered->index );
      }
    }
    else {
      camera.setTagged( null );

      if( leftClick ) {
        tagged.clear();
      }
    }
    leftClick = false;

    for( int i = 0; i < tagged.length(); ) {
      const Object* obj = orbis.objects[tagged[i]];

      if( obj == null ||
          ( ( obj->flags & Object::BOT_BIT ) && obj->life <= obj->clazz->life / 2.0f ) )
      {
        tagged.removeUO( i );
        continue;
      }

      AABB bb = *obj + ( Point3::ORIGIN - camera.p );
      if( bb.p * camera.at >= TAG_CLIP_DIST ) {
        Span span = projectBounds( bb );
        drawTaggedRect( obj, span );
      }
      ++i;
    }
  }

}
}
}
