/*
 *  DebugArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "DebugArea.h"

#include "matrix/World.h"
#include "matrix/Collider.h"
#include "client/Camera.h"

#include <GL/glu.h>

namespace oz
{
namespace client
{
namespace ui
{

  void DebugArea::onDraw()
  {
    glColor4f( 0.6f, 0.6f, 0.6f, 0.6f );
    fill( 0, 0, width, height );

    print( 5, -5 - textHeight * 1, "cam.p(%.2f %.2f %.2f) cam.relRot(%.2f %.2f)",
           camera.p.x, camera.p.y, camera.p.z, camera.h, camera.v );

    if( camera.bot != -1 ) {
      print( 5, -5 - textHeight * 2, "bot.pos(%.2f %.2f %.2f) bot.rot(%.2f %.2f)",
            camera.botObj->p.x, camera.botObj->p.y, camera.botObj->p.z,
            camera.botObj->h, camera.botObj->v );

      print( 5, -5 - textHeight * 3, "bot.vel(%.2f %.2f %.2f) bot.mom(%.2f %.2f %.2f) bot.wd %.2f",
             camera.botObj->velocity.x, camera.botObj->velocity.y, camera.botObj->velocity.z,
             camera.botObj->momentum.x, camera.botObj->momentum.y, camera.botObj->momentum.z,
             camera.botObj->depth );

      print( 5, -5 - textHeight * 4, "d %d fl %d lw %d up %d h %d fr %d iw %d s %d ld %d ovlp %d",
            ( camera.botObj->flags & Object::DISABLED_BIT ) != 0,
            ( camera.botObj->flags & Object::ON_FLOOR_BIT ) != 0,
            camera.botObj->lower,
            ( camera.botObj->flags & Object::UPPER_BIT ) != 0,
            ( camera.botObj->flags & Object::HIT_BIT ) != 0,
            ( camera.botObj->flags & Object::FRICTING_BIT ) != 0,
            ( camera.botObj->flags & Object::IN_WATER_BIT ) != 0,
            ( camera.botObj->flags & Object::ON_SLICK_BIT ) != 0,
            ( camera.botObj->flags & Object::ON_LADDER_BIT ) != 0,
            !collider.test( *camera.botObj, camera.botObj ) );

      if( camera.tagged != -1 ) {
        if( camera.tagged != -1 && ( camera.taggedObj->flags & Object::DYNAMIC_BIT ) ) {
          const Dynamic* dyn = static_cast<const Dynamic*>( camera.taggedObj );

          print( 5, -5 - textHeight * 5, "tagDyn.vel(%.2f %.2f %.2f) tagDyn.mom(%.2f %.2f %.2f)",
                 dyn->velocity.x, dyn->velocity.y, dyn->velocity.z,
                 dyn->momentum.x, dyn->momentum.y, dyn->momentum.z );
          print( 5, -5 - textHeight * 6, "d %d fl %d lw %d up %d h %d fr %d iw %d s %d",
               ( dyn->flags & Object::DISABLED_BIT ) != 0,
               ( dyn->flags & Object::ON_FLOOR_BIT ) != 0,
               dyn->lower,
               ( dyn->flags & Object::UPPER_BIT ) != 0,
               ( dyn->flags & Object::HIT_BIT ) != 0,
               ( dyn->flags & Object::FRICTING_BIT ) != 0,
               ( dyn->flags & Object::IN_WATER_BIT ) != 0,
               ( dyn->flags & Object::ON_SLICK_BIT ) != 0 );
        }
      }
    }
  }

}
}
}
