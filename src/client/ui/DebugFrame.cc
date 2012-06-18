/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/ui/DebugFrame.cc
 */

#include "stable.hh"

#include "client/ui/DebugFrame.hh"

#include "matrix/Collider.hh"

#include "client/Camera.hh"

namespace oz
{
namespace client
{
namespace ui
{

DebugFrame::DebugFrame() :
  Frame( 0, -38, 560, 10 + 7 * ( Font::INFOS[Font::MONO].height + 2 ), OZ_GETTEXT( "Debug" ) )
{
  flags = PINNED_BIT;

  x = ( Area::uiWidth - width ) / 2;

  int height = Font::INFOS[Font::MONO].height + 2;

  camPosRot.set( 5, 5 + height * 6, ALIGN_NONE, Font::MONO, " " );

  botPosRot.set( 5, 5 + height * 5, ALIGN_NONE, Font::MONO, " " );
  botVelMom.set( 5, 5 + height * 4, ALIGN_NONE, Font::MONO, " " );
  botFlagsState.set( 5, 5 + height * 3, ALIGN_NONE, Font::MONO, " " );

  tagPos.set( 5, 5 + height * 2, ALIGN_NONE, Font::MONO, " " );
  tagVelMom.set( 5, 5 + height * 1, ALIGN_NONE, Font::MONO, " " );
  tagFlags.set( 5, 5 + height * 0, ALIGN_NONE, Font::MONO, " " );
}

void DebugFrame::onDraw()
{
  Frame::onDraw();

  camPosRot.setText( "cam.p(%+.2f %+.2f %+.2f) cam.rot(%+.2f %+.2f %+.2f %+.2f)",
                     camera.p.x, camera.p.y, camera.p.z,
                     camera.rot.x, camera.rot.y, camera.rot.z, camera.rot.w );
  camPosRot.draw( this, true );

  if( camera.bot >= 0 ) {
    const Bot* bot = static_cast<const Bot*>( camera.botObj );

    botPosRot.setText( "bot.pos(%+.2f %+.2f %+.2f) bot.rot(%+.2f %+.2f)",
                       bot->p.x, bot->p.y, bot->p.z,
                       Math::deg( bot->h ), Math::deg( bot->v ) );
    botPosRot.draw( this, true );

    botVelMom.setText( "bot.vel(%+.2f %+.2f %+.2f) bot.mom(%+.2f %+.2f %+.2f) bot.wd %+.2f",
                       bot->velocity.x, bot->velocity.y, bot->velocity.z,
                       bot->momentum.x, bot->momentum.y, bot->momentum.z,
                       bot->depth );
    botVelMom.draw( this, true );

    botFlagsState.setText( "d %d fl %d lw %d fr %d bl %d lq %d sl %d ld %d ovlp %d sr %+.3f",
                           ( bot->flags & Object::DISABLED_BIT ) != 0,
                           ( bot->flags & Object::ON_FLOOR_BIT ) != 0,
                           bot->lower,
                           ( bot->flags & Object::FRICTING_BIT ) != 0,
                           ( bot->flags & Object::BELOW_BIT ) != 0,
                           ( bot->flags & Object::IN_LIQUID_BIT ) != 0,
                           ( bot->flags & Object::ON_SLICK_BIT ) != 0,
                           ( bot->flags & Object::ON_LADDER_BIT ) != 0,
                           collider.overlaps( bot, bot ),
                           bot->stairRate );
    botFlagsState.draw( this, true );
  }

  if( camera.object >= 0 && ( camera.objectObj->flags & Object::DYNAMIC_BIT ) ) {
    const Dynamic* dyn = static_cast<const Dynamic*>( camera.objectObj );

    tagPos.setText( "tagDyn.pos(%+.2f %+.2f %+.2f)", dyn->p.x, dyn->p.y, dyn->p.z );
    tagPos.draw( this, true );

    tagVelMom.setText( "tagDyn.vel(%+.2f %+.2f %+.2f) tagDyn.mom(%+.2f %+.2f %+.2f)",
                       dyn->velocity.x, dyn->velocity.y, dyn->velocity.z,
                       dyn->momentum.x, dyn->momentum.y, dyn->momentum.z );
    tagVelMom.draw( this, true );

    tagFlags.setText( "d %d fl %d lw %d fr %d bl %d lq %d sl %d ld %d",
                      ( dyn->flags & Object::DISABLED_BIT ) != 0,
                      ( dyn->flags & Object::ON_FLOOR_BIT ) != 0,
                      dyn->lower,
                      ( dyn->flags & Object::FRICTING_BIT ) != 0,
                      ( dyn->flags & Object::BELOW_BIT ) != 0,
                      ( dyn->flags & Object::IN_LIQUID_BIT ) != 0,
                      ( dyn->flags & Object::ON_SLICK_BIT ) != 0,
                      ( dyn->flags & Object::ON_LADDER_BIT ) != 0 );
    tagFlags.draw( this, true );
  }
}

}
}
}
