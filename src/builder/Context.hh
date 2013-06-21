/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file builder/Context.hh
 */

#pragma once

#include <builder/common.hh>
#include <ozEngine/GL.hh>

struct FIBITMAP;

namespace oz
{
namespace builder
{

class Context
{
  private:

    static const int DEFAULT_MAG_FILTER = GL_LINEAR;
    static const int DEFAULT_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR;

    static const char* const IMAGE_EXTENSIONS[];

    struct Image;

  public:

    class Texture
    {
      private:

        struct Level
        {
          ubyte* data;
          int    width;
          int    height;
          int    format;
          int    size;

          explicit Level();
          ~Level();

          Level( const Level& ) = delete;
          Level( Level&& l );

          Level& operator = ( const Level& ) = delete;
          Level& operator = ( Level&& l );
        };

        List<Level> levels;
        int         wrap;
        int         magFilter;
        int         minFilter;

      public:

        explicit Texture() = default;
        explicit Texture( Image* image, bool wrap, int magFilter, int minFilter );

        bool isEmpty() const;

        void write( OutputStream* os );

    };

    HashSet<String> usedTextures;
    HashSet<String> usedSounds;
    HashSet<String> usedModels;

    bool bumpmap;
    bool useS3TC;

  private:

    Image loadImage( const char* path, int forceFormat = 0 );

  public:

    Texture loadTexture( const char* path, bool wrap = true,
                         int magFilter = DEFAULT_MAG_FILTER, int minFilter = DEFAULT_MIN_FILTER );

    void loadTextures( Texture* diffuseTex, Texture* masksTex, Texture* normalsTex,
                       const char* basePath, bool wrap = true,
                       int magFilter = DEFAULT_MAG_FILTER, int minFilter = DEFAULT_MIN_FILTER );

    void init();
    void destroy();

};

extern Context context;

}
}
