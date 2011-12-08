/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Library.cc
 *
 * Map of all resources, object types, scripts etc.
 */

#include "stable.hh"

#include "matrix/Library.hh"

#include "matrix/Vehicle.hh"

#define OZ_REGISTER_BASECLASS( name ) \
  baseClasses.include( #name, name##Class::createClass )

namespace oz
{
namespace matrix
{

Library library;

Library::Resource::Resource( const String& name_, const String& path_ ) :
    name( name_ ), path( path_ )
{}

const BSP* Library::bsp( const char* name ) const
{
  const BSP* value = bsps.find( name );

  if( value == null ) {
    throw Exception( "Invalid BSP requested '%s'", name );
  }
  return value;
}

const ObjectClass* Library::objClass( const char* name ) const
{
  const ObjectClass* const* value = objClasses.find( name );

  if( value == null ) {
    throw Exception( "Invalid object class requested '%s'", name );
  }
  return *value;
}

const FragPool* Library::fragPool( const char* name ) const
{
  const FragPool* value = fragPools.find( name );

  if( value == null ) {
    throw Exception( "Invalid fragment pool requested '%s'", name );
  }
  return value;
}

int Library::textureIndex( const char* name ) const
{
  const int* value = textureIndices.find( name );

  if( value == null ) {
    throw Exception( "Invalid texture requested '%s'", name );
  }
  return *value;
}

int Library::soundIndex( const char* name ) const
{
  const int* value = soundIndices.find( name );

  if( value == null ) {
    throw Exception( "Invalid sound requested '%s'", name );
  }
  return *value;
}

int Library::shaderIndex( const char* name ) const
{
  const int* value = shaderIndices.find( name );

  if( value == null ) {
    throw Exception( "Invalid shader requested '%s'", name );
  }
  return *value;
}

int Library::terraIndex( const char* name ) const
{
  const int* value = terraIndices.find( name );

  if( value == null ) {
    throw Exception( "Invalid terra index requested '%s'", name );
  }
  return *value;
}

int Library::caelumIndex( const char* name ) const
{
  const int* value = caelumIndices.find( name );

  if( value == null ) {
    throw Exception( "Invalid caelum index requested '%s'", name );
  }
  return *value;
}

int Library::modelIndex( const char* name ) const
{
  const int* value = modelIndices.find( name );

  if( value == null ) {
    throw Exception( "Invalid model index requested '%s'", name );
  }
  return *value;
}

int Library::nameListIndex( const char* name ) const
{
  const int* value = nameListIndices.find( name );

  if( value == null ) {
    throw Exception( "Invalid name list index requested '%s'", name );
  }
  return *value;
}

int Library::deviceIndex( const char* name ) const
{
  const int* value = deviceIndices.find( name );

  return value == null ? -1 : *value;
}

int Library::imagoIndex( const char* name ) const
{
  const int* value = imagoIndices.find( name );

  return value == null ? -1 : *value;
}

int Library::audioIndex( const char* name ) const
{
  const int* value = audioIndices.find( name );

  return value == null ? -1 : *value;
}

void Library::freeBSPs()
{
  foreach( bsp, bsps.iter() ) {
    if( bsp.value().nUsers != 0 ) {
      bsp.value().unload();
    }
  }
}

void Library::initShaders()
{
  if( log.isVerbose ) {
    log.println( "Shaders (*.vert/*.frag in 'glsl') {" );
    log.indent();
  }

  File dir( "glsl" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "vert" ) ) {
      continue;
    }

    String name = file->baseName();

    if( log.isVerbose ) {
      log.println( "%s", name.cstr() );
    }

    shaderIndices.add( name, shaders.length() );
    shaders.add( Resource( name, "" ) );
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  shaders", shaders.length() );
  }
}

void Library::initTextures()
{
  if( log.isVerbose ) {
    log.println( "Textures (*.ozcTex in 'bsp/*') {" );
    log.indent();
  }

  File dir( "bsp" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    File subDir( file->path() );
    DArray<File> subDirList = subDir.ls();

    foreach( file, subDirList.citer() ) {
      if( !file->hasExtension( "ozcTex" ) ) {
        continue;
      }

      String name = subDir.name() + String( "/" ) + file->baseName();

      if( log.isVerbose ) {
        log.println( "%s", name.cstr() );
      }

      textureIndices.add( name, textures.length() );
      textures.add( Resource( name, file->path() ) );
    }
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  textures", textures.length() );
  }
}

void Library::initBuildTextures()
{
  if( log.isVerbose ) {
    log.println( "Textures (*.png, *.jpeg, *.jpg in 'data/textures/*') {" );
    log.indent();
  }

  File dir( "data/textures" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    File subDir( file->path() );
    DArray<File> subDirList = subDir.ls();

    foreach( file, subDirList.citer() ) {
      if( !file->hasExtension( "png" ) && !file->hasExtension( "jpeg" ) &&
          !file->hasExtension( "jpg" ) )
      {
        continue;
      }

      String name = subDir.name() + String( "/" ) + file->baseName();

      if( log.isVerbose ) {
        log.println( "%s", name.cstr() );
      }

      if( textureIndices.contains( name ) ) {
        throw Exception( "Duplicated texture '%s'", name.cstr() );
      }

      textureIndices.add( name, textures.length() );
      textures.add( Resource( name, file->path() ) );
    }
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  textures", textures.length() );
  }
}

void Library::initSounds()
{
  if( log.isVerbose ) {
    log.println( "Sounds (*.wav in 'snd') {" );
    log.indent();
  }

  File dir( "snd" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    File subDir( file->path() );
    DArray<File> subDirList = subDir.ls();

    foreach( file, subDirList.citer() ) {
      if( !file->hasExtension( "wav" ) ) {
        continue;
      }

      String name = subDir.name() + String( "/" ) + file->baseName();

      if( log.isVerbose ) {
        log.println( "%s", name.cstr() );
      }

      soundIndices.add( name, sounds.length() );
      sounds.add( Resource( name, file->path() ) );
    }
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  sounds", sounds.length() );
  }
}

void Library::initCaela()
{
  if( log.isVerbose ) {
    log.println( "Caela (*.ozcCaelum in 'caelum') {" );
    log.indent();
  }

  File dir( "caelum" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "ozcCaelum" ) ) {
      continue;
    }

    String name = file->baseName();

    if( log.isVerbose ) {
      log.println( "%s", name.cstr() );
    }

    caelumIndices.add( name, caela.length() );
    caela.add( Resource( name, file->path() ) );
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  caela", caela.length() );
  }
}

