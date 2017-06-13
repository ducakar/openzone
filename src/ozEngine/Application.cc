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

#include "Application.hh"

#include "Input.hh"

#include <SDL2/SDL.h>

#ifdef __native_client__
# include <ppapi_simple/ps.h>
# include <ppapi_simple/ps_event.h>
# include <ppapi_simple/ps_interface.h>
# include <ppapi_simple/ps_main.h>
# include <sys/mount.h>

extern "C" void NACL_SetScreenResolution(int width, int height, Uint32 format);
#endif

namespace oz
{

static const EnumMap<Window::Mode> WINDOW_MODES = {
  {Window::WINDOWED, "WINDOWED"},
  {Window::DESKTOP, "DESKTOP"},
  {Window::EXCLUSIVE, "EXCLUSIVE"}
};

static Application::Stage*   currentStage       = nullptr;
static Application::Stage*   nextStage          = nullptr;
static File                  configDir;
static File                  dataDir;
#ifdef __native_client__
static const PPB_InputEvent* ppbInputEvent      = nullptr;
#endif

Application::Stage::~Stage()
{}

void Application::Stage::load()
{}

void Application::Stage::unload()
{}

void Application::Stage::update()
{}

void Application::Stage::present(bool)
{}

const File&         Application::CONFIG_DIR = configDir;
const File&         Application::DATA_DIR   = dataDir;
Application::Config Application::defaults;
Json                Application::config;

void Application::setStage(Stage* stage)
{
  nextStage = stage;
}

void Application::run(Stage* initialStage)
{
  if (initialStage == nullptr) {
    OZ_ERROR("oz::Application: initialStage must point to a valid stage");
  }

  nextStage = initialStage;

#ifdef __native_client__
  Pepper::init();

  const PPB_View* view = PSInterfaceView();

  PSEventSetFilter(PSE_INSTANCE_DIDCHANGEVIEW);

  PP_Rect  rect;
  PSEvent* event = PSEventWaitAcquire();
  view->GetRect(event->as_resource, &rect);

  PSEventRelease(event);

  NACL_SetScreenResolution(rect.size.width, rect.size.height, 0);
  SDL_SetMainReady();
#endif

  File::init();

  configDir = File::CONFIG + "/" + defaults.name;
  dataDir   = File::DATA + "/" + defaults.name;

  if (defaults.loadConfig) {
    File configFile = configDir + "/config.json";

    Log::print("Loading configuration from `%s' ... ", configFile.c());

    bool success = config.load(configDir + "/config.json");

    Log::printEnd(success ? "OK" : "Failed");
  }

  if (config.type() != Json::OBJECT) {
    config = Json::OBJECT;
  }

  Json& windowConfig = config.include("window", Json::OBJECT);

  if (!Window::create(defaults.window.title,
                      windowConfig.include("width", defaults.window.width).get(0),
                      windowConfig.include("height", defaults.window.height).get(0),
                      windowConfig.include("mode", WINDOW_MODES[defaults.window.mode])
                      .get(Window::WINDOWED, WINDOW_MODES)))
  {
    OZ_ERROR("oz::Application: Window creation failed");
  }

  uint     fps        = config["timing"]["fps"].get(defaults.timing.fps);
  uint     nTicks     = 0;
  uint     frameTicks = 0;
  Duration tickTime   = 0_s;
  Duration timeLast   = Time::clock();
  Duration timeSpent  = 0_s;

  do {
    // read input & events
//    input.prepare();

#ifdef __native_client__
    PSEventSetFilter(PSE_INSTANCE_HANDLEINPUT | PSE_INSTANCE_DIDCHANGEVIEW |
                     PSE_MOUSELOCK_MOUSELOCKLOST);

    List<PSEvent*> eventQueue;
    PSEvent*       psEvent;

    while ((psEvent = PSEventTryAcquire()) != nullptr) {
      switch (psEvent->type) {
        case PSE_INSTANCE_HANDLEINPUT: {
          PP_InputEvent_Type type = ppbInputEvent->GetType(psEvent->as_resource);

          switch (type) {
            case PP_INPUTEVENT_TYPE_MOUSEMOVE: {
//              PP_Point point = ppbMouseInputEvent->GetMovement(psEvent->as_resource);

//              input.mouseX += point.x;
//              input.mouseY -= point.y;
              break;
            }
            default: {
              eventQueue.add(psEvent);
              continue;
            }
          }
          break;
        }
        case PSE_INSTANCE_DIDCHANGEFOCUS:
        case PSE_MOUSELOCK_MOUSELOCKLOST:
        case PSE_INSTANCE_DIDCHANGEVIEW: {
          eventQueue.add(psEvent);
          break;
        }
        default: {
          break;
        }
      }

      PSEventRelease(psEvent);
    }

    // Repost relevant events for SDL to process them.
    for (PSEvent* event : eventQueue) {
      PSEventPostResource(event->type, event->as_resource);
    }

    PSEventSetFilter(PSE_ALL);
#endif

    SDL_Event event;
    SDL_PumpEvents();

    while (SDL_PollEvent(&event) != 0) {
      if (!Window::processEvent(&event)) {
        nextStage = nullptr;
      }

      switch (event.type) {
        case SDL_KEYUP: {
          const SDL_Keysym& keysym = event.key.keysym;

          if (keysym.sym == SDLK_F9) {
          }
          else if (keysym.sym == SDLK_F11) {
            if (keysym.mod & KMOD_CTRL) {
              Window::setGrab(!Window::hasGrab());
            }
            else if (Window::mode() == Window::DESKTOP) {
              Window::resize(0, 0, Window::WINDOWED);
            }
            else {
              Window::resize(0, 0, Window::DESKTOP);
            }
          }
#ifndef __native_client__
          else if (keysym.sym == SDLK_F12) {
            if (keysym.mod & KMOD_CTRL) {
              nextStage = nullptr;
            }
            else {
              Window::minimise();
            }
          }
#endif

//          input.readEvent(&event);
          break;
        }
      }
    }

#ifdef __native_client__
//    input.keys[SDLK_ESCAPE]    = false;
//    input.oldKeys[SDLK_ESCAPE] = false;
#endif

    // Waste time when iconified.
    if (!Window::isActive()) {
      Time::sleep(1_s / fps);

      timeSpent = Time::clock() - timeLast;
      timeLast += timeSpent;
      continue;
    }

    if (nextStage != currentStage) {
      if (currentStage != nullptr) {
        currentStage->unload();
      }

      currentStage = nextStage;

      if (currentStage == nullptr) {
        break;
      }

//      input.reset();

      currentStage->load();

      timeLast = Time::clock();
      continue;
    }

    tickTime = (nTicks + 1) * 1_s / fps - nTicks * 1_s / fps;
    nTicks   = (nTicks + 1) % fps;

    currentStage->update();

    frameTicks += 1;
    timeSpent   = Time::clock() - timeLast;

    // Skip rendering graphics, only play sounds if there's not enough time left.
    if (timeSpent >= tickTime) {
      currentStage->present(false);
    }
    else {
      currentStage->present(true);

      frameTicks = 0;
      timeSpent  = Time::clock() - timeLast;

      // Sleep if there's still some time left.
      if (timeSpent < tickTime) {
        Time::sleep(tickTime - timeSpent);
        timeSpent = tickTime;
      }
    }

    // Drop skip time if we are more than 100 ms behind.
    if (timeSpent > 100_ms) {
      timeLast += timeSpent - tickTime;
    }

    timeLast += tickTime;
  }
  while (true);

  if (defaults.saveConfig) {
    File configFile = configDir + "/config.json";

    Log::print("Saving configuration to `%s' ... ", configFile.c());

    configDir.mkdir();
    bool success = config.save(configFile);

    Log::printEnd(success ? "OK" : "Failed");
  }

  Window::destroy();
  File::destroy();
}

}
