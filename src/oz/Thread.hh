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
 * Internal structure for mutex description.
 */
struct MutexDesc;

/**
 * Internal structure for barrier description.
 */
struct BarrierDesc;

/**
 * Internal structure for thread description.
 */
struct ThreadDesc;

/**
 * %Mutex.
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
     * Destructor.
     */
    ~Mutex()
    {
      hard_assert( descriptor == null );
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
     * Initialise mutex.
     */
    void init();

    /**
     * Destroy mutex and release resources.
     */
    void free();

    /**
     * Lock mutex.
     */
    void lock() const;

    /**
     * Unlock mutex.
     */
    void unlock() const;

};

/**
 * High-level conditional variable wrapper.
 *
 * This class can be used if one thread must wait another thread to finish some task. E.g.:
 * @code
 * Barrier bar;
 * bar.init();
 * ...
 * bar.begin();
 * otherThread.start();
 * ...
 * bar.wait(); // Waits until secondary thread has sent signal via bar.finish() call.
 * @endcode
 */
class Barrier
{
  private:

    /// %Barrier descriptor.
    BarrierDesc* descriptor;

  public:

    /**
     * Create uninitialised instance.
     */
    Barrier() :
      descriptor( null )
    {}

    /**
     * Destructor.
     */
    ~Barrier()
    {
      hard_assert( descriptor == null );
    }

    /**
     * No copying.
     */
    Barrier( const Barrier& ) = delete;

    /**
     * Move constructor, transfers ownership.
     */
    Barrier( Barrier&& b ) :
      descriptor( b.descriptor )
    {
      b.descriptor = null;
    }

    /**
     * No copying.
     */
    Barrier& operator = ( const Barrier& ) = delete;

    /**
     * Move operator, transfers ownership.
     */
    Barrier& operator = ( Barrier&& b )
    {
      descriptor   = b.descriptor;
      b.descriptor = null;
      return *this;
    }

    /**
     * Initialise barrier.
     */
    void init();

    /**
     * Destroy barrier and release resources.
     */
    void free();

    /**
     * Enable block (set conditional variable to false).
     */
    void begin() const;

    /**
     * Release block (set conditional variable to true).
     */
    void finish() const;

    /**
     * Wait until block is released (wait for conditional variable to become true).
     */
    void wait() const;

};

/**
 * %Thread.
 */
class Thread
{
  public:

    /// %Thread's main function.
    typedef void* Main( void* );

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
     * Destructor waits for join() if the thread is still running.
     */
    ~Thread();

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
     * Run thread.
     *
     * @param main pointer to thread's main function.
     * @param param parameter to thread's main function.
     */
    void start( Main* main, void* param = null );

    /**
     * Wait for thread to finish execution.
     *
     * @param result pointer variable in which return value of thread's main function should be
     * written or null if you wish to throw away the result.
     */
    void join( void** result = null );

};

}
