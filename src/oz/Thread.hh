/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file oz/Thread.hh
 *
 * Thread class.
 */

#pragma once

#include "common.hh"

namespace oz
{

// Internal structure for thread description.
struct ThreadDesc;

/**
 * %Thread.
 *
 * @ingroup oz
 */
class Thread
{
  public:

    /// %Thread's main function.
    typedef void Main( void* data );

  private:

    /// %Thread descriptor.
    ThreadDesc* descriptor;

  public:

    /**
     * Create instance.
     */
    Thread() :
      descriptor( null )
    {}

    /**
     * No copying.
     */
    Thread( const Thread& ) = delete;

    /**
     * Move constructor, transfers ownership.
     */
    Thread( Thread&& t ) :
      descriptor( t.descriptor )
    {
      t.descriptor = null;
    }

    /**
     * No copying.
     */
    Thread& operator = ( const Thread& ) = delete;

    /**
     * Move operator, transfers ownership.
     */
    Thread& operator = ( Thread&& t )
    {
      descriptor   = t.descriptor;
      t.descriptor = null;
      return *this;
    }

    /**
     * True iff thread has been started but not yet joined.
     */
    bool isValid() const
    {
      return descriptor != null;
    }

    /**
     * Run thread.
     *
     * @param main pointer to thread's main function.
     */
    void start( Main* main, void* data = null );

    /**
     * Wait for thread to finish execution.
     */
    void join();

};

}
