/*
 *  Translator.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.h"

#include "matrix/Translator.h"

#include "matrix/WeaponClass.h"
#include "matrix/BotClass.h"
#include "matrix/VehicleClass.h"

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
    DIR* dir;
    dirent* file;

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
      const ObjectClass::InitFunc* initFunc = baseClasses.find( classConfig["base"] );
      if( initFunc == null ) {
        log.println( "invalid base %s", classConfig["base"].cstr() );
        classConfig.clear();
        continue;
      }
      if( classes.contains( baseName ) ) {
        log.println( "duplicated class: %s", baseName.cstr() );
        classConfig.clear();
        continue;
      }
      classes.add( baseName, ( *initFunc )( baseName, &classConfig ) );
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

  void Translator::free()
  {
    textureIndices.clear();
    soundIndices.clear();
    bspIndices.clear();

    baseClasses.clear();
    classes.free();
  }

}
