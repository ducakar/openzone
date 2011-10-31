/*
 *  Library.cpp
 *
 *  Map of all resources, object types, scripts etc.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Library.hpp"

#include "matrix/WeaponClass.hpp"
#include "matrix/BotClass.hpp"
#include "matrix/VehicleClass.hpp"

#include <dirent.h>
#include <sys/types.h>

#define OZ_REGISTER_BASECLASS( name ) \
  baseClasses.add( #name, &name##Class::init )

namespace oz
{
namespace matrix
{

Library library;

Library::Resource::Resource( const String& name_, const String& path_ ) :
    name( name_ ), path( path_ )
{}

int Library::textureIndex( const char* name ) const
{
  const int* value = textureIndices.find( name );
  if( value != null ) {
    return *value;
  }
  else {
    throw Exception( "Invalid texture requested '" + String( name ) + "'" );
  }
}

int Library::soundIndex( const char* name ) const
{
  const int* value = soundIndices.find( name );
  if( value != null ) {
    return *value;
  }
  else {
    throw Exception( "Invalid sound requested '" + String( name ) + "'" );
  }
}

int Library::shaderIndex( const char* name ) const
{
  const int* value = shaderIndices.find( name );
  if( value != null ) {
    return *value;
  }
  else {
    throw Exception( "Invalid shader requested '" + String( name ) + "'" );
  }
}

int Library::terraIndex( const char* name ) const
{
  const int* value = terraIndices.find( name );
  if( value != null ) {
    return *value;
  }
  else {
    throw Exception( "Invalid terra index requested '" + String( name ) + "'" );
  }
}

int Library::caelumIndex( const char* name ) const
{
  const int* value = caelumIndices.find( name );
  if( value != null ) {
    return *value;
  }
  else {
    throw Exception( "Invalid caelum index requested '" + String( name ) + "'" );
  }
}

int Library::bspIndex( const char* name ) const
{
  const int* value = bspIndices.find( name );
  if( value != null ) {
    return *value;
  }
  else {
    throw Exception( "Invalid BSP index requested '" + String( name ) + "'" );
  }
}

int Library::modelIndex( const char* name ) const
{
  const int* value = modelIndices.find( name );
  if( value != null ) {
    return *value;
  }
  else {
    throw Exception( "Invalid model index requested '" + String( name ) + "'" );
  }
}

int Library::nameListIndex( const char* name ) const
{
  const int* value = nameListIndices.find( name );
  if( value != null ) {
    return *value;
  }
  else {
    throw Exception( "Invalid name list index requested '" + String( name ) + "'" );
  }
}

Struct* Library::createStruct( int index, int id, const Point3& p, Heading heading ) const
{
  return new Struct( index, id, p, heading );
}

Struct* Library::createStruct( int index, int id, InputStream* istream ) const
{
  return new Struct( index, id, istream );
}

Object* Library::createObject( int index, const char* name,
                               const Point3& p, Heading heading ) const
{
  const ObjectClass* const* value = classes.find( name );
  if( value != null ) {
    return ( *value )->create( index, p, heading );
  }
  else {
    throw Exception( "Invalid object class requested '" + String( name ) + "'" );
  }
}

Object* Library::createObject( int index, const char* name, InputStream* istream ) const
{
  const ObjectClass* const* value = classes.find( name );
  if( value != null ) {
    return ( *value )->create( index, istream );
  }
  else {
    throw Exception( "Invalid object class requested '" + String( name ) + "'" );
  }
}

void Library::init()
{
  OZ_REGISTER_BASECLASS( Object );
  OZ_REGISTER_BASECLASS( Dynamic );
  OZ_REGISTER_BASECLASS( Weapon );
  OZ_REGISTER_BASECLASS( Bot );
  OZ_REGISTER_BASECLASS( Vehicle );

  textures.alloc( 256 );
  sounds.alloc( 256 );
  shaders.alloc( 32 );
  terras.alloc( 16 );
  bsps.alloc( 64 );
  models.alloc( 256 );
  nameLists.alloc( 16 );
  musics.alloc( 32 );

  log.println( "Library mapping resources {" );
  log.indent();

  File dir;
  File subDir;
  DArray<File> dirList;
  DArray<File> subDirList;
  Config classConfig;

  log.println( "textures (*.ozcTex in 'bsp/*') {" );
  log.indent();

  dir.setPath( "bsp" );

  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'bsp'" );
    log.unindent();
    log.println( "}" );

    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    subDir.setPath( file->path() );
    if( !subDir.ls( &subDirList ) ) {
      free();

      log.println( "Cannot open directory '%s'", subDir.path() );
      log.unindent();
      log.println( "}" );

      throw Exception( "Library initialisation failure" );
    }

    foreach( file, subDirList.citer() ) {
      if( !file->hasExtension( "ozcTex" ) ) {
        continue;
      }

      String name = subDir.name() + String( "/" ) + file->baseName();

      log.println( "%s", name.cstr() );

      textureIndices.add( name, textures.length() );
      textures.add( Resource( name, file->path() ) );
    }
    subDirList.dealloc();
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "sounds (*.wav in 'snd') {" );
  log.indent();

  dir.setPath( "snd" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'snd'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    subDir.setPath( file->path() );
    if( !subDir.ls( &subDirList ) ) {
      free();

      log.println( "Cannot open directory '%s'", subDir.path() );
      log.unindent();
      log.println( "}" );

      throw Exception( "Library initialisation failure" );
    }

    foreach( file, subDirList.citer() ) {
      if( !file->hasExtension( "wav" ) ) {
        continue;
      }

      String name = subDir.name() + String( "/" ) + file->baseName();

      log.println( "%s", name.cstr() );

      soundIndices.add( name, sounds.length() );
      sounds.add( Resource( name, file->path() ) );
    }
    subDirList.dealloc();
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "shaders (*.vert/*.frag in 'glsl') {" );
  log.indent();

  dir.setPath( "glsl" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'glsl'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "vert" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    shaderIndices.add( name, shaders.length() );
    shaders.add( Resource( name, "" ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "Terrains (*.ozTerra/*.ozcTerra in 'terra') {" );
  log.indent();

  dir.setPath( "terra" );
  if( !dir.ls( &dirList) ) {
    free();

    log.println( "Cannot open directory 'terra'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "ozTerra" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    terraIndices.add( name, terras.length() );
    terras.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "Caela (*.ozcCaelum in 'caelum') {" );
  log.indent();

  dir.setPath( "caelum" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'caelum'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "ozcCaelum" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    caelumIndices.add( name, caela.length() );
    caela.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "BSP structures (*.ozBSP/*.ozcBSP in 'bsp') {" );
  log.indent();

  dir.setPath( "bsp" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'bsp'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.iter() ) {
    if( !file->hasExtension( "ozBSP" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    bspIndices.add( name, bsps.length() );
    bsps.add( Resource( name, file->path() ) );

    // read bounds
    if( !file->map() ) {
      throw Exception( "Cannot mmap BSP to read dimensions" );
    }

    InputStream is = file->inputStream();

    Point3 mins = is.readPoint3();
    Point3 maxs = is.readPoint3();

    file->unmap();

    bspBounds.add( Bounds( mins, maxs ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "models (*.ozcSMM, *.ozcMD2, *.ozcMD3 in 'mdl') {" );
  log.indent();

  dir.setPath( "mdl" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'mdl'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "ozcSMM" ) && !file->hasExtension( "ozcMD2" ) &&
        !file->hasExtension( "ozcMD3" ) )
    {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    if( modelIndices.contains( name ) ) {
      throw Exception( "Duplicated model '" + name + "'" );
    }

    modelIndices.add( name, models.length() );
    models.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "name lists (*.txt in 'name') {" );
  log.indent();

  dir.setPath( "name" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'name'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "txt" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    nameListIndices.add( name, nameLists.length() );
    nameLists.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "music (*.oga, *.mp3 in 'music') {" );
  log.indent();

  dir.setPath( "music" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'music'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "oga" ) && !file->hasExtension( "mp3" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    musics.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "object classes (*.rc in 'class') {" );
  log.indent();

  dir.setPath( "class" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'class'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    if( !classConfig.load( file->path() ) ) {
      log.println( "invalid config file %s", file->path() );
      classConfig.clear();
      throw Exception( "Class description error" );
    }

    String base = classConfig.get( "base", "" );
    if( base.isEmpty() ) {
      log.println( "missing base variable" );
      classConfig.clear();
      throw Exception( "Class description error" );
    }
    const ObjectClass::InitFunc* initFunc = baseClasses.find( base );
    if( initFunc == null ) {
      log.println( "invalid base %s", base.cstr() );
      classConfig.clear();
      throw Exception( "Class description error" );
    }
    if( classes.contains( name ) ) {
      log.println( "duplicated class: %s", name.cstr() );
      classConfig.clear();
      throw Exception( "Class description error" );
    }

    classConfig.add( "name", name );
    classes.add( name, ( *initFunc )( &classConfig ) );
    classConfig.clear();
  }
  dirList.dealloc();

  foreach( clazz, classes.citer() ) {
    const WeaponClass* weaponClass = dynamic_cast<const WeaponClass*>( clazz.value() );

    if( weaponClass != null ) {
      const_cast<WeaponClass*>( weaponClass )->fillAllowedUsers();
    }
  }

  log.unindent();
  log.println( "}" );
  log.unindent();
  log.println( "}" );
}

void Library::buildInit()
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
  nameLists.alloc( 16 );

  log.println( "Library mapping resources {" );
  log.indent();

  File dir;
  File subDir;
  DArray<File> dirList;
  DArray<File> subDirList;
  Config classConfig;

  log.println( "textures (*.png, *.jpeg, *.jpg in 'data/textures/*') {" );
  log.indent();

  dir.setPath( "data/textures" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'data/textures'" );
    log.unindent();
    log.println( "}" );

    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    subDir.setPath( file->path() );
    if( !subDir.ls( &subDirList ) ) {
      free();

      log.println( "Cannot open directory '%s'", subDir.path() );
      log.unindent();
      log.println( "}" );

      throw Exception( "Library initialisation failure" );
    }

    foreach( file, subDirList.citer() ) {
      if( !file->hasExtension( "png" ) && !file->hasExtension( "jpeg" ) &&
          !file->hasExtension( "jpg" ) )
      {
        continue;
      }

      String name = subDir.name() + String( "/" ) + file->baseName();

      log.println( "%s", name.cstr() );

      if( textureIndices.contains( name ) ) {
        throw Exception( "Duplicated texture '" + name + "'" );
      }

      textureIndices.add( name, textures.length() );
      textures.add( Resource( name, file->path() ) );
    }
    subDirList.dealloc();
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "sounds (*.wav in 'snd') {" );
  log.indent();

  dir.setPath( "snd" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'snd'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    subDir.setPath( file->path() );
    if( !subDir.ls( &subDirList ) ) {
      free();

      log.println( "Cannot open directory '%s'", subDir.path() );
      log.unindent();
      log.println( "}" );

      throw Exception( "Library initialisation failure" );
    }

    foreach( file, subDirList.citer() ) {
      if( !file->hasExtension( "wav" ) ) {
        continue;
      }

      String name = subDir.name() + String( "/" ) + file->baseName();

      log.println( "%s", name.cstr() );

      soundIndices.add( name, sounds.length() );
      sounds.add( Resource( name, file->path() ) );
    }
    subDirList.dealloc();
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "shaders (*.vert/*.frag in 'glsl') {" );
  log.indent();

  dir.setPath( "glsl" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'glsl'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "vert" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    shaderIndices.add( name, shaders.length() );
    shaders.add( Resource( name, "" ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "Terrains (*.rc in 'terra') {" );
  log.indent();

  dir.setPath( "terra" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'terra'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    terraIndices.add( name, terras.length() );
    terras.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "Caela (*.rc in 'caelum') {" );
  log.indent();

  dir.setPath( "caelum" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'caelum'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    caelumIndices.add( name, caela.length() );
    caela.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "BSP structures (*.rc in 'data/maps') {" );
  log.indent();

  dir.setPath( "data/maps" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'data/maps'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    bspIndices.add( name, bsps.length() );
    bsps.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "models (directories in 'mdl') {" );
  log.indent();

  dir.setPath( "mdl" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'mdl'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    String name = file->name();

    log.println( "%s", name.cstr() );

    modelIndices.add( name, models.length() );
    models.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "name lists (*.txt in 'name') {" );
  log.indent();

  dir.setPath( "name" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'name'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "txt" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    nameListIndices.add( name, nameLists.length() );
    nameLists.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "music (*.oga, *.mp3 in 'music') {" );
  log.indent();

  dir.setPath( "music" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'music'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "oga" ) && !file->hasExtension( "mp3" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    musics.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.println( "object classes (*.rc in 'class') {" );
  log.indent();

  dir.setPath( "class" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'class'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    if( !classConfig.load( file->path() ) ) {
      log.println( "invalid config file %s", file->path() );
      classConfig.clear();
      throw Exception( "Class description error" );
    }

    String base = classConfig.get( "base", "" );
    if( base.isEmpty() ) {
      log.println( "missing base variable" );
      classConfig.clear();
      throw Exception( "Class description error" );
    }
    const ObjectClass::InitFunc* initFunc = baseClasses.find( base );
    if( initFunc == null ) {
      log.println( "invalid base %s", base.cstr() );
      classConfig.clear();
      throw Exception( "Class description error" );
    }
    if( classes.contains( name ) ) {
      log.println( "duplicated class: %s", name.cstr() );
      classConfig.clear();
      throw Exception( "Class description error" );
    }

    classConfig.add( "name", name );
    classes.add( name, null );
    classConfig.clear();
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
  log.unindent();
  log.println( "}" );
}

void Library::free()
{
  textureIndices.clear();
  textureIndices.dealloc();
  soundIndices.clear();
  soundIndices.dealloc();
  shaderIndices.clear();
  shaderIndices.dealloc();
  terraIndices.clear();
  terraIndices.dealloc();
  caelumIndices.clear();
  caelumIndices.dealloc();
  bspIndices.clear();
  bspIndices.dealloc();
  modelIndices.clear();
  modelIndices.dealloc();
  nameListIndices.clear();
  nameListIndices.dealloc();

  textures.clear();
  textures.dealloc();
  sounds.clear();
  sounds.dealloc();
  shaders.clear();
  shaders.dealloc();
  terras.clear();
  terras.dealloc();
  caela.clear();
  caela.dealloc();
  bsps.clear();
  bsps.dealloc();
  models.clear();
  models.dealloc();
  nameLists.clear();
  nameLists.dealloc();
  musics.clear();
  musics.dealloc();

  bspBounds.clear();
  bspBounds.dealloc();

  baseClasses.clear();
  baseClasses.dealloc();
  classes.free();
  classes.dealloc();
}

}
}
