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
    int      width;                    ///< Image width.
    int      height;                   ///< Image height.
    int      hotspotLeft;              ///< Hotspot offset from the left.
    int      hotspotTop;               ///< Hotspot offset from the top.
    Duration delay;                    ///< Frame duration.

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

  static const Image* lastImage;       ///< Remember the current image to avoid unnecessary updates.

  Mode        mode_          = SYSTEM; ///< Render mode.
  int         frame_         = 0;      ///< Current animation frame.
  int         lastFrame_     = -1;     ///< Last frame for OS cursor to avoid unnecessary updates.
  Duration    frameDuration_;          ///< Time of the current animation frame.
  List<Image> images_;                 ///< Cursor images.

public:

  /**
   * Create an empty instance.
   */
  Cursor() = default;

  /**
   * Create from file.
   */
  explicit Cursor(const File& file, Mode mode_, int size = -1);

  /**
   * Destructor, destroys textures if loaded.
   */
  ~Cursor();

  /**
   * Move constructor.
   */
  Cursor(Cursor&& other);

  /**
   * Move operator.
   */
  Cursor& operator=(Cursor&& other);

  /**
   * True iff loaded.
   */
  bool isValid() const
  {
    return !images_.isEmpty();
  }

  /**
   * Hotspot offset from the left.
   */
  int hotspotLeft() const
  {
    return images_[frame_].hotspotLeft;
  }

  /**
   * Hotspot offset from the top.
   */
  int hotspotTop() const
  {
    return images_[frame_].hotspotTop;
  }

  /**
   * Cursor image width.
   */
  int width() const
  {
    return images_[frame_].width;
  }

  /**
   * Cursor image height.
   */
  int height() const
  {
    return images_[frame_].height;
  }

  /**
   * GL texture id for the current frame.
   */
  uint textureId() const
  {
    return mode_ == SYSTEM ? 0 : images_[frame_].textureId;
  }

  /**
   * Reset animation.
   */
  void reset();

  /**
   * Advance animation and update the current texture id or the system cursor.
   */
  void update(Duration duration);

  /**
   * Destroy textures if loaded.
   */
  void destroy();

};

}
