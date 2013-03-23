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
 * @file ozEngine/Window.cc
 */

#include "Window.hh"

#include "Pepper.hh"
#include "OpenGL.hh"

#include <SDL.h>
#ifdef __native_client__
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/instance.h>
# include <ppapi/cpp/graphics_3d.h>
# include <ppapi/gles2/gl2ext_ppapi.h>
#endif

namespace oz
{

#if defined( __native_client__ )
static Semaphore       flushSemaphore;
static pp::Graphics3D* context;
#elif SDL_MAJOR_VERSION < 2
static SDL_Surface*    descriptor;
#else
static SDL_Window*     descriptor;
static SDL_GLContext   context;
#endif

int  Window::width    = 0;
int  Window::height   = 0;
bool Window::isFull   = false;
bool Window::hasFocus = false;
bool Window::hasGrab  = false;

#ifdef __native_client__

static void flushCompleteCallback( void*, int )
{
  flushSemaphore.post();
}

// Because of array initialiser this code cannot reside inside OZ_MAIN_CALL macro, so it has been
// split into a separate function.
static void createContext()
{
  int attribs[] = {
    PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 16,
    PP_GRAPHICS3DATTRIB_WIDTH, Window::width,
    PP_GRAPHICS3DATTRIB_HEIGHT, Window::height,
    PP_GRAPHICS3DATTRIB_NONE
  };

  context = new pp::Graphics3D( System::instance, pp::Graphics3D(), attribs );
  if( context->is_null() ) {
    OZ_ERROR( "Failed to create OpenGL context" );
  }

  if( !System::instance->BindGraphics( *context ) ) {
    OZ_ERROR( "Failed to bind Graphics3D" );
  }
}

#endif

bool Window::isCreated()
{
#ifdef __native_client__
  return !context->is_null();
#else
  return descriptor != nullptr;
#endif
}

void Window::warpMouse()
{
  if( !hasFocus || !hasGrab ) {
    return;
  }

#ifdef __native_client__
  Pepper::moveX = 0.0f;
  Pepper::moveY = 0.0f;
  Pepper::moveZ = 0.0f;
  Pepper::moveW = 0.0f;
#elif SDL_MAJOR_VERSION >= 2
  SDL_WarpMouseInWindow( descriptor, width / 2, height / 2 );
  SDL_PumpEvents();
  SDL_GetRelativeMouseState( nullptr, nullptr );
#endif
}

void Window::swapBuffers()
{
#if defined( __native_client__ )
  OZ_STATIC_MAIN_CALL( {
    context->SwapBuffers( pp::CompletionCallback( flushCompleteCallback, nullptr ) );
  } )
  flushSemaphore.wait();
#elif SDL_MAJOR_VERSION < 2
  SDL_GL_SwapBuffers();
#else
  SDL_GL_SwapWindow( descriptor );
#endif
}

void Window::minimise()
{
#if defined( __native_client__ )
#elif SDL_MAJOR_VERSION < 2
  SDL_WM_GrabInput( SDL_GRAB_OFF );
  SDL_WM_IconifyWindow();
#else
  SDL_MinimizeWindow( descriptor );
#endif
}

bool Window::resize( int newWidth, int newHeight, bool fullscreen )
{
#ifdef __native_client__

  static_cast<void>( fullscreen );

  width  = newWidth;
  height = newHeight;

  OZ_STATIC_MAIN_CALL( {
    glSetCurrentContextPPAPI( 0 );
    context->ResizeBuffers( width, height );
    glSetCurrentContextPPAPI( context->pp_resource() );
  } )

#elif SDL_MAJOR_VERSION < 2
# ifdef _WIN32

  static_cast<void>( newWidth );
  static_cast<void>( newHeight );
  static_cast<void>( fullscreen );

# else

  width  = newWidth;
  height = newHeight;
  isFull = fullscreen;

  Log::print( "Resizing OpenGL window to %dx%d [%s] ... ",
              width, height, isFull ? "fullscreen" : "windowed" );

  SDL_FreeSurface( descriptor );
  descriptor = SDL_SetVideoMode( width, height, 0, fullscreen ? SDL_FULLSCREEN : 0 );

  if( descriptor == nullptr ) {
    Log::printEnd( "Window resize failed" );
    return false;
  }

# endif
#else

  width  = newWidth;
  height = newHeight;
  isFull = fullscreen;

  if( isFull ) {
    SDL_SetWindowSize( descriptor, width, height );
    SDL_SetWindowFullscreen( descriptor, SDL_TRUE );
  }
  else {
    SDL_SetWindowFullscreen( descriptor, SDL_FALSE );
    SDL_SetWindowSize( descriptor, width, height );
  }

#endif

  return true;
}

void Window::setGrab( bool grab )
{
  hasGrab = grab;

#if SDL_MAJOR_VERSION < 2
  SDL_ShowCursor( !hasGrab );
  SDL_WM_GrabInput( SDL_GrabMode( hasGrab && !System::isInstrumented() ) );
#else
  SDL_SetRelativeMouseMode( SDL_bool( hasGrab ) );
#endif
}

bool Window::create( const char* title, int width_, int height_, bool fullscreen )
{
  width    = width_;
  height   = height_;
  isFull   = fullscreen;
  hasFocus = true;
  hasGrab  = true;

#ifdef __native_client__

  static_cast<void>( title );

  flushSemaphore.init();

  OZ_STATIC_MAIN_CALL( {
    glInitializePPAPI( pp::Module::Get()->get_browser_interface() );
    createContext();
    glSetCurrentContextPPAPI( context->pp_resource() );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glFlush();

    context->SwapBuffers( pp::CompletionCallback( flushCompleteCallback, nullptr ) );
  } )
  flushSemaphore.wait();

#else

  // Don't mess with screensaver. In X11 it only makes effect for windowed mode, in fullscreen
  // mode screensaver never starts anyway. Turning off screensaver has a side effect: if the game
  // crashes, it remains turned off. Besides that, in X11 several programs (e.g. IM clients) rely
  // on screensaver's counter, so they don't detect that you are away if the screensaver is screwed.
#if SDL_MAJOR_VERSION < 2
  char allowScreensaverEnv[] = "SDL_VIDEO_ALLOW_SCREENSAVER=1";
  SDL_putenv( allowScreensaverEnv );
#else
  SDL_EnableScreenSaver();
#endif

#if SDL_MAJOR_VERSION < 2

  uint flags = SDL_OPENGL | ( isFull ? SDL_FULLSCREEN : 0 );

  if( width == 0 || height == 0 ) {
    const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();

    Log::verboseMode = true;
    Log::println( "Desktop video mode: %dx%d-%d", videoInfo->current_w, videoInfo->current_h,
                  videoInfo->vfmt->BitsPerPixel );
    Log::verboseMode = false;

    width  = videoInfo->current_w;
    height = videoInfo->current_h;
  }

  Log::print( "Creating OpenGL window %dx%d [%s] ... ",
              width, height, isFull ? "fullscreen" : "windowed" );

  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE,   0 );
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 0 );
  SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );

  descriptor = SDL_SetVideoMode( width, height, 0, flags );

  if( descriptor == nullptr ) {
    Log::printEnd( "Window creation failed" );
    return false;
  }

  SDL_WM_SetCaption( title, title );

