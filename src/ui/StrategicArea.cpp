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
#include "client/Shape.hpp"

#include "ui/Keyboard.hpp"

#include <SDL_ttf.h>
#include <GL/gl.h>

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

    float px = Math::round( ( t.x / -t.z ) * stepPixel );
    float py = Math::round( ( t.y / -t.z ) * stepPixel );

    return Pair<int>( camera.centreX + int( px ), camera.centreY + int( py ) );
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

    for( int i = 1; i < 8; ++i ) {
      span.minX = min( t[i].x, span.minX );
      span.maxX = max( t[i].x, span.maxX );
      span.minY = min( t[i].y, span.minY );
      span.maxY = max( t[i].y, span.maxY );
    }

    // we must contain boxes that are to far outside the screen, otherwise we get healthbars drawn
    // over half of the screen and similar defects
    // 2 px margin so that whole box is drawn inside screen
    span.minX = max( span.minX, 2 );
    span.maxX = min( span.maxX, camera.width - 2 );
    span.minY = max( span.minY, 2 );
    span.maxY = min( span.maxY, camera.height - 2 );

    return span;
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

    glUniform1i( param.oz_IsTextureEnabled, true );

    glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 1.0f );
    shape.fill( x + 1, y - 1, text->w, text->h );
    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
    shape.fill( x, y, text->w, text->h );

    glUniform1i( param.oz_IsTextureEnabled, false );

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

    hard_assert( 0.0f <= life && life <= 1.0f );

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

    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
    shape.tag( minX, minY, maxX, maxY );
  }

  StrategicArea::StrategicArea() : Area( camera.width, camera.height ), hadMouseEvent( false ),
      hovered( null )
  {
    flags = IGNORE_BIT | HIDDEN_BIT | UPDATE_FUNC_BIT | PINNED_BIT;
    setFont( Font::SANS );

    pixelStep = camera.coeff / float( camera.height / 2 );
    stepPixel = 1.0f / pixelStep;
  }

  StrategicArea::~StrategicArea()
  {}

  bool StrategicArea::onMouseEvent()
  {
    hard_assert( camera.state == Camera::STRATEGIC );

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

    hadMouseEvent = true;
    return true;
  }

  void StrategicArea::onUpdate()
  {
    hard_assert( camera.state == Camera::STRATEGIC );

    if( !hadMouseEvent ) {
      hovered = null;
    }
    hadMouseEvent = false;

    for( int i = 0; i < tagged.length(); ) {
      const Object* obj = orbis.objects[ tagged[i] ];

      if( obj == null ||
          ( ( obj->flags & Object::BOT_BIT ) && obj->life <= obj->clazz->life / 2.0f ) )
      {
        tagged.removeUO( i );
      }
      else {
        ++i;
      }
    }
  }

  void StrategicArea::onDraw()
  {
    hard_assert( camera.state == Camera::STRATEGIC );

    if( hovered != null ) {
      Span span = projectBounds( *hovered + ( Point3::ORIGIN - camera.p ) );

      if( span.maxX - span.minX >= 10 ) {
        drawHoveredRect( span );
      }
    }

    foreach( objIndex, tagged.citer() ) {
      const Object* obj = orbis.objects[*objIndex];

      if( obj != null ) {
        AABB bb = *obj + ( Point3::ORIGIN - camera.p );
        if( bb.p * camera.at >= TAG_CLIP_DIST ) {
          Span span = projectBounds( bb );

          if( span.maxX - span.minX >= 6 && span.maxY - span.minY >= 6 ) {
            drawTaggedRect( obj, span );
          }
        }
      }
    }
  }

}
}
}
