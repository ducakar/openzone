/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include <nirvana/Mind.hh>

#include <nirvana/LuaNirvana.hh>
#include <matrix/Bot.hh>

namespace oz
{

bool Mind::hasCollided(const Bot* botObj)
{
  for (const Object::Event& event : botObj->events) {
    if (event.id == Object::EVENT_HIT || event.id == Object::EVENT_DAMAGE) {
      return true;
    }
  }
  return false;
}

Mind::Mind(int bot_) :
  bot(bot_)
{
  luaNirvana.registerMind(bot);
}

Mind::Mind(int bot_, Stream* is) :
  bot(bot_)
{
  flags = is->readInt();
  side  = is->readInt();
}

Mind::~Mind()
{
  if (bot != -1) {
    luaNirvana.unregisterMind(bot);
  }
}

Mind::Mind(Mind&& m) :
  flags(m.flags), side(m.side), bot(m.bot)
{
  m.flags = 0;
  m.side  = 0;
  m.bot   = -1;
}

Mind& Mind::operator=(Mind&& m)
{
  if (&m != this) {
    flags = m.flags;
    side  = m.side;
    bot   = m.bot;

    m.flags = 0;
    m.side  = 0;
    m.bot   = -1;
  }
  return *this;
}

void Mind::update(bool doRegularUpdate)
{
  Bot* botObj = orbis.obj<Bot>(bot);

  OZ_ASSERT(botObj != nullptr && (botObj->flags & Object::BOT_BIT));

  if ((flags & PLAYER_BIT) || (botObj->state & Bot::DEAD_BIT) || botObj->mind.isEmpty()) {
    return;
  }

  if (doRegularUpdate || (flags & FORCE_UPDATE_BIT) ||
      ((flags & COLLISION_UPDATE_BIT) && hasCollided(botObj)))
  {
    flags &= ~FORCE_UPDATE_BIT;
    botObj->actions = 0;

    luaNirvana.mindCall(botObj->mind, this, botObj);
  }
}

void Mind::write(Stream* os) const
{
  os->writeInt(flags);
  os->writeInt(side);
}

}
