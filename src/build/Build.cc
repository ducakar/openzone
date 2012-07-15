/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file build/Build.cc
 *
 * Data builder.
 */

#include "stable.hh"

#include "build/Build.hh"

#include "BuildInfo.hh"

#include "client/Window.hh"
#include "client/Render.hh"

#include "build/Lingua.hh"
#include "build/Context.hh"
#include "build/Compiler.hh"
#include "build/UI.hh"
#include "build/Caelum.hh"
#include "build/Terra.hh"
#include "build/BSP.hh"
#include "build/Class.hh"
#include "build/FragPool.hh"
#include "build/OBJ.hh"
#include "build/MD2.hh"
#include "build/MD3.hh"

#include <unistd.h>

#include <SDL/SDL_main.h>
#include <FreeImage.h>

namespace oz
{
namespace build
{

Build build;

void Build::printUsage( const char* invocationName )
{
  Log::printRaw(
    "Usage:\n"
    "  %s [OPTIONS] <src_dir> [<out_dir>]\n"
    "\n"
    "  <src_dir>  Path to directory that includes data to be built. Name of this\n"
    "             directory name is used as package name.\n"
    "  <out_dir>  Directory where output directory and archive will be created.\n"
    "             Defaults to './share/" OZ_APPLICATION_NAME "'.\n"
    "  -v         More verbose log output.\n"
    "  -l         Build translations.\n"
    "  -u         Build UI.\n"
    "  -g         Copy shaders.\n"
    "  -c         Build caela (skies).\n"
    "  -t         Build terrae (terrains).\n"
    "  -b         Compile maps into BSPs and build BPSs with referenced textures.\n"
    "  -m         Build models.\n"
    "  -s         Copy sounds (only used ones).\n"
    "  -a         Copy object class definitions.\n"
    "  -f         Copy fragment pool definitions.\n"
    "  -n         Copy name lists.\n"
    "  -x         Check and copy Lua scripts.\n"
    "  -o         Build modules.\n"
    "  -r         Copy music tracks.\n"
    "  -p         Pack built files into ZIP archive.\n"
    "  -A         Everything above.\n"
    "  -B         Build with bumpmap vertex format.\n"
    "  -C         Use S3 texture compression.\n"
    "  -0         Use no compression for ZIP archive.\n"
    "  -7         Create non-solid LZMA-compressed 7zip archive instead of ZIP.\n"
    "\n",
    invocationName );
}

void Build::copyFiles( const char* srcDir, const char* destDir, const char* ext, bool recurse )
{
  String sSrcDir = srcDir;
  String sDestDir = destDir;
  PFile dir( sSrcDir );
  DArray<PFile> dirList = dir.ls();

  if( dirList.isEmpty() ) {
    return;
  }

  Log::println( "Copying '%s/*.%s' -> '%s' {", srcDir, ext, destDir );
  Log::indent();

  File::mkdir( destDir );

  sSrcDir  = sSrcDir + "/";
  sDestDir = sDestDir + "/";

  foreach( file, dirList.iter() ) {
    String fileName = file->name();

    file->stat();

    if( file->type() == File::DIRECTORY ) {
      if( recurse ) {
        copyFiles( srcDir + ( "/" + file->name() ), destDir + ( "/" + file->name() ), ext, true );
      }
    }
    else if( file->hasExtension( ext ) || fileName.beginsWith( "README" ) ||
             fileName.beginsWith( "COPYING" ) )
    {
      Log::print( "Copying '%s' ...", fileName.cstr() );

      if( !file->map() ) {
        throw Exception( "Failed to copy '%s'", file->path().cstr() );
      }

      InputStream is = file->inputStream();
      File destFile( String::str( "%s/%s", destDir, fileName.cstr() ) );

      if( !destFile.write( is.begin(), is.capacity() ) ) {
        throw Exception( "Failed to copy '%s'", file->path().cstr() );
      }

      file->unmap();

      Log::printEnd( " OK" );
      continue;
    }
  }

  Log::unindent();
  Log::println( "}" );
}

void Build::buildCaela()
{
  Log::println( "Building Caela {" );
  Log::indent();

  String srcDir = "caelum";
  PFile dir( srcDir );
  DArray<PFile> dirList = dir.ls();

  srcDir = srcDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "json" ) ) {
      continue;
    }

