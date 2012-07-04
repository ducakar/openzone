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
  private:

#ifdef __native_client__

    pp::Graphics3D* context;
    Semaphore       flushSemaphore;

    static void flushCompleteCallback( void* data, int );

    void createContext();

#else

    SDL_Surface*    descriptor;

#endif

    int  desktopWidth;
    int  desktopHeight;
    int  desiredWidth;
    int  desiredHeight;

  public:

    int  width;
    int  height;
    uint flags;
    bool isFull;

    Window();

    void resize();
    void toggleFull();
    void swapBuffers();

    void init();
    void free();

};

extern Window window;

}
}
