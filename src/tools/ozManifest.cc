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
 * @file tools/ozManifest.cc
 *
 * Generate packages.ozManifest required by NaCl port.
 */

#include <ozCore/ozCore.hh>

#include <cstdlib>

using namespace oz;

static void printUsage( const char* invocationName )
{
  Log::printRaw(
    "Usage: %s <data_dir>\n"
    "\n"
    "<data_dir>  Output directory where built packages are located.\n"
    "            Defaults to 'share/openzone'.\n\n",
    invocationName );
}

int main( int argc, char** argv )
{
  System::init();

  String invocationName = String::fileBaseName( argv[0] );

  if( argc > 2 ) {
    printUsage( invocationName );
    return EXIT_FAILURE;
  }

#ifdef _WIN32
  String outDirPath = argc < 2 ? "share/openzone" : String::replace( argv[1], '\\', '/' );
#else
  String outDirPath = argc < 2 ? "share/openzone" : argv[1];
#endif

  while( !outDirPath.isEmpty() && outDirPath.last() == '/' ) {
    outDirPath = outDirPath.substring( 0, outDirPath.length() - 1 );
  }
  if( outDirPath.isEmpty() ) {
    OZ_ERROR( "Package directory cannot be root ('/')" );
  }

  Log::println( "Package manifest {" );
  Log::indent();

  OutputStream os( 0, Endian::LITTLE );
  os.writeChars( "ozManifest", int( sizeof( "ozManifest" ) ) );

  File outDir = outDirPath;
  DArray<File> files = outDir.ls();
  Map<String, File> packages;

  foreach( file, files.citer() ) {
    if( file->hasExtension( "7z" ) ) {
      packages.add( file->baseName(), *file );
    }
    else if( file->hasExtension( "zip" ) ) {
      packages.include( file->baseName(), *file );
    }
  }

  os.writeInt( packages.length() );

  foreach( pkg, packages.iter() ) {
    File& file = pkg->value;

    String name = file.name().cstr();
    long64 time = file.time();

    Log::println( "%s, timestamp: %s", name.cstr(), Time::local( time ).toString().cstr() );

    os.writeString( name );
    os.writeLong64( time );
  }

  Log::unindent();
  Log::println( "}" );

  File manifest = outDirPath + "/packages.ozManifest";

  Log::print( "Writing manifest to '%s' ...", manifest.path().cstr() );

  if( !manifest.write( os.begin(), os.tell() ) ) {
    OZ_ERROR( "Failed to write manifest file" );
  }

  Log::printEnd( " OK" );

  return EXIT_SUCCESS;
}
