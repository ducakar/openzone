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
 * @file nirvana/Nirvana.hh
 */

#pragma once

#include "nirvana/Device.hh"
#include "nirvana/Mind.hh"

namespace oz
{
namespace nirvana
{

class Nirvana
{
  private:

    // mind is only updated once per UPDATE_INTERVAL frames (maybe less, depends on when it was
    // added to the list)
    // to force update, set Mind::FORCE_UPDATE_BIT
    static const int UPDATE_INTERVAL = 32;

    int updateModulo;

  public:

    HashString<Device::CreateFunc*, 32> deviceClasses;

    HashIndex<Device*, 8191> devices;
    HashIndex<Mind*, 12281>  minds;

    void sync();
    void update();

    void read( InputStream* istream );
    void write( BufferStream* ostream ) const;

    void load();
    void unload();

    void init();
    void free();

};

extern Nirvana nirvana;

}
}