void Library::initBuildCaela()
{
  if( log.isVerbose ) {
    log.println( "Caela (*.rc in 'caelum') {" );
    log.indent();
  }

  File dir( "caelum" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    if( log.isVerbose ) {
      log.println( "%s", name.cstr() );
    }

    caelumIndices.add( name, caela.length() );
    caela.add( Resource( name, file->path() ) );
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  caela", caela.length() );
  }
}

void Library::initTerrae()
{
  if( log.isVerbose ) {
    log.println( "Terrae (*.ozTerra/*.ozcTerra in 'terra') {" );
    log.indent();
  }

  File dir( "terra" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "ozTerra" ) ) {
      continue;
    }

    String name = file->baseName();

    if( log.isVerbose ) {
      log.println( "%s", name.cstr() );
    }

    terraIndices.add( name, terrae.length() );
    terrae.add( Resource( name, file->path() ) );
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  terras", terrae.length() );
  }
}

void Library::initBuildTerrae()
{
  if( log.isVerbose ) {
    log.println( "Terrae (*.rc in 'terra') {" );
    log.indent();
  }

  File dir( "terra" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    if( log.isVerbose ) {
      log.println( "%s", name.cstr() );
    }

    terraIndices.add( name, terrae.length() );
    terrae.add( Resource( name, file->path() ) );
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  terras", terrae.length() );
  }
}

