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

#include "Window.hh"

#include "GL.hh"
#include "Input.hh"

#include <AL/alc.h>
#include <cstdio>
#include <png.h>
#include <SDL2/SDL.h>

#ifdef __native_client__
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/graphics_3d.h>
# include <ppapi/cpp/instance.h>
# include <ppapi/cpp/module.h>
# include <ppapi/gles2/gl2ext_ppapi.h>
# include <ppapi_simple/ps.h>
# include <ppapi_simple/ps_interface.h>

extern "C" void alSetPpapiInfo(PP_Instance instance, PPB_GetInterface getInterface);
#endif

namespace oz
{

struct ScreenshotInfo
{
  String basePath;
  int    width;
  int    height;
  ubyte* pixels;
};

#ifdef __native_client__
static const PPB_Fullscreen* ppbFullscreen      = nullptr;
static const PPB_MouseLock*  ppbMouseLock       = nullptr;
static pp::Graphics3D*       glContext          = nullptr;
static Semaphore             flushSemaphore;
static Semaphore             mouseLockSemaphore;
#else
static SDL_GLContext         glContext          = nullptr;
#endif
static ALCdevice*            alDevice           = nullptr;
static ALCcontext*           alContext          = nullptr;
static SDL_Window*           window             = nullptr;
static Thread                screenshotThread;

#ifdef __native_client__

static void onFlushComplete(void*, int)
{
  flushSemaphore.post();
}

#endif

static void* screenshotMain(void* data)
{
  const ScreenshotInfo* info = static_cast<const ScreenshotInfo*>(data);

  String path = String::format("%s %s.png", info->basePath.c(), Time::local().toString().c());
  FILE*  file = fopen(path, "wb");

  if (file == nullptr) {
    return nullptr;
  }

  int         stride  = Alloc::alignUp<int>(info->width * 3, 4);
  png_struct* png     = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  png_info*   pngInfo = png_create_info_struct(png);

  png_set_IHDR(png, pngInfo, info->width, info->height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_init_io(png, file);
  png_write_info(png, pngInfo);

  for (int i = info->height - 1; i >= 0; --i) {
    png_write_row(png, &info->pixels[i * stride]);
  }

  png_write_end(png, pngInfo);
  png_destroy_write_struct(&png, &pngInfo);

  delete[] info->pixels;
  delete info;

  return nullptr;
}

int          Window::screenWidth  = 0;
int          Window::screenHeight = 0;
int          Window::windowWidth  = 0;
int          Window::windowHeight = 0;
Window::Mode Window::windowMode   = Window::WINDOWED;
bool         Window::windowActive = true;
bool         Window::windowFocus  = true;
bool         Window::windowGrab   = false;

void Window::measureScreen()
{
  SDL_DisplayMode mode;
  SDL_GetDesktopDisplayMode(0, &mode);

  screenWidth  = mode.w;
  screenHeight = mode.h;
}

bool Window::isCreated()
{
#ifdef __native_client__
  return !glContext->is_null();
#else
  return window != nullptr;
#endif
}

void Window::warpMouse()
{
  SDL_WarpMouseInWindow(window, windowWidth / 2, windowHeight / 2);
}

void Window::setGrab(bool grab)
{
#ifdef __native_client__

  if (grab && !windowGrab) {
    MainCall() += []
    {
      ppbMouseLock->LockMouse(PSGetInstanceId(), PP_MakeCompletionCallback([](void*, int result)
      {
        windowGrab = result == PP_OK;
        mouseLockSemaphore.post();
      },
      nullptr));
    };
    mouseLockSemaphore.wait();
  }
  else if (!grab && windowGrab) {
    MainCall() << []
    {
      ppbMouseLock->UnlockMouse(PSGetInstanceId());
      windowGrab = false;
    };
  }

#else

  bool success = SDL_SetRelativeMouseMode(SDL_bool(grab)) == 0;

  windowGrab = grab && success;

#endif
}

void Window::swapBuffers()
{
#ifdef __native_client__

  MainCall() << []
  {
    glContext->SwapBuffers(pp::CompletionCallback(onFlushComplete, nullptr));
  };
  flushSemaphore.wait();

#else

  SDL_GL_SwapWindow(window);

#endif
}

void Window::screenshot(const char* basePath)
{
  if (screenshotThread.isValid()) {
    screenshotThread.join();
  }

  int    stride = Alloc::alignUp<int>(windowWidth * 3, 4);
  ubyte* pixels = new ubyte[windowHeight * stride];

  ScreenshotInfo* info = new ScreenshotInfo{basePath, windowWidth, windowHeight, pixels};

  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, info->pixels);
  screenshotThread = Thread("screenshot", screenshotMain, info);
}

void Window::resize(int newWidth, int newHeight, Mode newMode)
{
  if (windowMode != WINDOWED) {
    SDL_SetWindowFullscreen(window, 0);
    SDL_RestoreWindow(window);
  }

  if (newWidth != 0 && newHeight != 0) {
    SDL_SetWindowSize(window, newWidth, newHeight);
  }

  if (newMode != WINDOWED) {
    SDL_SetWindowFullscreen(window,
                            newMode == EXCLUSIVE
                            ? SDL_WINDOW_FULLSCREEN
                            : SDL_WINDOW_FULLSCREEN_DESKTOP);
  }

  windowMode = newMode;
}

void Window::minimise()
{
  SDL_MinimizeWindow(window);
}

bool Window::processEvent(const SDL_Event* event)
{
  switch (event->type) {
    case SDL_WINDOWEVENT: {
      switch (event->window.event) {
        case SDL_WINDOWEVENT_FOCUS_GAINED: {
          windowFocus = true;
          break;
        }
        case SDL_WINDOWEVENT_FOCUS_LOST: {
          windowFocus = false;
          break;
        }
        case SDL_WINDOWEVENT_RESIZED: {
          windowWidth  = event->window.data1;
          windowHeight = event->window.data2;

#ifdef __native_client__
          MainCall() << []
          {
            ppbFullscreen->SetFullscreen(PSGetInstanceId(), PP_Bool(windowMode != WINDOWED));

            glSetCurrentContextPPAPI(0);
            glContext->ResizeBuffers(windowWidth, windowHeight);
            glSetCurrentContextPPAPI(glContext->pp_resource());
          };
#endif
          break;
        }
        case SDL_WINDOWEVENT_MINIMIZED: {
          alcSuspendContext(alContext);
          windowActive = false;
          break;
        }
        case SDL_WINDOWEVENT_RESTORED: {
          alcProcessContext(alContext);
          windowActive = true;
          break;
        }
        case SDL_WINDOWEVENT_CLOSE: {
          return false;
        }
      }
      break;
    }
    case SDL_QUIT: {
      return false;
    }
  }
  return true;
}

bool Window::create(const char* title, int width, int height, Mode mode)
{
  destroy();

  bool success = false;

  Log::print("Initialising SDL ... ");

#ifdef __native_client__
  success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) == 0;
#else
  success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_NOPARACHUTE) == 0;
