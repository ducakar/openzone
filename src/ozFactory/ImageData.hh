/*
 * ozFactory - OpenZone Assets Builder Library.
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

/**
 * @file ozFactory/ImageData.hh
 *
 * `ImageData` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * RGBA image pixel data with basic metadata (dimensions and transparency).
 */
class ImageData
{
private:

  /// Alpha flag.
  static constexpr int ALPHA_BIT = 0x01;

private:

  int   width_  = 0;       ///< Width.
  int   height_ = 0;       ///< Height.
  int   flags_  = 0;       ///< Flags.
  char* pixels_ = nullptr; ///< Pixels data in RGBA format.

public:

  /**
   * Create empty instance, no allocation is performed.
   */
  ImageData() = default;

  /**
   * Create an image an allocate memory for pixel data.
   */
  explicit ImageData(int width, int height, bool hasAlpha);

  /**
   * Destructor.
   */
  ~ImageData();

  OZ_NO_COPY(ImageData)
  OZ_GENERIC_MOVE(ImageData)

  /**
   * Width.
   */
  OZ_ALWAYS_INLINE
  int width() const noexcept
  {
    return width_;
  }

  /**
   * Height.
   */
  OZ_ALWAYS_INLINE
  int height() const noexcept
  {
    return height_;
  }

  /**
   * True iff it has transparency.
   */
  OZ_ALWAYS_INLINE
  bool hasAlpha() const noexcept
  {
    return flags_ & ALPHA_BIT;
  }

  /**
   * Set transparency.
   */
  OZ_ALWAYS_INLINE
  void setAlpha(bool value)
  {
    if (value) {
      flags_ |= ALPHA_BIT;
    }
    else {
      flags_ &= ~ALPHA_BIT;
    }
  }

  /**
   * Constant pixel data.
   */
  OZ_ALWAYS_INLINE
  const char* pixels() const noexcept
  {
    return pixels_;
  }

  /**
   * Non-constant pixel data.
   */
  OZ_ALWAYS_INLINE
  char* pixels() noexcept
  {
    return pixels_;
  }

  /**
   * True iff it holds no image data.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const noexcept
  {
    return pixels_ == nullptr;
  }

  /**
   * Pixels size in bytes.
   */
  OZ_ALWAYS_INLINE
  int size() const noexcept
  {
    return width_ * height_ * 4;
  }

  /**
   * Constant reference to the `i`-th byte in pixels array.
   */
  OZ_ALWAYS_INLINE
  const char& operator[](int i) const
  {
    OZ_ASSERT(uint(i) < uint(size()));

    return pixels_[i];
  }

  /**
   * Non-constant reference to the `i`-th byte in pixels array.
   */
  OZ_ALWAYS_INLINE
  char& operator[](int i)
  {
    OZ_ASSERT(uint(i) < uint(size()));

    return pixels_[i];
  }

};

}
