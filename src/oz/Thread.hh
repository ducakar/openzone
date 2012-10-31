/*
 * liboz - OpenZone Core Library.
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

    Descriptor* descriptor; ///< %Thread descriptor.

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
    explicit Thread() :
      descriptor( nullptr )
    {}

    /**
     * Destructor, all started threads must be detached or joined.
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
     * Create a new thread and run it.
     *
     * When a new thread is started it is attached to the Thread object that started it. `detach()`
     * or `join()` must be called afterwards, before the Thread object destruction or starting
     * another thread.
     *
     * @param name thread name (string is copied to an internal buffer).
     * @param main pointer to the thread's main function.
     * @param data pointer to user data, passed to the thread's main function.
     */
    void start( const char* name, Main* main, void* data );

    /**
     * Detach a started thread.
     */
    void detach();

    /**
     * Wait for the started thread to finish execution.
     */
    void join();

};

}
