/*
 *  Context.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Translator.hpp"

namespace oz
{
namespace Client
{

  class Context
  {
    private:

      struct Texture
      {
        uint id;

        union {
          int nUsers;
          int nextSlot;
        };
      };

      struct Sound
      {
        uint buffer;
        int  nUsers;
      };

      struct Lists
      {
        uint base;

        union {
          int count;
          int nextSlot;
        };
      };

      Texture         *textures;
      Sound           *sounds;
      Sparse<Lists>   lists;
      Sparse<Texture> bufferTextures;

      static uint buildTexture( const ubyte *data, int width, int height, int bytesPerPixel,
                                bool wrap, int magFilter, int minFilter );
      static uint buildNormalmap( ubyte *data, const Vec3 &lightNormal, int width,int height,
                                  int bytesPerPixel, bool wrap, int magFilter, int minFilter );

    public:

      Context();

      void init();
      void free();

      uint createTexture( const ubyte *data,
                          int width,
                          int height,
                          int bytesPerPixel,
                          bool wrap = true,
                          int magFilter = GL_LINEAR_MIPMAP_NEAREST,
                          int minFilter = GL_LINEAR_MIPMAP_NEAREST );

      uint createNormalmap( ubyte *data,
                            const Vec3 &lightNormal,
                            int width,
                            int height,
                            int bytesPerPixel,
                            bool wrap = true,
                            int magFilter = GL_LINEAR_MIPMAP_NEAREST,
                            int minFilter = GL_LINEAR_MIPMAP_NEAREST );

      uint loadTexture( int resource,
                        bool wrap = true,
                        int magFilter = GL_LINEAR_MIPMAP_NEAREST,
                        int minFilter = GL_LINEAR_MIPMAP_NEAREST );

      uint loadNormalmap( int resource,
                          const Vec3 &lightNormal,
                          bool wrap = true,
                          int magFilter = GL_LINEAR_MIPMAP_NEAREST,
                          int minFilter = GL_LINEAR_MIPMAP_NEAREST );

      void freeTexture( uint texId );

      uint loadSound( int resource );
      void freeSound( uint SoundId );

      uint genList();
      uint genLists( int count );
      void freeLists( uint listId );

      /*
       * Loading by file name
       */
      uint loadTexture( const char *file,
                        bool wrap = true,
                        int magFilter = GL_LINEAR_MIPMAP_NEAREST,
                        int minFilter = GL_LINEAR_MIPMAP_NEAREST )
      {
        return loadTexture( translator.getTexture( file ), wrap, magFilter, minFilter );
      }

      uint loadNormalmap( const char *file,
                          const Vec3 &lightNormal,
                          bool wrap = true,
                          int magFilter = GL_LINEAR_MIPMAP_NEAREST,
                          int minFilter = GL_LINEAR_MIPMAP_NEAREST )
      {
        return loadNormalmap( translator.getTexture( file ), lightNormal, wrap,
                              magFilter, minFilter );
      }

      uint loadSound( const char *file )
      {
        return loadSound( translator.getSound( file ) );
      }

  };

  extern Context context;

}
}
