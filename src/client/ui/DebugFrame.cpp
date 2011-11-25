/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/ui/DebugFrame.cpp
 */

#include "stable.hpp"

#include "client/ui/DebugFrame.hpp"

#include "matrix/Orbis.hpp"
#include "matrix/Collider.hpp"
#include "client/Camera.hpp"

namespace oz
{
namespace client
{
namespace ui
{

DebugFrame::DebugFrame() :
    Frame( 0, -38, 560, 10 + 7 * ( Font::INFOS[Font::MONO].height + 2 ), gettext( "Debug" ) )
{
  flags = PINNED_BIT;

  x = ( Area::uiWidth - width ) / 2;

  int height = textHeight + 2;

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

  camPosRot.setText( "cam.p(%+.2f %+.2f %+.2f) cam.relRot(%+.2f %+.2f)",
                     camera.p.x, camera.p.y, camera.p.z,
                     Math::deg( camera.h ), Math::deg( camera.v ) );
  camPosRot.draw( this );

  if( camera.bot != -1 ) {
    const Bot* bot = static_cast<const Bot*>( camera.botObj );

    botPosRot.setText( "bot.pos(%+.2f %+.2f %+.2f) bot.rot(%+.2f %+.2f)",
                       bot->p.x, bot->p.y, bot->p.z,
                       Math::deg( bot->h ), Math::deg( bot->v ) );
    botPosRot.draw( this );

    botVelMom.setText( "bot.vel(%+.2f %+.2f %+.2f) bot.mom(%+.2f %+.2f %+.2f) bot.wd %+.2f",
                       bot->velocity.x, bot->velocity.y, bot->velocity.z,
                       bot->momentum.x, bot->momentum.y, bot->momentum.z,
                       bot->depth );
    botVelMom.draw( this );

    botFlagsState.setText( "d %d fl %d lw %d h %d fr %d b %d iw %d s %d ld %d ovlp %d sr %+.3f",
                           ( bot->flags & Object::DISABLED_BIT ) != 0,
                           ( bot->flags & Object::ON_FLOOR_BIT ) != 0,
                           bot->lower,
                           ( bot->flags & Object::HIT_BIT ) != 0,
                           ( bot->flags & Object::FRICTING_BIT ) != 0,
                           ( bot->flags & Object::BELOW_BIT ) != 0,
                           ( bot->flags & Object::IN_WATER_BIT ) != 0,
                           ( bot->flags & Object::ON_SLICK_BIT ) != 0,
                           ( bot->flags & Object::ON_LADDER_BIT ) != 0,
                           collider.overlaps( bot, bot ),
                           bot->stepRate * 10.0f );
    botFlagsState.draw( this );
  }

  if( camera.tagged != -1 && ( camera.taggedObj->flags & Object::DYNAMIC_BIT ) ) {
    const Dynamic* dyn = static_cast<const Dynamic*>( camera.taggedObj );

    tagPos.setText( "tagDyn.pos(%+.2f %+.2f %+.2f)", dyn->p.x, dyn->p.y, dyn->p.z );
    tagPos.draw( this );

    tagVelMom.setText( "tagDyn.vel(%+.2f %+.2f %+.2f) tagDyn.mom(%+.2f %+.2f %+.2f)",
                       dyn->velocity.x, dyn->velocity.y, dyn->velocity.z,
                       dyn->momentum.x, dyn->momentum.y, dyn->momentum.z );
    tagVelMom.draw( this );

    tagFlags.setText( "d %d fl %d lw %d h %d fr %d b %d iw %d s %d ld %d",
                      ( dyn->flags & Object::DISABLED_BIT ) != 0,
                      ( dyn->flags & Object::ON_FLOOR_BIT ) != 0,
                      dyn->lower,
                      ( dyn->flags & Object::HIT_BIT ) != 0,
                      ( dyn->flags & Object::FRICTING_BIT ) != 0,
                      ( dyn->flags & Object::BELOW_BIT ) != 0,
                      ( dyn->flags & Object::IN_WATER_BIT ) != 0,
                      ( dyn->flags & Object::ON_SLICK_BIT ) != 0,
                      ( dyn->flags & Object::ON_LADDER_BIT ) != 0 );
    tagFlags.draw( this );
  }
}

}
}
}
