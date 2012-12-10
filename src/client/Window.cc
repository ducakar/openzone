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
 * @file client/Window.cc
 */

#include <stable.hh>
#include <client/Window.hh>
#include <client/Input.hh>
#include <client/OpenGL.hh>
#include <client/NaClPlatform.hh>

#ifdef __native_client__
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/instance.h>
# include <ppapi/cpp/graphics_3d.h>
# include <ppapi/gles2/gl2ext_ppapi.h>
#endif

namespace oz
{
namespace client
{

#ifdef __native_client__

void Window::flushCompleteCallback( void* data, int )
{
  Window* window = static_cast<Window*>( data );

  window->flushSemaphore.post();
}

// Because of array initialiser this code cannot reside inside OZ_MAIN_CALL macro, so it has been
// split into a separate function.
void Window::createContext()
{
  int attribs[] = {
    PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 16,
    PP_GRAPHICS3DATTRIB_WIDTH, width,
    PP_GRAPHICS3DATTRIB_HEIGHT, height,
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

Window::Window() :
  width( 0 ), height( 0 ), flags( 0 ), isFull( 0 )
{}

void Window::warpMouse()
{
  if( !hasFocus || !hasGrab ) {
    return;
  }

#ifdef __native_client__
  NaClPlatform::moveX = 0.0f;
  NaClPlatform::moveY = 0.0f;
  NaClPlatform::moveZ = 0.0f;
  NaClPlatform::moveW = 0.0f;
#elif SDL_MAJOR_VERSION >= 2
  SDL_WarpMouseInWindow( descriptor, width / 2, height / 2 );
  SDL_PumpEvents();
  SDL_GetRelativeMouseState( nullptr, nullptr );
#endif
}

void Window::swapBuffers()
{
#if defined( __native_client__ )

  OZ_MAIN_CALL( this, {
    _this->context->SwapBuffers( pp::CompletionCallback( &_this->flushCompleteCallback, _this ) );
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
  SDL_WM_IconifyWindow();
#else
  SDL_MinimizeWindow( descriptor );
#endif
}

void Window::resize()
{
#ifdef __native_client__

  width  = NaClPlatform::width;
  height = NaClPlatform::height;

  OZ_MAIN_CALL( this, {
    glSetCurrentContextPPAPI( 0 );
    _this->context->ResizeBuffers( _this->width, _this->height );
    glSetCurrentContextPPAPI( _this->context->pp_resource() );
  } )

#endif
}

void Window::setGrab( bool grab )
{
  hasGrab = grab;

#if SDL_MAJOR_VERSION < 2
  SDL_ShowCursor( !hasGrab );
#else
  SDL_SetRelativeMouseMode( SDL_bool( hasGrab ) );
#endif
}

void Window::setFullscreen( bool fullscreen )
{
  if( fullscreen == isFull ) {
    return;
  }

#if defined( __native_client__ )
#elif SDL_MAJOR_VERSION < 2
# ifndef _WIN32

    width   = isFull ? desiredWidth  : screenWidth;
    height  = isFull ? desiredHeight : screenHeight;
    flags  ^= SDL_FULLSCREEN;
    isFull  = !isFull;

    SDL_FreeSurface( descriptor );
    descriptor = SDL_SetVideoMode( width, height, 0, flags );

    if( descriptor == nullptr ) {
      OZ_ERROR( "Fullscreen mode switch failed" );
    }

# endif
#else

    if( isFull ) {
      width   = desiredWidth;
      height  = desiredHeight;
      flags  ^= SDL_WINDOW_FULLSCREEN;
      isFull  = false;

      SDL_SetWindowFullscreen( descriptor, SDL_FALSE );
      SDL_SetWindowSize( descriptor, width, height );
    }
    else {
      width   = screenWidth;
      height  = screenHeight;
      flags  ^= SDL_WINDOW_FULLSCREEN;
      isFull  = true;

      SDL_SetWindowSize( descriptor, width, height );
      SDL_SetWindowFullscreen( descriptor, SDL_TRUE );
    }

#endif
}

void Window::init()
{
  hasFocus = true;
  hasGrab  = true;

#ifdef __native_client__

  flushSemaphore.init();

  width  = NaClPlatform::width;
  height = NaClPlatform::height;
  flags  = 0;
  isFull = false;

  OZ_MAIN_CALL( this, {
    glInitializePPAPI( System::module->get_browser_interface() );
    _this->createContext();
    glSetCurrentContextPPAPI( _this->context->pp_resource() );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glFlush();

    _this->context->SwapBuffers( pp::CompletionCallback( &_this->flushCompleteCallback, _this ) );
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

  display       = config.include( "window.display",       0    ).asInt();
  isFull        = config.include( "window.fullscreen",    true ).asBool();
  desiredWidth  = config.include( "window.desiredWidth",  1280 ).asInt();
  desiredHeight = config.include( "window.desiredHeight", 720  ).asInt();
  screenWidth   = config.include( "window.screenWidth",   0    ).asInt();
  screenHeight  = config.include( "window.screenHeight",  0    ).asInt();

#if SDL_MAJOR_VERSION < 2

  flags = SDL_OPENGL | ( isFull ? SDL_FULLSCREEN : 0 );

  if( screenWidth == 0 || screenHeight == 0 ) {
    const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();

    Log::verboseMode = true;
    Log::println( "Desktop video mode: %dx%d-%d", videoInfo->current_w, videoInfo->current_h,
                  videoInfo->vfmt->BitsPerPixel );
    Log::verboseMode = false;

    screenWidth  = videoInfo->current_w;
    screenHeight = videoInfo->current_h;
  }

#else

  flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | ( isFull ? SDL_WINDOW_FULLSCREEN : 0 );

  if( screenWidth == 0 || screenHeight == 0 ) {
    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode( display, &mode );

    screenWidth  = mode.w;
    screenHeight = mode.h;
  }

#endif

  if( desiredWidth == 0 || desiredHeight == 0 ) {
    desiredWidth  = screenWidth;
    desiredHeight = screenHeight;
  }

  if( isFull ) {
    width  = screenWidth;
    height = screenHeight;
  }
  else {
    width  = desiredWidth;
    height = desiredHeight;
  }

  Log::print( "Creating OpenGL window %dx%d [%s] ...",
              width, height, isFull ? "fullscreen" : "windowed" );

#if SDL_MAJOR_VERSION < 2

  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE,   0 );
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 0 );
  SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );

  descriptor = SDL_SetVideoMode( width, height, 0, flags );
  if( descriptor == nullptr ) {
    OZ_ERROR( "SDL window creation failed" );
  }

  SDL_WM_SetCaption( "OpenZone " OZ_VERSION, "OpenZone " OZ_VERSION );

#else

  descriptor = SDL_CreateWindow( "OpenZone " OZ_VERSION,
                                 SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                 width, height, flags );
  if( descriptor == nullptr ) {
    OZ_ERROR( "SDL window creation failed" );
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

  Log::printEnd( " %dx%d ... OK", width, height );

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
}

void Window::destroy()
{
#if defined( __native_client__ )

  OZ_MAIN_CALL( this, {
    glSetCurrentContextPPAPI( 0 );
    delete _this->context;
    glTerminatePPAPI();
  } )

  flushSemaphore.destroy();

#elif SDL_MAJOR_VERSION < 2

  SDL_FreeSurface( descriptor );

  descriptor = nullptr;

#else

  SDL_GL_DeleteContext( context );
  SDL_DestroyWindow( descriptor );

  descriptor = nullptr;

#endif
}

Window window;

}
}
