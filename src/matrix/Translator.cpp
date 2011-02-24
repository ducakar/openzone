/*
 *  Translator.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Translator.hpp"

#include "matrix/WeaponClass.hpp"
#include "matrix/BotClass.hpp"
#include "matrix/VehicleClass.hpp"

#include <dirent.h>
#include <sys/types.h>

#define OZ_REGISTER_BASECLASS( name ) \
  baseClasses.add( #name, &name##Class::init )

namespace oz
{

  Translator translator;

  Translator::Resource::Resource( const String& name_, const String& path_ ) :
      name( name_ ), path( path_ )
  {}

  Struct* Translator::createStruct( int index, const char* name, const Point3& p,
                                    Struct::Rotation rot ) const
  {
    const int* value = bspIndices.find( name );
    if( value != null ) {
      return new Struct( index, *value, p, rot );
    }
    else {
      throw Exception( "Invalid Structure class requested" );
    }
  }

  int Translator::textureIndex( const char* name ) const
  {
    const int* value = textureIndices.find( name );
    if( value != null ) {
      return *value;
    }
    else {
      log.println( "W: invalid texture file index requested: %s", name );
      return -1;
    }
  }

  int Translator::soundIndex( const char* name ) const
  {
    const int* value = soundIndices.find( name );
    if( value != null ) {
      return *value;
    }
    else {
      log.println( "W: invalid sound file index requested: %s", name );
      return -1;
    }
  }

  int Translator::bspIndex( const char* name ) const
  {
    const int* value = bspIndices.find( name );
    if( value != null ) {
      return *value;
    }
    else {
      throw Exception( "Invalid BSP index requested" );
    }
  }

  Struct* Translator::createStruct( int index, const char* name, InputStream* istream ) const
  {
    const int* value = bspIndices.find( name );
    if( value != null ) {
      return new Struct( index, *value, istream );
    }
    else {
      throw Exception( "Invalid Structure class requested" );
    }
  }

  Object* Translator::createObject( int index, const char* name, const Point3& p ) const
  {
    const ObjectClass* const* value = classes.find( name );
    if( value != null ) {
      return ( *value )->create( index, p );
    }
    else {
      throw Exception( "Invalid Object class requested" );
    }
  }

  Object* Translator::createObject( int index, const char* name, InputStream* istream ) const
  {
    const ObjectClass* const* value = classes.find( name );
    if( value != null ) {
      return ( *value )->create( index, istream );
    }
    else {
      throw Exception( "Invalid Object class requested" );
    }
  }

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
    struct dirent* file;

    log.println( "textures (*.png, *.jpeg, *.jpg in 'textures/oz') {" );
    log.indent();

    dir = opendir( "textures/oz" );
    if( dir == null ) {
      free();

      log.println( "Cannot open directory 'textures/oz'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( !extension.equals( ".png" ) &&
          !extension.equals( ".jpeg" ) &&
          !extension.equals( ".jpg" ) )
      {
        continue;
      }

      String baseName = name.substring( 0, dot );
      String fileName = "textures/oz/" + name;

      if( textureIndices.contains( baseName ) ) {
        log.println( "duplicated texture: %s", baseName.cstr() );
        throw Exception( "Translator initialisation failure" );
      }
      textureIndices.add( baseName, textures.length() );
      textures.add( Resource( baseName, fileName ) );

      log.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    log.unindent();
    log.println( "}" );
    log.println( "sounds (*.au, *.wav in 'snd') {" );
    log.indent();

    dir = opendir( "snd" );
    if( dir == null ) {
      free();

      log.println( "Cannot open directory 'snd'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( !extension.equals( ".au" ) &&
          !extension.equals( ".wav" ) )
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
      sounds.add( Resource( baseName, fileName ) );

      log.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    log.unindent();
    log.println( "}" );
    log.println( "models (* directories in 'mdl') {" );
    log.indent();

    dir = opendir( "mdl" );
    if( dir == null ) {
      free();

      log.println( "Cannot open directory 'mdl'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;

      hard_assert( !name.isEmpty() );

      if( name[0] == '.' || file->d_type != DT_DIR ) {
        continue;
      }

      models.add( Resource( name, "mdl/" + name ) );

      log.println( "%s", name.cstr() );
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
      throw Exception( "Translator initialisation failure" );
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

      String baseName = name.substring( 0, dot );

      if( bspIndices.contains( baseName ) ) {
        log.println( "duplicated bsp: %s", baseName.cstr() );
        throw Exception( "Translator initialisation failure" );
      }
      bspIndices.add( baseName, bsps.length() );
      bsps.add( Resource( baseName, "" ) );

      log.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    log.unindent();
    log.println( "}" );
    log.println( "Terrain heightmaps (*.rc in 'terra') {" );
    log.indent();

    dir = opendir( "terra" );
    if( dir == null ) {
      free();

      log.println( "Cannot open directory 'terra'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
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

      String baseName = name.substring( 0, dot );

      terras.add( Resource( baseName, "" ) );

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
      throw Exception( "Translator initialisation failure" );
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
      throw Exception( "Translator initialisation failure" );
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

      matrixScripts.add( Resource( baseName, fileName ) );
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
      throw Exception( "Translator initialisation failure" );
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

      nirvanaScripts.add( Resource( baseName, fileName ) );
      log.println( "%s", baseName.cstr() );
    }
    closedir( dir );

    log.unindent();
    log.println( "}" );
    log.println( "names (*.txt in 'name') {" );
    log.indent();

    dir = opendir( "name" );
    if( dir == null ) {
      free();

      log.println( "Cannot open directory 'name'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    while( ( file = readdir( dir ) ) != null ) {
      String name = file->d_name;
      int dot = name.lastIndex( '.' );

      if( dot <= 0 ) {
        continue;
      }
      String extension = name.substring( dot );
      if( !extension.equals( ".txt" ) ) {
        continue;
      }

      String fileName = "name/" + name;
      String baseName = name.substring( 0, dot );

      names.add( Resource( baseName, fileName ) );
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
    textureIndices.dealloc();
    soundIndices.clear();
    soundIndices.dealloc();
    bspIndices.clear();
    bspIndices.dealloc();

    textures.clear();
    textures.dealloc();
    sounds.clear();
    sounds.dealloc();
    bsps.clear();
    bsps.dealloc();

    models.clear();
    models.dealloc();
    terras.clear();
    terras.dealloc();

    matrixScripts.clear();
    matrixScripts.dealloc();
    nirvanaScripts.clear();
    nirvanaScripts.dealloc();

    names.clear();
    names.dealloc();

    baseClasses.clear();
    baseClasses.dealloc();
    classes.free();
    classes.dealloc();
  }

}
