/*
 *  Context.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Context.h"

#include "matrix/Translator.h"

#define OZ_REGISTER_MODELCLASS( name ) \
  modelClasses.add( #name, &name##Class::create )

namespace oz
{
namespace client
{

  Context context;

  Context::Context() : textures( null ), sounds( null )
  {}

  uint Context::buildTexture( const ubyte *data, int width, int height, int bytesPerPixel,
                              bool wrap, int magFilter, int minFilter )
  {
//     assert( glGetError() == GL_NO_ERROR );

    GLenum format = bytesPerPixel == 4 ? GL_RGBA : GL_RGB;

    uint texNum;
    glGenTextures( 1, &texNum );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glBindTexture( GL_TEXTURE_2D, texNum );

    if( minFilter >= GL_NEAREST_MIPMAP_NEAREST ) {
      gluBuild2DMipmaps( GL_TEXTURE_2D, bytesPerPixel, width, height, format,
                         GL_UNSIGNED_BYTE, data );
    }
    else {
      glTexImage2D( GL_TEXTURE_2D, 0, bytesPerPixel, width, height, 0, format,
                    GL_UNSIGNED_BYTE, data );
    }

    if( !wrap ) {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    }

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

//     if( glGetError() != GL_NO_ERROR ) {
//       glDeleteTextures( 1, &texNum );
//       texNum = ~0;
//
//       do {
//       }
//       while( glGetError() != GL_NO_ERROR );
//     }
    return texNum;
  }

  uint Context::buildNormalmap( ubyte *data, const Vec3 &lightNormal, int width, int height,
                                int bytesPerPixel, bool wrap, int magFilter, int minFilter )
  {
    assert( glGetError() == GL_NO_ERROR );

    ubyte *dataEnd = data + width * height * bytesPerPixel;

    for( ubyte *p = data; p < dataEnd; p += bytesPerPixel ) {
      float x = ( (float) p[0] - 128.0f ) / 128.0f;
      float y = ( (float) p[1] - 128.0f ) / 128.0f;
      float z = ( (float) p[2] - 128.0f ) / 128.0f;

      float dot = x * lightNormal.x + y * lightNormal.y + z * lightNormal.z;
      ubyte color = (ubyte) bound( dot * 256.0f, 0.0f, 255.0f );

      p[0] = color;
      p[1] = color;
      p[2] = color;
    }

    GLenum format = bytesPerPixel == 4 ? GL_RGBA : GL_RGB;

    uint texNum;
    glGenTextures( 1, &texNum );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glBindTexture( GL_TEXTURE_2D, texNum );

    if( minFilter >= GL_NEAREST_MIPMAP_NEAREST ) {
      gluBuild2DMipmaps( GL_TEXTURE_2D, bytesPerPixel, width, height, format,
                         GL_UNSIGNED_BYTE, data );
    }
    else {
      glTexImage2D( GL_TEXTURE_2D, 0, bytesPerPixel, width, height, 0, format,
                    GL_UNSIGNED_BYTE, data );
    }

    if( !wrap ) {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    }

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    if( glGetError() != GL_NO_ERROR ) {
      glDeleteTextures( 1, &texNum );
      texNum = ~0;

      do {
      }
      while( glGetError() != GL_NO_ERROR );
    }
    return texNum;
  }

  void Context::init()
  {
    assert( textures == null && sounds == null );

    logFile.println( "Context created" );

    textures = new Texture[translator.textures.length()];
    sounds = new Sound[translator.sounds.length()];

    for( int i = 0; i < translator.textures.length(); i++ ) {
      textures[i].nUsers = -1;
    }
    for( int i = 0; i < translator.sounds.length(); i++ ) {
      sounds[i].nUsers = -1;
    }
  }

  void Context::free()
  {
    assert( textures != null && sounds != null );

    delete[] textures;
    delete[] sounds;
    lists.clear();

    textures = null;
    sounds = null;
  }

  uint Context::createTexture( const ubyte *data, int width, int height, int bytesPerPixel,
                               bool wrap, int magFilter, int minFilter )
  {
    logFile.print( "Creating texture from buffer ..." );

    int texNum = buildTexture( data, width, height, bytesPerPixel, wrap, magFilter, minFilter );

    if( texNum == ~0 ) {
      logFile.printRaw( " Error\n" );
    }
    else {
      logFile.printRaw( " OK\n" );
    }

    return texNum;
  }

  uint Context::createNormalmap( ubyte *data, const Vec3 &lightNormal, int width, int height,
                                 int bytesPerPixel, bool wrap, int magFilter, int minFilter )
  {
    logFile.print( "Creating normalmap texture from buffer ..." );

    int texNum = buildNormalmap( data, lightNormal, width, height, bytesPerPixel, wrap,
                                 magFilter, minFilter );

    if( texNum == ~0 ) {
      logFile.printRaw( " Error\n" );
    }
    else {
      logFile.printRaw( " OK\n" );
    }

    return texNum;
  }

  uint Context::requestTexture( int resource, bool wrap, int magFilter, int minFilter )
  {
    if( textures[resource].nUsers >= 0 ) {
      textures[resource].nUsers++;
      return textures[resource].id;
    }

    String fileName = translator.textures[resource];

    logFile.print( "Loading registered texture '%s' ...", fileName.cstr() );

    SDL_Surface *image = IMG_Load( fileName.cstr() );
    if( image == null ) {
      logFile.printRaw( " No such file\n" );
      return 0;
    }
    logFile.printRaw( " OK\n" );

    assert( image->w == image->h );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createTexture( (const ubyte*) image->pixels, image->w, image->h,
                                bytesPerPixel, wrap, magFilter, minFilter );

    SDL_FreeSurface( image );

    textures[resource].id = texNum;
    textures[resource].nUsers = 1;
    return texNum;
  }

  uint Context::requestNormalmap( int resource, const Vec3 &lightNormal,
                                  bool wrap, int magFilter, int minFilter )
  {
    if( textures[resource].nUsers >= 0 ) {
      textures[resource].nUsers++;
      return textures[resource].id;
    }

    String fileName = translator.textures[resource];

    logFile.print( "Loading registerded normalmap texture '%s' ...", fileName.cstr() );

    SDL_Surface *image = IMG_Load( fileName.cstr() );
    if( image == null ) {
      logFile.printRaw( " No such file\n" );
      return 0;
    }
    logFile.printRaw( " OK\n" );

    assert( image->w == image->h );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createNormalmap( (ubyte*) image->pixels, lightNormal, image->w, image->h,
                                  bytesPerPixel, wrap, magFilter, minFilter );

    SDL_FreeSurface( image );

    textures[resource].id = texNum;
    textures[resource].nUsers = 1;
    return texNum;
  }

  void Context::releaseTexture( int resource )
  {
    assert( textures[resource].nUsers > 0 );

    textures[resource].nUsers--;

    if( textures[resource].nUsers == 0 ) {
      glDeleteTextures( 1, &textures[resource].id );
      textures[resource].nUsers = -1;
    }
  }

  uint Context::loadTexture( const char *fileName, bool wrap, int magFilter, int minFilter )
  {
    logFile.print( "Loading texture from file '%s' ...", fileName );

    SDL_Surface *image = IMG_Load( fileName );
    if( image == null ) {
      logFile.printRaw( " No such file\n" );
      return 0;
    }
    logFile.printRaw( " OK\n" );

//     assert( image->w == image->h );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createTexture( (const ubyte*) image->pixels, image->w, image->h,
                                bytesPerPixel, wrap, magFilter, minFilter );

    SDL_FreeSurface( image );

    return texNum;
  }

  uint Context::loadNormalmap( const char *fileName, const Vec3 &lightNormal,
                               bool wrap, int magFilter, int minFilter )
  {
    logFile.print( "Loading normalmap texture from file '%s' ...", fileName );

    SDL_Surface *image = IMG_Load( fileName );
    if( image == null ) {
      logFile.printRaw( " No such file\n" );
      return 0;
    }
    logFile.printRaw( " OK\n" );

    assert( image->w == image->h );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createNormalmap( (ubyte*) image->pixels, lightNormal, image->w, image->h,
                                  bytesPerPixel, wrap, magFilter, minFilter );

    SDL_FreeSurface( image );

    return texNum;
  }

  void Context::freeTexture( uint id )
  {
    glDeleteTextures( 1, &id );
  }

  uint Context::genList()
  {
    int index = lists.add();
    lists[index].base = glGenLists( 1 );
    lists[index].count = 1;
    return lists[index].base;
  }

  uint Context::genLists( int count )
  {
    int index = lists.add();
    lists[index].base = glGenLists( count );
    lists[index].count = count;
    return lists[index].base;
  }

  void Context::freeLists( uint listId )
  {
    for( int i = 0; i < lists.length(); i++ ) {
      if( lists[i].base == listId ) {
        glDeleteLists( lists[i].base, lists[i].count );
      }
    }
  }

}
}
