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
#include "Model.h"
#include "Audio.h"

namespace oz
{
namespace client
{

  class Context
  {
    private:

      static const int DEFAULT_MAG_FILTER = GL_LINEAR;
      static const int DEFAULT_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR;

      struct Lists
      {
        uint base;

        // we can use union here, those two members are never in use at the same time
        union
        {
          int count;
          int nextSlot;
        };
      };

    public:

      template <class Type>
      struct Resource
      {
        Type id;
        int  nUsers;
      };

      template <class Type>
      struct Resource<Type*>
      {
        Type *object;
        int  nUsers;

        ~Resource()
        {
          delete object;
        }
      };

      Resource<uint> *textures;
      Resource<uint> *sounds;

    private:

      Sparse<Lists>  lists;

      HashString< Resource<MD2*>, 253 > md2Models;
      HashString< Resource<uint>, 253 > md2StaticModels;
      HashString< Resource<MD3*>, 253 > md3Models;
      HashString< Resource<uint>, 253 > md3StaticModels;
      HashString< Resource<uint>, 253 > objModels;

      HashString<Model::InitFunc, 253> modelClasses;
      HashString<Audio::InitFunc, 253> audioClasses;

      static uint buildTexture( const ubyte *data, int width, int height, int bytesPerPixel,
                                bool wrap, int magFilter, int minFilter );
      static uint buildNormalmap( ubyte *data, const Vec3 &lightNormal, int width,int height,
                                  int bytesPerPixel, bool wrap, int magFilter, int minFilter );

    public:

      uint createTexture( const ubyte *data,
                          int width,
                          int height,
                          int bytesPerPixel,
                          bool wrap = true,
                          int magFilter = DEFAULT_MAG_FILTER,
                          int minFilter = DEFAULT_MIN_FILTER );

      uint createNormalmap( ubyte *data,
                            const Vec3 &lightNormal,
                            int width,
                            int height,
                            int bytesPerPixel,
                            bool wrap = true,
                            int magFilter = DEFAULT_MAG_FILTER,
                            int minFilter = DEFAULT_MIN_FILTER );

      uint requestTexture( int resource,
                           bool wrap = true,
                           int magFilter = DEFAULT_MAG_FILTER,
                           int minFilter = DEFAULT_MIN_FILTER );

      uint requestNormalmap( int resource,
                             const Vec3 &lightNormal,
                             bool wrap = true,
                             int magFilter = DEFAULT_MAG_FILTER,
                             int minFilter = DEFAULT_MIN_FILTER );

      void releaseTexture( int resource );

      uint loadTexture( const char *path,
                        bool wrap = true,
                        int magFilter = DEFAULT_MAG_FILTER,
                        int minFilter = DEFAULT_MIN_FILTER );

      uint loadNormalmap( const char *path,
                          const Vec3 &lightNormal,
                          bool wrap = true,
                          int magFilter = DEFAULT_MAG_FILTER,
                          int minFilter = DEFAULT_MIN_FILTER );

      void freeTexture( uint texId );

      uint requestSound( int resource );
      void releaseSound( int resource );

      uint genList();
      uint genLists( int count );
      void freeLists( uint listId );

      MD2  *loadMD2Model( const char *name );
      uint loadMD2StaticModel( const char *name );
      MD3  *loadMD3Model( const char *name );
      uint loadMD3StaticModel( const char *name );
      uint loadOBJModel( const char *name );

      Model *createModel( const Object *obj )
      {
        if( !modelClasses.contains( obj->type->modelType ) ) {
          obj->type->flags &= Object::MODEL_BIT;
          return null;
        }
        return modelClasses.cachedValue()( obj );
      }

      Audio *createAudio( const Object *obj )
      {
        if( !audioClasses.contains( obj->type->audioType ) ) {
          obj->type->flags &= Object::AUDIO_BIT;
          return null;
        }
        return audioClasses.cachedValue()( obj );
      }

      Context();

      void init();
      void free();

  };

  extern Context context;

}
}
