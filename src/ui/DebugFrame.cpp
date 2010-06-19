/*
 *  DebugFrame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.hpp"

#include "ui/DebugFrame.hpp"

#include "matrix/World.hpp"
#include "matrix/Collider.hpp"
#include "client/Camera.hpp"

#include <SDL_opengl.h>

namespace oz
{
namespace client
{
namespace ui
{

  DebugFrame::DebugFrame() : Frame( 5, -15 - 6 * font.monoHeight, 480, 10 + 6 * font.monoHeight )
  {
    setFont( MONO );
    setFontColor( 0xff, 0xff, 0xff );
  }

  void DebugFrame::onDraw()
  {
    Frame::onDraw();

    print( 5, -5 - textHeight * 1, "cam.p(%.2f %.2f %.2f) cam.relRot(%.2f %.2f)",
           camera.p.x, camera.p.y, camera.p.z, camera.h, camera.v );

    if( camera.bot != -1 ) {
      const Bot* bot = static_cast<const Bot*>( camera.botObj );

      print( 5, -5 - textHeight * 2, "bot.pos(%.2f %.2f %.2f) bot.rot(%.2f %.2f)",
            bot->p.x, bot->p.y, bot->p.z,
            bot->h, bot->v );

      print( 5, -5 - textHeight * 3, "bot.vel(%.2f %.2f %.2f) bot.mom(%.2f %.2f %.2f) bot.wd %.2f",
             bot->velocity.x, bot->velocity.y, bot->velocity.z,
             bot->momentum.x, bot->momentum.y, bot->momentum.z,
             bot->depth );

      print( 5, -5 - textHeight * 4, "d %d fl %d lw %d up %d h %d fr %d iw %d s %d ld %d ovlp %d sR %.3f",
            ( bot->flags & Object::DISABLED_BIT ) != 0,
            ( bot->flags & Object::ON_FLOOR_BIT ) != 0,
            bot->lower,
            ( bot->flags & Object::UPPER_BIT ) != 0,
            ( bot->flags & Object::HIT_BIT ) != 0,
            ( bot->flags & Object::FRICTING_BIT ) != 0,
            ( bot->flags & Object::IN_WATER_BIT ) != 0,
            ( bot->flags & Object::ON_SLICK_BIT ) != 0,
            ( bot->flags & Object::ON_LADDER_BIT ) != 0,
            !collider.test( *bot, bot ),
            bot->stepRate * 10.0f );
    }

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
