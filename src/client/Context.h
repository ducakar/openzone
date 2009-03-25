/*
 *  Context.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Translator.h"

#include "MD2.h"
#include "MD3.h"
#include "OBJ.h"

namespace oz
{
namespace client
{

  class Context
  {
    private:

      struct Lists
      {
        uint base;
        int count;
        // TODO union
        int nextSlot;
      };

      template <class Type>
      struct Resource
      {
        Type id;
        int  nUsers;
        int  nextSlot;
      };

      Resource<uint> *textures;
      Resource<uint> *sounds;
      Sparse<Lists>  lists;

      HashString< Resource<MD2*>, 253 > md2Models;
      HashString< Resource<uint>, 253 > md2StaticModels;
      HashString< Resource<MD3*>, 253 > md3Models;
      HashString< Resource<uint>, 253 > md3StaticModels;
      HashString< Resource<uint>, 253 > objModels;

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

      uint requestTexture( int resource,
                           bool wrap = true,
                           int magFilter = GL_LINEAR_MIPMAP_NEAREST,
                           int minFilter = GL_LINEAR_MIPMAP_NEAREST );

      uint requestNormalmap( int resource,
                             const Vec3 &lightNormal,
                             bool wrap = true,
                             int magFilter = GL_LINEAR_MIPMAP_NEAREST,
                             int minFilter = GL_LINEAR_MIPMAP_NEAREST );

      void releaseTexture( int resource );

      uint loadTexture( const char *file,
                        bool wrap = true,
                        int magFilter = GL_LINEAR_MIPMAP_NEAREST,
                        int minFilter = GL_LINEAR_MIPMAP_NEAREST );

      uint loadNormalmap( const char *file,
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

      MD2  *loadMD2Model( const char *path );
      uint loadMD2StaticModel( const char *path );
      MD3  *loadMD3Model( const char *path );
      uint loadMD3StaticModel( const char *path );
      uint loadOBJModel( const char *path );

  };

  extern Context context;

}
}
