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

#pragma once

#include "Window.hh"

namespace oz
{

class Application
{
public:

  class Stage
  {
  public:

    /**
     * Destructor.
     */
    virtual ~Stage();

    /**
     * Load stage, called after the previous stage was unloaded.
     */
    virtual void load();

    /**
     * Unload stage.
     */
    virtual void unload();

    /**
     * Update game world, called once per update.
     *
     * This function should update the game world, not perform any rendering or frontend stuff.
     */
    virtual void update();

    /**
     * Present game world, called after `update()`.
     *
     * In case there is not enough time left this function should skip frame rendering and only do
     * some non-demanding things that mus be performed even if the frame is dropped (e.g. play sound
     * effects).
     *
     * @param isEnoughTime whether there is still enough time to render full frame.
     */
    virtual void present(bool isEnoughTime);
  };

  struct Config
  {
    struct WindowConfig
    {
      const char*  title  = "Untitled";
      int          width  = 1280;
      int          height = 720;
      Window::Mode mode   = Window::WINDOWED;
    };

    struct TimingConfig
    {
      int  fps      = 60;
      bool isFixed  = true;
    };

    const char*  name       = "untitled";
    bool         loadConfig = false;
    bool         saveConfig = false;
    WindowConfig window;
    TimingConfig timing;
  };

public:

  static const File& CONFIG_DIR;
  static const File& DATA_DIR;

public:

  static Config defaults;
  static Json   config;

public:

  Application() = delete;

  static void setStage(Stage* stage);

  static void run(Stage* initialStage);

};

}
