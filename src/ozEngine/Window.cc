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
 * @file ozEngine/Window.cc
 */

#include "Window.hh"

#include "GL.hh"

#include <cstdio>
#include <jpeglib.h>
#include <SDL.h>

#ifdef __native_client__
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/instance.h>
# include <ppapi/cpp/graphics_3d.h>
# include <ppapi/gles2/gl2ext_ppapi.h>
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
static Semaphore       flushSemaphore;
static pp::Graphics3D* context = nullptr;
#else
static SDL_Window*     descriptor;
static SDL_GLContext   context;
#endif
static Thread          screenshotThread;

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
    jpeg_set_quality(&cinfo, info->quality, true);
    jpeg_start_compress(&cinfo, true);

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

#ifdef __native_client__

static void flushCompleteCallback(void*, int)
{
  flushSemaphore.post();
}

#endif

int  Window::screenWidth  = 0;
int  Window::screenHeight = 0;
int  Window::windowWidth  = 0;
int  Window::windowHeight = 0;
bool Window::fullscreen   = false;
bool Window::windowFocus  = true;
bool Window::windowGrab   = false;

void Window::measureScreen()
{
#ifdef __native_client__

  screenWidth  = Pepper::width;
  screenHeight = Pepper::height;

#else

  SDL_DisplayMode mode;
  SDL_GetDesktopDisplayMode(0, &mode);

  screenWidth  = mode.w;
  screenHeight = mode.h;

#endif
}

bool Window::isCreated()
{
#ifdef __native_client__
  return !context->is_null();
#else
  return descriptor != nullptr;
#endif
}

void Window::setGrab(bool grab)
{
  windowGrab = grab;

#if SDL_MAJOR_VERSION < 2
  SDL_ShowCursor(!windowGrab);
  SDL_WM_GrabInput(SDL_GrabMode(windowGrab));
#else
  SDL_SetRelativeMouseMode(SDL_bool(windowGrab));
#endif
}

void Window::warpMouse()
{
  if (!windowFocus || !windowGrab) {
    return;
  }

#ifdef __native_client__

  Pepper::moveX = 0.0f;
  Pepper::moveY = 0.0f;
  Pepper::moveZ = 0.0f;
  Pepper::moveW = 0.0f;

#else

  SDL_WarpMouseInWindow(descriptor, windowWidth / 2, windowHeight / 2);
  SDL_PumpEvents();
  SDL_GetRelativeMouseState(nullptr, nullptr);

#endif
}

void Window::swapBuffers()
{
#if defined(__native_client__)

  MainCall() << []
  {
    context->SwapBuffers(pp::CompletionCallback(flushCompleteCallback, nullptr));
  };
  flushSemaphore.wait();

#else

  SDL_GL_SwapWindow(descriptor);

#endif
}

void Window::screenshot(const File& file, int quality)
{
  if (screenshotThread.isValid()) {
    screenshotThread.join();
  }

  int   pitch  = ((windowWidth * 3 + 3) / 4) * 4;
  char* pixels = new char[windowWidth * pitch];

  ScreenshotInfo* info = new ScreenshotInfo{ file, quality, windowWidth, windowHeight, pixels };

  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, info->pixels);
  screenshotThread = Thread("screenshot", screenshotMain, info);
}

void Window::minimise()
{
#ifndef __native_client__
  SDL_RestoreWindow(descriptor);
  SDL_MinimizeWindow(descriptor);
#endif
}

bool Window::resize(int newWidth, int newHeight, bool fullscreen_)
{
#ifdef __native_client__

  static_cast<void>(fullscreen_);

  windowWidth  = newWidth;
  windowHeight = newHeight;

  Log::print("Resizing OpenGL window to %dx%d ... ", windowWidth, windowHeight);

  MainCall() << []
  {
    glSetCurrentContextPPAPI(0);
    context->ResizeBuffers(windowWidth, windowHeight);
    glSetCurrentContextPPAPI(context->pp_resource());
  };

  Log::printEnd("OK");

#else

  windowWidth  = windowWidth  == 0 ? screenWidth  : newWidth;
  windowHeight = windowHeight == 0 ? screenHeight : newHeight;
  fullscreen   = fullscreen_;

  Log::print("Resizing OpenGL window to %dx%d [%s] ... ",
             windowWidth, windowHeight, fullscreen ? "fullscreen" : "windowed");

  if (fullscreen) {
    SDL_SetWindowSize(descriptor, windowWidth, windowHeight);
    SDL_SetWindowFullscreen(descriptor, SDL_TRUE);
  }
  else {
    SDL_SetWindowFullscreen(descriptor, SDL_FALSE);
    SDL_SetWindowSize(descriptor, windowWidth, windowHeight);
  }

  Log::printEnd("OK");

#endif

  return true;
}

