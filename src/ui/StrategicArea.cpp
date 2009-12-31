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

  void StrategicArea::projectBounds( oz::Area& area, const Object* obj ) const
  {
    area.minX = 0;
    area.maxX = width;
    area.minY = 0;
    area.maxY = height;

    Pair<int> t[8];

    t[0] = project( obj->p + Vec3( -obj->dim.x, -obj->dim.y, -obj->dim.z ) );
    t[1] = project( obj->p + Vec3( +obj->dim.x, -obj->dim.y, -obj->dim.z ) );
    t[2] = project( obj->p + Vec3( -obj->dim.x, +obj->dim.y, -obj->dim.z ) );
    t[3] = project( obj->p + Vec3( +obj->dim.x, +obj->dim.y, -obj->dim.z ) );
    t[4] = project( obj->p + Vec3( -obj->dim.x, -obj->dim.y, +obj->dim.z ) );
    t[5] = project( obj->p + Vec3( +obj->dim.x, -obj->dim.y, +obj->dim.z ) );
    t[6] = project( obj->p + Vec3( -obj->dim.x, +obj->dim.y, +obj->dim.z ) );
    t[7] = project( obj->p + Vec3( +obj->dim.x, +obj->dim.y, +obj->dim.z ) );

    for( int i = 0; i < 8; i++ ) {
      area.minX = max( t[i].x, area.minX );
      area.maxX = min( t[i].x, area.maxX );
      area.minY = max( t[i].y, area.minY );
      area.maxY = min( t[i].y, area.maxY );
    }
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
      end();
      return;
    }

    Vec3 at( float( mouse.x - mouse.centerX ) * pixelStep * 100.0f,
             100.0f,
             float( mouse.y - mouse.centerY ) * pixelStep * 100.0f );

    at = camera.rotMat * at;
    collider.translate( camera.p, at );

    hover = collider.hit.obj == null ? -1 : collider.hit.obj->index;
  }

  void StrategicArea::onDraw()
  {
    if( hover != -1 && world.objects[hover] != null ) {
      Object *obj = world.objects[hover];

      oz::Area area;
      projectBounds( area, obj );

      glColor4fv( Colors::WHITE );
      rect( area.minX, area.minY, area.maxX - area.minX, area.maxY - area.minY );
    }

    for( int i = 0; i < tagged.length(); ) {
      if( world.objects[tagged[i]] == null ) {
        tagged.remove( i );
      }
      else {
        i++;
      }
    }
  }

  void StrategicArea::begin()
  {
    flags &= ~( IGNORE_BIT | HIDDEN_BIT );
    flags |= UPDATE_BIT;
  }

  void StrategicArea::end()
  {
    flags |= IGNORE_BIT | HIDDEN_BIT;
    flags &= ~UPDATE_BIT;
  }

}
}
}
