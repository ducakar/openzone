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
 * @file client/NaClGLContext.cc
 */

#include "stable.hh"

#ifdef __native_client__

#include "client/NaClGLContext.hh"

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/gles2/gl2ext_ppapi.h>
#include <GLES2/gl2.h>

namespace oz
{
namespace client
{

static Semaphore      flushSemaphore;
static pp::Graphics3D context;

static void flushCompleteCallback( void*, int )
{
  flushSemaphore.post();
}

void NaClGLContext::activate()
{
  hard_assert( !context.is_null() );

  glSetCurrentContextPPAPI( context.pp_resource() );
}

void NaClGLContext::deactivate()
{
  glSetCurrentContextPPAPI( 0 );
}

void NaClGLContext::resize()
{
  glSetCurrentContextPPAPI( 0 );
  context.ResizeBuffers( System::width, System::height );
}

void NaClGLContext::flush()
{
  context.SwapBuffers( pp::CompletionCallback( &flushCompleteCallback, null ) );
}

void NaClGLContext::wait()
{
  flushSemaphore.wait();
}

void NaClGLContext::init()
{
  hard_assert( context.is_null() );

  flushSemaphore.init();

  glInitializePPAPI( System::module->get_browser_interface() );

  int attribs[] = {
    PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
    PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
    PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8,
    PP_GRAPHICS3DATTRIB_SAMPLES, 0,
    PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, 0,
    PP_GRAPHICS3DATTRIB_WIDTH, System::width,
    PP_GRAPHICS3DATTRIB_HEIGHT, System::height,
    PP_GRAPHICS3DATTRIB_NONE
  };

  context = pp::Graphics3D( System::instance, pp::Graphics3D(), attribs );
  if( context.is_null() ) {
    throw Exception( "Failed to create OpenGL context" );
  }

  if( !System::instance->BindGraphics( context ) ) {
    throw Exception( "Failed to bind Graphics3D" );
  }

  glSetCurrentContextPPAPI( context.pp_resource() );

  glClearColor( 1.0f, 0.0f, 1.0f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}

void NaClGLContext::free()
{
  glSetCurrentContextPPAPI( 0 );
  glTerminatePPAPI();

  flushSemaphore.destroy();
}

}
}

#endif
