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
 * @file client/NaClDownloader.hh
 */

#pragma once

#ifdef __native_client__

#include <client/common.hh>

namespace pp
{

class URLLoader;

};

namespace oz
{
namespace client
{

class NaClDownloader
{
  private:

    Semaphore      semaphore;
    BufferStream   buffer;
    pp::URLLoader* loader;
    const char*    url;
    volatile float downloadProgress;

    static void readCallback( void* data, int result );
    static void beginCallback( void* data, int result );

  public:

    ~NaClDownloader();

    bool isComplete() const;
    float progress() const;

    void begin( const char* url );

    /**
     * Get (with move semantics) buffer with downloaded contents.
     */
    BufferStream take();

};

}
}

#endif
