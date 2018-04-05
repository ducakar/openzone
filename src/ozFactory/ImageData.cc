/*
 * ozFactory - OpenZone Assets Builder Library.
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

#include "ImageData.hh"

namespace oz
{

ImageData::ImageData(int width, int height, bool hasAlpha)
  : width_(width), height_(height), flags_(hasAlpha ? ALPHA_BIT : 0),
    pixels_(new char[width * height * 4])
{}

ImageData::~ImageData()
{
  delete[] pixels_;
}

ImageData::ImageData(ImageData&& other) noexcept
  : width_(other.width_), height_(other.height_), flags_(other.flags_), pixels_(other.pixels_)
{
  OZ_MOVE_CTOR_BODY(ImageData);
}

ImageData& ImageData::operator=(ImageData&& other) noexcept
{
  OZ_MOVE_OP_BODY(ImageData);
}

}
