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

#include "stable.hh"

#include "client/Window.hh"
#include "client/OpenGL.hh"
#include "client/NaCl.hh"

#include "client/ui/Mouse.hh"

namespace oz
{
namespace client
{

Window window;

Window::Window() :
  descriptor( null ), width( 0 ), height( 0 ), bpp( 0 ), flags( 0 ), isFull( 0 )
{}

void Window::resize()
{
#ifdef __native_client__

  width  = NaCl::width;
  height = NaCl::height;

  OZ_MAIN_CALL( this, {
    NaCl::resizeGLContext();
  } )

#else

  descriptor = SDL_SetVideoMode( width, height, bpp, flags );

  if( descriptor == null ) {
    throw Exception( "Failed to resize surface after window resize" );
  }

  width  = descriptor->w;
  height = descriptor->h;

#endif
}

void Window::toggleFull()
{
#ifndef __native_client__

  if( SDL_WM_ToggleFullScreen( descriptor ) != 0 ) {
    isFull = !isFull;
    flags ^= SDL_FULLSCREEN;

    ui::mouse.isJailed = true;
    ui::mouse.reset();

    SDL_ShowCursor( false );
  }

#endif
}

void Window::init()
{
#ifdef __native_client__

  width  = NaCl::width;
  height = NaCl::height;
  bpp    = 32;
  flags  = 0;
  isFull = false;

  OZ_MAIN_CALL( this, {
    NaCl::initGLContext();
    NaCl::activateGLContext();

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glFlush();

    NaCl::flushGLContext();
  } )

#else

  // Don't mess with screensaver. In X11 it only makes effect for windowed mode, in fullscreen
  // mode screensaver never starts anyway. Turning off screensaver has a side effect: if the game
  // crashes, it remains turned off. Besides that, in X11 several programs (e.g. IM clients) rely
  // on screensaver's counter, so they don't detect that you are away if the screensaver is screwed.
  static char allowScreensaverEnv[] = "SDL_VIDEO_ALLOW_SCREENSAVER=1";
  SDL_putenv( allowScreensaverEnv );

  width            = config.getSet( "window.width", 0 );
  height           = config.getSet( "window.height", 0 );
  bpp              = config.getSet( "window.bpp", 0 );
  flags            = SDL_OPENGL | SDL_RESIZABLE;
  isFull           = config.getSet( "window.fullscreen", false );
  bool enableVSync = config.getSet( "window.vsync", true );

  if( isFull ) {
    flags |= SDL_FULLSCREEN;
  }

  const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();

  Log::verboseMode = true;
  Log::println( "Desktop video mode: %dx%d-%d", videoInfo->current_w, videoInfo->current_h,
                videoInfo->vfmt->BitsPerPixel );
  Log::verboseMode = false;

  if( width == 0 || height == 0 ) {
    width  = videoInfo->current_w;
    height = videoInfo->current_h;
  }
  if( bpp == 0 ) {
    bpp = videoInfo->vfmt->BitsPerPixel;
  }

  SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, enableVSync );

  Log::print( "Creating OpenGL window %dx%d-%d [%s] ...",
              width, height, bpp, isFull ? "fullscreen" : "windowed" );

  if( SDL_VideoModeOK( width, height, bpp, flags ) == 0 ) {
    throw Exception( "Video mode not supported" );
  }

  descriptor = SDL_SetVideoMode( width, height, bpp, flags );

  if( descriptor == null ) {
    throw Exception( "Window creation failed" );
  }

  SDL_WM_SetCaption( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION,
                     OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION );

  width  = descriptor->w;
  height = descriptor->h;
  bpp    = descriptor->format->BitsPerPixel;

  Log::printEnd( " %dx%d-%d ... OK", width, height, bpp );

  SDL_ShowCursor( SDL_FALSE );

  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
  glFlush();

  SDL_GL_SwapBuffers();

#endif
}

void Window::free()
{
#ifdef __native_client__
  NaCl::freeGLContext();
#endif
}

}
}
