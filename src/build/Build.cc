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

#include "client/Render.hh"

#include "build/Lingua.hh"
#include "build/Context.hh"
#include "build/Compiler.hh"
#include "build/UI.hh"
#include "build/Caelum.hh"
#include "build/Terra.hh"
#include "build/BSP.hh"
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

void Build::printUsage()
{
  Log::println( "Usage:" );
  Log::indent();
  Log::println( "ozBuild [OPTIONS] <data_src> <out_root>" );
  Log::println();
  Log::println( "<data_src>  Path to directory that includes data to be built. Name of this" );
  Log::println( "            directory name is used as package name." );
  Log::println( "<out_root>  Directory where output directory and archive will be created." );
  Log::println( "-v          More verbose log output." );
  Log::println( "-l          Build translations." );
  Log::println( "-u          Build UI." );
  Log::println( "-g          Copy shaders." );
  Log::println( "-c          Build caela (skies)." );
  Log::println( "-t          Build terrae (terrains)." );
  Log::println( "-b          Compile maps into BSPs and build BPSs with referenced textures." );
  Log::println( "-m          Build models." );
  Log::println( "-s          Copy sounds (only used ones)." );
  Log::println( "-a          Copy object class definitions." );
  Log::println( "-f          Copy fragment pool definitions." );
  Log::println( "-n          Copy name lists." );
  Log::println( "-x          Check and copy Lua scripts." );
  Log::println( "-o          Build modules." );
  Log::println( "-r          Copy music tracks." );
  Log::println( "-p          Pack built files into ZIP archive." );
  Log::println( "-A          Everything above." );
  Log::println( "-B          Build with bumpmap vertex format." );
  Log::println( "-C          Use S3 texture compression." );
  Log::println( "-0          Use no compression for ZIP archive." );
  Log::println( "-7          Create non-solid LZMA-compressed 7zip archive instead of ZIP." );
  Log::unindent();
}

