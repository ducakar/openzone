/*
 *  DebugArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "DebugArea.h"

#include "matrix/World.h"
#include "matrix/Collider.h"
#include "client/Camera.h"

namespace oz
{
namespace client
{
namespace ui
{

  void DebugArea::draw()
  {
    glColor4f( 0.6f, 0.6f, 0.6f, 0.6f );
    fill( 0, 0, width, height );

    print( 5, -5 - textHeight * 1, "cam.p(%.2f %.2f %.2f) cam.relRot(%.2f %.2f)",
           camera.p.x, camera.p.y, camera.p.z, camera.h, camera.v );

    if( camera.bot != null ) {
      print( 5, -5 - textHeight * 2, "bot.pos(%.2f %.2f %.2f) bot.rot(%.2f %.2f)",
            camera.bot->p.x, camera.bot->p.y, camera.bot->p.z,
            camera.bot->h, camera.bot->v );

      print( 5, -5 - textHeight * 3, "bot.vel(%.2f %.2f %.2f) bot.mom(%.2f %.2f %.2f)",
             camera.bot->velocity.x, camera.bot->velocity.y, camera.bot->velocity.z,
             camera.bot->momentum.x, camera.bot->momentum.y, camera.bot->momentum.z );

      print( 5, -5 - textHeight * 4, "d %d fl %d lw %d h %d fr %d iw %d uw %d ld %d s %d ovlp %d",
            ( camera.bot->flags & Object::DISABLED_BIT ) != 0,
            ( camera.bot->flags & Object::ON_FLOOR_BIT ) != 0,
            camera.bot->lower >= 0,
            ( !camera.bot->events.isEmpty() ),
            ( camera.bot->flags & Object::FRICTING_BIT ) != 0,
            ( camera.bot->flags & Object::IN_WATER_BIT ) != 0,
            ( camera.bot->flags & Object::UNDER_WATER_BIT ) != 0,
            ( camera.bot->flags & Object::ON_LADDER_BIT ) != 0,
            ( camera.bot->flags & Object::ON_SLICK_BIT ) != 0,
            collider.test( *camera.bot ) );
    }
  }

}
}
}
