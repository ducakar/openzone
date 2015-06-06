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
 * @file ozCore/Java.cc
 */

#ifdef __ANDROID__

#include "Java.hh"

extern "C"
void SDL_Android_Init(JNIEnv*, jclass);

OZ_WEAK
void SDL_Android_Init(JNIEnv*, jclass)
{}

namespace oz
{

static JavaVM* javaVM = nullptr;

JavaVM* Java::vm()
{
  return javaVM;
}

void Java::application(JNIEnv* env, jclass clazz)
{
  SDL_Android_Init(env, clazz);
  env->GetJavaVM(&javaVM);

  char  argv0[] = "";
  char* argv[]  = { argv0, nullptr };

  javaMain(1, argv);
}

}

#endif
