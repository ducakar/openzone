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
#include "client/Module.hh"
#include "client/OpenGL.hh"

#include "build/Lingua.hh"
#include "build/Context.hh"
#include "build/Compiler.hh"
#include "build/Mouse.hh"
#include "build/Caelum.hh"
#include "build/Terra.hh"
#include "build/BSP.hh"
#include "build/OBJ.hh"
#include "build/MD2.hh"
#include "build/MD3.hh"

#include "build/modules/GalileoModule.hh"

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
  log.println( "Usage:" );
  log.indent();
  log.println( "ozBuild [OPTIONS] <data_src> <out_root>" );
  log.println();
  log.println( "<data_src>" );
  log.println( "\tPath to directory that includes data to be built. Name of this" );
  log.println( "\tdirectory name is used as package name." );
  log.println();
  log.println( "<out_root>" );
  log.println( "\tDirectory where output directory and archive will be created." );
  log.println();
  log.println( "-v" );
  log.println( "\tMore verbose log output." );
  log.println();
  log.println( "-l" );
  log.println( "\tBuild translations." );
  log.println();
  log.println( "-u" );
  log.println( "\tBuild UI." );
  log.println();
  log.println( "-g" );
  log.println( "\tCopy shaders." );
  log.println();
  log.println( "-c" );
  log.println( "\tBuild caela (skies)." );
  log.println();
  log.println( "-t" );
  log.println( "\tBuild terrae (terrains)." );
  log.println();
  log.println( "-b" );
  log.println( "\tCompile maps into BSPs and build BPSs with referenced textures." );
  log.println();
  log.println( "-m" );
  log.println( "\tBuild models." );
  log.println();
  log.println( "-s" );
  log.println( "\tCopy sounds (only used ones)." );
  log.println();
  log.println( "-a" );
  log.println( "\tCopy object class definitions." );
  log.println();
  log.println( "-f" );
  log.println( "\tCopy fragment pool definitions." );
  log.println();
  log.println( "-n" );
  log.println( "\tCopy name lists." );
  log.println();
  log.println( "-x" );
  log.println( "\tCheck and copy Lua scripts." );
  log.println();
  log.println( "-o" );
  log.println( "\tBuild modules." );
  log.println();
  log.println( "-r" );
  log.println( "\tCopy music tracks." );
  log.println();
  log.println( "-p" );
  log.println( "\tPack built files into ZIP archive." );
  log.println();
  log.println( "-A" );
  log.println( "\tBuild everything." );
  log.println();
  log.println( "-C" );
  log.println( "\tUse S3 texture compression" );
  log.println();
  log.unindent();
}

