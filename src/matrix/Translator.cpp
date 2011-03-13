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

  int Translator::textureIndex( const char* name ) const
  {
    const int* value = textureIndices.find( name );
    if( value != null ) {
      return *value;
    }
    else {
      throw Exception( "Invalid texture requested '" + String( name ) + "'" );
    }
  }

  int Translator::soundIndex( const char* name ) const
  {
    const int* value = soundIndices.find( name );
    if( value != null ) {
      return *value;
    }
    else {
      throw Exception( "Invalid sound requested '" + String( name ) + "'" );
    }
  }

  int Translator::shaderIndex( const char* name ) const
  {
    const int* value = shaderIndices.find( name );
    if( value != null ) {
      return *value;
    }
    else {
      throw Exception( "Invalid shader requested '" + String( name ) + "'" );
    }
  }

  int Translator::terraIndex( const char* name ) const
  {
    const int* value = terraIndices.find( name );
    if( value != null ) {
      return *value;
    }
    else {
      throw Exception( "Invalid terra index requested '" + String( name ) + "'" );
    }
  }

  int Translator::bspIndex( const char* name ) const
  {
    const int* value = bspIndices.find( name );
    if( value != null ) {
      return *value;
    }
    else {
      throw Exception( "Invalid BSP index requested '" + String( name ) + "'" );
    }
  }

  int Translator::modelIndex( const char* name ) const
  {
    const int* value = modelIndices.find( name );
    if( value != null ) {
      return *value;
    }
    else {
      throw Exception( "Invalid Model index requested '" + String( name ) + "'" );
    }
  }

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

#ifdef OZ_BUILD_TOOLS
  void Translator::prebuildInit()
  {
    OZ_REGISTER_BASECLASS( Object );
    OZ_REGISTER_BASECLASS( Dynamic );
    OZ_REGISTER_BASECLASS( Weapon );
    OZ_REGISTER_BASECLASS( Bot );
    OZ_REGISTER_BASECLASS( Vehicle );

    textures.alloc( 256 );
    sounds.alloc( 256 );
    terras.alloc( 16 );
    bsps.alloc( 64 );
    models.alloc( 256 );

    matrixScripts.alloc( 64 );
    nirvanaScripts.alloc( 64 );

    names.alloc( 16 );

    log.println( "Translator mapping resources {" );
    log.indent();

    Directory dir;
    Config classConfig;

    log.println( "textures (*.png, *.jpeg, *.jpg in 'bsp/tex') {" );
    log.indent();

    dir.open( "bsp/tex" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'bsp/tex'" );
      log.unindent();
      log.println( "}" );

      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "png" ) && !ent.hasExtension( "jpeg" ) &&
          !ent.hasExtension( "jpg" ) )
      {
        continue;
      }

      String name = ent.baseName();
      String path = String( "bsp/tex/" ) + ent;

      textureIndices.add( name, textures.length() );
      textures.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "sounds (*.au, *.wav in 'snd') {" );
    log.indent();

    dir.open( "snd" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'snd'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "au" ) && !ent.hasExtension( "wav" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "snd/" ) + ent;

      soundIndices.add( name, sounds.length() );
      sounds.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "shaders (*.vert/*.frag in 'glsl') {" );
    log.indent();

    dir.open( "glsl" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'glsl'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "vert" ) ) {
        continue;
      }

      String name = ent.baseName();

      shaderIndices.add( name, shaders.length() );
      shaders.add( Resource( name, "" ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "Terrains (*.rc in 'terra') {" );
    log.indent();

    dir.open( "terra" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'terra'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "rc" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "terra/" ) + ent;

      terraIndices.add( name, terras.length() );
      terras.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "BSP structures (*.rc in 'bsp') {" );
    log.indent();

    dir.open( "bsp" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'bsp'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "rc" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "bsp/" ) + ent;

      bspIndices.add( name, bsps.length() );
      bsps.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "models (* in 'mdl') {" );
    log.indent();

    dir.open( "mdl" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'mdl'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( ent.hasExtension() ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "mdl/" ) + ent;

      modelIndices.add( name, models.length() );
      models.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "matrix scripts (*.lua in 'lua/matrix') {" );
    log.indent();

    dir.open( "lua/matrix" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'lua/matrix'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "lua" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "lua/matrix/" ) + ent;

      matrixScripts.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "nirvana scripts (*.lua in 'lua/nirvana') {" );
    log.indent();

    dir.open( "lua/nirvana" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'lua/nirvana'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "lua" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "lua/nirvana/" ) + ent;

      nirvanaScripts.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "names (*.txt in 'name') {" );
    log.indent();

    dir.open( "name" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'name'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "txt" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "name/" ) + ent;

      names.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "object classes (*.rc in 'class') {" );
    log.indent();

    dir.open( "class" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'class'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "rc" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "class/" ) + ent;

      if( !classConfig.load( path ) ) {
        log.println( "invalid config file %s", path.cstr() );
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
      if( classes.contains( name ) ) {
        log.println( "duplicated class: %s", name.cstr() );
        classConfig.clear();
        continue;
      }
      classes.add( name, ( *initFunc )( name, &classConfig ) );
      classConfig.clear();

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.unindent();
    log.println( "}" );
  }
