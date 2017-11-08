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

#include "Cursor.hh"

#include "GL.hh"

#include <SDL2/SDL.h>

#ifdef __native_client__
# include <ppapi/cpp/instance.h>
# include <ppapi/cpp/mouse_cursor.h>
# include <ppapi_simple/ps.h>
#endif

namespace oz
{

const Cursor::Image* Cursor::lastImage = nullptr;

Cursor::Cursor(const File& file, Mode mode, int size)
  : mode_(mode)
{
  // Implementation is based on specifications from xcursor(3) manual.
  Stream is(0, Endian::LITTLE);

  if (!file.read(&is) || is.available() < 12 || !String::beginsWith(is.begin(), "Xcur")) {
    return;
  }

  is.seek(12);

  int nEntries = is.readInt();
  images_.resize(nEntries);

  for (Image& image : images_) {
    uint type     = is.readUInt();
    int  subtype  = is.readInt();
    int  position = is.readInt();

    if (type != 0xfffd0002) {
      continue;
    }
    else if (size == -1) {
      size = subtype;
    }
    else if (subtype != size) {
      continue;
    }

    int tablePos = is.tell();
    is.seek(position);

    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();

    image.width       = is.readInt();
    image.height      = is.readInt();
    image.hotspotLeft = is.readInt();
    image.hotspotTop  = is.readInt();
    image.delay       = is.readInt() * 1_ms;

    int   nBytes = image.width * image.height * 4;
    char* pixels = new char[nBytes];

    is.read(pixels, nBytes);

    if (mode_ == SYSTEM) {
#ifdef __native_client__

      pp::ImageData imageData(pp::InstanceHandle(PSGetInstanceId()), PP_IMAGEDATAFORMAT_BGRA_PREMUL,
                              pp::Size(image.width, image.height), false);

      for (int i = 0; i < image.height; ++i) {
      }

#else

      SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixels, image.width, image.height, 32,
                                                      image.width * 4, 0x00ff0000, 0x0000ff00,
                                                      0x000000ff, 0xff000000);

      image.sdlCursor = SDL_CreateColorCursor(surface, image.hotspotLeft, image.hotspotTop);

      SDL_FreeSurface(surface);

#endif
    }
    else {
#ifdef OZ_GL_ES
      GLenum srcFormat = GL_RGBA;

      // BGRA -> RGBA
      for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
          int baseIndex = y * image.width + y;

          swap(pixels[baseIndex + 0], pixels[baseIndex + 2]);
        }
      }
#else
      GLenum srcFormat = GL_BGRA;
#endif

      MainCall() << [&]
      {
        glGenTextures(1, &image.textureId);
        glBindTexture(GL_TEXTURE_2D, image.textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, srcFormat,
                     GL_UNSIGNED_BYTE, pixels);
      };
    }

    delete[] pixels;

    is.seek(tablePos);
  }
}

Cursor::~Cursor()
{
  destroy();
}

Cursor::Cursor(Cursor&& other) noexcept
  : mode_(other.mode_), frame_(other.frame_), frameDuration_(other.frameDuration_),
    images_(static_cast<List<Image>&&>(other.images_))
{
  other.mode_          = SYSTEM;
  other.frame_         = 0;
  other.lastFrame_     = -1;
  other.frameDuration_ = Duration::ZERO;
}

Cursor& Cursor::operator=(Cursor&& other) noexcept
{
  if (&other != this) {
    destroy();

    mode_          = other.mode_;
    frame_         = other.frame_;
    lastFrame_     = other.lastFrame_;
    frameDuration_ = other.frameDuration_;
    images_        = static_cast<List<Image>&&>(other.images_);

    other.mode_          = SYSTEM;
    other.frame_         = 0;
    other.lastFrame_     = -1;
    other.frameDuration_ = Duration::ZERO;
  }
  return *this;
}

void Cursor::reset()
{
  frame_         = 0;
  frameDuration_ = Duration::ZERO;
}

void Cursor::update(Duration duration)
{
  if (images_.isEmpty()) {
    return;
  }

  Duration delay = images_[frame_].delay;

  frameDuration_ += duration;
  frame_          = (frame_ + frameDuration_ / delay) % images_.size();
  frameDuration_  = frameDuration_ % delay;

  if (mode_ == SYSTEM) {
    const Image& image = images_[frame_];

    if (&image != lastImage) {
      lastImage = &image;

#ifdef __native_client__
      pp::MouseCursor().SetCursor(pp::InstanceHandle(PSGetInstanceId()), PP_MOUSECURSOR_TYPE_CUSTOM,
                                  *image.imageData, pp::Point(image.hotspotLeft, image.hotspotTop));
#else
      SDL_SetCursor(image.sdlCursor);
#endif
    }
  }
}

void Cursor::destroy()
{
  if (images_.isEmpty()) {
    return;
  }

  if (mode_ == SYSTEM) {
    for (const Image& image : images_) {
#ifdef __native_client__
      delete image.imageData;
#else
      SDL_FreeCursor(image.sdlCursor);
#endif
    }
  }
  else {
    MainCall() << [&]
    {
      for (const Image& image : images_) {
        glDeleteTextures(1, &image.textureId);
      }
    };
  }

  mode_          = SYSTEM;
  frame_         = 0;
  lastFrame_     = -1;
  frameDuration_ = Duration::ZERO;
  images_.clear();
}

}