void Library::initBSPs()
{
  if( log.isVerbose ) {
    log.println( "BSP structures (*.ozBSP/*.ozcBSP in 'bsp') {" );
    log.indent();
  }

  File dir( "bsp" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.iter() ) {
    if( !file->hasExtension( "ozBSP" ) ) {
      continue;
    }

    String name = file->baseName();

    if( log.isVerbose ) {
      log.println( "%s", name.cstr() );
    }

    BSP* bsp = bsps.add( name, BSP( name, bsps.length() ) );
    bsp->init();
  }

  nBSPs = bsps.length();

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  BSPs", nBSPs );
  }
}

void Library::initBuildBSPs()
{
  if( log.isVerbose ) {
    log.println( "BSP structures (*.rc in 'data/maps') {" );
    log.indent();
  }

  File dir( "data/maps" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    if( log.isVerbose ) {
      log.println( "%s", name.cstr() );
    }

    bsps.add( name, BSP( name, bsps.length() ) );
  }

  nBSPs = bsps.length();

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  BSPs", nBSPs );
  }
}

void Library::initModels()
{
  if( log.isVerbose ) {
    log.println( "Models (*.ozcSMM, *.ozcMD2, *.ozcMD3 in 'mdl') {" );
    log.indent();
  }

  File dir( "mdl" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "ozcSMM" ) && !file->hasExtension( "ozcMD2" ) &&
        !file->hasExtension( "ozcMD3" ) )
    {
      continue;
    }

    String name = file->baseName();

    if( log.isVerbose ) {
      log.println( "%s", name.cstr() );
    }

    if( modelIndices.contains( name ) ) {
      throw Exception( "Duplicated model '%s'", name.cstr() );
    }

    modelIndices.add( name, models.length() );
    models.add( Resource( name, file->path() ) );
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  models", models.length() );
  }
}

void Library::initBuildModels()
{
  if( log.isVerbose ) {
    log.println( "Models (directories in 'mdl') {" );
    log.indent();
  }

  File dir( "mdl" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    String name = file->name();

    if( log.isVerbose ) {
      log.println( "%s", name.cstr() );
    }

    modelIndices.add( name, models.length() );
    models.add( Resource( name, file->path() ) );
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  models", models.length() );
  }
}

void Library::initMusicRecurse( const char* path )
{
  File dir( path );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.iter() ) {
    if( file->getType() == File::DIRECTORY ) {
      initMusicRecurse( file->path() );
    }
    if( !file->hasExtension( "oga" ) && !file->hasExtension( "ogg" ) &&
        !file->hasExtension( "mp3" ) )
    {
      continue;
    }

    if( log.isVerbose ) {
      log.println( "%s", file->path() );
    }

    musics.add( Resource( file->baseName(), file->path() ) );
  }
}

void Library::initMusic()
{
  const char* userMusicPath = config.getSet( "dir.music", "" );

  if( log.isVerbose ) {
    if( String::isEmpty( userMusicPath ) ) {
      log.println( "Music (*.oga, *.ogg, *.mp3 in 'music') {" );
    }
    else {
      log.println( "Music (*.oga, *.ogg, *.mp3 in 'music' and '%s') {", userMusicPath );
    }
    log.indent();
  }

  File dir( "music" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "oga" ) && !file->hasExtension( "ogg" ) &&
        !file->hasExtension( "mp3" ) )
    {
      continue;
    }

    String name = file->baseName();

    if( log.isVerbose ) {
      log.println( "%s", name.cstr() );
    }

    musics.add( Resource( name, file->path() ) );
  }

  if( !String::isEmpty( userMusicPath ) ) {
    initMusicRecurse( userMusicPath );
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  music tracks", musics.length() );
  }
}

void Library::initNameLists()
{
  if( log.isVerbose ) {
    log.println( "Name lists (*.txt in 'name') {" );
    log.indent();
  }

  File dir( "name" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "txt" ) ) {
      continue;
    }

    String name = file->baseName();

    if( log.isVerbose ) {
      log.println( "%s", name.cstr() );
    }

    nameListIndices.add( name, nameLists.length() );
    nameLists.add( Resource( name, file->path() ) );
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  name lists", nameLists.length() );
  }
}

