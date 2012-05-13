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
 * @file oz/Mutex.hh
 *
 * Mutex class.
 */

#pragma once

#include "common.hh"

namespace oz
{

// Internal structure for mutex description.
struct MutexDesc;

/**
 * %Mutex.
 *
 * @ingroup oz
 */
class Mutex
{
  private:

    /// %Mutex descriptor.
    MutexDesc* descriptor;

  public:

    /**
     * Create uninitialised instance.
     */
    Mutex() :
      descriptor( null )
    {}

    /**
     * Destructor, destroys mutex if initialised.
     */
    ~Mutex()
    {
      if( descriptor != null ) {
        destroy();
      }
    }

    /**
     * No copying.
     */
    Mutex( const Mutex& ) = delete;

    /**
     * Move constructor, transfers ownership.
     */
    Mutex( Mutex&& m ) :
      descriptor( m.descriptor )
    {
      m.descriptor = null;
    }

    /**
     * No copying.
     */
    Mutex& operator = ( const Mutex& ) = delete;

    /**
     * Move operator, transfers ownership.
     */
    Mutex& operator = ( Mutex&& m )
    {
      descriptor   = m.descriptor;
      m.descriptor = null;
      return *this;
    }

    /**
     * True iff initialised.
     */
    bool isValid() const
    {
      return descriptor != null;
    }

    /**
     * Lock mutex.
     */
    void lock() const;

    /**
     * Try to lock mutex.
     */
    bool tryLock() const;

    /**
     * Unlock mutex.
     */
    void unlock() const;

    /**
     * Initialise mutex.
     */
    void init();

    /**
     * Destroy mutex and release resources.
     */
    void destroy();

};

}
