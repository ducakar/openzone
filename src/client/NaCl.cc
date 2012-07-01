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

#include "stable.hh"

#include "client/NaCl.hh"

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/core.h>
#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/gles2/gl2ext_ppapi.h>

namespace oz
{
namespace client
{

static Semaphore      flushSemaphore;
static pp::Graphics3D context;

Semaphore NaCl::semaphore;

int       NaCl::width  = 0;
int       NaCl::height = 0;

int       NaCl::moveX  = 0;
int       NaCl::moveY  = 0;
int       NaCl::moveZ  = 0;
int       NaCl::moveW  = 0;

static void flushCompleteCallback( void*, int )
{
  flushSemaphore.post();
}

bool NaCl::isMainThread()
{
  return System::core->IsMainThread();
}

void NaCl::call( Callback* callback, void* caller )
{
  System::core->CallOnMainThread( 0, pp::CompletionCallback( callback, caller ) );
  semaphore.wait();
}

void NaCl::send( const char* message )
{
  OZ_MAIN_CALL( const_cast<char*>( message ), {
    const char* message = reinterpret_cast<const char*>( _this );

    System::instance->PostMessage( pp::Var( message ) );
  } )
}

void NaCl::activateGLContext()
{
  hard_assert( !context.is_null() );

  glSetCurrentContextPPAPI( context.pp_resource() );
}

void NaCl::deactivateGLContext()
{
  glSetCurrentContextPPAPI( 0 );
}

void NaCl::resizeGLContext()
{
  glSetCurrentContextPPAPI( 0 );
  context.ResizeBuffers( width, height );
  glSetCurrentContextPPAPI( context.pp_resource() );
}

void NaCl::flushGLContext()
{
  context.SwapBuffers( pp::CompletionCallback( &flushCompleteCallback, null ) );
}

void NaCl::waitGLContext()
{
  flushSemaphore.wait();
}

void NaCl::initGLContext()
{
  hard_assert( context.is_null() );

  flushSemaphore.init();

  glInitializePPAPI( System::module->get_browser_interface() );

  int attribs[] = {
    PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 16,
    PP_GRAPHICS3DATTRIB_WIDTH, width,
    PP_GRAPHICS3DATTRIB_HEIGHT, height,
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
}

void NaCl::freeGLContext()
{
  glSetCurrentContextPPAPI( 0 );
  glTerminatePPAPI();

  flushSemaphore.destroy();
}

void NaCl::init()
{
  semaphore.init();

  // Hacks
  config.include( "lingua", "sl" );
  config.include( "render.distance", "100.0" );
  config.include( "ui.showBuild", "true" );
  config.include( "ui.showFPS", "true" );

  config.include( "profile.class", "beast" );
  config.include( "profile.item00", "beast_weapon.plasmagun" );
  config.include( "profile.item01", "nvGoggles" );
  config.include( "profile.item02", "binoculars" );
  config.include( "profile.item03", "galileo" );
  config.include( "profile.item04", "musicPlayer" );
  config.include( "profile.item05", "cvicek" );
  config.include( "profile.name", "Jst" );
  config.include( "profile.weaponItem", "0" );
}

void NaCl::free()
{
  semaphore.destroy();
}

}
}

#endif