#endif

  if (success) {
    Log::printEnd("OK");
  }
  else {
    Log::printEnd("Failed");
    return false;
  }

  measureScreen();

  windowWidth  = width  == 0 ? screenWidth  : width;
  windowHeight = height == 0 ? screenHeight : height;
  windowMode   = mode;
  windowFocus  = true;

  Log::print("Creating window %dx%d [%s] ... ", windowWidth, windowHeight,
             mode == EXCLUSIVE ? "exclusive" : mode == DESKTOP ? "desktop" : "windowed");

  uint flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE |
               (mode == EXCLUSIVE ? SDL_WINDOW_FULLSCREEN
                : mode == DESKTOP ? SDL_WINDOW_FULLSCREEN_DESKTOP
                : 0);

  // Force old Mesa drivers to turn on partial S3TC support even when libtxc_dxtn is not present.
  // We don't use online texture compression anywhere so partial S3TC support is enough.
#ifdef __unix__
  SDL_setenv("force_s3tc_enable", "true", true);
#endif

# ifdef OZ_GL_ES
# ifndef __EMSCRIPTEN__
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_ES);
# endif
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
# else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
# endif
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,            24);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,            0);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,          0);

  window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            windowWidth, windowHeight, flags);
  if (window == nullptr) {
    Log::printEnd("Window creation failed");
    return false;
  }

