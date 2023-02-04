/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

union SDL_Event;

namespace oz
{

/**
 * Static class for game window management.
 */
class Window
{
public:

  /**
   * %Window mode.
   */
  enum Mode
  {
    WINDOWED,  ///< Windowed.
    DESKTOP,   ///< Desktop fullscreen. Always uses desktop resolution.
    EXCLUSIVE  ///< Exclusive fullscreen. Switches desktop resolution if necessary.
  };

private:

  static int  screenWidth;  ///< Screen width (desktop resolution).
  static int  screenHeight; ///< Screen height (desktop resolution).

  static int  windowWidth;  ///< %Window inner width.
  static int  windowHeight; ///< %Window inner height.
  static Mode windowMode;   ///< True iff in full screen mode;

  static bool windowActive; ///< True iff shown (not minimised).
  static bool windowFocus;  ///< True iff the window has (input) focus.
  static bool windowGrab;   ///< True if in relative mouse mode.

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
   * %Window width.
   */
  OZ_ALWAYS_INLINE
  static int width() noexcept
  {
    return windowWidth;
  }

  /**
   * %Window height.
   */
  OZ_ALWAYS_INLINE
  static int height() noexcept
  {
    return windowHeight;
  }

  /**
   * True iff in fullscreen mode.
   */
  OZ_ALWAYS_INLINE
  static Mode mode() noexcept
  {
    return windowMode;
  }

  /**
   * True iff the window is shown (not minimised).
   */
  OZ_ALWAYS_INLINE
  static bool isActive() noexcept
  {
    return windowActive;
  }

  /**
   * True iff the window has focus.
   */
  OZ_ALWAYS_INLINE
  static bool hasFocus() noexcept
  {
    return windowFocus;
  }

  /**
   * Warp mouse cursor to the centre of the window.
   */
  static void warpMouse();

  /**
   * True iff the window has input grab.
   */
  OZ_ALWAYS_INLINE
  static bool hasGrab() noexcept
  {
    return windowGrab;
  }

  /**
   * Toggle input grab.
   */
  static void setGrab(bool grab);

  /**
   * Swap OpenGL buffers.
   */
  static void swapBuffers();

  /**
   * Save screenshot in a PNG file.
   *
   * Creation of PNG is performed on a background thread as the best compression is used and it may
   * take some time. Only one screenshot thread at a time is possible, so making two screenshots in
   * a short time will block the second call until the first screenshot is written.
   *
   * @param basePath the screenshot is witten to a file named "<basePath>-<dataAndTime>.png".
   */
  static void screenshot(const char* basePath);

  /**
   * Resize window and/or toggle full-screen mode.
   *
   * Size is not changed if either dimension is zero. That should always be the case when switching
   * into "desktop" fullscreen mode.
   */
  static void resize(int newWidth, int newHeight, Mode newMode);

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
  static bool create(const char* title, int width, int height, Mode mode = WINDOWED);

  /**
   * Destroy the window.
   *
   * Destroying a non-existent window is a legal NOP.
   */
  static void destroy();

};

}
