/*
 *  Context.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "build/common.hpp"

namespace oz
{
namespace build
{

class Context
{
  public:

    static const int DEFAULT_MAG_FILTER;
    static const int DEFAULT_MIN_FILTER;

  private:

    // texture reading buffer
    static const int BUFFER_SIZE          = 256 * 1024;
    // default audio format
    static const int DEFAULT_AUDIO_FREQ   = 44100;
    static const int DEFAULT_AUDIO_FORMAT = AUDIO_S16LSB;

  public:

    static bool useS3TC;

  private:

    static uint buildTexture( const void* data, int width, int height, uint format,
                              bool wrap, int magFilter, int minFilter );

  public:

    static uint createTexture( const void* data, int width, int height, uint format,
                               bool wrap = true, int magFilter = DEFAULT_MAG_FILTER,
                               int minFilter = DEFAULT_MIN_FILTER );

    static uint loadRawTexture( const char* path, bool wrap = true,
                                int magFilter = DEFAULT_MAG_FILTER,
                                int minFilter = DEFAULT_MIN_FILTER );

    static void writeTexture( uint id, OutputStream* stream );

};

}
}