#else

  uint flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | ( isFull ? SDL_WINDOW_FULLSCREEN : 0 );

  if( width == 0 || height == 0 ) {
    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode( 0, &mode );

    width  = mode.w;
    height = mode.h;
  }

  Log::print( "Creating OpenGL window %dx%d [%s] ... ",
              width, height, isFull ? "fullscreen" : "windowed" );

  descriptor = SDL_CreateWindow( title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                 width, height, flags );
  if( descriptor == nullptr ) {
    Log::printEnd( "Window creation failed" );
    return false;
  }

# ifdef GL_ES_VERSION_2_0
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_ES );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
# else
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
# endif
  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE,            0 );
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE,          0 );

  SDL_GL_SetSwapInterval( 1 );

  context = SDL_GL_CreateContext( descriptor );

#endif

  Log::printEnd( "%dx%d ... OK", width, height );

  glViewport( 0, 0, width, height );
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
  glFlush();

#if SDL_MAJOR_VERSION < 2
  SDL_GL_SwapBuffers();
#else
  SDL_GL_SwapWindow( descriptor );
#endif
#endif

  return true;
}

void Window::destroy()
{
#if defined( __native_client__ )

  if( !context->is_null() ) {
    OZ_STATIC_MAIN_CALL( {
      glSetCurrentContextPPAPI( 0 );
      delete context;
      glTerminatePPAPI();
    } )
    flushSemaphore.destroy();
  }

#else

  if( descriptor != nullptr ) {
# if SDL_MAJOR_VERSION < 2
    SDL_FreeSurface( descriptor );
# else
    SDL_GL_DeleteContext( context );
    SDL_DestroyWindow( descriptor );
# endif
    descriptor = nullptr;
  }

#endif
}

}
