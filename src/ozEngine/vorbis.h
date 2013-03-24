/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozEngine/vorbis.h
 *
 * Internal header with common Ogg Vorbis utilities used by `ALBuffer` and `ALStreamingBuffer`.
 */

#pragma once

#include "common.hh"

// We don't use those callbacks anywhere and they don't compile on MinGW.
#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

namespace oz
{

// Ogg Vorbis callbacks for reading from InputStream.
extern ov_callbacks VORBIS_CALLBACKS;

// Decode next chunk of Ogg Vorbis stream and write PCM data to the given buffer.
bool decodeVorbis( OggVorbis_File* stream, char* buffer, int size );

}
