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
 * @file client/NaCl.cc
 */

#ifdef __native_client__

#include <stable.hh>
#include <client/NaCl.hh>

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/core.h>
#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/gles2/gl2ext_ppapi.h>

namespace oz
{
namespace client
{

static SpinLock       messageLock;
static List<String>   messageQueue;
static pp::Graphics3D context;

Semaphore NaCl::mainCallSemaphore;

int       NaCl::width    = 0;
int       NaCl::height   = 0;

int       NaCl::moveX    = 0;
int       NaCl::moveY    = 0;
int       NaCl::moveZ    = 0;
int       NaCl::moveW    = 0;

bool      NaCl::hasFocus = false;

bool NaCl::isMainThread()
{
  return System::core->IsMainThread();
}

void NaCl::call( Callback* callback, void* caller )
{
  System::core->CallOnMainThread( 0, pp::CompletionCallback( callback, caller ) );
  mainCallSemaphore.wait();
}

void NaCl::post( const char* message )
{
  OZ_MAIN_CALL( const_cast<char*>( message ), {
    const char* message = reinterpret_cast<const char*>( _this );

    System::instance->PostMessage( pp::Var( message ) );
  } )
}

String NaCl::poll()
{
  messageLock.lock();
  String s = messageQueue.isEmpty() ? String() : messageQueue.popFirst();
  messageLock.unlock();

  return s;
}

void NaCl::push( const char* message )
{
  messageLock.lock();
  messageQueue.pushLast( message );
  messageLock.unlock();
}

void NaCl::init()
{
  mainCallSemaphore.init();
}

void NaCl::free()
{
  messageQueue.clear();
  messageQueue.deallocate();

  mainCallSemaphore.destroy();
}

}
}

#endif
