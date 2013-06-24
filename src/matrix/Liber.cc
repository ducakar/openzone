/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 */

/**
 * @file matrix/Library.cc
 *
 * Map of all resources, object types, scripts etc.
 */

#include <matrix/Liber.hh>

#include <matrix/Vehicle.hh>

#define OZ_REGISTER_BASECLASS( name ) \
  baseClasses.include( #name, name##Class::createClass )

namespace oz
{
namespace matrix
{

Liber::Resource::Resource( const String& name_, const String& path_ ) :
  name( name_ ), path( path_ )
{}

const FragPool* Liber::fragPool( const char* name ) const
{
  const FragPool* value = fragPools.find( name );

  if( value == nullptr ) {
    OZ_ERROR( "Invalid fragment pool requested '%s'", name );
  }
  return value;
}

const ObjectClass* Liber::objClass( const char* name ) const
{
  const ObjectClass* const* value = objClasses.find( name );

  if( value == nullptr ) {
    OZ_ERROR( "Invalid object class requested '%s'", name );
  }
  return *value;
}

const BSP* Liber::bsp( const char* name ) const
{
  const BSP* value = bsps.find( name );

  if( value == nullptr ) {
    OZ_ERROR( "Invalid BSP requested '%s'", name );
  }
  return value;
}

int Liber::shaderIndex( const char* name ) const
{
  const int* value = shaderIndices.find( name );

  if( value == nullptr ) {
    OZ_ERROR( "Invalid shader requested '%s'", name );
  }
  return *value;
}

int Liber::textureIndex( const char* name ) const
{
  const int* value = textureIndices.find( name );

  if( value == nullptr ) {
    OZ_ERROR( "Invalid texture requested '%s'", name );
  }
  return *value;
}

int Liber::soundIndex( const char* name ) const
{
  const int* value = soundIndices.find( name );

  if( value == nullptr ) {
    OZ_ERROR( "Invalid sound requested '%s'", name );
  }
  return *value;
}

int Liber::caelumIndex( const char* name ) const
{
  const int* value = caelumIndices.find( name );

  if( value == nullptr ) {
    OZ_ERROR( "Invalid caelum index requested '%s'", name );
  }
  return *value;
}

int Liber::terraIndex( const char* name ) const
{
  const int* value = terraIndices.find( name );

  if( value == nullptr ) {
    OZ_ERROR( "Invalid terra index requested '%s'", name );
  }
  return *value;
}

int Liber::modelIndex( const char* name ) const
{
  const int* value = modelIndices.find( name );

  if( value == nullptr ) {
    OZ_ERROR( "Invalid model index requested '%s'", name );
  }
  return *value;
}

int Liber::nameListIndex( const char* name ) const
{
  const int* value = nameListIndices.find( name );

  if( value == nullptr ) {
    OZ_ERROR( "Invalid name list index requested '%s'", name );
  }
  return *value;
}

int Liber::musicTrackIndex( const char* name ) const
{
  const int* value = musicTrackIndices.find( name );

  if( value == nullptr ) {
    OZ_ERROR( "Invalid music track index requested '%s'", name );
  }
  return *value;
}

int Liber::deviceIndex( const char* name ) const
{
  const int* value = deviceIndices.find( name );

  return value == nullptr ? -1 : *value;
}

int Liber::imagoIndex( const char* name ) const
{
  const int* value = imagoIndices.find( name );

  return value == nullptr ? -1 : *value;
}

int Liber::audioIndex( const char* name ) const
{
  const int* value = audioIndices.find( name );

  return value == nullptr ? -1 : *value;
}

void Liber::freeBSPs()
{
  foreach( bsp, bsps.iter() ) {
    if( bsp->value.nUsers != 0 ) {
      bsp->value.unload();
      bsp->value.nUsers = 0;
    }
  }
}

void Liber::initShaders()
{
  Log::println( "Shader programs (*.json in 'glsl') {" );
  Log::indent();

  List<Resource> shadersList;

  File dir( "@glsl" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "json" ) ) {
      continue;
    }

    String name = file->baseName();

    Log::println( "%s", name.cstr() );

    shaderIndices.add( name, shadersList.length() );
    shadersList.add( Resource( name, "" ) );
  }

  shaders.resize( shadersList.length() );
  aMove<Resource>( shaders.begin(), shadersList.begin(), shadersList.length() );

  Log::unindent();
  Log::println( "}" );
}

void Liber::initTextures()
{
  Log::println( "Textures (*.ozcTex in 'tex/*') {" );
  Log::indent();

  List<Resource> texturesList;

  File dir( "@tex" );
  DArray<File> dirList = dir.ls();

  foreach( subDir, dirList.iter() ) {
    if( subDir->type() != File::DIRECTORY ) {
      continue;
    }

    DArray<File> subDirList = subDir->ls();

    foreach( file, subDirList.citer() ) {
      if( !file->hasExtension( "ozcTex" ) ) {
        continue;
      }

      String name = subDir->name() + "/" + file->baseName();

      Log::println( "%s", name.cstr() );

      textureIndices.add( name, texturesList.length() );
      texturesList.add( Resource( name, file->path() ) );
    }
  }

  textures.resize( texturesList.length() );
  aMove<Resource>( textures.begin(), texturesList.begin(), texturesList.length() );

  Log::unindent();
  Log::println( "}" );
}

void Liber::initSounds()
{
  Log::println( "Sounds (*.wav, *.oga, *.ogg in 'snd') {" );
  Log::indent();

  List<Resource> soundsList;

  File dir( "@snd" );
  DArray<File> dirList = dir.ls();

  foreach( subDir, dirList.iter() ) {
    if( subDir->type() != File::DIRECTORY ) {
      continue;
    }

    DArray<File> subDirList = subDir->ls();

    foreach( file, subDirList.citer() ) {
      if( !file->hasExtension( "wav" ) && !file->hasExtension( "oga" ) &&
          !file->hasExtension( "ogg" ) )
      {
        continue;
      }

      String name = subDir->name() + "/" + file->baseName();

      Log::println( "%s", name.cstr() );

      soundIndices.add( name, soundsList.length() );
      soundsList.add( Resource( name, file->path() ) );
    }
  }

  sounds.resize( soundsList.length() );
  aMove<Resource>( sounds.begin(), soundsList.begin(), soundsList.length() );

  Log::unindent();
  Log::println( "}" );
}

void Liber::initCaela()
{
  Log::println( "Caela (*.ozcCaelum in 'caelum') {" );
  Log::indent();

  List<Resource> caelaList;

  File dir( "@caelum" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "ozcCaelum" ) ) {
      continue;
    }

    String name = file->baseName();

    Log::println( "%s", name.cstr() );

    caelumIndices.add( name, caelaList.length() );
    caelaList.add( Resource( name, file->path() ) );
  }

  caela.resize( caelaList.length() );
  aMove<Resource>( caela.begin(), caelaList.begin(), caelaList.length() );

  Log::unindent();
  Log::println( "}" );
}

