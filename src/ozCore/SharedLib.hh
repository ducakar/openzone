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
 * `SharedLib` class.
 */

#pragma once

#include "common.hh"

/**
 * @def OZ_DL_DECLARE
 * Declare pointer to function for a given function definition.
 */
#define OZ_DL_DECLARE( func ) \
  decltype( ::func )* func

/**
 * @def OZ_DL_DEFINE
 * Define pointer to function (initially `nullptr`) for a given function definition.
 */
#define OZ_DL_DEFINE( func ) \
  decltype( ::func )* func = nullptr

/**
 * @def OZ_DL_LOAD
 * Get address of the requested function in a shared library.
 *
 * On error, `System::error()` is invoked.
 */
#define OZ_DL_LOAD( l, func ) \
  func = reinterpret_cast< decltype( func ) >( l.get( #func ) ); \
  if( func == nullptr ) { \
    OZ_ERROR( "Failed to link function: " #func ); \
  }

namespace oz
{

/**
 * Dynamic linker interface for linking shared libraries.
 */
class SharedLib
{
public:

  /**
   * True iff platform supports dynamic linking.
   */
  static const bool IS_SUPPORTED;

  /**
   * Generic method type to avoid casts between pointer-to-function and pointer-to-object.
   */
  typedef void Method();

private:

  void* handle; ///< Internal library handle.

public:

  /**
   * Create uninitialised instance.
   */
  explicit SharedLib() :
    handle( nullptr )
  {}

  /**
   * Destructor, closes the library if opened.
   */
  ~SharedLib()
  {
    close();
  }

  /**
   * Move constructor.
   */
  SharedLib( SharedLib&& l ) :
    handle( l.handle )
  {
    l.handle = nullptr;
  }

  /**
   * Move operator.
   */
  SharedLib& operator = ( SharedLib&& l )
  {
    if( &l == this ) {
      return *this;
    }

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
   * Obtain a pointer to the requested function from thr library (`nullptr` on error).
   */
  Method* get( const char* symbol ) const;

  /**
   * Open a shared library with a given (file) name.
   */
  bool open( const char* name );

  /**
   * Close the library if opened.
   */
  void close();

};

}
