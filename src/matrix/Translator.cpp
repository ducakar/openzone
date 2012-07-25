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

    dir = opendir( "textures/oz" );
    if( dir == null ) {
      free();

      logFile.println( "Cannot open textures/oz" );
      logFile.unindent();
      logFile.println( "}" );
      return false;
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      String fileName = "textures/oz/" + name;

      if( name.length() <= 4  ) {
        continue;
      }

      String extension = name.substring( name.length() - 4 );
      if( extension != ".jpg" && extension != ".png" ) {
        continue;
      }

      textureIndices.add( fileName, textures.length() );
      textures << fileName;

      logFile.println( "texture: %s", fileName.cstr() );
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
      String name = file->d_name;
      String fileName = "snd/" + name;

      if( name.length() <= 4  ) {
        continue;
      }

      String extension = name.substring( name.length() - 4 );
      if( extension != ".wav" && extension != ".ogg" ) {
        continue;
      }

      soundIndices.add( fileName, sounds.length() );
      sounds << fileName;

      logFile.println( "sound: %s", fileName.cstr() );
    }
    closedir( dir );

    dir = opendir( "class" );
    if( dir == null ) {
      free();

      logFile.println( "Cannot open class/" );
      logFile.unindent();
      logFile.println( "}" );
      return false;
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      String fileName = "class/" + name;

      if( name.length() <= 4 ) {
        continue;
      }

      String extension = name.substring( name.length() - 4 );
      if( extension != ".xml" ) {
        continue;
      }

      name = name.substring( 0, name.length() - 4 );

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

      classConfig.add( "name", name );
      classes.add( name, baseClasses.cachedValue()( name, &classConfig ) );
      classConfig.clear();
    }
    closedir( dir );

    logFile.unindent();
    logFile.println( "}" );

    return true;
  }

  int Translator::textureIndex( const char *file )
  {
    return textureIndices.contains( file ) ?
        textureIndices.cachedValue() :
        ( assert( false ), -1 );
  }

  int Translator::soundIndex( const char *file )
  {
    return soundIndices.contains( file ) ?
        soundIndices.cachedValue() :
        ( assert( false ), -1 );
  }

  void Translator::free()
  {
    textureIndices.clear();
    soundIndices.clear();

    baseClasses.clear();
    classes.free();
  }

}
