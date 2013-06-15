/*
 * ozEngine - OpenZone Engine Library.
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
 * @file ozEngine/Pepper.cc
 */

#include "Pepper.hh"

#ifdef __native_client__

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/core.h>
#include <ppapi/cpp/graphics_3d.h>

namespace oz
{

static SpinLock     messageLock;
static List<String> messageQueue;
static pp::Core*    core;

Semaphore Pepper::mainCallSemaphore;

int       Pepper::width    = 0;
int       Pepper::height   = 0;

float     Pepper::moveX    = 0.0f;
float     Pepper::moveY    = 0.0f;
float     Pepper::moveZ    = 0.0f;
float     Pepper::moveW    = 0.0f;

bool      Pepper::hasFocus = false;

bool Pepper::isMainThread()
{
  return core->IsMainThread();
}

void Pepper::mainCall( Callback* callback, void* data )
{
  core->CallOnMainThread( 0, pp::CompletionCallback( callback, data ) );
}

void Pepper::post( const char* message )
{
  pp::Var var( message );

  MainCall() << [&]()
  {
    System::instance->PostMessage( var );
  };
}

String Pepper::pop()
{
  messageLock.lock();
  String s = messageQueue.isEmpty() ? String() : messageQueue.popFirst();
  messageLock.unlock();

  return s;
}

void Pepper::push( const char* message )
{
  messageLock.lock();
  messageQueue.pushLast( message );
  messageLock.unlock();
}

void Pepper::init()
{
  mainCallSemaphore.init();

  core = pp::Module::Get()->core();
}

void Pepper::destroy()
{
  messageQueue.clear();
  messageQueue.deallocate();

  mainCallSemaphore.destroy();
}

}

#endif
