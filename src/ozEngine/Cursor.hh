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
 * @file ozEngine/Cursor.hh
 *
 * `Cursor` class.
 */

#pragma once

#include "common.hh"

#ifdef __native_client__
namespace pp { class ImageData; }
#else
struct SDL_Cursor;
#endif

namespace oz
{

/**
 * Mouse cursor loader.
 *
 * There are two modes for rendering cursors. The first, OpenGL textures are generated for each
 * frame. You can retrieve OpenGL texture id for the current frame with `textureId()` call and
 * render it via your UI render path. The second, your OS-provided cursor is replaced (SDL 2 only).
 *
 * Cursor files must be in Xcursor format, which is the standard cursor format on Linux (or more
 * accurately, on all X11 and Wayland systems).
 */
class Cursor
{
public:

  /**
   * Cursor render mode.
   */
  enum Mode
  {
    SYSTEM,
    TEXTURE
  };

private:

  /**
   * Cursor image.
   *
   * For animated cursors there may are multiple images, one for each animation frame.
   */
  struct Image
  {
    int width;                         ///< Image width.
    int height;                        ///< Image height.
    int hotspotLeft;                   ///< Hotspot offset from the left.
    int hotspotTop;                    ///< Hotspot offset from the top.
    int delay;                         ///< Frame time in milliseconds.

    union
    {
      uint           textureId;        ///< GL texture id.
#ifdef __native_client__
      pp::ImageData* imageData;        ///< PPAPI cursor image.
#else
      SDL_Cursor*    sdlCursor;        ///< SDL cursor.
#endif
    };
  };

  Mode             mode      = SYSTEM; ///< Render mode.
  int              frame     = 0;      ///< Current animation frame.
  int              lastFrame = -1;     ///< Last frame for OS cursor to avoid unnecessary updates.
  int              frameTime = 0;      ///< Time in milliseconds of the current animation frame.
  SList<Image, 32> images;             ///< Cursor images.

public:

  /**
   * Create an empty instance.
   */
  Cursor() = default;

  /**
   * Create from file.
   */
  explicit Cursor(const File& file, Mode mode, int size = -1);

  /**
   * Destructor, destroys textures if loaded.
   */
  ~Cursor();

  /**
   * Move constructor.
   */
  Cursor(Cursor&& c);

  /**
   * Move operator.
   */
  Cursor& operator = (Cursor&& c);

  /**
   * True iff loaded.
   */
  bool isValid() const
  {
    return !images.isEmpty();
  }

  /**
   * Hotspot offset from the left.
   */
  int hotspotLeft() const
  {
    return images[frame].hotspotLeft;
  }

  /**
   * Hotspot offset from the top.
   */
  int hotspotTop() const
  {
    return images[frame].hotspotTop;
  }

  /**
   * Cursor image width.
   */
  int width() const
  {
    return images[frame].width;
  }

  /**
   * Cursor image height.
   */
  int height() const
  {
    return images[frame].height;
  }

  /**
   * GL texture id for the current frame.
   */
  uint textureId() const
  {
    return mode == SYSTEM ? 0 : images[frame].textureId;
  }

  /**
   * Reset animation.
   */
  void reset();

  /**
   * Advance animation and update the current texture id or the system cursor.
   */
  void update(int millis);

  /**
   * Destroy textures if loaded.
   */
  void destroy();

};

}
