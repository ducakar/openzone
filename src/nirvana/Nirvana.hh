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
 * @file nirvana/Nirvana.hh
 */

#pragma once

#include <nirvana/Device.hh>
#include <nirvana/Mind.hh>

namespace oz
{

class Nirvana
{
private:

  int updateModulo;

public:

  HashMap<String, Device::CreateFunc*> deviceClasses;

  HashMap<int, Device*> devices;
  HashMap<int, Mind>    minds;

  void sync();
  void update();

  void read(Stream* is);
  void write(Stream* os) const;

  void load();
  void unload();

  void init();
  void destroy();

};

extern Nirvana nirvana;

}