    String name = file->baseName();

    File::mkdir( "caelum" );
    Caelum::build( name );
  }

  Log::unindent();
  Log::println( "}" );
}

void Build::buildTerrae()
{
  Log::println( "Building Terrae {" );
  Log::indent();

  String srcDir = "terra";
  PFile dir( srcDir );
  DArray<PFile> dirList = dir.ls();

  srcDir = srcDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "json" ) ) {
      continue;
    }

    File::mkdir( "terra" );
    Terra::build( file->baseName() );
  }

  Log::unindent();
  Log::println( "}" );
}

void Build::buildBSPs()
{
  Log::println( "Building BSPs {" );
  Log::indent();

  String srcDir = "baseq3/maps";
  String destDir = "bsp";
  PFile dir( srcDir );
  DArray<PFile> dirList = dir.ls();

  srcDir = srcDir + "/";
  destDir = destDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "json" ) ) {
      continue;
    }

    File::mkdir( "bsp" );
    BSP::build( file->baseName() );
  }

  Log::unindent();
  Log::println( "}" );
}

void Build::buildBSPTextures()
{
  if( context.usedTextures.isEmpty() ) {
    return;
  }

  Log::println( "Building used BSP textures {" );
  Log::indent();

  Map<String> usedDirs;

  PFile dir( "baseq3/textures" );
  DArray<PFile> dirList = dir.ls();

  foreach( subDir, dirList.iter() ) {
    subDir->stat();

    if( subDir->type() != File::DIRECTORY ) {
      continue;
    }

    DArray<PFile> texList = subDir->ls();

    foreach( file, texList.citer() ) {
      String name = file->name();
      String path = file->path();

      int dot   = path.lastIndex( '.' );
      int slash = path.lastIndex( '/' );

      if( slash >= dot ) {
        continue;
      }

      hard_assert( slash > 15 );

      // strlen( "baseq3/textures/" ) == 16
      name = path.substring( 16, dot );
      path = path.substring( 0, dot );

      if( name.endsWith( "_m" ) || name.endsWith( "_n" ) ) {
        continue;
      }

      if( !context.usedTextures.contains( name ) ) {
        continue;
      }

      Log::println( "Building texture '%s' {", name.cstr() );
      Log::indent();

      usedDirs.include( subDir->path() );

      File::mkdir( "tex" );
      File::mkdir( "tex/" + subDir->name() );

      File destFile( String::str( "tex/%s.ozcTex", name.cstr() ) );

      uint diffuseId, masksId, normalsId;
      context.loadTexture( &diffuseId, &masksId, &normalsId, path );

      BufferStream os;

      Log::println( "Compiling into '%s'", destFile.path().cstr() );

      int textureFlags = Mesh::DIFFUSE_BIT;

      if( masksId != 0 ) {
        textureFlags |= Mesh::MASKS_BIT;
      }
      if( normalsId != 0 ) {
        textureFlags |= Mesh::NORMALS_BIT;
      }

      os.writeInt( textureFlags );

      context.writeLayer( diffuseId, &os );
      glDeleteTextures( 1, &diffuseId );

      if( masksId != 0 ) {
        context.writeLayer( masksId, &os );
        glDeleteTextures( 1, &masksId );
      }
      if( normalsId != 0 ) {
        context.writeLayer( normalsId, &os );
        glDeleteTextures( 1, &normalsId );
      }

      if( !destFile.write( os.begin(), os.length() ) ) {
        throw Exception( "Failed to write texture '%s'", destFile.path().cstr() );
      }

      Log::unindent();
      Log::println( "}" );
    }
  }

  foreach( subDirPath, usedDirs.citer() ) {
    PFile subDir( *subDirPath );

    DArray<PFile> texList = subDir.ls();

    foreach( file, texList.iter() ) {
      String name = file->name();
      String path = file->path();

      if( name.beginsWith( "COPYING" ) || name.beginsWith( "README" ) ) {
        Log::print( "Copying '%s' ...", path.cstr() );

        if( !file->map() ) {
          throw Exception( "Failed to read '%s'", file->path().cstr() );
        }

        InputStream is = file->inputStream();
        File destFile( String::str( "tex/%s/%s", subDir.name().cstr(), name.cstr() ) );

        File::mkdir( "tex" );
        File::mkdir( "tex/" + subDir.name() );

        if( !destFile.write( is.begin(), is.capacity() ) ) {
          throw Exception( "Failed to write '%s'", destFile.path().cstr() );
        }

        file->unmap();

        Log::printEnd( " OK" );
        continue;
      }
    }
  }

  Log::unindent();
  Log::println( "}" );
}