void Library::initFragPools()
{
  if( log.isVerbose ) {
    log.println( "Fragment pools (*.rc in 'frag') {" );
    log.indent();
  }

  File dir( "frag" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    if( log.isVerbose ) {
      log.println( "%s", name.cstr() );
    }

    fragPools.add( name, FragPool( name, fragPools.length() ) );
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  fragment pools", fragPools.length() );
  }
}

void Library::initClasses()
{
  OZ_REGISTER_BASECLASS( Object );
  OZ_REGISTER_BASECLASS( Dynamic );
  OZ_REGISTER_BASECLASS( Weapon );
  OZ_REGISTER_BASECLASS( Bot );
  OZ_REGISTER_BASECLASS( Vehicle );

  Config classConfig;

  if( log.isVerbose ) {
    log.println( "Object classes (*.rc in 'class') {" );
    log.indent();
  }

  File dir( "class" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    if( objClasses.contains( name ) ) {
      throw Exception( "Duplicated class '%s'", name.cstr() );
    }
    if( !classConfig.load( file->path() ) ) {
      throw Exception( "%s: Class parse error", name.cstr() );
    }

    const char* base = classConfig.get( "base", "" );
    if( String::isEmpty( base ) ) {
      throw Exception( "%s: 'base' missing in class description", name.cstr() );
    }

    const ObjectClass::CreateFunc* createFunc = baseClasses.find( base );
    if( createFunc == null ) {
      throw Exception( "%s: Invalid class base '%s'", name.cstr(), base );
    }

    // First we only add class instances, we don't initialise them as each class may have references
    // to other classes that have not been created yet.
    objClasses.add( name, ( *createFunc )() );

    // index device, imago and audio classes
    const char* sDevice = classConfig.get( "deviceType", "" );
    if( !String::isEmpty( sDevice ) ) {
      deviceIndices.include( sDevice, deviceIndices.length() );
    }

    const char* sImago = classConfig.get( "imagoType", "" );
    if( !String::isEmpty( sImago ) ) {
      imagoIndices.include( sImago, imagoIndices.length() );
    }

    const char* sAudio = classConfig.get( "audioType", "" );
    if( !String::isEmpty( sAudio ) ) {
      audioIndices.include( sAudio, audioIndices.length() );
    }

    classConfig.clear();
  }

  nDeviceClasses = deviceIndices.length();
  nImagoClasses  = imagoIndices.length();
  nAudioClasses  = audioIndices.length();

  // initialise all classes
  foreach( classIter, objClasses.iter() ) {
    if( log.isVerbose ) {
      log.println( "%s", classIter.key().cstr() );
    }

    String path = "class/" + classIter.key() + ".rc";

    if( !classConfig.load( path ) ) {
      throw Exception( "Class parse error" );
    }

    classConfig.add( "name", classIter.key() );
    classIter.value()->initClass( &classConfig );
    classConfig.clear( true );
  }

  foreach( classIter, objClasses.iter() ) {
    ObjectClass* objClazz = classIter.value();

    // check that all items are valid
    for( int i = 0; i < objClazz->defaultItems.length(); ++i ) {
      const ObjectClass* itemClazz = objClazz->defaultItems[i];

      if( ( itemClazz->flags & ( Object::DYNAMIC_BIT | Object::ITEM_BIT ) ) !=
          ( Object::DYNAMIC_BIT | Object::ITEM_BIT ) )
      {
        throw Exception( "Invalid item class '%s' in '%s'",
                         itemClazz->name.cstr(), objClazz->name.cstr() );
      }
    }

    // fill allowedUsers for weapons
    if( objClazz->flags & Object::WEAPON_BIT ) {
      WeaponClass* weaponClazz = static_cast<WeaponClass*>( objClazz );

      int underscore = weaponClazz->name.index( '_' );
      if( underscore == -1 ) {
        throw Exception( "Weapon class file must be named <botClass>_weapon.<weapon>.rc" );
      }

      String matchClassBaseName = weaponClazz->name.substring( 0, underscore );

      foreach( clazz, objClasses.citer() ) {
        String botClassBaseName = clazz.value()->name;

        int dot = botClassBaseName.index( '.' );
        if( dot != -1 ) {
          botClassBaseName = botClassBaseName.substring( 0, dot );
        }

        if( matchClassBaseName.equals( botClassBaseName ) ) {
          weaponClazz->allowedUsers.add( clazz.value() );
        }
      }
    }
  }

  foreach( classIter, objClasses.citer() ) {
    // check if weaponItem is a valid weapon for bots
    if( classIter.value()->flags & Object::BOT_BIT ) {
      const BotClass* botClazz = static_cast<const BotClass*>( classIter.value() );

      if( botClazz->weaponItem != -1 ) {
        if( uint( botClazz->weaponItem ) >= uint( botClazz->defaultItems.length() ) ) {
          throw Exception( "Invalid weaponItem for '%s'", botClazz->name.cstr() );
        }

        // we already checked it the previous loop it's non-null and a valid item
        const ObjectClass* itemClazz = botClazz->defaultItems[botClazz->weaponItem];

        if( !( itemClazz->flags & Object::WEAPON_BIT ) ) {
          throw Exception( "Default weapon of '%s' is of a non-weapon class",
                           botClazz->name.cstr() );
        }

        const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( itemClazz );

        if( !weaponClazz->allowedUsers.contains( botClazz ) ) {
          throw Exception( "Default weapon of '%s' is not allowed for this bot class",
                           botClazz->name.cstr() );
        }
      }
    }
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }
  else {
    log.println( "%5d  object classes", objClasses.length() );
  }
}