void Liber::initTerrae()
{
  Log::println( "Terrae (*.ozTerra, *.ozcTerra in 'terra') {" );
  Log::indent();

  List<Resource> terraeList;

  File dir( "@terra" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "ozTerra" ) ) {
      continue;
    }

    String name = file->baseName();

    Log::println( "%s", name.cstr() );

    terraIndices.add( name, terraeList.length() );
    terraeList.add( Resource( name, file->path() ) );
  }

  terrae.resize( terraeList.length() );
  aMove<Resource>( terrae.begin(), terraeList.begin(), terraeList.length() );

  Log::unindent();
  Log::println( "}" );
}

void Liber::initModels()
{
  Log::println( "Models (directories in 'mdl') {" );
  Log::indent();

  List<Resource> modelsList;

  File dir( "@mdl" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.iter() ) {
    if( file->type() != File::DIRECTORY ) {
      continue;
    }

    String name = file->name();
    String path;

    if( File( file->path() + "/data.ozcSMM" ).type() == File::REGULAR ) {
      path = file->path() + "/data.ozcSMM";
    }
    else if( File( file->path() + "/data.ozcMD2" ).type() == File::REGULAR ) {
      path = file->path() + "/data.ozcMD2";
    }
    else if( File( file->path() + "/data.ozcMD3" ).type() == File::REGULAR ) {
      path = file->path() + "/data.ozcMD3";
    }
    else {
      OZ_ERROR( "Invalid model '%s'", name.cstr() );
    }

    Log::println( "%s", name.cstr() );

    if( modelIndices.contains( name ) ) {
      OZ_ERROR( "Duplicated model '%s'", name.cstr() );
    }

    modelIndices.add( name, modelsList.length() );
    modelsList.add( Resource( name, path ) );
  }

  models.resize( modelsList.length() );
  aMove<Resource>( models.begin(), modelsList.begin(), modelsList.length() );

  Log::unindent();
  Log::println( "}" );
}

