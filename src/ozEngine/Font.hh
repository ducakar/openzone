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
 * @file ozEngine/Font.hh
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %Font renderer.
 *
 * It uses SDL_ttf library to generate a texture containing a given text.
 */
class Font
{
private:

  void*  handle     = nullptr; ///< TTF_Font handle.
  int    fontHeight = 0;       ///< %Font height.
  Stream fileBuffer;           ///< Cached font file, must be loaded all the time.

public:

  /**
   * Construct empty instance.
   */
  Font() = default;

  /**
   * Destroy.
   */
  ~Font();

  /**
   * Load font of specified height from a file.
   */
  Font(const File& file, int height);

  /**
   * Move constructor.
   */
  Font(Font&& f);

  /**
   * Move operator.
   */
  Font& operator = (Font&& f);

  /**
   * Height.
   */
  int height() const
  {
    return fontHeight;
  }

  /**
   * Report size of a texture the would contain the given text in one line.
   */
  void sizeOf(const char* s, int* width = nullptr, int* height = nullptr) const;

  /**
   * Generate texture data for the given text and upload it via `glTexImage2D`.
   *
   * `*width` and `*height` are set to the final texture size if not null. `*width` is also an
   * input parameter; if it is > 0, the text is wrapped to `*width` pixels.
   *
   * The resulting texture contains anti-aliased white text on transparent background.
   */
  void upload(const char* s, int* width = nullptr, int* height = nullptr) const;

  /**
   * Unload and release resources.
   */
  void destroy();

};

}
