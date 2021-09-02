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

/**
 * Application class.
 */
class Application
{
public:

  /**
   * Application stage.
   *
   * E.g. main menu, game, editor ...
   */
  class Stage
  {
  public:

    /**
     * Trivial default constructor.
     */
    Stage() = default;

    /**
     * Destructor.
     */
    virtual ~Stage();

    /**
     * No copying.
     */
    Stage(const Stage&) = delete;

    /**
     * No moving.
     */
    Stage(Stage&&) noexcept = delete;

    /**
     * No copying.
     */
    Stage& operator=(const Stage&) = delete;

    /**
     * No moving.
     */
    Stage& operator=(Stage&&) noexcept = delete;

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

  /**
   * Application configuration.
   */
  struct Config
  {
    /**
     * Window configuration.
     */
    struct WindowConfig
    {
      const char*  title  = "Untitled";       ///< Title.
      int          width  = 1280;             ///< Width.
      int          height = 720;              ///< Height.
      Window::Mode mode   = Window::WINDOWED; ///< Window/fullscreen mode.
    };

    /**
     * Application update timing configuration.
     */
    struct TimingConfig
    {
      int fps = 60;   ///< Number of updates per second.
    };

    const char*  name       = "untitled"; ///< Filesystem name.
    bool         loadConfig = false;      ///< Load configuration on startup.
    bool         saveConfig = false;      ///< Save configuration on shutdown.
    WindowConfig window;                  ///< Window configuration.
    TimingConfig timing;                  ///< Timing configuration.
  };

public:

  static const File& CONFIG_DIR; ///< Application configuration directory.
  static const File& DATA_DIR;   ///< Application data/state directory.

public:

  static Config defaults; ///< Default configuration.
  static Json   config;   ///< Configuration.

public:

  /**
   * Switch to another game stage.
   */
  static void setStage(Stage* stage);

  /**
   * Run application.
   */
  static void run(Stage* initialStage);

};

}