#ifdef __native_client__

  ppbFullscreen = static_cast<const PPB_Fullscreen*>(PSGetInterface(PPB_FULLSCREEN_INTERFACE));
  ppbMouseLock  = static_cast<const PPB_MouseLock*>(PSGetInterface(PPB_MOUSELOCK_INTERFACE));
  windowMode    = WINDOWED;

  MainCall() << []
  {
    glInitializePPAPI(pp::Module::Get()->get_browser_interface());

    int attribs[] = {
      PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
      PP_GRAPHICS3DATTRIB_WIDTH, windowWidth,
      PP_GRAPHICS3DATTRIB_HEIGHT, windowHeight,
      PP_GRAPHICS3DATTRIB_NONE
    };

    glContext = new pp::Graphics3D(pp::InstanceHandle(PSGetInstanceId()),
                                   pp::Graphics3D(),
                                   attribs);

    if (glContext->is_null()) {
      Log::printEnd("Failed to create OpenGL context");
      delete glContext;
    }
    else if (!PSInterfaceInstance()->BindGraphics(PSGetInstanceId(), glContext->pp_resource())) {
      Log::printEnd("Failed to bind Graphics3D");
      delete glContext;
    }
    else {
      glSetCurrentContextPPAPI(glContext->pp_resource());

      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      glFlush();

      glContext->SwapBuffers(pp::CompletionCallback(onFlushComplete, nullptr));
    }
  };
  flushSemaphore.wait();

  if (glContext->is_null()) {
    Log::printEnd("Failed to create OpenGL context");
    return false;
  }

#else

  SDL_ClearError();

  glContext = SDL_GL_CreateContext(window);

  if (!String::isEmpty(SDL_GetError())) {
    Log::printEnd("Failed to create OpenGL context: %s", SDL_GetError());
    return false;
  }

  SDL_GL_SetSwapInterval(1);
  GL::init();

  glViewport(0, 0, windowWidth, windowHeight);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glFlush();

  SDL_GL_SwapWindow(window);

#endif

#ifdef __native_client__
  alSetPpapiInfo(PSGetInstanceId(), pp::Module::Get()->get_browser_interface());
#endif

  alDevice = alcOpenDevice(nullptr);
  if (alDevice == nullptr) {
    Log::printEnd("Failed to open default OpenAL device");
    return false;
  }

  alContext = alcCreateContext(alDevice, nullptr);
  if (alContext == nullptr) {
    Log::printEnd("Failed to create OpenAL context");
    return false;
  }

  if (alcMakeContextCurrent(alContext) != ALC_TRUE) {
    Log::printEnd("Failed to activate OpenAL context");
    return false;
  }

  Log::printEnd("OK");
  return true;
}

void Window::destroy()
{
  if (screenshotThread.isValid()) {
    screenshotThread.join();
  }

  if (window != nullptr) {
    if (alContext != nullptr) {
      alcDestroyContext(alContext);
      alContext = nullptr;
    }

    if (alDevice != nullptr) {
      alcCloseDevice(alDevice);
      alDevice = nullptr;
    }

    if (glContext != nullptr) {
#ifdef __native_client__
      MainCall() << []
      {
        glSetCurrentContextPPAPI(0);
        delete glContext;
        glTerminatePPAPI();
      };
#else
      SDL_GL_DeleteContext(glContext);
#endif
      glContext = nullptr;
    }

    SDL_DestroyWindow(window);
    window = nullptr;

    SDL_Quit();
  }
}

}