void Build::buildClasses( const String& pkgName )
{
  Log::println( "Building object classes {" );
  Log::indent();

  String dirName = "class";
  PFile dir( dirName );
  DArray<PFile> dirList = dir.ls();

  BufferStream os;

  foreach( file, dirList.iter() ) {
    if( !file->hasExtension( "json" ) ) {
      continue;
    }

    String name = file->baseName();

    Log::print( "%s ...", name.cstr() );

    clazz.build( &os, name );

    Log::printEnd( " OK" );
  }

  if( !clazz.names.isEmpty() ) {
    BufferStream headerStream;

    headerStream.writeInt( clazz.names.length() );
    headerStream.writeInt( clazz.devices.length() );
    headerStream.writeInt( clazz.imagines.length() );
    headerStream.writeInt( clazz.audios.length() );

    for( int i = 0; i < clazz.names.length(); ++i ) {
      headerStream.writeString( clazz.names[i] );
      headerStream.writeString( clazz.bases[i] );
    }
    foreach( device, clazz.devices.citer() ) {
      headerStream.writeString( *device );
    }
    foreach( imago, clazz.imagines.citer() ) {
      headerStream.writeString( *imago );
    }
    foreach( audio, clazz.audios.citer() ) {
      headerStream.writeString( *audio );
    }

    int headerSize = headerStream.length();
    int bodySize   = os.length();

    os.forward( headerSize );
    memmove( os.begin() + headerSize, os.begin(), size_t( bodySize ) );
    memcpy( os.begin(), headerStream.begin(), size_t( headerSize ) );

    headerStream.dealloc();

    File::mkdir( "class" );
    File outFile( "class/" + pkgName + ".ozClasses" );

    Log::print( "Writing to '%s' ...", outFile.path().cstr() );

    if( !outFile.write( os.begin(), os.length() ) ) {
      throw Exception( "Failed to write object class file '%s'", outFile.path().cstr() );
    }

    Log::printEnd( " OK" );
  }

  clazz.free();

  Log::unindent();
  Log::println( "}" );
}

void Build::buildFragPools( const String& pkgName )
{
  Log::println( "Building fragment pools {" );
  Log::indent();

  String dirName = "frag";
  PFile dir( dirName );
  DArray<PFile> dirList = dir.ls();

  BufferStream os;

  foreach( file, dirList.iter() ) {
    if( !file->hasExtension( "json" ) ) {
      continue;
    }

    String name = file->baseName();

    Log::print( "%s ...", name.cstr() );

    fragPool.build( &os, name );

    Log::printEnd( " OK" );
  }

  if( os.length() != 0 ) {
    File::mkdir( "frag" );
    File outFile( "frag/" + pkgName + ".ozFragPools" );

    Log::print( "Writing to '%s' ...", outFile.path().cstr() );

    if( !outFile.write( os.begin(), os.length() ) ) {
      throw Exception( "Failed to write fragment pool file '%s'", outFile.path().cstr() );
    }

    Log::printEnd( " OK" );
  }

  clazz.free();

  Log::unindent();
  Log::println( "}" );
}

