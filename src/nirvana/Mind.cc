/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

#include <nirvana/Mind.hh>

#include <nirvana/LuaNirvana.hh>
#include <matrix/Bot.hh>

namespace oz
{

Mind::Mind() :
  flags( 0 ), side( 0 ), botObj( nullptr )
{}

Mind::Mind( int bot_ ) :
  flags( 0 ), side( 0 ), bot( bot_ ), botObj( nullptr )
{
  luaNirvana.registerMind( bot );
}

Mind::Mind( int bot_, InputStream* is ) :
  bot( bot_ ), botObj( nullptr )
{
  flags = is->readInt();
  side  = is->readInt();
}

Mind::~Mind()
{
  if( bot >= 0 ) {
    luaNirvana.unregisterMind( bot );
  }
}

Mind::Mind( Mind&& m ) :
  flags( m.flags ), side( m.side ), bot( m.bot ), botObj( nullptr )
{
  m.bot   = -1;
  m.flags = 0;
  m.side  = 0;
}

Mind& Mind::operator = ( Mind&& m )
{
  if( &m == this ) {
    return *this;
  }

  flags  = m.flags;
  side   = m.side;
  bot    = m.bot;
  botObj = m.botObj;

  m.flags  = 0;
  m.side   = 0;
  m.bot    = -1;
  m.botObj = nullptr;

  return *this;
}

void Mind::update()
{
  hard_assert( orbis.obj( bot ) != nullptr && ( orbis.obj( bot )->flags & Object::BOT_BIT ) );

  botObj = static_cast<Bot*>( orbis.obj( bot ) );

  if( !botObj->mindFunc.isEmpty() && !( botObj->state & Bot::DEAD_BIT ) ) {
    flags &= ~FORCE_UPDATE_BIT;
    botObj->actions = 0;

    luaNirvana.mindCall( botObj->mindFunc, this, botObj );
  }
}

void Mind::write( OutputStream* os ) const
{
  os->writeInt( flags );
  os->writeInt( side );
}

}