void Build::copyFiles( const char* srcDir, const char* destDir, const char* ext, bool recurse )
{
  String sSrcDir = srcDir;
  String sDestDir = destDir;
  PhysFile dir( sSrcDir );
  DArray<PhysFile> dirList = dir.ls();

  if( dirList.isEmpty() ) {
    return;
  }

  Log::println( "Copying *.%s from '%s' to '%s' {", ext, srcDir, destDir );
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
  PhysFile dir( srcDir );
  DArray<PhysFile> dirList = dir.ls();

  srcDir = srcDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
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
  PhysFile dir( srcDir );
  DArray<PhysFile> dirList = dir.ls();

  srcDir = srcDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
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
  PhysFile dir( srcDir );
  DArray<PhysFile> dirList = dir.ls();

  srcDir = srcDir + "/";
  destDir = destDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
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
  Log::println( "Building used BSP textures {" );
  Log::indent();

  Map<String> usedDirs;

  PhysFile dir( "baseq3/textures" );
  DArray<PhysFile> dirList = dir.ls();

  foreach( subDir, dirList.iter() ) {
    subDir->stat();

    if( subDir->type() != File::DIRECTORY ) {
      continue;
    }

    DArray<PhysFile> texList = subDir->ls();

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

      int textureFlags = client::Mesh::DIFFUSE_BIT;

      if( masksId != 0 ) {
        textureFlags |= client::Mesh::MASKS_BIT;
      }
      if( normalsId != 0 ) {
        textureFlags |= client::Mesh::NORMALS_BIT;
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
    PhysFile subDir( *subDirPath );

    DArray<PhysFile> texList = subDir.ls();

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

void Build::tagClassResources()
{
  Log::print( "Extracting model and sound names form object class definitions ..." );

  String dirName = "class";
  PhysFile dir( dirName );
  DArray<PhysFile> dirList = dir.ls();

  foreach( file, dirList.iter() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    Config classConfig;
    classConfig.load( *file );

    const char* imagoModel      = classConfig.get( "imagoModel", "" );

    const char* createSound     = classConfig.get( "audioSound.create", "" );
    const char* destroySound    = classConfig.get( "audioSound.destroy", "" );
    const char* useSound        = classConfig.get( "audioSound.use", "" );
    const char* damageSound     = classConfig.get( "audioSound.damage", "" );
    const char* hitSound        = classConfig.get( "audioSound.hit", "" );
    const char* splashSound     = classConfig.get( "audioSound.splash", "" );
    const char* frictingSound   = classConfig.get( "audioSound.fricting", "" );
    const char* shotSound       = classConfig.get( "audioSound.shot", "" );
    const char* shotEmptySound  = classConfig.get( "audioSound.shotEmpty", "" );
    const char* hitHardSound    = classConfig.get( "audioSound.hitHard", "" );
    const char* landSound       = classConfig.get( "audioSound.land", "" );
    const char* jumpSound       = classConfig.get( "audioSound.jump", "" );
    const char* flipSound       = classConfig.get( "audioSound.flip", "" );
    const char* deathSound      = classConfig.get( "audioSound.death", "" );
    const char* stepSound       = classConfig.get( "audioSound.step", "" );
    const char* waterStepSound  = classConfig.get( "audioSound.waterStep", "" );
    const char* swimSound       = classConfig.get( "audioSound.swim", "" );
    const char* engineSound     = classConfig.get( "audioSound.engine", "" );
    const char* nextWeaponSound = classConfig.get( "audioSound.nextWeapon", "" );
    const char* shot0Sound      = classConfig.get( "audioSound.shot0", "" );
    const char* shot1Sound      = classConfig.get( "audioSound.shot1", "" );
    const char* shot2Sound      = classConfig.get( "audioSound.shot2", "" );
    const char* shot3Sound      = classConfig.get( "audioSound.shot3", "" );

    context.usedModels.include( imagoModel );

    context.usedSounds.include( createSound );
    context.usedSounds.include( destroySound );
    context.usedSounds.include( useSound );
    context.usedSounds.include( damageSound );
    context.usedSounds.include( hitSound );
    context.usedSounds.include( splashSound );
    context.usedSounds.include( frictingSound );
    context.usedSounds.include( shotSound );
    context.usedSounds.include( shotEmptySound );
    context.usedSounds.include( hitHardSound );
    context.usedSounds.include( landSound );
    context.usedSounds.include( jumpSound );
    context.usedSounds.include( flipSound );
    context.usedSounds.include( deathSound );
    context.usedSounds.include( stepSound );
    context.usedSounds.include( waterStepSound );
    context.usedSounds.include( swimSound );
    context.usedSounds.include( engineSound );
    context.usedSounds.include( nextWeaponSound );
    context.usedSounds.include( shot0Sound );
    context.usedSounds.include( shot1Sound );
    context.usedSounds.include( shot2Sound );
    context.usedSounds.include( shot3Sound );
  }

  Log::printEnd( " OK" );
}

void Build::tagFragResources()
{
  Log::print( "Extracting model names form fragment pool definitions..." );

  String dirName = "frag";
  PhysFile dir( dirName );
  DArray<PhysFile> dirList = dir.ls();

  foreach( file, dirList.iter() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    Config fragConfig;
    fragConfig.load( *file );

    char buffer[] = "model  ";
    for( int i = 0; i < matrix::FragPool::MAX_MODELS; ++i ) {
      hard_assert( i < 100 );

      buffer[ sizeof( buffer ) - 3 ] = char( '0' + ( i / 10 ) );
      buffer[ sizeof( buffer ) - 2 ] = char( '0' + ( i % 10 ) );

      context.usedModels.include( fragConfig.get( buffer, "" ) );
    }
  }

  Log::printEnd( " OK" );
}

void Build::buildModels()
{
  Log::println( "Building used models {" );
  Log::indent();

  PhysFile mdlDir( "mdl" );
  File::mkdir( mdlDir.path() );
  DArray<PhysFile> dirList = mdlDir.ls();

  foreach( dir, dirList.iter() ) {
    if( !context.usedModels.contains( dir->name() ) ) {
      continue;
    }

    String path = dir->path();
    File::mkdir( path );
    DArray<PhysFile> fileList = dir->ls();

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

    if( PhysFile( path + "/data.obj" ).stat() ) {
      OBJ::build( path );
    }
    else if( PhysFile( path + "/tris.md2" ).stat() ) {
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
  Log::println( "Copying used sounds {" );
  Log::indent();

  Map<String> usedDirs;

  PhysFile dir( "snd" );
  DArray<PhysFile> dirList = dir.ls();

  foreach( subDir, dirList.iter() ) {
    subDir->stat();

    if( subDir->type() != File::DIRECTORY ) {
      continue;
    }

    DArray<PhysFile> sndList = subDir->ls();

    foreach( file, sndList.iter() ) {
      String name = file->name();
      String path = file->path();

      int dot   = path.lastIndex( '.' );
      int slash = path.lastIndex( '/' );

      if( slash == -1 || slash >= dot ) {
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
    PhysFile subDir( *subDirPath );
    DArray<PhysFile> texList = subDir.ls();

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
  PhysFile dir( path );
  DArray<PhysFile> dirList = dir.ls();

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

  int size = archive.size();

  if( size >= 0 ) {
    Log::println();
    Log::println( "Archive size: %.1f MiB = %.1f MB",
                  float( size ) / ( 1024.0f * 1024.0f ),
                  float( size ) / ( 1000.0f * 1000.0f ) );
  }

  Log::unindent();
  Log::println( "}" );
}

int Build::main( int argc, char** argv )
{
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
  bool doPack         = false;
  bool useCompression = true;
  bool use7zip        = false;

  context.bumpmap = false;
  context.useS3TC = false;

  optind = 1;
  int opt;
  while( ( opt = getopt( argc, argv, "lugtcbmsafnxorpABC07" ) ) != -1 ) {
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
      case 'p': {
        doPack = true;
        break;
      }
      case 'A': {
        doCat     = true;
        doUI      = true;
        doShaders = true;
        doCaela   = true;
        doTerrae  = true;
        doBSPs    = true;
        doModels  = true;
        doSounds  = true;
        doClasses = true;
        doFrags   = true;
        doNames   = true;
        doLua     = true;
        doModules = true;
        doMusic   = true;
        doPack    = true;
        break;
      }
      case 'B': {
        context.bumpmap = true;
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
        Log::println();
        printUsage();
        return EXIT_FAILURE;
      }
    }
  }

  if( optind != argc - 2 ) {
    printUsage();
    return EXIT_FAILURE;
  }

  String srcDir = String::replace( argv[optind], '\\', '/' );
  String outDir = String::replace( argv[optind + 1], '\\', '/' );

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
  PhysFile::init();
  FreeImage_Initialise();

  File::mkdir( outDir );

  Log::println( "Chdir to output directory '%s'", outDir.cstr() );
  if( !File::chdir( outDir ) ) {
    throw Exception( "Failed to set working directory '%s'", outDir.cstr() );
  }

  Log::println( "Adding source directory '%s' to search path", srcDir.cstr() );
  if( !PhysFile::mount( srcDir, null, true ) ) {
    throw Exception( "Failed to add directory '%s' to search path", srcDir.cstr() );
  }

  int  windowWidth  = 400;
  int  windowHeight = 40;
  uint windowFlags  = SDL_OPENGL;

  Log::print( "Creating OpenGL window %dx%d [windowed] ...", windowWidth, windowHeight );

  if( SDL_VideoModeOK( windowWidth, windowHeight, 0, windowFlags ) == 1 ) {
    throw Exception( "Video mode not supported" );
  }

  SDL_Surface* window = SDL_SetVideoMode( windowWidth, windowHeight, 0, windowFlags );

  if( window == null ) {
    throw Exception( "Window creation failed" );
  }

  SDL_WM_SetCaption( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION " :: Building data ...", null );

  windowWidth  = window->w;
  windowHeight = window->h;

  Log::printEnd( " %dx%d-%d ... OK", windowWidth, windowHeight, window->format->BitsPerPixel );

  client::render.init( window, windowWidth, windowHeight, true );

  if( !client::shader.hasS3TC && context.useS3TC ) {
    throw Exception( "S3 texture compression enabled but not supported" );
  }

  context.init();
  compiler.init();

  uint startTime = Time::clock();

  // copy package README
  DArray<PhysFile> dirList = PhysFile( "/" ).ls();

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
    copyFiles( "glsl", "glsl", "rc", false );
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
    tagClassResources();
    copyFiles( "class", "class", "rc", false );
  }
  if( doFrags ) {
    tagFragResources();
    copyFiles( "frag", "frag", "rc", false );
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
    checkLua( "lua/mission" );

    copyFiles( "lua", "lua", "lua", true );
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
  config.clear( true );

  FreeImage_DeInitialise();
  PhysFile::free();
  SDL_Quit();

  Alloc::printSummary();

  Log::print( OZ_APPLICATION_TITLE " Build finished on " );
  Log::printTime( Time::local() );
  Log::printEnd();

  return EXIT_SUCCESS;
}

}
}
