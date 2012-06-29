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
 * @file client/NaClUpdater.cc
 */

#ifdef __native_client__

#include "stable.hh"

#include "client/NaClUpdater.hh"

#include "client/NaClDownloader.hh"

namespace oz
{
namespace client
{

NaClUpdater naclUpdater;

Vector<NaClUpdater::Package> NaClUpdater::readManifest( InputStream* is ) const
{
  Vector<Package> packages;
  int lineLen;

  while( ( lineLen = String::index( is->getPos(), '\n' ) ) != -1 ) {
    String line = String( lineLen, is->forward( lineLen ) ).trim();

    int space = max( line.lastIndex( ' ' ), line.lastIndex( '\t' ) );

    String pkgName   = line.substring( 0, space ).trim();
    String pkgStamp  = line.substring( space + 1 );
    long64 timeStamp = pkgStamp.parseLong();

    packages.add( Package( { pkgName, timeStamp } ) );
  }

  return packages;
}

bool NaClUpdater::checkUpdates()
{
  Log::print( "Checking for updates ..." );

  NaClDownloader downloader;

  downloader.begin( "/manifest.txt" );
  do {
    Time::sleep( 1000 );
    Log::printRaw( "." );
  }
  while( !downloader.isComplete() );

  BufferStream bs = downloader.take();

  if( bs.length() < 11 || !String::beginsWith( bs.begin(), "ozManifest" ) ) {
    Log::printEnd( " Failed to download manifest file" );
    return false;
  }

  try {
    int nEntries = bs.readInt();
    for( int i = 0; i < nEntries; ++i ) {
      Time time = Time::local( time.epoch );

      Log::printRaw( " Latest: " );
      Log::printTime( time );
      Log::printEnd();
    }
  }
  catch( const std::exception& ) {
    Log::printEnd( " Error reading manifest file" );
    return false;
  }
}

void NaClUpdater::downloadUpdates()
{
  NaClDownloader downloader;

  foreach( pkg, packages.citer() ) {
    File pkgFile( "/local/share/" + pkg->name );

    if( pkgFile.stat() ) {
      Time time = Time::local( pkgFile.time() );
      Log::print( "%s: timestamp %s, ", pkg->name.cstr(), time.toString().cstr() );

      Log::printEnd( " Up-to-date" );
      continue;
    }

    String url = pkg->name;

    Log::print( "Downloading '%s' into '%s' ...", url.cstr(), pkgFile.path().cstr() );

    downloader.begin( url );
    do {
      Time::sleep( 1000 );
      Log::printRaw( "." );
    }
    while( !downloader.isComplete() );

    BufferStream bs = downloader.take();

    Log::printRaw( " %.2f MiB transferred ...", float( bs.length() ) / ( 1024.0f*1024.0f ) );

    if( bs.length() < 2 || bs[0] != '7' || bs[1] != 'z' ) {
      Log::printEnd( " Failed" );
      continue;
    }

    if( !pkgFile.write( bs.begin(), bs.length() ) ) {
      throw Exception( "Cannot write to local storage" );
    }

    Log::printEnd( " OK" );
  }
}

void NaClUpdater::update()
{
  Log::println( "Updating game data files {" );
  Log::indent();

  if( checkUpdates() ) {
    downloadUpdates();
  }

  Log::unindent();
  Log::println( "}" );
}

void NaClUpdater::init()
{}

void NaClUpdater::free()
{
  packages.dealloc();
}

}
}

#endif
