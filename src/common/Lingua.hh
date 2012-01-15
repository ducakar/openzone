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
 * @file common/Lingua.hh
 */

#pragma once

#include "oz/oz.hh"

// we need to define this macro because xgettext is not able to extract strings otherwise
#define OZ_GETTEXT( s ) \
  oz::lingua.get( s )

namespace oz
{

class Lingua
{
  private:

    struct Message
    {
      String   original;
      String   translation;
      Message* next;

      OZ_PLACEMENT_POOL_ALLOC( Message, 256 );
    };

    static String locale;

    Message**     messages;
    int           nMessages;
    Pool<Message> msgPool;

  public:

    Lingua();

    const char* get( const char* message ) const;

    bool initDomain( const char* domain );
    bool init( const char* locale );
    void free();

};

extern Lingua lingua;

}