void Build::copyFiles( const char* srcDir, const char* destDir, const char* ext, bool recurse )
{
  log.println( "Copying *.%s from '%s' to '%s' {", ext, srcDir, destDir );
  log.indent();

  String sSrcDir = srcDir;
  String sDestDir = destDir;
  PhysFile dir( sSrcDir );
  DArray<PhysFile> dirList = dir.ls();

  if( !dirList.isEmpty() ) {
    File::mkdir( destDir );
  }

  sSrcDir  = sSrcDir + "/";
  sDestDir = sDestDir + "/";

  foreach( file, dirList.iter() ) {
    String fileName = file->name();

    if( file->getType() == File::DIRECTORY ) {
      if( recurse ) {
        copyFiles( srcDir + ( "/" + file->name() ), destDir + ( "/" + file->name() ), ext, true );
      }
    }
    else if( file->hasExtension( ext ) || fileName.beginsWith( "README" ) ||
             fileName.beginsWith( "COPYING" ) )
    {
      log.print( "Copying '%s' ...", fileName.cstr() );

      if( !file->map() ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      InputStream is = file->inputStream();
      File destFile( String::str( "%s/%s", destDir, fileName.cstr() ) );

      if( !destFile.write( &is ) ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      file->unmap();

      log.printEnd( " OK" );
      continue;
    }
  }

  log.unindent();
  log.println( "}" );
}

void Build::buildTextures( const char* srcDir, const char* destDir, bool wrap,
                           int magFilter, int minFilter )
{
  log.println( "Building textures in '%s' to '%s' {", srcDir, destDir );
  log.indent();

  String sSrcDir = srcDir;
  String sDestDir = destDir;
  PhysFile dir( sSrcDir );
  DArray<PhysFile> dirList = dir.ls();

  if( !dirList.isEmpty() ) {
    File::mkdir( destDir );
  }

  sSrcDir  = sSrcDir + "/";
  sDestDir = sDestDir + "/";

  foreach( file, dirList.iter() ) {
    String fileName = file->name();

    if( fileName.beginsWith( "README" ) || fileName.beginsWith( "COPYING" ) ) {
      log.print( "Copying '%s' ...", fileName.cstr() );

      if( !file->map() ) {
        throw Exception( "Failed to read '%s'", file->realPath().cstr() );
      }

      InputStream is = file->inputStream();
      File destFile( String::str( "%s/%s", destDir, fileName.cstr() ) );

      if( !destFile.write( &is ) ) {
        throw Exception( "Failed to write '%s'", file->realPath().cstr() );
      }

      file->unmap();

      log.printEnd( " OK" );
      continue;
    }

    if( !file->hasExtension( "png" ) && !file->hasExtension( "jpg" ) ) {
      continue;
    }

    String srcPath = file->path();
    String destPath = sDestDir + file->baseName() + ".ozcTex";

    log.println( "Building texture '%s' {", srcPath.cstr() );
    log.indent();

    uint id = context.loadRawTexture( srcPath, wrap, magFilter, minFilter );

    hard_assert( id != 0 );

    BufferStream os;

    log.println( "Compiling into '%s'", destPath.cstr() );
    context.writeTexture( id, &os );

    glDeleteTextures( 1, &id );

    if( !File( destPath ).write( &os ) ) {
      throw Exception( "Texture writing failed" );
    }

    log.unindent();
    log.println( "}" );
  }

  log.unindent();
  log.println( "}" );
}

void Build::buildCaela()
{
  log.println( "Building Caela {" );
  log.indent();

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

  log.unindent();
  log.println( "}" );
}

void Build::buildTerrae()
{
  log.println( "Building Terrae {" );
  log.indent();

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

  log.unindent();
  log.println( "}" );
}

void Build::buildBSPs()
{
  log.println( "Building BSPs {" );
  log.indent();

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

  log.unindent();
  log.println( "}" );
}

void Build::buildBSPTextures()
{
  log.println( "Building used BSP textures {" );
  log.indent();

  PhysFile dir( "baseq3/textures" );
  DArray<PhysFile> dirList = dir.ls();

  foreach( subDir, dirList.iter() ) {
    if( subDir->getType() != File::DIRECTORY ) {
      continue;
    }

    DArray<PhysFile> texList = subDir->ls();

    foreach( file, texList.iter() ) {
      String name = file->name();
      String path = file->path();

      if( name.beginsWith( "COPYING" ) || name.beginsWith( "README" ) ) {
        log.print( "Copying '%s' ...", path.cstr() );

        if( !file->map() ) {
          throw Exception( "Failed to read '%s'", file->realPath().cstr() );
        }

        InputStream is = file->inputStream();
        File destFile( String::str( "tex/%s/%s", subDir->name().cstr(), name.cstr() ) );

        File::mkdir( "tex" );
        File::mkdir( "tex/" + subDir->name() );

        if( !destFile.write( &is ) ) {
          throw Exception( "Failed to write '%s'", destFile.path().cstr() );
        }

        file->unmap();

        log.printEnd( " OK" );
        continue;
      }

      int dot   = path.lastIndex( '.' );
      int slash = path.lastIndex( '/' );

      if( slash >= dot ) {
        continue;
      }

      hard_assert( slash > 15 );

      // strlen( "baseq3/textures/" ) == 16
      name = path.substring( 16, dot );

      if( !context.usedTextures.contains( name ) ) {
        continue;
      }

      log.println( "Building texture '%s' {", name.cstr() );
      log.indent();

      File::mkdir( "tex" );
      File::mkdir( "tex/" + subDir->name() );

      File destFile( String::str( "tex/%s.ozcTex", name.cstr() ) );

      uint id = context.loadRawTexture( path );

      BufferStream os;

      log.println( "Compiling into '%s'", destFile.path().cstr() );
      context.writeTexture( id, &os );

      if( !destFile.write( &os ) ) {
        throw Exception( "Failed to write texture '%s'", destFile.path().cstr() );
      }

      log.unindent();
      log.println( "}" );
    }
  }

  log.unindent();
  log.println( "}" );
}

void Build::tagClassResources()
{
  log.print( "Extracting model and sound names form object class definitions ..." );

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
    context.usedSounds.include( engineSound );
    context.usedSounds.include( nextWeaponSound );
    context.usedSounds.include( shot0Sound );
    context.usedSounds.include( shot1Sound );
    context.usedSounds.include( shot2Sound );
    context.usedSounds.include( shot3Sound );
  }

  log.printEnd( " OK" );
}

void Build::tagFragResources()
{
  log.print( "Extracting model names form fragment pool definitions..." );

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

  log.printEnd( " OK" );
}

void Build::buildModels()
{
  log.println( "Building used models {" );
  log.indent();

  String dirName = "mdl";
  PhysFile dir( dirName );
  DArray<PhysFile> dirList = dir.ls();

  dirName = dirName + "/";

  foreach( file, dirList.citer() ) {
    if( !context.usedModels.contains( file->name() ) ) {
      continue;
    }

    String path = file->path();

    if( PhysFile( path + "/data.obj" ).getType() != File::MISSING ) {
      File::mkdir( "mdl" );
      OBJ::build( path );
    }
    else if( PhysFile( path + "/tris.md2" ).getType() != File::MISSING ) {
      File::mkdir( "mdl" );
      MD2::build( path );
    }
    else if( PhysFile( path + "/.md3" ).getType() != File::MISSING ) {
      File::mkdir( "mdl" );
      MD3::build( path );
    }
  }

  log.unindent();
  log.println( "}" );
}

void Build::copySounds()
{
  log.println( "Copying used sounds {" );
  log.indent();

  PhysFile dir( "snd" );
  DArray<PhysFile> dirList = dir.ls();

  foreach( subDir, dirList.iter() ) {
    if( subDir->getType() != File::DIRECTORY ) {
      continue;
    }

    DArray<PhysFile> texList = subDir->ls();

    foreach( file, texList.iter() ) {
      String name = file->name();
      String path = file->path();

      if( name.beginsWith( "COPYING" ) || name.beginsWith( "README" ) ) {
        log.print( "Copying '%s' ...", path.cstr() );

        if( !file->map() ) {
          throw Exception( "Failed to read '%s'", file->realPath().cstr() );
        }

        InputStream is = file->inputStream();
        File destFile( path );

        File::mkdir( "snd" );
        File::mkdir( "snd/" + subDir->name() );

        if( !destFile.write( &is ) ) {
          throw Exception( "Failed to write '%s'", destFile.path().cstr() );
        }

        file->unmap();

        log.printEnd( " OK" );
        continue;
      }

      int dot   = path.lastIndex( '.' );
      int slash = path.lastIndex( '/' );

      if( slash >= dot ) {
        continue;
      }

      hard_assert( slash > 3 );

      name = path.substring( 4, dot );

      if( !context.usedSounds.contains( name ) ) {
        continue;
      }

      log.print( "Copying '%s' ...", name.cstr() );

      File::mkdir( "snd" );
      File::mkdir( "snd/" + subDir->name() );

      if( !file->map() ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      InputStream is = file->inputStream();

      File destFile( file->path() );

      if( !destFile.write( &is ) ) {
        throw Exception( "Failed to write '%s'", destFile.path().cstr() );
      }

      file->unmap();

      log.printEnd( " OK" );
    }
  }

  log.unindent();
  log.println( "}" );
}

void Build::buildModules()
{
  log.println( "Building Modules {" );
  log.indent();

  GalileoModule::build();

  log.unindent();
  log.println( "}" );
}

void Build::checkLua( const char* path )
{
  log.println( "Checking Lua scripts '%s' {", path );
  log.indent();

  String srcDir = String::str( "%s/", path );
  PhysFile dir( path );
  DArray<PhysFile> dirList = dir.ls();

  String sources;

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "lua" ) ) {
      continue;
    }

    String cmdLine = "luac -p " + file->realPath();

    log.println( "%s", cmdLine.cstr() );
    if( system( cmdLine ) != 0 ) {
      throw Exception( "Lua syntax check failed" );
    }
  }

  log.unindent();
  log.println( "}" );
}

