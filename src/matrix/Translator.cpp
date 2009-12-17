/*
 *  Translator.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Translator.h"

#include "WeaponClass.h"
#include "BotClass.h"
#include "VehicleClass.h"

#include <dirent.h>
#include <sys/types.h>

#define OZ_REGISTER_BASECLASS( name ) \
  baseClasses.add( #name, &name##Class::init )

namespace oz
{

  Translator translator;

  void Translator::init()
  {
    OZ_REGISTER_BASECLASS( Object );
    OZ_REGISTER_BASECLASS( Dynamic );
    OZ_REGISTER_BASECLASS( Weapon );
    OZ_REGISTER_BASECLASS( Bot );
    OZ_REGISTER_BASECLASS( Vehicle );

    log.println( "Translator mapping resources {" );
    log.indent();

    Config classConfig;
    DIR *dir;
    struct dirent *file;

    log.println( "textures (*.png, *.jpg in 'textures/oz') {" );
    log.indent();

    dir = opendir( "textures/oz" );
    if( dir == null ) {
      free();

      log.println( "Cannot open directory 'textures/oz'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialization failure" );
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( !extension.equals( ".png" ) && !extension.equals( ".jpg" ) ) {
        continue;
      }

      String fileName = "textures/oz/" + name;
      String baseName = name.substring( 0, dot );
      if( textureIndices.contains( baseName ) ) {
        log.println( "duplicated texture: %s", baseName.cstr() );
        throw Exception( "Translator initialization failure" );
      }
      textureIndices.add( baseName, textures.length() );
      textures << Resource( baseName, fileName );

      log.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    log.unindent();
    log.println( "}" );
    log.println( "sounds (*.au, *.wav, *.oga in 'snd') {" );
    log.indent();

    dir = opendir( "snd" );
    if( dir == null ) {
      free();

      log.println( "Cannot open directory 'snd'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialization failure" );
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( !extension.equals( ".au" ) &&
          !extension.equals( ".wav" ) &&
          !extension.equals( ".oga" ) &&
          !extension.equals( ".ogg" ) )
      {
        continue;
      }

      String fileName = "snd/" + name;
      String baseName = name.substring( 0, dot );
      if( soundIndices.contains( baseName ) ) {
        log.println( "duplicated sound: %s", baseName.cstr() );
        continue;
      }
      soundIndices.add( baseName, sounds.length() );
      sounds << Resource( baseName, fileName );

      log.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    log.unindent();
    log.println( "}" );
    log.println( "BSP structures (*.rc in 'bsp') {" );
    log.indent();

    dir = opendir( "maps" );
    if( dir == null ) {
      free();

      log.println( "Cannot open directory 'maps'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialization failure" );
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( !extension.equals( ".rc" ) ) {
        continue;
      }

      String fileName = "maps/" + name;
      String baseName = name.substring( 0, dot );
      if( bspIndices.contains( baseName ) ) {
        log.println( "duplicated bsp: %s", baseName.cstr() );
        throw Exception( "Translator initialization failure" );
      }
      bspIndices.add( baseName, bsps.length() );
      bsps << Resource( baseName, "" );

      log.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    log.unindent();
    log.println( "}" );
    log.println( "object classes (*.rc in 'class') {" );
    log.indent();

    dir = opendir( "class" );
    if( dir == null ) {
      free();

      log.println( "Cannot open directory 'class'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialization failure" );
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( !extension.equals( ".rc" ) ) {
        continue;
      }

      String fileName = "class/" + name;
      String baseName = name.substring( 0, dot );

      if( !classConfig.load( fileName ) ) {
        log.println( "invalid config file %s", fileName.cstr() );
        classConfig.clear();
        continue;
      }
      if( !classConfig.contains( "base" ) ) {
        log.println( "missing base variable" );
        classConfig.clear();
        continue;
      }
      if( !baseClasses.contains( classConfig["base"] ) ) {
        log.println( "invalid base %s", classConfig["base"].cstr() );
        classConfig.clear();
        continue;
      }
      if( classes.contains( baseName ) ) {
        log.println( "duplicated class: %s", baseName.cstr() );
        classConfig.clear();
        continue;
      }
      classConfig.add( "name", baseName );
      classes.add( baseName, baseClasses.cachedValue()( baseName, &classConfig ) );
      classConfig.clear();

      log.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    log.unindent();
    log.println( "}" );
    log.println( "matrix scripts (*.lua in 'lua/matrix') {" );
    log.indent();

    dir = opendir( "lua/matrix" );
    if( dir == null ) {
      free();

      log.println( "Cannot open directory 'lua/matrix'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialization failure" );
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( !extension.equals( ".lua" ) ) {
        continue;
      }

      String fileName = "lua/matrix/" + name;
      String baseName = name.substring( 0, dot );

      matrixScripts << Resource( baseName, fileName );
      log.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    log.unindent();
    log.println( "}" );
    log.println( "nirvana scripts (*.lua in 'lua/nirvana') {" );
    log.indent();

    dir = opendir( "lua/nirvana" );
    if( dir == null ) {
      free();

      log.println( "Cannot open directory 'lua/nirvana'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialization failure" );
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( !extension.equals( ".lua" ) ) {
        continue;
      }

      String fileName = "lua/nirvana/" + name;
      String baseName = name.substring( 0, dot );

      nirvanaScripts << Resource( baseName, fileName );
      log.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    log.unindent();
    log.println( "}" );
    log.unindent();
    log.println( "}" );
  }

  int Translator::textureIndex( const char *file )
  {
    if( textureIndices.contains( file ) ) {
      return textureIndices.cachedValue();
    }
    else {
      log.println( "W: invalid texture file index requested: %s", file );
      return -1;
    }
  }

  int Translator::soundIndex( const char *file )
  {
    if( soundIndices.contains( file ) ) {
      return soundIndices.cachedValue();
    }
    else {
      log.println( "W: invalid sound file index requested: %s", file );
      return -1;
    }
  }

  int Translator::bspIndex( const char *file )
  {
    if( bspIndices.contains( file ) ) {
      return bspIndices.cachedValue();
    }
    else {
      throw Exception( "Invalid BSP index requested" );
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
