/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

#include <SDL.h>

namespace oz
{

/**
 * Static class for game window management.
 */
class Window
{
private:

  static int  screenWidth;  ///< Screen width (desktop resolution).
  static int  screenHeight; ///< Screen height (desktop resolution).
  static int  windowWidth;  ///< Window inner width.
  static int  windowHeight; ///< Window inner height.
  static bool fullscreen;   ///< True iff in full screen mode;
  static bool windowFocus;  ///< True iff window has (input) focus.

private:

  /**
   * Obtain screen (desktop) width and height.
   */
  static void measureScreen();

public:

  /**
   * Static class.
   */
  Window() = delete;

  /**
   * True the window is created.
   */
  static bool isCreated();

  /**
   * Desktop width.
   */
  static int desktopWidth()
  {
    if (screenWidth == 0) {
      measureScreen();
    }
    return screenWidth;
  }

  /**
   * Desktop height.
   */
  static int desktopHeight()
  {
    if (screenHeight == 0) {
      measureScreen();
    }
    return screenHeight;
  }

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
   * True iff the window has focus (must be set by `setFocus()`).
   */
  static bool hasFocus()
  {
    return windowFocus;
  }

  /**
   * Warp mouse cursor to the centre of the window.
   */
  static void warpMouse();

  /**
   * Swap OpenGL buffers.
   */
  static void swapBuffers();

  /**
   * Save screenshot in a JPEG file.
   *
   * Creation of JPEG is performed on a background thread. Only one screenshot thread at a time is
   * possible, so making two screenshots in a short time will block the second call until the first
   * screenshot is written.
   */
  static void screenshot(const File& file, int quality = 85);

  /**
   * Resize window and/or toggle full-screen mode.
   *
   * If width (or height) is 0, desktop width (or height) is used.
   */
  static void resize(int newWidth, int newHeight, bool fullscreen);

  /**
   * Minimise window.
   */
  static void minimise();

  /**
   * Process SDL event.
   *
   * This updates GL context after window resize, stops/starts AL context on minimise/restore.
   * It returns false iff the window got a close request (i.e. Alt + F4 or close button clicked).
   */
  static bool processEvent(const SDL_Event* event);

  /**
   * Create the window.
   *
   * If width (or height) is 0, desktop width (or height) is used.
   * Invoking this function when the window is already created is an error.
   */
  static bool create(const char* title, int width, int height, bool fullscreen = false);

  /**
   * Destroy the window.
   *
   * Destroying a non-existent window is a legal NOP.
   */
  static void destroy();

};

}
