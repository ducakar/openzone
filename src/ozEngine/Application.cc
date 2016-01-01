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

namespace oz
{

static Application::Stage* currentStage = nullptr;
static Application::Stage* nextStage    = nullptr;

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

Application::Config Application::config;

void Application::setStage(Stage* stage)
{
  nextStage = stage;
}

void Application::run(Stage* initialStage)
{
  currentStage = initialStage;
  nextStage    = initialStage;

  File::init();

  if (!Window::create(config.window.title, config.window.width, config.window.height,
                      config.window.mode))
  {
    OZ_ERROR("oz::Application: Window creation failed");
  }

  uint timeLast   = Time::uclock();
  uint timeSpent  = 0;
  uint tickTime   = Math::lround(config.timing.tickTime * 1000000.0f);
  uint frameTicks = 0;

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
              PP_Point point = ppbMouseInputEvent->GetMovement(psEvent->as_resource);

              input.mouseX += point.x;
              input.mouseY -= point.y;
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

    input.keys[SDLK_ESCAPE]    = false;
    input.oldKeys[SDLK_ESCAPE] = false;

#endif

    if (nextStage != currentStage) {
      currentStage->unload();

      if (nextStage == nullptr) {
        break;
      }

      //input.reset();

      currentStage = nextStage;
      currentStage->load();

      timeLast = Time::uclock();
      continue;
    }

    // Waste time when iconified.
    if (!Window::isActive()) {
      Time::usleep(tickTime);

      timeSpent = Time::uclock() - timeLast;
      timeLast += timeSpent;
      continue;
    }

    currentStage->update();

    frameTicks += 1;
    timeSpent   = Time::uclock() - timeLast;

    // Skip rendering graphics, only play sounds if there's not enough time left.
    if (timeSpent >= tickTime) {
      currentStage->present(false);
    }
    else {
      currentStage->present(true);

      frameTicks = 0;
      timeSpent  = Time::uclock() - timeLast;

      // Sleep if there's still some time left.
      if (timeSpent < tickTime) {
        Time::usleep(tickTime - timeSpent);
        timeSpent = tickTime;
      }
    }

    // Drop skip time if we are more than 100 ms behind.
    if (timeSpent > 100 * 1000) {
      timeLast += timeSpent - tickTime;
    }
    timeLast += tickTime;
  }
  while (true);

  Window::destroy();
  File::destroy();
}

}
