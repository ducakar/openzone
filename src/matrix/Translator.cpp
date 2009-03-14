/*
 *  Translator.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "Translator.hpp"

#include <dirent.h>
#include <sys/types.h>

namespace oz
{

  Translator translator;

  bool Translator::init()
  {
    logFile.println( "Translator mapping resources {" );
    logFile.indent();

    DIR *dir;
    struct dirent *file;

    dir = opendir( "tex" );
    if( dir == null ) {
      free();

      logFile.println( "Cannot open tex/" );
      logFile.unindent();
      logFile.println( "}" );
      return false;
    }
    while( ( file = readdir( dir ) ) != null ) {
      textureIndices.add( file->d_name, textures.length() );
      textures << file->d_name;
    }
    closedir( dir );

    dir = opendir( "snd" );
    if( dir == null ) {
      free();

      logFile.println( "Cannot open snd/" );
      logFile.unindent();
      logFile.println( "}" );
      return false;
    }
    while( ( file = readdir( dir ) ) != null ) {
      soundIndices.add( file->d_name, sounds.length() );
      sounds << file->d_name;
    }
    closedir( dir );

    dir = opendir( "mdl" );
    if( dir == null ) {
      free();
      logFile.println( "Cannot open mdl/" );
      logFile.unindent();
      logFile.println( "}" );
      return false;
    }
    while( ( file = readdir( dir ) ) != null ) {
      modelIndices.add( file->d_name, models.length() );
      models << file->d_name;
    }
    closedir( dir );

    logFile.unindent();
    logFile.println( "}" );

    return true;
  }

  int Translator::getTexture( const char *name )
  {
    if( textureIndices.contains( name ) ) {
      return textureIndices.cachedValue();
    }
    else {
      assert( false );

      return -1;
    }
  }

  int Translator::getSound( const char *name )
  {
    if( soundIndices.contains( name ) ) {
      return soundIndices.cachedValue();
    }
    else {
      assert( false );

      return -1;
    }
  }

  int Translator::getModel( const char *name )
  {
    if( modelIndices.contains( name ) ) {
      return modelIndices.cachedValue();
    }
    else {
      assert( false );

      return -1;
    }
  }

  void Translator::free()
  {
    textureIndices.clear();
    soundIndices.clear();
    modelIndices.clear();
  }

}
