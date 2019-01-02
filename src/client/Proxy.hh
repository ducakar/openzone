/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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
 * @file client/Proxy.hh
 */

#pragma once

#include <client/common.hh>

namespace oz::client
{

class Proxy
{
public:

  virtual ~Proxy();

  virtual void begin()                 = 0;
  virtual void end()                   = 0;

  virtual void prepare()               = 0;
  virtual void update()                = 0;

  virtual void reset()                 = 0;

  virtual void read(Stream* is)        = 0;
  virtual void read(const Json& json)  = 0;

  virtual void write(Stream* os) const = 0;
  virtual Json write() const           = 0;

};

}
