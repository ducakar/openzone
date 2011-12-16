/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file nirvana/Mind.cc
 */

#include "stable.hh"

#include "nirvana/Mind.hh"

#include "nirvana/Lua.hh"
#include "matrix/Bot.hh"

namespace oz
{
namespace nirvana
{

Pool<Mind, 1024> Mind::pool;

Mind::Mind( int bot_ ) : flags( 0 ), bot( bot_ )
{
  lua.registerMind( bot );
}

Mind::Mind( int bot_, InputStream* istream ) : bot( bot_ )
{
  flags = istream->readInt();
}

Mind::~Mind()
{
  lua.unregisterMind( bot );
}

void Mind::update()
{
  hard_assert( orbis.objects[bot] != null );
  hard_assert( orbis.objects[bot]->flags & Object::BOT_BIT );

  Bot* bot = static_cast<Bot*>( orbis.objects[this->bot] );

  if( !bot->mindFunc.isEmpty() && !( bot->state & Bot::DEAD_BIT ) ) {
    flags &= ~FORCE_UPDATE_BIT;
    bot->actions = 0;
    lua.mindCall( bot->mindFunc, bot );

    if( lua.forceUpdate ) {
      flags |= FORCE_UPDATE_BIT;
    }
  }
}

void Mind::write( BufferStream* ostream ) const
{
  ostream->writeInt( flags );
}

}
}
