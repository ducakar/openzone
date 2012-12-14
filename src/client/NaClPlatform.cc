/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/NaClPlatform.cc
 */

#ifdef __native_client__

#include <stable.hh>
#include <client/NaClPlatform.hh>

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/core.h>
#include <ppapi/cpp/graphics_3d.h>

namespace oz
{
namespace client
{

static SpinLock     messageLock;
static List<String> messageQueue;
static pp::Core*    core;

Semaphore NaClPlatform::mainCallSemaphore;

int       NaClPlatform::width    = 0;
int       NaClPlatform::height   = 0;

float     NaClPlatform::moveX    = 0.0f;
float     NaClPlatform::moveY    = 0.0f;
float     NaClPlatform::moveZ    = 0.0f;
float     NaClPlatform::moveW    = 0.0f;

bool      NaClPlatform::hasFocus = false;

bool NaClPlatform::isMainThread()
{
  return core->IsMainThread();
}

void NaClPlatform::call( Callback* callback, void* caller )
{
  core->CallOnMainThread( 0, pp::CompletionCallback( callback, caller ) );
  mainCallSemaphore.wait();
}

void NaClPlatform::post( const char* message )
{
  OZ_MAIN_CALL( const_cast<char*>( message ), {
    const char* message = reinterpret_cast<const char*>( _this );

    System::instance->PostMessage( pp::Var( message ) );
  } )
}

String NaClPlatform::poll()
{
  messageLock.lock();
  String s = messageQueue.isEmpty() ? String() : messageQueue.popFirst();
  messageLock.unlock();

  return s;
}

void NaClPlatform::push( const char* message )
{
  messageLock.lock();
  messageQueue.pushLast( message );
  messageLock.unlock();
}

void NaClPlatform::init()
{
  mainCallSemaphore.init();

  core = pp::Module::Get()->core();
}

void NaClPlatform::destroy()
{
  messageQueue.clear();
  messageQueue.deallocate();

  mainCallSemaphore.destroy();
}

}
}

#endif