void Build::buildModels()
{
  if( context.usedModels.isEmpty() ) {
    return;
  }

  Log::println( "Building used models {" );
  Log::indent();

  PFile mdlDir( "mdl" );
  File::mkdir( mdlDir.path() );
  DArray<PFile> dirList = mdlDir.ls();

  foreach( dir, dirList.iter() ) {
    if( !context.usedModels.contains( dir->name() ) ) {
      continue;
    }

    String path = dir->path();
    File::mkdir( path );
    DArray<PFile> fileList = dir->ls();

    foreach( file, fileList.iter() ) {
      String name = file->name();
      String path = file->path();

      if( name.beginsWith( "COPYING" ) || name.beginsWith( "README" ) ) {
        Log::print( "Copying '%s' ...", path.cstr() );

        if( !file->map() ) {
          throw Exception( "Failed to read '%s'", file->path().cstr() );
        }

        InputStream is = file->inputStream();
        File destFile( path );

        if( !destFile.write( is.begin(), is.length() ) ) {
          throw Exception( "Failed to write '%s'", destFile.path().cstr() );
        }

        file->unmap();

        Log::printEnd( " OK" );
        continue;
      }
    }

    if( PFile( path + "/data.obj" ).stat() ) {
      OBJ::build( path );
    }
    else if( PFile( path + "/tris.md2" ).stat() ) {
      MD2::build( path );
    }
    else {
      MD3::build( path );
    }
  }

  Log::unindent();
  Log::println( "}" );
}

void Build::copySounds()
{
  if( context.usedSounds.isEmpty() ) {
    return;
  }

  Log::println( "Copying used sounds {" );
  Log::indent();

  Map<String> usedDirs;

  PFile dir( "snd" );
  DArray<PFile> dirList = dir.ls();

  foreach( subDir, dirList.iter() ) {
    subDir->stat();

    if( subDir->type() != File::DIRECTORY ) {
      continue;
    }

    DArray<PFile> sndList = subDir->ls();

    foreach( file, sndList.iter() ) {
      String name = file->name();
      String path = file->path();

      int dot   = path.lastIndex( '.' );
      int slash = path.lastIndex( '/' );

      if( slash < 0 || slash >= dot ) {
        continue;
      }

      hard_assert( slash > 3 );

      name = path.substring( 4, dot );

      if( !context.usedSounds.contains( name ) ) {
        continue;
      }

      Log::print( "Copying '%s' ...", name.cstr() );

      usedDirs.include( subDir->path() );

      File::mkdir( "snd" );
      File::mkdir( "snd/" + subDir->name() );

      if( !file->map() ) {
        throw Exception( "Failed to copy '%s'", file->path().cstr() );
      }

      InputStream is = file->inputStream();

      File destFile( file->path() );

      if( !destFile.write( is.begin(), is.capacity() ) ) {
        throw Exception( "Failed to write '%s'", destFile.path().cstr() );
      }

      file->unmap();

      Log::printEnd( " OK" );
    }
  }

  foreach( subDirPath, usedDirs.citer() ) {
    PFile subDir( *subDirPath );
    DArray<PFile> texList = subDir.ls();

    foreach( file, texList.iter() ) {
      String name = file->name();
      String path = file->path();

      if( name.beginsWith( "COPYING" ) || name.beginsWith( "README" ) ) {
        Log::print( "Copying '%s' ...", path.cstr() );

        if( !file->map() ) {
          throw Exception( "Failed to read '%s'", file->path().cstr() );
        }

        InputStream is = file->inputStream();
        File destFile( path );

        File::mkdir( "snd" );
        File::mkdir( "snd/" + subDir.name() );

        if( !destFile.write( is.begin(), is.capacity() ) ) {
          throw Exception( "Failed to write '%s'", destFile.path().cstr() );
        }

        file->unmap();

        Log::printEnd( " OK" );
        continue;
      }
    }
  }

  Log::unindent();
  Log::println( "}" );
}

