/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/SharedLib.hh
 *
 * SharedLib class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Class for loading shared libraries.
 */
class SharedLib
{
  public:

    /**
     * True iff platform supports shared libraries.
     */
    static const bool SUPPORTED;

    /**
     * Generic method type to avoid casts between pointer-to-function and pointer-to-object.
     */
    typedef void Method();

  private:

    void* handle; ///< Internal handle to the library.

  public:

    /**
     * Create uninitialised instance.
     */
    explicit SharedLib() :
      handle( nullptr )
    {}

    /**
     * Destructor, closes library if still opened.
     */
    ~SharedLib()
    {
      close();
    }

    /**
     * Move constructor, transfers ownership.
     */
    SharedLib( SharedLib&& l ) :
      handle( l.handle )
    {
      l.handle = nullptr;
    }

    /**
     * Move operator, transfers ownership.
     */
    SharedLib& operator = ( SharedLib&& l )
    {
      handle   = l.handle;
      l.handle = nullptr;
      return *this;
    }

    /**
     * True iff successfully opened.
     */
    bool isOpened() const
    {
      return handle != nullptr;
    }

    /**
     * Obtain pointer to the requested function in the library (`nullptr` on error).
     */
    Method* get( const char* symbol ) const;

    /**
     * Open shared library with the given (file) name.
     */
    bool open( const char* name );

    /**
     * Close library if opened.
     */
    void close();

};

}
