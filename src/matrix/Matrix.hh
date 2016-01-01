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

/**
 * @file matrix/Matrix.hh
 */

#pragma once

#include <matrix/common.hh>

namespace oz
{

class Matrix
{
private:

  static const float MAX_VELOCITY2;

  int maxStructs;
  int maxEvents;
  int maxObjects;
  int maxDynamics;
  int maxWeapons;
  int maxBots;
  int maxVehicles;
  int maxFrags;

public:

  void update();

  void read(Stream* is);
  void read(const Json& json);

  void write(Stream* os) const;
  Json write() const;

  void load();
  void unload();

  void init();
  void destroy();

};

extern Matrix matrix;

}