bool Window::create(const char* title, int width, int height, bool fullscreen_)
{
  destroy();
  measureScreen();

  windowWidth  = width  == 0 ? screenWidth  : width;
  windowHeight = height == 0 ? screenHeight : height;
  fullscreen   = fullscreen_;
  windowFocus  = true;
  windowGrab   = true;

#ifdef __native_client__

  static_cast<void>(title);

  windowWidth  = screenWidth;
  windowHeight = screenHeight;

  Log::print("Creating OpenGL surface %dx%d ... ", windowWidth, windowHeight);

  MainCall() << []
  {
    glInitializePPAPI(pp::Module::Get()->get_browser_interface());

    int attribs[] = {
      PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
      PP_GRAPHICS3DATTRIB_WIDTH, windowWidth,
      PP_GRAPHICS3DATTRIB_HEIGHT, windowHeight,
      PP_GRAPHICS3DATTRIB_NONE
    };

    context = new pp::Graphics3D(Pepper::instance(), pp::Graphics3D(), attribs);

    if (context->is_null()) {
      Log::printEnd("Failed to create OpenGL context");

      delete context;
      context = nullptr;
    }
    else if (!Pepper::instance()->BindGraphics(*context)) {
      Log::printEnd("Failed to bind Graphics3D");

      delete context;
      context = nullptr;
    }
    else {
      glSetCurrentContextPPAPI(context->pp_resource());

      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      glFlush();

      context->SwapBuffers(pp::CompletionCallback(flushCompleteCallback, nullptr));
      Log::printEnd("OK");
    }
  };
  flushSemaphore.wait();

#else

  // Don't mess with screensaver. In X11 it only makes effect for windowed mode, in fullscreen
  // mode screensaver never starts anyway. Turning off screensaver has a side effect: if the game
  // crashes, it remains turned off. Besides that, in X11 several programs (e.g. IM clients) rely
  // on screensaver's counter, so they don't detect that you are away if the screensaver is screwed.
  SDL_EnableScreenSaver();

  // Force old Mesa drivers to turn on partial S3TC support even when libtxc_dxtn is not present.
  // We don't use online texture compression anywhere so partial S3TC support is enough.
#ifdef __unix__
  SDL_setenv("force_s3tc_enable", "true", true);
#endif

  uint flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE |
               (fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

  Log::print("Creating OpenGL window %dx%d [%s] ... ",
             windowWidth, windowHeight, fullscreen ? "fullscreen" : "windowed");

  MainCall() << [&]
  {
    descriptor = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  windowWidth, windowHeight, flags);
    if (descriptor == nullptr) {
      return;
    }

# ifdef OZ_GL_ES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_ES);
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

    SDL_GL_SetSwapInterval(1);

    context = SDL_GL_CreateContext(descriptor);
  };

  if (descriptor == nullptr) {
    Log::printEnd("Window creation failed");
    return false;
  }

  Log::printEnd("OK");

  MainCall() << []
  {
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glFlush();

    SDL_GL_SwapWindow(descriptor);
  };

#endif

  return true;
}

void Window::destroy()
{
#if defined(__native_client__)

  if (context != nullptr) {
    MainCall() << []
    {
      glSetCurrentContextPPAPI(0);
      delete context;
      context = nullptr;
      glTerminatePPAPI();
    };
  }

#else

  if (descriptor != nullptr) {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(descriptor);
    descriptor = nullptr;
  }

#endif

  if (screenshotThread.isValid()) {
    screenshotThread.join();
  }
}

}
