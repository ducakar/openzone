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
 * @file nirvana/Mind.hh
 */

#pragma once

#include <nirvana/common.hh>

namespace oz
{

class Bot;

class Mind
{
public:

  // Normally, mind is only updated once in UPDATE_INTERVAL ticks.
  static const int UPDATE_INTERVAL = 32;
  // Force mind update in the next tick. Cleared after update.
  static const int FORCE_UPDATE_BIT = 0x01;
  // Force mind update when a collision occurs (in physical world).
  static const int COLLISION_UPDATE_BIT = 0x02;
  // Disabled because player is currently controlling the bot.
  static const int PLAYER_BIT = 0x04;

  Mind* prev[1];
  Mind* next[1];

  int   flags;
  int   side;
  int   bot;

  static bool hasCollided(const Bot* botObj);

public:

  Mind();
  explicit Mind(int bot);
  explicit Mind(int bot, InputStream* is);
  ~Mind();

  Mind(Mind&& m);
  Mind& operator = (Mind&& m);

  void update(bool doRegularUpdate);

  void write(OutputStream* os) const;

};

}
