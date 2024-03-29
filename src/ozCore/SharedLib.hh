/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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
#define OZ_DL_DECLARE(func) \
  decltype(::func)* func;

/**
 * @def OZ_DL_DEFINE
 * Define pointer to function (initially `nullptr`) for a given function definition.
 */
#define OZ_DL_DEFINE(func) \
  decltype(::func)* func = nullptr;

/**
 * @def OZ_DL_LOAD
 * Get address of the requested function in a shared library.
 *
 * On error, `System::error()` is invoked.
 */
#define OZ_DL_LOAD(l, func) \
  func = reinterpret_cast<decltype(func)>(l.get(#func)); \
  if (func == nullptr) { \
    OZ_ERROR("Failed to link function: " #func); \
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
#ifdef __native_client__
  static constexpr bool IS_SUPPORTED = false;
#else
  static constexpr bool IS_SUPPORTED = true;
#endif

  /**
   * Generic function type to avoid casts between pointer-to-function and pointer-to-object.
   */
  using Function = void ();

private:

  void* handle_ = nullptr; ///< Internal library handle.

public:

  /**
   * Create an empty instance, no library is opened.
   */
  SharedLib() = default;

  /**
   * Open a shared library with a given (file) name.
   */
  explicit SharedLib(const char* name);

  /**
   * Destructor, closes the library if opened.
   */
  ~SharedLib();

  /**
   * No copying.
   */
  SharedLib(const SharedLib&) = delete;

  /**
   * Move constructor.
   */
  SharedLib(SharedLib&& other) noexcept;

  /**
   * No copying.
   */
  SharedLib& operator=(const SharedLib&) = delete;

  /**
   * Move operator.
   */
  SharedLib& operator=(SharedLib&& other) noexcept;

  /**
   * True iff successfully opened.
   */
  OZ_ALWAYS_INLINE
  bool isOpened() const noexcept
  {
    return handle_ != nullptr;
  }

  /**
   * Obtain a pointer to the requested function from thr library (`nullptr` on error).
   */
  Function* get(const char* symbol) const;

  /**
   * Close the library if opened.
   */
  void close();

};

}
