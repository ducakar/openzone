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
 * @file client/NaClMainCall.cc
 */

#ifdef __native_client__

#include "stable.hh"

#include "client/NaClMainCall.hh"

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/core.h>

namespace oz
{
namespace client
{

Semaphore NaClMainCall::semaphore;

bool NaClMainCall::isMainThread()
{
  return System::core->IsMainThread();
}

void NaClMainCall::call( Callback* callback, void* caller )
{
  System::core->CallOnMainThread( 0, pp::CompletionCallback( callback, caller ) );
  semaphore.wait();
}

void NaClMainCall::init()
{
  semaphore.init();
}

void NaClMainCall::free()
{
  semaphore.destroy();
}

}
}

#endif