#endif

  void Translator::init()
  {
    OZ_REGISTER_BASECLASS( Object );
    OZ_REGISTER_BASECLASS( Dynamic );
    OZ_REGISTER_BASECLASS( Weapon );
    OZ_REGISTER_BASECLASS( Bot );
    OZ_REGISTER_BASECLASS( Vehicle );

    textures.alloc( 256 );
    sounds.alloc( 256 );
    terras.alloc( 16 );
    bsps.alloc( 64 );
    models.alloc( 256 );

    matrixScripts.alloc( 64 );
    nirvanaScripts.alloc( 64 );

    names.alloc( 16 );

    log.println( "Translator mapping resources {" );
    log.indent();

    Directory dir;
    Config classConfig;

    log.println( "textures (*.ozcTex in 'bsp/tex') {" );
    log.indent();

    dir.open( "bsp/tex" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'bsp/tex'" );
      log.unindent();
      log.println( "}" );

      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "ozcTex" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "bsp/tex/" ) + ent;

      textureIndices.add( name, textures.length() );
      textures.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "sounds (*.au, *.wav in 'snd') {" );
    log.indent();

    dir.open( "snd" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'snd'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "au" ) && !ent.hasExtension( "wav" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "snd/" ) + ent;

      soundIndices.add( name, sounds.length() );
      sounds.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "shaders (*.vert/*.frag in 'glsl') {" );
    log.indent();

    dir.open( "glsl" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'glsl'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "vert" ) ) {
        continue;
      }

      String name = ent.baseName();

      shaderIndices.add( name, shaders.length() );
      shaders.add( Resource( name, "" ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "Terrains (*.ozTerra/*.ozcTerra in 'terra') {" );
    log.indent();

    dir.open( "terra" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'terra'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "ozTerra" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "terra/" ) + ent;

      terraIndices.add( name, terras.length() );
      terras.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "BSP structures (*.ozBSP/*.ozcBSP in 'bsp') {" );
    log.indent();

    dir.open( "bsp" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'bsp'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "ozBSP" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "bsp/" ) + ent;

      bspIndices.add( name, bsps.length() );
      bsps.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "models (*.ozcSMM, *.ozcMD2 in 'mdl') {" );
    log.indent();

    dir.open( "mdl" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'mdl'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "ozcSMM" ) && !ent.hasExtension( "ozcMD2" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "mdl/" ) + ent;

      modelIndices.add( name, models.length() );
      models.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "matrix scripts (*.luac in 'lua/matrix') {" );
    log.indent();

    dir.open( "lua/matrix" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'lua/matrix'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "luac" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "lua/matrix/" ) + ent;

      matrixScripts.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "nirvana scripts (*.luac in 'lua/nirvana') {" );
    log.indent();

    dir.open( "lua/nirvana" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'lua/nirvana'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "luac" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "lua/nirvana/" ) + ent;

      nirvanaScripts.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "names (*.txt in 'name') {" );
    log.indent();

    dir.open( "name" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'name'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "txt" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "name/" ) + ent;

      names.add( Resource( name, path ) );

      log.println( "%s", name.cstr() );
    }
    dir.close();

    log.unindent();
    log.println( "}" );
    log.println( "object classes (*.rc in 'class') {" );
    log.indent();

    dir.open( "class" );
    if( !dir.isOpened() ) {
      free();

      log.println( "Cannot open directory 'class'" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Translator initialisation failure" );
    }
    foreach( ent, dir.citer() ) {
      if( !ent.hasExtension( "rc" ) ) {
        continue;
      }

      String name = ent.baseName();
      String path = String( "class/" ) + ent;

      if( !classConfig.load( path ) ) {
        log.println( "invalid config file %s", path.cstr() );
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
      if( classes.contains( name ) ) {
        log.println( "duplicated class: %s", name.cstr() );
        classConfig.clear();
        continue;
      }
      classes.add( name, ( *initFunc )( name, &classConfig ) );
      classConfig.clear();

      log.println( "%s", name.cstr() );
    }
    dir.close();

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
    shaderIndices.clear();
    shaderIndices.dealloc();
    terraIndices.clear();
    terraIndices.dealloc();
    bspIndices.clear();
    bspIndices.dealloc();
    modelIndices.clear();
    modelIndices.dealloc();

    textures.clear();
    textures.dealloc();
    sounds.clear();
    sounds.dealloc();
    shaders.clear();
    shaders.dealloc();
    terras.clear();
    terras.dealloc();
    bsps.clear();
    bsps.dealloc();
    models.clear();
    models.dealloc();

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
