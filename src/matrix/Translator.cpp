/*
 *  Translator.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Translator.h"

#include "ObjectClass.h"
#include "DynObjectClass.h"
#include "BotClass.h"
#include "VehicleClass.h"

#include <dirent.h>
#include <sys/types.h>

#define OZ_REGISTER_BASECLASS( name ) \
  baseClasses.add( #name, &name##Class::init )

namespace oz
{

  Translator translator;

  bool Translator::init()
  {
    OZ_REGISTER_BASECLASS( Object );
    OZ_REGISTER_BASECLASS( DynObject );
    OZ_REGISTER_BASECLASS( Bot );
    OZ_REGISTER_BASECLASS( Vehicle );

    logFile.println( "Translator mapping resources {" );
    logFile.indent();

    Config classConfig;
    DIR *dir;
    struct dirent *file;

    logFile.println( "textures (*.png, *.jpg in 'textures/oz') {" );
    logFile.indent();

    dir = opendir( "textures/oz" );
    if( dir == null ) {
      free();

      logFile.println( "Cannot open directory 'textures/oz'" );
      logFile.unindent();
      logFile.println( "}" );
      return false;
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( extension != ".png" && extension != ".jpg" ) {
        continue;
      }

      String fileName = "textures/oz/" + name;
      String baseName = name.substring( 0, dot );
      if( textureIndices.contains( baseName ) ) {
        logFile.println( "duplicated texture: %s", baseName.cstr() );
        return false;
      }
      textureIndices.add( baseName, textures.length() );
      textures << Resource( baseName, fileName );

      logFile.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    logFile.unindent();
    logFile.println( "}" );
    logFile.println( "sounds (*.au, *.wav, *.oga in 'snd') {" );
    logFile.indent();

    dir = opendir( "snd" );
    if( dir == null ) {
      free();

      logFile.println( "Cannot open directory 'snd'" );
      logFile.unindent();
      logFile.println( "}" );
      return false;
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( extension != ".au" &&
          extension != ".wav" &&
          extension != ".oga" &&
          extension != ".ogg" )
      {
        continue;
      }

      String fileName = "snd/" + name;
      String baseName = name.substring( 0, dot );
      if( soundIndices.contains( baseName ) ) {
        logFile.println( "duplicated sound: %s", baseName.cstr() );
        continue;
      }
      soundIndices.add( baseName, sounds.length() );
      sounds << Resource( baseName, fileName );;

      logFile.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    logFile.unindent();
    logFile.println( "}" );
    logFile.println( "BSP structures (*.xml in 'bsp') {" );
    logFile.indent();

    dir = opendir( "bsp" );
    if( dir == null ) {
      free();

      logFile.println( "Cannot open directory 'bsp'" );
      logFile.unindent();
      logFile.println( "}" );
      return false;
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( extension != ".xml" ) {
        continue;
      }

      String fileName = "bsp/" + name;
      String baseName = name.substring( 0, dot );
      if( bspIndices.contains( baseName ) ) {
        logFile.println( "duplicated bsp: %s", baseName.cstr() );
        return false;
      }
      bspIndices.add( baseName, bsps.length() );
      bsps << Resource( baseName, "" );;

      logFile.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    logFile.unindent();
    logFile.println( "}" );
    logFile.println( "object classes (*.xml in 'class') {" );
    logFile.indent();

    dir = opendir( "class" );
    if( dir == null ) {
      free();

      logFile.println( "Cannot open directory 'class'" );
      logFile.unindent();
      logFile.println( "}" );
      return false;
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( extension != ".xml" ) {
        continue;
      }

      String fileName = "class/" + name;
      String baseName = name.substring( 0, dot );

      if( !classConfig.load( fileName ) ) {
        logFile.println( "invalid config file %s", fileName.cstr() );
        classConfig.clear();
        continue;
      }
      if( !classConfig.contains( "base" ) ) {
        logFile.println( "missing base variable" );
        classConfig.clear();
        continue;
      }
      if( !baseClasses.contains( classConfig["base"] ) ) {
        logFile.println( "invalid base %s", classConfig["base"].cstr() );
        classConfig.clear();
        continue;
      }

      if( classes.contains( baseName ) ) {
        logFile.println( "duplicated class: %s", baseName.cstr() );
        return false;
      }
      classConfig.add( "name", baseName );
      classes.add( baseName, baseClasses.cachedValue()( baseName, &classConfig ) );
      classConfig.clear();

      logFile.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    logFile.unindent();
    logFile.println( "}" );
    logFile.unindent();
    logFile.println( "}" );

    return true;
  }

  int Translator::textureIndex( const char *file )
  {
    if( textureIndices.contains( file ) ) {
      return textureIndices.cachedValue();
    }
    else {
      logFile.println( "W: invalid texture file index requested: %s", file );
      return -1;
    }
  }

  int Translator::soundIndex( const char *file )
  {
    if( soundIndices.contains( file ) ) {
      return soundIndices.cachedValue();
    }
    else {
      logFile.println( "W: invalid sound file index requested: %s", file );
      return -1;
    }
  }

  int Translator::bspIndex( const char *file )
  {
    if( bspIndices.contains( file ) ) {
      return bspIndices.cachedValue();
    }
    else {
      logFile.println( "W: invalid bsp file index requested: %s", file );
      return -1;
    }
  }

  void Translator::free()
  {
    textureIndices.clear();
    soundIndices.clear();
    bspIndices.clear();

    baseClasses.clear();
    classes.free();
  }

}