void Liber::initNameLists()
{
  Log::println( "Name lists (*.txt in 'name') {" );
  Log::indent();

  List<Resource> nameListsList;

  File dir( "@name" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "txt" ) ) {
      continue;
    }

    String name = file->baseName();

    Log::println( "%s", name.cstr() );

    nameListIndices.add( name, nameListsList.length() );
    nameListsList.add( Resource( name, file->path() ) );
  }

  nameLists.resize( nameListsList.length() );
  aMove<Resource>( nameLists.begin(), nameListsList.begin(), nameListsList.length() );

  Log::unindent();
  Log::println( "}" );
}

void Liber::initFragPools()
{
  Log::println( "Fragment pools (*.ozFragPools in 'frag') {" );
  Log::indent();

  File dir( "@frag" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.iter() ) {
    if( !file->hasExtension( "ozFragPools" ) ) {
      continue;
    }

    Buffer buffer = file->read();
    if( buffer.isEmpty() ) {
      OZ_ERROR( "Failed to read '%s'", file->path().cstr() );
    }

    InputStream is = buffer.inputStream();

    while( is.isAvailable() ) {
      const char* name = is.readString();

      Log::println( "%s", name );

      fragPools.add( name, FragPool( &is, name, fragPools.length() ) );
    }
  }

  nFragPools = fragPools.length();

  Log::unindent();
  Log::println( "}" );
}