void Library::init()
{
  shaders.alloc( 64 );
  textures.alloc( 256 );
  sounds.alloc( 256 );
  caela.alloc( 16 );
  terrae.alloc( 16 );
  models.alloc( 256 );
  musics.alloc( 64 );
  nameLists.alloc( 16 );

  log.println( "Library mapping resources {" );
  log.indent();

  initShaders();
  initTextures();
  initSounds();
  initCaela();
  initTerrae();
  initBSPs();
  initModels();
  initMusic();
  initNameLists();
  initFragPools();
  initClasses();

  log.unindent();
  log.println( "}" );
}

void Library::buildInit()
{
  shaders.alloc( 64 );
  textures.alloc( 256 );
  sounds.alloc( 256 );
  caela.alloc( 16 );
  terrae.alloc( 16 );
  models.alloc( 256 );
  musics.alloc( 64 );
  nameLists.alloc( 16 );

  log.println( "Library mapping resources {" );
  log.indent();

  initShaders();
  initBuildTextures();
  initSounds();
  initBuildCaela();
  initBuildTerrae();
  initBuildBSPs();
  initBuildModels();
  initMusic();
  initNameLists();
  initFragPools();
  initClasses();

  log.unindent();
  log.println( "}" );
}

void Library::free()
{
  shaders.clear();
  shaders.dealloc();
  textures.clear();
  textures.dealloc();
  sounds.clear();
  sounds.dealloc();
  caela.clear();
  caela.dealloc();
  terrae.clear();
  terrae.dealloc();
  models.clear();
  models.dealloc();
  nameLists.clear();
  nameLists.dealloc();
  musics.clear();
  musics.dealloc();

  shaderIndices.clear();
  shaderIndices.dealloc();
  textureIndices.clear();
  textureIndices.dealloc();
  soundIndices.clear();
  soundIndices.dealloc();
  caelumIndices.clear();
  caelumIndices.dealloc();
  terraIndices.clear();
  terraIndices.dealloc();
  modelIndices.clear();
  modelIndices.dealloc();
  nameListIndices.clear();
  nameListIndices.dealloc();

  deviceIndices.clear();
  deviceIndices.dealloc();
  imagoIndices.clear();
  imagoIndices.dealloc();
  audioIndices.clear();
  audioIndices.dealloc();

  baseClasses.clear();
  baseClasses.dealloc();
  objClasses.free();
  objClasses.dealloc();
  bsps.clear();
  bsps.dealloc();
  fragPools.clear();
  fragPools.dealloc();
}

}
}
