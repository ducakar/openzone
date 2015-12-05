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

#include "Window.hh"

#include "GL.hh"
#include "Input.hh"

#include <AL/alc.h>
#include <SDL.h>
#include <cstdio>
#include <jpeglib.h>

#ifdef __native_client__
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/graphics_3d.h>
# include <ppapi/cpp/instance.h>
# include <ppapi/gles2/gl2ext_ppapi.h>
# include <ppapi_simple/ps.h>
# include <ppapi_simple/ps_interface.h>
#endif

namespace oz
{

struct ScreenshotInfo
{
  File  file;
  int   quality;
  int   width;
  int   height;
  char* pixels;
};

#ifdef __native_client__
static const PPB_Fullscreen* ppbFullscreen      = nullptr;
static pp::Graphics3D*       glContext          = nullptr;
static Semaphore             flushSemaphore;
#else
static SDL_GLContext         glContext          = nullptr;
#endif
static ALCdevice*            alDevice           = nullptr;
static ALCcontext*           alContext          = nullptr;
static SDL_Window*           window             = nullptr;
static Thread                screenshotThread;
static bool                  inputGrab          = false;

#ifdef __native_client__

static void onFlushComplete(void*, int)
{
  flushSemaphore.post();
}

#endif

static void screenshotMain(void* data)
{
  const ScreenshotInfo* info = static_cast<const ScreenshotInfo*>(data);

  FILE* file = fopen(info->file, "wb");

  if (file != nullptr) {
    jpeg_compress_struct cinfo;
    jpeg_error_mgr       jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, file);

    cinfo.image_width      = info->width;
    cinfo.image_height     = info->height;
    cinfo.input_components = 3;
    cinfo.in_color_space   = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, info->quality, boolean(true));
    jpeg_start_compress(&cinfo, boolean(true));

    int pitch = ((info->width * 3 + 3) / 4) * 4;

    while (cinfo.next_scanline < cinfo.image_height) {
      int      line = cinfo.image_height - 1 - cinfo.next_scanline;
      JSAMPROW row  = reinterpret_cast<ubyte*>(&info->pixels[line * pitch]);

      jpeg_write_scanlines(&cinfo, &row, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(file);
  }

  delete[] info->pixels;
  delete info;
}

int  Window::screenWidth  = 0;
int  Window::screenHeight = 0;
int  Window::windowWidth  = 0;
int  Window::windowHeight = 0;
bool Window::fullscreen   = false;
bool Window::windowFocus  = true;

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

void Window::screenshot(const File& file, int quality)
{
  if (screenshotThread.isValid()) {
    screenshotThread.join();
  }

  int   pitch  = ((windowWidth * 3 + 3) / 4) * 4;
  char* pixels = new char[windowHeight * pitch];

  ScreenshotInfo* info = new ScreenshotInfo{file, quality, windowWidth, windowHeight, pixels};

  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, info->pixels);
  screenshotThread = Thread("screenshot", screenshotMain, info);
}

void Window::resize(int newWidth, int newHeight, bool fullscreen_)
{
  newWidth  = newWidth  == 0 ? screenWidth  : newWidth;
  newHeight = newHeight == 0 ? screenHeight : newHeight;

  if (fullscreen) {
    SDL_SetWindowFullscreen(window, SDL_bool(fullscreen_));
    SDL_SetWindowSize(window, newWidth, newHeight);
    fullscreen = false;
  }
  else {
    SDL_SetWindowSize(window, newWidth, newHeight);
    fullscreen = SDL_SetWindowFullscreen(window, SDL_bool(fullscreen_)) == 0;
  }
}

void Window::minimise()
{
  SDL_MinimizeWindow(window);
}

bool Window::processEvent(const SDL_Event* event)
{
  switch (event->type) {
    case SDL_WINDOWEVENT_FOCUS_GAINED: {
      windowFocus = true;

      Input::setGrab(inputGrab);
      break;
    }
    case SDL_WINDOWEVENT_FOCUS_LOST: {
      windowFocus = false;
      inputGrab   = Input::hasGrab();

      Input::setGrab(false);
      break;
    }
    case SDL_WINDOWEVENT_RESIZED: {
      windowWidth  = event->window.data1;
      windowHeight = event->window.data2;

      Log::print("Resizing window to %dx%d [%s] ... ",
                 windowWidth, windowHeight, fullscreen ? "fullscreen" : "windowed");

#ifdef __native_client__
      MainCall() << []
      {
        ppbFullscreen->SetFullscreen(PSGetInstanceId(), PP_Bool(fullscreen));

        glSetCurrentContextPPAPI(0);
        glContext->ResizeBuffers(windowWidth, windowHeight);
        glSetCurrentContextPPAPI(glContext->pp_resource());
      };
#endif

      Log::printEnd("OK");
      break;
    }
    case SDL_WINDOWEVENT_MINIMIZED: {
      alcSuspendContext(alContext);
      break;
    }
    case SDL_WINDOWEVENT_RESTORED: {
      alcProcessContext(alContext);
      break;
    }
    case SDL_WINDOWEVENT_CLOSE:
    case SDL_QUIT: {
      return false;
    }
  }
  return true;
}

bool Window::create(const char* title, int width, int height, bool fullscreen_)
{
  destroy();

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_NOPARACHUTE) != 0) {
    Log::println("Failed to initialise SDL");
    return false;
  }

  measureScreen();

  windowWidth  = width  == 0 ? screenWidth  : width;
  windowHeight = height == 0 ? screenHeight : height;
  fullscreen   = fullscreen_;
  windowFocus  = true;

  uint flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE |
               (fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

  Log::print("Creating window %dx%d [%s] ... ",
             windowWidth, windowHeight, fullscreen ? "fullscreen" : "windowed");

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
  fullscreen    = false;

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
  }

  SDL_Quit();
}

}
