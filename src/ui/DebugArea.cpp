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
      Bot &bot = *camera.bot;

      print( 5, -5 - textHeight * 2, "bot.pos(%.2f %.2f %.2f) bot.rot(%.2f %.2f)",
            bot.p.x, bot.p.y, bot.p.z,
            bot.h, bot.v );

      print( 5, -5 - textHeight * 3, "bot.vel(%.2f %.2f %.2f) bot.mom(%.2f %.2f %.2f) bot.wd %.2f",
             bot.velocity.x, bot.velocity.y, bot.velocity.z,
             bot.momentum.x, bot.momentum.y, bot.momentum.z,
             bot.waterDepth );

      print( 5, -5 - textHeight * 4, "d %d fl %d lw %d h %d fr %d iw %d s %d ld %d ovlp %d",
            ( bot.flags & Object::DISABLED_BIT ) != 0,
            ( bot.flags & Object::ON_FLOOR_BIT ) != 0,
            bot.lower,
            ( bot.flags & Object::HIT_BIT ) != 0,
            ( bot.flags & Object::FRICTING_BIT ) != 0,
            ( bot.flags & Object::IN_WATER_BIT ) != 0,
            ( bot.flags & Object::ON_SLICK_BIT ) != 0,
            ( bot.flags & Object::ON_LADDER_BIT ) != 0,
            !collider.test( bot, &bot ) );

      if( bot.grabObjIndex >= 0 ) {
        DynObject &obj = *(DynObject*) world.objects[bot.grabObjIndex];

        print( 5, -5 - textHeight * 5, "gobj.vel(%.2f %.2f %.2f) gobj.mom(%.2f %.2f %.2f)",
               obj.velocity.x, obj.velocity.y, obj.velocity.z,
               obj.momentum.x, obj.momentum.y, obj.momentum.z );

        print( 5, -5 - textHeight * 6, "d %d fl %d lw %d h %d fr %d iw %d s %d",
               ( obj.flags & Object::DISABLED_BIT ) != 0,
               ( obj.flags & Object::ON_FLOOR_BIT ) != 0,
               obj.lower,
               ( obj.flags & Object::HIT_BIT ) != 0,
               ( obj.flags & Object::FRICTING_BIT ) != 0,
               ( bot.flags & Object::IN_WATER_BIT ) != 0,
               ( bot.flags & Object::ON_SLICK_BIT ) != 0 );
      }
    }
  }

}
}
}
