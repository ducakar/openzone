/*
 *  DebugArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "DebugArea.h"

#include "matrix/World.h"
#include "matrix/Collider.h"
#include "client/Camera.h"
#include "ui.h"

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

    if( camera.bot != null ) {
      Bot &bot = *camera.bot;

      print( 5, -5 - textHeight * 2, "bot.pos(%.2f %.2f %.2f) bot.rot(%.2f %.2f)",
            bot.p.x, bot.p.y, bot.p.z,
            bot.h, bot.v );

      print( 5, -5 - textHeight * 3, "bot.vel(%.2f %.2f %.2f) bot.mom(%.2f %.2f %.2f) bot.wd %.2f",
             bot.velocity.x, bot.velocity.y, bot.velocity.z,
             bot.momentum.x, bot.momentum.y, bot.momentum.z,
             bot.depth );

      print( 5, -5 - textHeight * 4, "d %d fl %d lw %d up %d h %d fr %d iw %d s %d ld %d ovlp %d",
            ( bot.flags & Object::DISABLED_BIT ) != 0,
            ( bot.flags & Object::ON_FLOOR_BIT ) != 0,
            bot.lower,
            ( bot.flags & Object::UPPER_BIT ) != 0,
            ( bot.flags & Object::HIT_BIT ) != 0,
            ( bot.flags & Object::FRICTING_BIT ) != 0,
            ( bot.flags & Object::IN_WATER_BIT ) != 0,
            ( bot.flags & Object::ON_SLICK_BIT ) != 0,
            ( bot.flags & Object::ON_LADDER_BIT ) != 0,
            !collider.test( bot, &bot ) );

      if( taggedObj != -1 && world.objects[taggedObj] != null ) {
        const Dynamic &obj = *static_cast<const Dynamic*>( world.objects[taggedObj] );

        print( 5, -5 - textHeight * 5, "gobj.vel(%.2f %.2f %.2f) gobj.mom(%.2f %.2f %.2f)",
               obj.velocity.x, obj.velocity.y, obj.velocity.z,
               obj.momentum.x, obj.momentum.y, obj.momentum.z );

        print( 5, -5 - textHeight * 6, "d %d fl %d lw %d up %d h %d fr %d iw %d s %d",
               ( obj.flags & Object::DISABLED_BIT ) != 0,
               ( obj.flags & Object::ON_FLOOR_BIT ) != 0,
               obj.lower,
               ( bot.flags & Object::UPPER_BIT ) != 0,
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
