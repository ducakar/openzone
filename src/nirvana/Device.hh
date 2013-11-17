/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file nirvana/Device.hh
 */

#pragma once

#include <nirvana/common.hh>

namespace oz
{

class Device
{
  public:

    static const int MEMO_BIT     = 0x0001;
    static const int COMPUTER_BIT = 0x0002;

    typedef Device* CreateFunc( int id, InputStream* is );

    int flags;

  protected:

    explicit Device( int flags_ ) :
      flags( flags_ )
    {}

    virtual void onUse( const Bot* user );
    virtual void onUpdate();

  public:

    virtual ~Device();

    virtual const char* type() const = 0;

    virtual const char* getMemo() const;

    virtual void write( OutputStream* os ) const;

};

}
