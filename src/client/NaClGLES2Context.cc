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
 * @file client/NaClGLES2Context.cc
 */

#include "stable.hh"

#ifndef __LINE__
# define __native_client__
#endif

#ifdef __native_client__

#include "client/NaClGLES2Context.hh"

#include <ppapi/c/ppb_opengles2.h>
#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/cpp/graphics_3d_client.h>
#include <ppapi/gles2/gl2ext_ppapi.h>

namespace oz
{

class Graphics3DClient : public pp::Graphics3DClient
{
  public:

    explicit Graphics3DClient( pp::Instance* instance ) :
      pp::Graphics3DClient( instance )
    {}

    ~Graphics3DClient()
    {}

    void Graphics3DContextLost()
    {
      System::error( 0, "GL context lost" );
    }

};

static Semaphore            flushSemaphore;
static pp::Graphics3D       context;
static const PPB_OpenGLES2* iGLES2;

static void flushCompleteCallback( void*, int )
{
  flushSemaphore.post();
}

void NaClGLES2Context::makeCurrent()
{
  glSetCurrentContextPPAPI( context.pp_resource() );
}

void NaClGLES2Context::resize()
{
  glSetCurrentContextPPAPI( 0 );
  context.ResizeBuffers( System::width, System::height );
}

void NaClGLES2Context::swapBuffers()
{
  context.SwapBuffers( pp::CompletionCallback( &flushCompleteCallback, null ) );
  flushSemaphore.wait();
}

void NaClGLES2Context::init()
{
  flushSemaphore.init();

  const void* pGLES2Interface = System::module->GetBrowserInterface( PPB_OPENGLES2_INTERFACE );
  iGLES2 = static_cast<const PPB_OpenGLES2*>( pGLES2Interface );

  if( iGLES2 == null ) {
    throw Exception( "Cannot obtain OpenGLES2 browser interface" );
  }

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

  System::instance->BindGraphics( context );
}

void NaClGLES2Context::free()
{
  glSetCurrentContextPPAPI( 0 );

  flushSemaphore.destroy();
}

}

#endif
