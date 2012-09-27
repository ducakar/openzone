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

/**
 * %Thread.
 */
class Thread
{
  public:

    /// %Thread's main function.
    typedef void Main( void* data );

  private:

    struct Descriptor;

    /// %Thread descriptor.
    Descriptor* descriptor;

  public:

    /**
     * Get current thread's name.
     *
     * If thread was not started via start() method, `nullptr` is returned.
     */
    static const char* name();

    /**
     * Create uninitialised instance.
     */
    Thread() :
      descriptor( nullptr )
    {}

    /**
     * Destructor, thread must be either uninitialised, joined or detached.
     */
    ~Thread()
    {
      hard_assert( descriptor == nullptr );
    }

    /**
     * Move constructor, transfers ownership.
     */
    Thread( Thread&& t ) :
      descriptor( t.descriptor )
    {
      t.descriptor = nullptr;
    }

    /**
     * Move operator, transfers ownership.
     */
    Thread& operator = ( Thread&& t )
    {
      descriptor   = t.descriptor;
      t.descriptor = nullptr;
      return *this;
    }

    /**
     * True iff thread has been started but not yet joined.
     */
    bool isValid() const
    {
      return descriptor != nullptr;
    }

    /**
     * Create a new thread and run it. Thread must be in uninitialised state.
     *
     * @param name thread name (string is copied to an internal buffer).
     * @param main pointer to thread's main function.
     * @param data pointer to user data, passed to thread's main function.
     */
    void start( const char* name, Main* main, void* data );

    /**
     * Detach a started thread and return Thread object into uninitialised state.
     */
    void detach();

    /**
     * Wait for a thread to finish execution and return Thread object into uninitialised state.
     */
    void join();

};

}
