/*
 *  StrategicArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "StrategicArea.h"

#include "matrix/BotClass.h"
#include "client/Camera.h"
#include "client/Colors.h"
#include "client/Context.h"

#include <GL/glu.h>

namespace oz
{
namespace client
{
namespace ui
{

  Pair<int> StrategicArea::project( const Vec3& p ) const
  {
    Vec3 t = camera.rotTMat * ( p - camera.p );

    t.x = Math::round( ( t.x / t.y ) * stepPixel );
    t.z = Math::round( ( t.z / t.y ) * stepPixel );

    return Pair<int>( camera.centerX + int( t.x ), camera.centerY + int( t.z ) );
  }

  void StrategicArea::projectBounds( Span& span, const Object* obj ) const
  {
    Pair<int> t[8];

    t[0] = project( obj->p + Vec3( -obj->dim.x, -obj->dim.y, -obj->dim.z ) );
    t[1] = project( obj->p + Vec3( +obj->dim.x, -obj->dim.y, -obj->dim.z ) );
    t[2] = project( obj->p + Vec3( -obj->dim.x, +obj->dim.y, -obj->dim.z ) );
    t[3] = project( obj->p + Vec3( +obj->dim.x, +obj->dim.y, -obj->dim.z ) );
    t[4] = project( obj->p + Vec3( -obj->dim.x, -obj->dim.y, +obj->dim.z ) );
    t[5] = project( obj->p + Vec3( +obj->dim.x, -obj->dim.y, +obj->dim.z ) );
    t[6] = project( obj->p + Vec3( -obj->dim.x, +obj->dim.y, +obj->dim.z ) );
    t[7] = project( obj->p + Vec3( +obj->dim.x, +obj->dim.y, +obj->dim.z ) );

    span.minX = t[0].x;
    span.maxX = t[0].x;
    span.minY = t[0].y;
    span.maxY = t[0].y;

    for( int i = 0; i < 8; i++ ) {
      span.minX = min( t[i].x, span.minX );
      span.maxX = max( t[i].x, span.maxX );
      span.minY = min( t[i].y, span.minY );
      span.maxY = max( t[i].y, span.maxY );
    }
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

  void StrategicArea::drawHoveredRect( const Span& span )
  {
    const ObjectClass *clazz = hovered->type;

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

    setFontColor( 0x00, 0x00, 0x00 );
    printCentered( ( span.minX + span.maxX ) / 2 + 1,
                   ( span.maxY + 16 ) - 1,
                   "%s",
                   description.cstr() );
    setFontColor( 0xff, 0xff, 0xff );
    printCentered( ( span.minX + span.maxX ) / 2,
                   ( span.maxY + 16 ),
                   "%s",
                   description.cstr() );

    float life = ( hovered->flags & Object::BOT_BIT ) ?
        ( hovered->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f ) :
        hovered->life / clazz->life;
    float barWidth = maxX - minX - 2;
    float lifeWidth = life * barWidth;
    float lifeWidthLeft = barWidth - lifeWidth;

    glColor4f( 1.0f - life, life, 0.0f, 1.0f );
    fillRect( minX + 1.0f, maxY + 1.0f, lifeWidth, 6.0f );
    glColor4f( 0.0f, 0.0f, 0.0f, 0.2f );
    fillRect( minX + 1.0f + lifeWidth, maxY + 1.0f, lifeWidthLeft, 6.0f );

    glColor4fv( Colors::WHITE );
    drawRect( minX, maxY, maxX - minX, 8.0f );
  }

  void StrategicArea::drawTaggedRect( const Object* obj, const Span& span )
  {
    float minX = float( span.minX );
    float maxX = float( span.maxX );
    float minY = float( span.minY );
    float maxY = float( span.maxY );

    if( obj != hovered ) {
      const ObjectClass *clazz = obj->type;
      float life = ( obj->flags & Object::BOT_BIT ) ?
          ( obj->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f ) :
          obj->life / clazz->life;
      float barWidth = maxX - minX - 2.0f;
      float lifeWidth = life * barWidth;
      float lifeWidthLeft = barWidth - lifeWidth;

      glColor4f( 1.0f - life, life, 0.0f, 0.5f );
      fillRect( minX + 1, maxY + 1.0f, lifeWidth, 6.0f );
      glColor4f( 0.0f, 0.0f, 0.0f, 0.2f );
      fillRect( minX + 1.0f + lifeWidth, maxY + 1.0f, lifeWidthLeft, 6.0f );

      glColor4f( 1.0f, 1.0f, 1.0f, 0.5f );
      drawRect( minX, maxY, barWidth + 2.0f, 8.0f );

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

    glColor4fv( Colors::WHITE );
    glBegin( GL_LINES );
      glVertex2f( minX + 0.5f, minY + 0.5f );
      glVertex2f( minX + 0.5f, minY + 5.5f );
      glVertex2f( minX + 1.5f, minY + 0.5f );
      glVertex2f( minX + 5.5f, minY + 0.5f );

      glVertex2f( maxX - 0.5f, minY + 0.5f );
      glVertex2f( maxX - 5.5f, minY + 0.5f );
      glVertex2f( maxX - 0.5f, minY + 1.5f );
      glVertex2f( maxX - 0.5f, minY + 5.5f );

      glVertex2f( maxX - 0.5f, maxY - 0.5f );
      glVertex2f( maxX - 5.5f, maxY - 0.5f );
      glVertex2f( maxX - 0.5f, maxY - 1.5f );
      glVertex2f( maxX - 0.5f, maxY - 5.5f );

      glVertex2f( minX + 0.5f, maxY - 0.5f );
      glVertex2f( minX + 5.5f, maxY - 0.5f );
      glVertex2f( minX + 0.5f, maxY - 1.5f );
      glVertex2f( minX + 0.5f, maxY - 5.5f );
    glEnd();
  }

  StrategicArea::StrategicArea() : Area( camera.width, camera.height )
  {
    flags |= IGNORE_BIT | HIDDEN_BIT | UPDATE_FUNC_BIT;
    setFont( SANS );

    pixelStep = Math::tan( Math::rad( camera.angle / 2.0f ) ) / float( camera.height / 2 );
    stepPixel = 1.0f / pixelStep;
  }

  StrategicArea::~StrategicArea()
  {}

  void StrategicArea::onUpdate()
  {
    if( camera.state != Camera::STRATEGIC ) {
      show( false );
      return;
    }
  }

  void StrategicArea::onDraw()
  {
    Vec3 at( float( mouse.x - camera.centerX ) * pixelStep * 100.0f,
             100.0f,
             float( mouse.y - camera.centerY ) * pixelStep * 100.0f );

    at = camera.rotMat * at;
    collider.translate( camera.p, at );
    hovered = collider.hit.obj;

    if( hovered != null && ( hovered->flags & ( Object::BOT_BIT | Object::VEHICLE_BIT ) ) ) {
      Span span;
      projectBounds( span, hovered );
      drawHoveredRect( span );
      camera.setTagged( hovered );

      if( mouse.leftClick ) {
        tagged.clear();
        tagged << hovered->index;
      }
    }
    else {
      camera.setTagged( null );

      if( mouse.leftClick ) {
        tagged.clear();
      }
    }

    for( int i = 0; i < tagged.length(); ) {
      const Object* obj = world.objects[tagged[i]];

      if( obj == null ) {
        tagged.remove( i );
      }
      else {
        i++;
        Span span;
        projectBounds( span, obj );
        drawTaggedRect( obj, span );
      }
    }
  }

}
}
}
