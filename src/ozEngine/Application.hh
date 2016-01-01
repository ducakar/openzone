/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

    virtual ~Stage();

    virtual void load();
    virtual void unload();

    virtual void update();
    virtual void present(bool isEnoughTime);
  };

  struct Config
  {
    struct WindowConfig
    {
      const char*  title  = "Untitled";
      int          width  = 800;
      int          height = 600;
      Window::Mode mode   = Window::WINDOWED;
    };

    struct TimingConfig
    {
      float tickTime = 1.0f / 60.0f;
      bool  isFixed  = true;
    };

    const char*  name;
    WindowConfig window;
    TimingConfig timing;
  };

  static Config config;

public:

  Application() = delete;

  static void setStage(Stage* stage);

  static void run(Stage* initialStage);

};

}
