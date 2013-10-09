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
 * @file client/NaClUpdater.cc
 */

#ifdef __native_client__

#include <client/NaClUpdater.hh>

#include <client/NaClDownloader.hh>

namespace oz
{
namespace client
{

const char MANIFEST_MAGIC[]  = "ozManifest";
const char LOCAL_MANIFEST[]  = "/local/share/openzone/packages.ozManifest";
const char REMOTE_MANIFEST[] = "packages.ozManifest";

NaClUpdater naclUpdater;

DArray<NaClUpdater::Package> NaClUpdater::readManifest( InputStream* is ) const
{
  DArray<Package> packages;

  if( is->available() < int( sizeof( MANIFEST_MAGIC ) ) ||
      !String::beginsWith( is->begin(), MANIFEST_MAGIC ) )
  {
    return packages;
  }

  is->forward( int( sizeof( MANIFEST_MAGIC ) ) );

  int nPackages = is->readInt();
  if( nPackages == 0 ) {
    return packages;
  }

  packages.resize( nPackages );

  for( int i = 0; i < nPackages; ++i ) {
    packages[i].name = is->readString();
    packages[i].time = is->readLong64();
  }
  return packages;
}

void NaClUpdater::writeLocalManifest() const
{
  Log::print( "Writing local manifest '%s' ...", LOCAL_MANIFEST );

  OutputStream os( 0, Endian::LITTLE );

  os.writeString( "ozManifest" );
  os.writeInt( remotePackages.length() );

  foreach( pkg, remotePackages.citer() ) {
    os.writeString( pkg->name );
    os.writeLong64( pkg->time );
  }

  File localManifest = LOCAL_MANIFEST;

  if( !localManifest.write( os.begin(), os.tell() ) ) {
    OZ_ERROR( "Failed to write local manifest" );
  }

  Log::printEnd( " OK" );
}

bool NaClUpdater::checkUpdates()
{
  Log::print( "Checking for updates ..." );

  localPackages.clear();
  remotePackages.clear();

  File localManifest = LOCAL_MANIFEST;

  if( localManifest.map() ) {
    InputStream is = localManifest.inputStream( Endian::LITTLE );
    localPackages = readManifest( &is );
  }

  NaClDownloader downloader;
  downloader.begin( REMOTE_MANIFEST );

  do {
    Time::sleep( 20 );
    Log::printRaw( "." );
  }
  while( !downloader.isComplete() );

  OutputStream os = downloader.take();
  InputStream  is = os.inputStream();

  is.rewind();

  remotePackages = readManifest( &is );

  if( remotePackages.isEmpty() ) {
    Log::printEnd( " Failed" );
    return false;
  }

  Log::printEnd( " OK" );
  return true;
}

void NaClUpdater::downloadUpdates()
{
  NaClDownloader downloader;

  int nRemotePackages = remotePackages.length();
  int packageNum      = 1;

  foreach( pkg, remotePackages.citer() ) {
    File pkgFile = "/local/share/openzone/" + pkg->name;

    if( pkgFile.type() == File::REGULAR ) {
      long64 localTime = 0;

      foreach( localPkg, localPackages.citer() ) {
        if( localPkg->name.equals( pkg->name ) ) {
          localTime = localPkg->time;
          break;
        }
      }

      Log::print( "%s: timestamp %s, ", pkg->name.cstr(),
                  Time::local( localTime ).toString().cstr() );

      if( localTime == pkg->time ) {
        Log::printEnd( " Up-to-date" );
        continue;
      }
      else {
        Log::printEnd( " Out-of-date" );
      }
    }

    String url = pkg->name;

    Log::print( "Downloading '%s' into '%s' ...", url.cstr(), pkgFile.path().cstr() );

    downloader.begin( url );
    do {
      Time::sleep( 100 );
      Log::printRaw( "." );

      float progress = downloader.progress() * 100.0f;

      if( Math::isNaN( progress ) ) {
        Pepper::post( String::str( "upd1:%d/%d", packageNum, nRemotePackages ) );
      }
      else {
        Pepper::post( String::str( "upd1:%d/%d: %.0f %%", packageNum, nRemotePackages, progress ) );
      }
    }
    while( !downloader.isComplete() );

    OutputStream os = downloader.take();

    Log::printRaw( " %.2f MiB transferred ...", float( os.tell() ) / ( 1024.0f*1024.0f ) );

    if( os.tell() < 2 || ( ( os[0] != 'P' || os[1] != 'K' ) &&
                           ( os[0] != '7' || os[1] != 'z' ) ) )
    {
      Log::printEnd( " Failed" );
      continue;
    }

    if( !pkgFile.write( os.begin(), os.tell() ) ) {
      OZ_ERROR( "Cannot write to local storage" );
    }

    ++packageNum;

    Log::printEnd( " OK" );
  }

  foreach( localPkg, localPackages.citer() ) {
    bool isOrphan = true;

    foreach( remotePkg, remotePackages.citer() ) {
      if( remotePkg->name.equals( localPkg->name ) ) {
        isOrphan = false;
        break;
      }
    }

    if( isOrphan ) {
      File pkgFile = "/local/share/openzone/" + localPkg->name;

      Log::print( "Deleting obsolete package '%s' ...", pkgFile.path().cstr() );
      if( File::rm( pkgFile.path() ) ) {
        Log::printEnd( " OK" );
      }
      else {
        Log::printEnd( " Failed" );
      }
    }
  }

  writeLocalManifest();
}

DArray<String> NaClUpdater::update()
{
  DArray<String> packages;

  Log::println( "Updating game data files {" );
  Log::indent();

  Pepper::post( "upd0:" );

  if( checkUpdates() ) {
    downloadUpdates();

    packages.resize( remotePackages.length() );

    for( int i = 0; i < packages.length(); ++i ) {
      packages[i] = static_cast<String&&>( remotePackages[i].name );
    }
  }

  localPackages.clear();
  remotePackages.clear();

  Pepper::post( "none:" );

  Log::unindent();
  Log::println( "}" );

  return packages;
}

}
}

#endif
