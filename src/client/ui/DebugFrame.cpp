/*
 *  DebugFrame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/DebugFrame.hpp"

#ifndef NDEBUG

#include "matrix/Orbis.hpp"
#include "matrix/Collider.hpp"
#include "client/Camera.hpp"

#include <GL/gl.h>

namespace oz
{
namespace client
{
namespace ui
{

  DebugFrame::DebugFrame() :
      Frame( 8, -8, 560, 10 + 6 * ( Font::INFOS[Font::MONO].height + 2 ), OZ_GETTEXT( "Debug" ) )
  {
    flags = PINNED_BIT;

    setFont( Font::MONO );
  }

  void DebugFrame::onDraw()
  {
    Frame::onDraw();

    setFont( Font::MONO );

    int height = textHeight + 2;

    print( 5, 5 + height * 5, ALIGN_NONE,
           "cam.p(%.2f %.2f %.2f) cam.relRot(%.2f %.2f)",
           camera.p.x, camera.p.y, camera.p.z, camera.h / Math::TAU, camera.v / Math::TAU );

    if( camera.bot != -1 ) {
      const Bot* bot = static_cast<const Bot*>( camera.botObj );

      print( 5, 5 + height * 4, ALIGN_NONE,
             "bot.pos(%.2f %.2f %.2f) bot.rot(%.2f %.2f)",
             bot->p.x, bot->p.y, bot->p.z,
             bot->h / Math::TAU, bot->v / Math::TAU );

      print( 5, 5 + height * 3, ALIGN_NONE,
             "bot.vel(%.2f %.2f %.2f) bot.mom(%.2f %.2f %.2f) bot.wd %.2f",
             bot->velocity.x, bot->velocity.y, bot->velocity.z,
             bot->momentum.x, bot->momentum.y, bot->momentum.z,
             bot->depth );

      print( 5, 5 + height * 2, ALIGN_NONE,
             "d %d fl %d lw %d up %d h %d fr %d iw %d s %d ld %d ovlp %d sR %.3f",
             ( bot->flags & Object::DISABLED_BIT ) != 0,
             ( bot->flags & Object::ON_FLOOR_BIT ) != 0,
             bot->lower,
             ( bot->flags & Object::UPPER_BIT ) != 0,
             ( bot->flags & Object::HIT_BIT ) != 0,
             ( bot->flags & Object::FRICTING_BIT ) != 0,
             ( bot->flags & Object::IN_WATER_BIT ) != 0,
             ( bot->flags & Object::ON_SLICK_BIT ) != 0,
             ( bot->flags & Object::ON_LADDER_BIT ) != 0,
             collider.overlaps( bot, bot ),
             bot->stepRate * 10.0f );
    }

    if( camera.tagged != -1 && ( camera.taggedObj->flags & Object::DYNAMIC_BIT ) ) {
      const Dynamic* dyn = static_cast<const Dynamic*>( camera.taggedObj );

      print( 5, 5 + height * 1, ALIGN_NONE,
             "tagDyn.vel(%.2f %.2f %.2f) tagDyn.mom(%.2f %.2f %.2f)",
             dyn->velocity.x, dyn->velocity.y, dyn->velocity.z,
             dyn->momentum.x, dyn->momentum.y, dyn->momentum.z );
      print( 5, 5 + height * 0, ALIGN_NONE,
             "d %d fl %d lw %d up %d h %d fr %d iw %d s %d",
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

#endif
