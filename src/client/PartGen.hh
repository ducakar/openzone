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
 * @file client/PartGen.hh
 */

#pragma once

#include <client/PartClass.hh>

namespace oz
{
namespace client
{

class PartGen
{
  friend class Chain<PartGen>;

public:

  static const int UPDATED_BIT = 0x01;

private:

  struct Part;

private:

  PartGen*     next[1];

  Mat4         transf;
  PartClass*   clazz;
  DArray<Part> parts;
  int          flags;

private:

  void draw() const;

public:

  static void drawScheduled();
  static void clearScheduled();

  static void dellocate();

  explicit PartGen( InputStream* is );
  ~PartGen();

  void update();
  void schedule();

};

}
}