void Liber::initClasses()
{
  OZ_REGISTER_BASECLASS( Object );
  OZ_REGISTER_BASECLASS( Dynamic );
  OZ_REGISTER_BASECLASS( Weapon );
  OZ_REGISTER_BASECLASS( Bot );
  OZ_REGISTER_BASECLASS( Vehicle );

  Log::println( "Object classes (*.ozClasses in 'class') {" );
  Log::indent();

  File dir( "@class" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.iter() ) {
    if( !file->hasExtension( "ozClasses" ) ) {
      continue;
    }

    Buffer buffer = file->read();
    if( buffer.isEmpty() ) {
      OZ_ERROR( "Failed to read '%s'", file->path().cstr() );
    }

    InputStream is = buffer.inputStream();

    int nClasses  = is.readInt();
    int nDevices  = is.readInt();
    int nImagines = is.readInt();
    int nAudios   = is.readInt();

    for( int i = 0; i < nClasses; ++i ) {
      const char* name = is.readString();
      const char* base = is.readString();

      if( objClasses.contains( name ) ) {
        OZ_ERROR( "Duplicated class '%s'", name );
      }

      if( String::isEmpty( base ) ) {
        OZ_ERROR( "%s: 'base' missing in class description", name );
      }

      ObjectClass::CreateFunc* const* createFunc = baseClasses.find( base );
      if( createFunc == nullptr ) {
        OZ_ERROR( "%s: Invalid class base '%s'", name, base );
      }

      // First we only add class instances, we don't initialise them as each class may have
      // references to other classes that have not been created yet.
      objClasses.add( name, ( *createFunc )() );
    }

    // Map integer indices to all referenced Device, Imago and Audio classes.
    for( int i = 0; i < nDevices; ++i ) {
      const char* sDevice = is.readString();

      if( !String::isEmpty( sDevice ) ) {
        deviceIndices.include( sDevice, deviceIndices.length() );
      }
    }

    for( int i = 0; i < nImagines; ++i ) {
      const char* sImago = is.readString();

      if( !String::isEmpty( sImago ) ) {
        imagoIndices.include( sImago, imagoIndices.length() );
      }
    }

    for( int i = 0; i < nAudios; ++i ) {
      const char* sAudio = is.readString();

      if( !String::isEmpty( sAudio ) ) {
        audioIndices.include( sAudio, audioIndices.length() );
      }
    }
  }

  nDeviceClasses = deviceIndices.length();
  nImagoClasses  = imagoIndices.length();
  nAudioClasses  = audioIndices.length();

  // Initialise all classes.
  foreach( file, dirList.iter() ) {
    if( file->type() != File::REGULAR || !file->hasExtension( "ozClasses" ) ) {
      continue;
    }

    Buffer buffer = file->read();
    InputStream is = buffer.inputStream();

    int nClasses  = is.readInt();
    int nDevices  = is.readInt();
    int nImagines = is.readInt();
    int nAudios   = is.readInt();

    int nStrings  = nClasses * 2 + nDevices + nImagines + nAudios;

    // Forward to class data.
    for( int i = 0; i < nStrings; ++i ) {
      is.readString();
    }

    for( int i = 0; i < nClasses; ++i ) {
      const char* name = is.readString();

      Log::println( "%s", name );

      ObjectClass* const* clazz = objClasses.find( name );
      if( clazz == nullptr ) {
        OZ_ERROR( "Class '%s' body missing in corrupted class file '%s'",
                  name, file->path().cstr() );
      }

      ( *clazz )->init( &is, name );
    }
  }

  foreach( classIter, objClasses.citer() ) {
    ObjectClass* objClazz = classIter->value;

    // check that all items are valid
    for( int i = 0; i < objClazz->defaultItems.length(); ++i ) {
      const ObjectClass* itemClazz = objClazz->defaultItems[i];

      if( ( itemClazz->flags & ( Object::DYNAMIC_BIT | Object::ITEM_BIT ) ) !=
          ( Object::DYNAMIC_BIT | Object::ITEM_BIT ) )
      {
        OZ_ERROR( "Invalid item class '%s' in '%s', must be dynamic and have item flag",
                  itemClazz->name.cstr(), objClazz->name.cstr() );
      }
    }

    // check if weaponItem is a valid weapon for bots
    if( objClazz->flags & Object::BOT_BIT ) {
      const BotClass* botClazz = static_cast<const BotClass*>( objClazz );

      if( botClazz->weaponItem >= 0 ) {
        if( uint( botClazz->weaponItem ) >= uint( botClazz->defaultItems.length() ) ) {
          OZ_ERROR( "Invalid weaponItem index for '%s'", botClazz->name.cstr() );
        }

        // we already checked it in the previous loop it's non-nullptr and a valid item
        const ObjectClass* itemClazz = botClazz->defaultItems[botClazz->weaponItem];

        if( !( itemClazz->flags & Object::WEAPON_BIT ) ) {
          OZ_ERROR( "Default weapon of '%s' is of a non-weapon class", botClazz->name.cstr() );
        }

        const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( itemClazz );

        if( !botClazz->name.beginsWith( weaponClazz->userBase ) ) {
          OZ_ERROR( "Default weapon of '%s' is not allowed for this bot class",
                    botClazz->name.cstr() );
        }
      }
    }
  }

  Log::unindent();
  Log::println( "}" );
}

void Liber::initBSPs()
{
  Log::println( "BSP structures (*.ozBSP, *.ozcBSP in 'bsp') {" );
  Log::indent();

  File dir( "@bsp" );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "ozBSP" ) ) {
      continue;
    }

    String name = file->baseName();

    Log::println( "%s", name.cstr() );

    bsps.add( name, BSP( name, bsps.length() ) );
  }

  nBSPs = bsps.length();

  Log::unindent();
  Log::println( "}" );
}

