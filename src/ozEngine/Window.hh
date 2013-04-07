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
 * @file ozEngine/Window.hh
 *
 * `Window` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Static class for game window management.
 */
class Window
{
  private:

    static int  windowWidth;  ///< Window inner width.
    static int  windowHeight; ///< Window inner height.
    static bool fullscreen;   ///< True iff in full screen mode;
    static bool windowFocus;  ///< True iff window has (input) focus.
    static bool windowGrab;   ///< True iff window has exclusive input grab.

  public:

    /**
     * Forbid instances.
     */
    explicit Window() = delete;

    /**
     * True the window is created.
     */
    static bool isCreated();

    /**
     * Window width.
     */
    static int width()
    {
      return windowWidth;
    }

    /**
     * Window height.
     */
    static int height()
    {
      return windowHeight;
    }

    /**
     * True iff in fullscreen mode.
     */
    static bool isFullscreen()
    {
      return fullscreen;
    }

    /**
     * True iff the window has focus.
     */
    static bool hasFocus()
    {
      return windowFocus;
    }

    /**
     * True iff the window has input grab.
     */
    static bool hasGrab()
    {
      return windowGrab;
    }

    /**
     * Toggle input grab.
     */
    static void setGrab( bool grab );

    /**
     * Warp mouse cursor to the centre of the window.
     */
    static void warpMouse();

    /**
     * Swap OpenGL buffers.
     */
    static void swapBuffers();

    /**
     * Minimise window.
     */
    static void minimise();

    /**
     * Resize window and/or toggle full-screen mode.
     *
     * If either width or height is 0, desktop resolution is used.
     * On error, window is destroyed.
     */
    static bool resize( int newWidth, int newHeight, bool fullscreen = false );

    /**
     * Create the window.
     *
     * If either width or height is 0, desktop resolution is used.
     * Invoking this function when the window is already created is an error.
     */
    static bool create( const char* title, int width, int height, bool fullscreen = false );

    /**
     * Destroy the window.
     *
     * Destroying a non-existent window is a legal NOP.
     */
    static void destroy();

};

}