void Build::buildModules()
{
  Log::println( "Building Modules {" );
  Log::indent();

  Log::unindent();
  Log::println( "}" );
}

void Build::checkLua( const char* path )
{
  Log::println( "Checking Lua scripts '%s' {", path );
  Log::indent();

  String srcDir = String::str( "%s/", path );
  PFile dir( path );
  DArray<PFile> dirList = dir.ls();

  String sources;

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "lua" ) ) {
      continue;
    }

    String cmdLine = "luac -p " + file->realDir() + "/" + file->path();

    Log::println( "%s", cmdLine.cstr() );
    if( system( cmdLine ) != 0 ) {
      throw Exception( "Lua syntax check failed" );
    }
  }

  Log::unindent();
  Log::println( "}" );
}

void Build::buildMissions()
{
  Log::println( "Building missions {" );
  Log::indent();

  DArray<PFile> missions = PFile( "mission" ).ls();
  foreach( mission, missions.citer() ) {
    checkLua( mission->path() );

    copyFiles( mission->path(), mission->path(), "lua", false );
    copyFiles( mission->path(), mission->path(), "json", false );

    PFile srcFile( mission->path() + "/description.png" );
    File outFile( mission->path() + "/description.ozThumbnail" );

    if( !srcFile.stat() ) {
      continue;
    }

    Log::println( "Building thumbnail {" );
    Log::indent();

    uint imageId = context.loadLayer( srcFile.path(), false, GL_LINEAR, GL_LINEAR );

    BufferStream os;
    context.writeLayer( imageId, &os );

    glDeleteTextures( 1, &imageId );

    if( !outFile.write( os.begin(), os.length() ) ) {
      throw Exception( "Failed to write '%s'", outFile.path().cstr() );
    }

    Log::unindent();
    Log::println( "}" );
  }

  lingua.buildMissions();

  Log::unindent();
  Log::println( "}" );
}

void Build::packArchive( const char* name, bool useCompression, bool use7zip )
{
  Log::println( "Packing archive {" );
  Log::indent();

  File archive( String::str( "../%s.%s", name, use7zip ? "7z" : "zip" ) );

  String cmdLine = use7zip ?
                   String::str( "7z u -ms=off %s '%s' *",
                                useCompression ? "-mx=9" : "-m0=copy",
                                archive.path().cstr() ) :
                   String::str( "zip -ur %s '%s' *",
                                useCompression ? "-9" : "-Z store",
                                archive.path().cstr() );

  Log::println( "%s", cmdLine.cstr() );
  Log::println();

  if( system( cmdLine ) != 0 ) {
    throw Exception( use7zip ? "Packing 7zip archive failed" : "Packing ZIP archive failed" );
  }

  archive.stat();
  int size = archive.size();

  if( size >= 0 ) {
    Log::println();
    Log::println( "Archive size: %.2f MiB = %.2f MB",
                  float( size ) / ( 1024.0f * 1024.0f ),
                  float( size ) / ( 1000.0f * 1000.0f ) );
  }

  Log::unindent();
  Log::println( "}" );
}

