/*
 * ozCore - OpenZone Core Library.
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
 * @file ozCore/Pepper.cc
 */

#ifdef __native_client__

#include "Pepper.hh"

#include "List.hh"
#include "SpinLock.hh"
#include "Thread.hh"

#include <nacl_io/nacl_io.h>
#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/core.h>
#include <ppapi/cpp/fullscreen.h>
#include <ppapi/cpp/graphics_2d.h>
#include <ppapi/cpp/input_event.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/mouse_lock.h>

#include <SDL_nacl.h>

OZ_WEAK
void SDL_NACL_SetInstance(PP_Instance, PPB_GetInterface, int, int)
{}

OZ_WEAK
void SDL_NACL_PushEvent(PP_Resource)
{}

namespace oz
{

static SpinLock      messageLock;
static List<String>  messageQueue;
static Thread        mainThread;

static pp::Core*     ppCore     = nullptr;
static pp::Instance* ppInstance = nullptr;

struct Pepper::Instance : pp::Instance, pp::MouseLock
{
  pp::Fullscreen fullscreen;
  bool           isStarted;

  static void mainThreadMain(void*);
  static void onMouseLocked(void*, int result);

  explicit Instance(PP_Instance instance_);
  ~Instance() override;

  void DidChangeView(const pp::View& view) override;
  void HandleMessage(const pp::Var& message) override;
  bool HandleInputEvent(const pp::InputEvent& event) override;
  void MouseLockLost() override;
};

OZ_HIDDEN
void Pepper::Instance::mainThreadMain(void*)
{
  Instance* instance = static_cast<Instance*>(ppInstance);
  char      argv0[]  = "/";
  char*     argv[]   = { argv0, nullptr };

  naclMain(1, argv);

  instance->fullscreen.SetFullscreen(false);
  instance->UnlockMouse();
}

OZ_HIDDEN
void Pepper::Instance::onMouseLocked(void*, int result)
{
  hasFocus = result == PP_OK;
}

OZ_HIDDEN
Pepper::Instance::Instance(PP_Instance instance_) :
  pp::Instance(instance_), pp::MouseLock(this), fullscreen(this), isStarted(false)
{
  ppCore     = pp::Module::Get()->core();
  ppInstance = this;

  nacl_io_init_ppapi(instance_, pp::Module::Get()->get_browser_interface());
  RequestInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD | PP_INPUTEVENT_CLASS_MOUSE |
                     PP_INPUTEVENT_CLASS_WHEEL);
}

OZ_HIDDEN
Pepper::Instance::~Instance()
{
  messageQueue.clear();
  messageQueue.trim();

  nacl_io_uninit();

  ppInstance = nullptr;
}

OZ_HIDDEN
void Pepper::Instance::DidChangeView(const pp::View& view)
{
  width  = view.GetRect().width();
  height = view.GetRect().height();

  if (!isStarted) {
    SDL_NACL_SetInstance(pp_instance(), pp::Module::Get()->get_browser_interface(), width, height);

    mainThread = Thread("naclMain", mainThreadMain);
    mainThread.detach();
    isStarted = true;
  }
}

OZ_HIDDEN
void Pepper::Instance::HandleMessage(const pp::Var& message)
{
  push(message.AsString().c_str());
}

OZ_HIDDEN
bool Pepper::Instance::HandleInputEvent(const pp::InputEvent& event)
{
  switch (event.GetType()) {
    case PP_INPUTEVENT_TYPE_MOUSEDOWN: {
      if (!Pepper::hasFocus) {
        LockMouse(pp::CompletionCallback(onMouseLocked, this));
        return true;
      }
      break;
    }
    case PP_INPUTEVENT_TYPE_MOUSEMOVE: {
      pp::MouseInputEvent mouseEvent(event);
      pp::Point move = mouseEvent.GetMovement();

      moveX += float(move.x());
      moveY += float(move.y());
      break;
    }
    case PP_INPUTEVENT_TYPE_WHEEL: {
      pp::WheelInputEvent wheelEvent(event);
      pp::FloatPoint move = wheelEvent.GetDelta();

      moveZ += move.x();
      moveW += move.y();
      break;
    }
    case PP_INPUTEVENT_TYPE_KEYDOWN: {
      pp::KeyboardInputEvent keyEvent(event);

      if ((keyEvent.GetKeyCode() == 122 || keyEvent.GetKeyCode() == 13) &&
          event.GetModifiers() == 0)
      {
        fullscreen.SetFullscreen(!fullscreen.IsFullscreen());
        return true;
      }
      break;
    }
    default: {
      break;
    }
  }

  SDL_NACL_PushEvent(event.pp_resource());
  return true;
}

OZ_HIDDEN
void Pepper::Instance::MouseLockLost()
{
  hasFocus = false;
  fullscreen.SetFullscreen(false);
}

struct Pepper::Module : pp::Module
{
  pp::Instance* CreateInstance(PP_Instance instance) override;
};

OZ_HIDDEN
pp::Instance* Pepper::Module::CreateInstance(PP_Instance instance)
{
  return new Instance(instance);
}

int   Pepper::width    = 0;
int   Pepper::height   = 0;
float Pepper::moveX    = 0.0f;
float Pepper::moveY    = 0.0f;
float Pepper::moveZ    = 0.0f;
float Pepper::moveW    = 0.0f;
bool  Pepper::hasFocus = false;

bool Pepper::isMainThread()
{
  return ppCore->IsMainThread();
}

void Pepper::mainCall(Callback* callback, void* data)
{
  ppCore->CallOnMainThread(0, pp::CompletionCallback(callback, data));
}

pp::Instance* Pepper::instance()
{
  return ppInstance;
}

void Pepper::post(const char* message)
{
  pp::Var var(message);

  MainCall() << [&]
  {
    ppInstance->PostMessage(var);
  };
}

String Pepper::pop()
{
  messageLock.lock();
  String s = messageQueue.isEmpty() ? String() : messageQueue.popFirst();
  messageLock.unlock();

  return s;
}

void Pepper::push(const char* message)
{
  messageLock.lock();
  messageQueue.pushLast(message);
  messageLock.unlock();
}

pp::Module* Pepper::createModule()
{
  return new Module();
}

thread_local Semaphore* MainCall::localSemaphore = nullptr;

}

#endif