void Liber::initMusicRecurse( const char* path, List<Resource>* musicTracksList )
{
  File dir( path );
  DArray<File> dirList = dir.ls();

  foreach( file, dirList.iter() ) {
    if( file->type() == File::DIRECTORY ) {
      initMusicRecurse( file->path(), musicTracksList );
    }
    if( file->hasExtension( "oga" ) || file->hasExtension( "ogg" ) ||
        ( mapMP3s && file->hasExtension( "mp3" ) ) || ( mapAACs && file->hasExtension( "aac" ) ) )
    {
      Log::println( "%s", file->path().cstr() );

      musicTracksList->add( Resource( file->baseName(), file->path() ) );
    }
  }
}

void Liber::initMusic( const char* userMusicPath )
{
  if( userMusicPath == nullptr || String::isEmpty( userMusicPath ) ) {
    Log::println( "Music (*.oga, *.ogg%s%s in 'music') {",
                  mapMP3s ? ", *.mp3" : "", mapAACs ? "*.aac" : "" );
  }
  else {
    Log::println( "Music (*.oga, *.ogg%s%s in 'music' and '%s') {",
                  mapMP3s ? ", *.mp3" : "", mapAACs ? ", *.aac" : "", userMusicPath );
  }
  Log::indent();

  List<Resource> musicTracksList;

  initMusicRecurse( "@music", &musicTracksList );

  for( int i = 0; i < musicTracksList.length(); ++i ) {
    musicTrackIndices.add( musicTracksList[i].name, i );
  }

  initMusicRecurse( "@userMusic", &musicTracksList );

  musicTracks.resize( musicTracksList.length() );
  aMove<Resource>( musicTracks.begin(), musicTracksList.begin(), musicTracksList.length() );

  Log::unindent();
  Log::println( "}" );
}

void Liber::init( const char* userMusicPath )
{
  Log::println( "Initialising Library {" );
  Log::indent();

  Log::verboseMode = true;

  Log::println( "Mapping resources {" );
  Log::indent();

  initShaders();
  initTextures();
  initSounds();
  initCaela();
  initTerrae();
  initModels();
  initNameLists();
  initFragPools();
  initClasses();
  initBSPs();
  initMusic( userMusicPath );

  Log::unindent();
  Log::println( "}" );

  Log::verboseMode = false;

  Log::println( "Summary {" );
  Log::indent();

  Log::println( "%5d  shaders", shaders.length() );
  Log::println( "%5d  textures", textures.length() );
  Log::println( "%5d  sounds", sounds.length() );
  Log::println( "%5d  caela", caela.length() );
  Log::println( "%5d  terrae", terrae.length() );
  Log::println( "%5d  BSPs", nBSPs );
  Log::println( "%5d  models", models.length() );
  Log::println( "%5d  music tracks", musicTracks.length() );
  Log::println( "%5d  name lists", nameLists.length() );
  Log::println( "%5d  fragment pools", fragPools.length() );
  Log::println( "%5d  object classes", objClasses.length() );

  Log::unindent();
  Log::println( "}" );

  Log::unindent();
  Log::println( "}" );
}

void Liber::destroy()
{
  shaders.clear();
  textures.clear();
  sounds.clear();
  caela.clear();
  terrae.clear();
  models.clear();
  nameLists.clear();
  musicTracks.clear();

  shaderIndices.clear();
  shaderIndices.deallocate();
  textureIndices.clear();
  textureIndices.deallocate();
  soundIndices.clear();
  soundIndices.deallocate();
  caelumIndices.clear();
  caelumIndices.deallocate();
  terraIndices.clear();
  terraIndices.deallocate();
  modelIndices.clear();
  modelIndices.deallocate();
  nameListIndices.clear();
  nameListIndices.deallocate();
  musicTrackIndices.clear();
  musicTrackIndices.deallocate();

  deviceIndices.clear();
  deviceIndices.deallocate();
  imagoIndices.clear();
  imagoIndices.deallocate();
  audioIndices.clear();
  audioIndices.deallocate();

  bsps.clear();
  bsps.deallocate();
  baseClasses.clear();
  baseClasses.deallocate();
  objClasses.free();
  objClasses.deallocate();
  fragPools.clear();
  fragPools.deallocate();
}

Liber liber;

}
}
