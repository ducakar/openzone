/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/Java.hh
 *
 * `Java` class and JNI facilities.
 */

#pragma once

#include "common.hh"

#if defined(__ANDROID__) || defined(DOXYGEN_IGNORE)

#include <jni.h>

/**
 * @def OZ_JAVA_ENTRY_POINT
 * Implement entry point for a Java JNI application.
 *
 * `name` parameter should be your application's entry point function. For Android SDL applications
 * it should be something like `Java_com_example_myapp_SDLActivity_nativeInit`.
 */
#define OZ_JAVA_ENTRY_POINT(name) \
  extern "C" \
  void name(JNIEnv* env, jclass clazz); \
  void name(JNIEnv* env, jclass clazz) \
  { \
    oz::Java::application(env, clazz); \
  }

/**
 * Main function/entry point for Java JNI applications.
 *
 * For a Java JNI application, you must implement this function and put `OZ_JAVA_ENTRY_POINT()`
 * macro in a `.cc` file (out of any namespace). An empty string is passed as the zero argument
 * (i.e. `argc = 1` and `argv = { "" }`).
 */
int javaMain(int argc, char** argv);

namespace oz
{

/**
 * Java JNI entry point implementation and instance access.
 */
class Java
{
public:

  /**
   * Forbid instances.
   */
  Java() = delete;

  /**
   * Return Java VM handle or `nullptr` if not initialised.
   *
   * Application must use `OZ_JAVA_ENTRY_POINT()` to initialise this handle.
   */
  static JavaVM* vm();

  /**
   * Helper function for `OZ_JAVA_ENTRY_POINT()` macro.
   */
  static void application(JNIEnv* env, jclass clazz);

};

}

#else

#define OZ_JAVA_ENTRY_POINT(name)

#endif