void Build::packArchive( const char* name )
{
  log.println( "Packing archive {" );
  log.indent();

  String path = String::str( "../%s.zip", name );
  String cmdLine = String::str( "zip -Z store -r \"%s\" *", path.cstr() );

  log.println( "%s", cmdLine.cstr() );
  log.println();

  system( cmdLine );

  log.unindent();
  log.println( "}" );
}

void Build::shutdown()
{
  compiler.free();
  context.free();
  client::render.free( true );
  config.clear( true );

  FreeImage_DeInitialise();
  PhysFile::free();
  SDL_Quit();

  Alloc::printSummary();

  log.print( OZ_APPLICATION_TITLE " Build finished on " );
  log.printTime();
  log.printEnd();
}

int Build::main( int argc, char** argv )
{
  bool doCat     = false;
  bool doUI      = false;
  bool doShaders = false;
  bool doCaela   = false;
  bool doTerrae  = false;
  bool doBSPs    = false;
  bool doModels  = false;
  bool doSounds  = false;
  bool doClasses = false;
  bool doFrags   = false;
  bool doNames   = false;
  bool doLua     = false;
  bool doModules = false;
  bool doMusic   = false;
  bool doPack    = false;

  optind = 1;
  int opt;
  while( ( opt = getopt( argc, argv, "vlugtcbmsafnxorpCA" ) ) != -1 ) {
    switch( opt ) {
      case 'v': {
        log.isVerbose = true;
        break;
      }
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
      case 'C': {
        context.useS3TC = true;
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
      default: {
        log.println();
        printUsage();
        return EXIT_FAILURE;
      }
    }
  }

  if( optind != argc - 2 ) {
    printUsage();
    return EXIT_FAILURE;
  }

  String dataDir = String::replace( argv[optind], '\\', '/' );
  String outDir  = String::replace( argv[optind + 1], '\\', '/' );

  while( !dataDir.isEmpty() && dataDir.last() == '/' ) {
    dataDir = dataDir.substring( 0, dataDir.length() - 1 );
  }

  if( dataDir.isEmpty() ) {
    throw Exception( "Source directory cannot be root ('/')" );
  }

  String pkgName = dataDir.substring( dataDir.lastIndex( '/' ) + 1 );

  if( dataDir[0] != '/' ) {
    dataDir = File::cwd() + "/" + dataDir;
  }
  if( outDir[0] != '/' ) {
    outDir = File::cwd() + "/" + outDir + "/" + pkgName;
  }

  log.print( OZ_APPLICATION_TITLE " Build started on " );
  log.printTime();
  log.printEnd();

  log.println( "Build details {" );
  log.indent();

  log.println( "Date:            %s", BuildInfo::TIME );
  log.println( "Host system:     %s", BuildInfo::HOST_SYSTEM );
  log.println( "Target system:   %s", BuildInfo::TARGET_SYSTEM );
  log.println( "Build type:      %s", BuildInfo::BUILD_TYPE );
  log.println( "Compiler:        %s", BuildInfo::COMPILER );
  log.println( "Compiler flags:  %s", BuildInfo::CXX_FLAGS );
  log.println( "Linker flags:    %s", BuildInfo::EXE_LINKER_FLAGS );

  log.unindent();
  log.println( "}" );

  SDL_Init( SDL_INIT_VIDEO );
  if( !PhysFile::init() ) {
    throw Exception( "PhysicsFS initialisation failed" );
  }
  FreeImage_Initialise();

  File::mkdir( outDir );

  log.println( "Chdir to output directory '%s'", outDir.cstr() );
  if( !File::chdir( outDir ) ) {
    throw Exception( "Failed to set working directory '%s'", outDir.cstr() );
  }

  log.println( "Adding source directory '%s' to search path", dataDir.cstr() );
  if( !PhysFile::mount( dataDir, null, true ) ) {
    throw Exception( "Failed to add directory '%s' to search path", dataDir.cstr() );
  }

  uint startTime = Time::clock();

  config.add( "screen.width", "400" );
  config.add( "screen.height", "40" );
  config.add( "screen.full", "false" );
  client::render.init( true );
  SDL_WM_SetCaption( OZ_APPLICATION_TITLE " :: Building data ...", null );

  if( !client::shader.hasS3TC && context.useS3TC ) {
    throw Exception( "S3 texture compression enabled but not supported" );
  }

  context.init();
  compiler.init();

  // copy package README
  DArray<PhysFile> dirList = PhysFile( "/" ).ls();

  foreach( file, dirList.iter() ) {
    String fileName = file->name();

    if( fileName.beginsWith( "README" ) || fileName.beginsWith( "COPYING" ) ) {
      log.print( "Copying '%s' ...", fileName.cstr() );

      if( !file->map() ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      InputStream is = file->inputStream();
      File destFile( fileName );

      if( !destFile.write( &is ) ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      file->unmap();

      log.printEnd( " OK" );
    }
  }

  dirList.dealloc();

  if( doCat ) {
    lingua.build();
  }
  if( doUI ) {
    bool useS3TC = context.useS3TC;
    context.useS3TC = false;

    if( PhysFile( "ui/cur" ).getType() != File::MISSING ) {
      Mouse::build();
    }

    copyFiles( "ui", "ui", "", false );
    copyFiles( "ui/font", "ui/font", "ttf", false );
    buildTextures( "ui/icon", "ui/icon", true, GL_LINEAR, GL_LINEAR );
    buildTextures( "ui/galileo", "ui/galileo", true, GL_LINEAR, GL_LINEAR );

    context.useS3TC = useS3TC;
  }
  if( doShaders ) {
    copyFiles( "glsl", "glsl", "glsl", false );
    copyFiles( "glsl", "glsl", "vert", false );
    copyFiles( "glsl", "glsl", "frag", false );
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
  }
  if( doPack ) {
    packArchive( pkgName );
  }

  uint endTime = Time::clock();

  log.println( "Build time: %.2f s", float( endTime - startTime ) / 1000.0f );

  return EXIT_SUCCESS;
}

}
}
