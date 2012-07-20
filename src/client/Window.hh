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
 * @file client/Window.hh
 */

#pragma once

#include "client/common.hh"

#ifdef __native_client__
namespace pp { class Graphics3D; }
#endif

namespace oz
{
namespace client
{

class Window
{
  friend class Input;

  private:

#if defined( __native_client__ )

    Semaphore       flushSemaphore;
    pp::Graphics3D* context;

    static void flushCompleteCallback( void* data, int );

    void createContext();

#elif SDL_MAJOR_VERSION < 2

    SDL_Surface*    descriptor;

#else

    SDL_Window*     descriptor;
    SDL_GLContext   context;

#endif

    int  desktopWidth;
    int  desktopHeight;
    int  desiredWidth;
    int  desiredHeight;

  public:

    int  display;
    int  width;
    int  height;
    uint flags;
    bool isFull;

    bool hasFocus;
    bool hasGrab;

    Window();

    void warpMouse();
    void swapBuffers();

    void minimise();
    void resize();

    void setGrab( bool grab );
    void setFullscreen( bool fullscreen );

    void init();
    void free();

};

extern Window window;

}
}