int Build::main( int argc, char** argv )
{
  String invocationName = File( argv[0] ).baseName();

  config.setObject();

  bool doCat          = false;
  bool doUI           = false;
  bool doShaders      = false;
  bool doCaela        = false;
  bool doTerrae       = false;
  bool doBSPs         = false;
  bool doModels       = false;
  bool doSounds       = false;
  bool doClasses      = false;
  bool doFrags        = false;
  bool doNames        = false;
  bool doLua          = false;
  bool doModules      = false;
  bool doMusic        = false;
  bool doMissions     = false;
  bool doPack         = false;
  bool useCompression = true;
  bool use7zip        = false;

  context.bumpmap = false;
  context.useS3TC = false;

  optind = 1;
  int opt;
  while( ( opt = getopt( argc, argv, "lugctbmsafnxoripAC07h?" ) ) >= 0 ) {
    switch( opt ) {
      case 'l': {
        doCat = true;
        break;
      }
      case 'u': {
        doUI = true;
        break;
      }
      case 'g': {
        doShaders = true;
        break;
      }
      case 'c': {
        doCaela = true;
        break;
      }
      case 't': {
        doTerrae = true;
        break;
      }
      case 'b': {
        doBSPs = true;
        break;
      }
      case 'm': {
        doModels = true;
        break;
      }
      case 's': {
        doSounds = true;
        break;
      }
      case 'a': {
        doClasses = true;
        break;
      }
      case 'f': {
        doFrags = true;
        break;
      }
      case 'n': {
        doNames = true;
        break;
      }
      case 'x': {
        doLua = true;
        break;
      }
      case 'o': {
        doModules = true;
        break;
      }
      case 'r': {
        doMusic = true;
        break;
      }
      case 'i': {
        doMissions = true;
        break;
      }
      case 'p': {
        doPack = true;
        break;
      }
      case 'A': {
        doCat      = true;
        doUI       = true;
        doShaders  = true;
        doCaela    = true;
        doTerrae   = true;
        doBSPs     = true;
        doModels   = true;
        doSounds   = true;
        doClasses  = true;
        doFrags    = true;
        doNames    = true;
        doLua      = true;
        doModules  = true;
        doMusic    = true;
        doMissions = true;
        doPack     = true;
        break;
      }
      case 'C': {
        context.useS3TC = true;
        break;
      }
      case '0': {
        useCompression = false;
        break;
      }
      case '7': {
        use7zip = true;
        break;
      }
      default: {
        printUsage( invocationName );
        return EXIT_FAILURE;
      }
    }
  }

  if( optind != argc - 2 && optind != argc - 1 ) {
    printUsage( invocationName );
    return EXIT_FAILURE;
  }

#ifdef _WIN32
  String srcDir = String::replace( argv[optind], '\\', '/' );
  String outDir = optind == argc - 1 ? "share/" OZ_APPLICATION_NAME :
                                       String::replace( argv[optind + 1], '\\', '/' );
#else
  String srcDir = argv[optind];
  String outDir = optind == argc - 1 ? "share/" OZ_APPLICATION_NAME : argv[optind + 1];
#endif

  while( !srcDir.isEmpty() && srcDir.last() == '/' ) {
    srcDir = srcDir.substring( 0, srcDir.length() - 1 );
  }
  if( srcDir.isEmpty() ) {
    throw Exception( "Source directory cannot be root ('/')" );
  }

  String pkgName = srcDir.substring( srcDir.lastIndex( '/' ) + 1 );

  if( srcDir[0] != '/' ) {
    srcDir = File::cwd() + "/" + srcDir;
  }
  if( outDir[0] != '/' ) {
    outDir = File::cwd() + "/" + outDir + "/" + pkgName;
  }

  Log::print( OZ_APPLICATION_TITLE " Build started on " );
  Log::printTime( Time::local() );
  Log::printEnd();

  Log::println( "Build details {" );
  Log::indent();

  Log::println( "Date:            %s", BuildInfo::TIME );
  Log::println( "Host system:     %s", BuildInfo::HOST_SYSTEM );
  Log::println( "Target system:   %s", BuildInfo::TARGET_SYSTEM );
  Log::println( "Build type:      %s", BuildInfo::BUILD_TYPE );
  Log::println( "Compiler:        %s", BuildInfo::COMPILER );
  Log::println( "Compiler flags:  %s", BuildInfo::CXX_FLAGS );
  Log::println( "Linker flags:    %s", BuildInfo::EXE_LINKER_FLAGS );

  Log::unindent();
  Log::println( "}" );

  SDL_Init( SDL_INIT_VIDEO );
  PFile::init();
  FreeImage_Initialise();

  File::mkdir( outDir );

  Log::println( "Chdir to output directory '%s'", outDir.cstr() );
  if( !File::chdir( outDir ) ) {
    throw Exception( "Failed to set working directory '%s'", outDir.cstr() );
  }

  Log::println( "Adding source directory '%s' to search path", srcDir.cstr() );
  if( !PFile::mount( srcDir, null, true ) ) {
    throw Exception( "Failed to add directory '%s' to search path", srcDir.cstr() );
  }

  config.add( "window.width", 400 );
  config.add( "window.height", 40 );
  config.add( "window.fullscreen", false );

  window.init();

  client::render.init( true );

  if( !client::shader.hasS3TC && context.useS3TC ) {
    throw Exception( "S3 texture compression enabled but not supported" );
  }

  context.init();
  compiler.init();

  uint startTime = Time::clock();

  // copy package README
  DArray<PFile> dirList = PFile( "/" ).ls();

  foreach( file, dirList.iter() ) {
    String fileName = file->name();

    if( fileName.beginsWith( "README" ) || fileName.beginsWith( "COPYING" ) ) {
      Log::print( "Copying '%s' ...", fileName.cstr() );

      if( !file->map() ) {
        throw Exception( "Failed to copy '%s'", file->path().cstr() );
      }

      InputStream is = file->inputStream();
      File destFile( fileName );

      if( !destFile.write( is.begin(), is.capacity() ) ) {
        throw Exception( "Failed to copy '%s'", file->path().cstr() );
      }

      file->unmap();

      Log::printEnd( " OK" );
    }
  }

  dirList.dealloc();

  if( doCat ) {
    lingua.build();
  }
  if( doUI ) {
    UI::buildCursors();
    UI::buildIcons();
    copyFiles( "ui/font", "ui/font", "ttf", false );
    copyFiles( "ui/icon", "ui/icon", "", true );
  }
  if( doShaders ) {
    copyFiles( "glsl", "glsl", "glsl", false );
    copyFiles( "glsl", "glsl", "vert", false );
    copyFiles( "glsl", "glsl", "frag", false );
    copyFiles( "glsl", "glsl", "json", false );
  }
  if( doCaela ) {
    buildCaela();
  }
  if( doTerrae ) {
    buildTerrae();
  }
  if( doBSPs ) {
    buildBSPs();
    buildBSPTextures();
  }
  if( doClasses ) {
    buildClasses( pkgName );
    copyFiles( "class", "class", "txt", false );
  }
  if( doFrags ) {
    buildFragPools( pkgName );
    copyFiles( "frag", "frag", "txt", false );
  }
  if( doModels ) {
    buildModels();
  }
  if( doSounds ) {
    copySounds();
  }
  if( doNames ) {
    copyFiles( "name", "name", "txt", false );
  }
  if( doLua ) {
    checkLua( "lua/matrix" );
    checkLua( "lua/nirvana" );

    copyFiles( "lua/matrix", "lua/matrix", "lua", false );
    copyFiles( "lua/nirvana", "lua/nirvana", "lua", false );
  }
  if( doMissions ) {
    buildMissions();
  }
  if( doModules ) {
    buildModules();
  }
  if( doMusic ) {
    copyFiles( "music", "music", "oga", true );
    copyFiles( "music", "music", "ogg", true );
  }
  if( doPack ) {
    packArchive( pkgName, useCompression, use7zip );
  }

  uint endTime = Time::clock();
  Log::println( "Build time: %.2f s", float( endTime - startTime ) / 1000.0f );

  compiler.free();
  context.free();
  client::render.free( true );
  window.free();
  config.clear();

  FreeImage_DeInitialise();
  PFile::free();
  SDL_Quit();

  Alloc::printSummary();

  Log::print( OZ_APPLICATION_TITLE " Build finished on " );
  Log::printTime( Time::local() );
  Log::printEnd();

  return EXIT_SUCCESS;
}

}
}
