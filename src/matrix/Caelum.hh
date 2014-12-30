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
 * @file matrix/Caelum.hh
 */

#pragma once

#include <matrix/common.hh>

namespace oz
{

class Caelum
{
public:

  int   id;

  float heading;
  float period;
  float time;

public:

  void reset();
  void update();

  void read(const Json& json);
  void read(InputStream* is);

  Json write() const;
  void write(OutputStream* os) const;

};

}
