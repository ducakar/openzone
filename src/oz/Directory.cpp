/*
 *  Directory.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Directory.hpp"

#include <dirent.h>

namespace oz
{

  Directory::CIterator::CIterator() : B( null ), dir( null )
  {}

  Directory::CIterator::CIterator( const Directory& d ) : B( null ), dir( d.dir )
  {
    hard_assert( dir != null );

    do {
      struct dirent* file = readdir( reinterpret_cast<DIR*>( dir ) );

      if( file == null ) {
        elem = null;
      }
      else {
        elem = file->d_name;

        switch( file->d_type ) {
          case DT_REG: {
            type = REGULAR_FILE;
            break;
          }
          case DT_DIR: {
            type = DIRECTORY;
            break;
          }
          default: {
            type = OTHER;
            break;
          }
        }
      }
    }
    while( elem != null && elem[0] == '.' );
  }

  Directory::CIterator& Directory::CIterator::operator ++ ()
  {
    hard_assert( elem != null );

    do {
      struct dirent* file = readdir( reinterpret_cast<DIR*>( dir ) );

      if( file == null ) {
        elem = null;
      }
      else {
        elem = file->d_name;

        switch( file->d_type ) {
          case DT_REG: {
            type = REGULAR_FILE;
            break;
          }
          case DT_DIR: {
            type = DIRECTORY;
            break;
          }
          default: {
            type = OTHER;
            break;
          }
        }
      }
    }
    while( elem != null && elem[0] == '.' );

    return *this;
  }

  String Directory::CIterator::baseName() const
  {
    hard_assert( elem != null );

    const char* dot = String::findLast( elem, '.' );
    if( dot == null ) {
      return elem;
    }

    return String( elem, int( dot - elem ) );
  }

  String Directory::CIterator::extension() const
  {
    hard_assert( elem != null );

    const char* dot = String::findLast( elem, '.' );
    if( dot == null ) {
      return "";
    }

    return dot;
  }

  bool Directory::CIterator::hasExtension() const
  {
    hard_assert( elem != null );

    const char* dot = String::findLast( elem, '.' );
    return dot != null;
  }

  bool Directory::CIterator::hasExtension( const char* ext ) const
  {
    hard_assert( elem != null );

    const char* dot = String::findLast( elem, '.' );
    if( dot == null ) {
      return false;
    }

    return String::equals( ext, dot + 1 );
  }

  Directory::Directory() : dir( null )
  {}

  Directory::Directory( const char* path ) : dir( null )
  {
    open( path );
  }

  Directory::~Directory()
  {
    close();
  }

  Directory::CIterator Directory::citer() const
  {
    return CIterator( *this );
  }

  bool Directory::isOpened() const
  {
    return dir != null;
  }

  const char* Directory::name() const
  {
    return path;
  }

  void Directory::open( const char* path_ )
  {
    if( dir != null ) {
      closedir( reinterpret_cast<DIR*>( dir ) );
    }

    dir = opendir( path_ );
    path = dir == null ? "" : path_;
  }

  void Directory::close()
  {
    if( dir != null ) {
      closedir( reinterpret_cast<DIR*>( dir ) );
      dir = null;
      path = "";
    }
  }

}
