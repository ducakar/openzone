/*
 *  NamePool.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/NamePool.hpp"

#include "matrix/Translator.hpp"

#include <dirent.h>
#include <sys/types.h>

namespace oz
{

  NamePool namePool;

  const String& NamePool::genName( int list ) const
  {
    hard_assert( names.length() > 0 );

    if( list == -1 ) {
      return names[0];
    }

    int index = listPositions[list] + Math::randn( listPositions[list + 1] - listPositions[list] );
    return names[index];
  }

  void NamePool::init()
  {
    log.println( "Loading name databases {" );
    log.indent();

    // a default entry, we need it if nothing is read
    names.add( "" );
    listPositions.add( names.length() );

    for( int i = 0; i < translator.nameLists.length(); ++i ) {
      log.print( "'%s' ...", translator.nameLists[i].name.cstr() );

      FILE* file = fopen( translator.nameLists[i].path, "r" );
      if( file == null ) {
        log.printEnd( " Failed" );
        return;
      }

      char buffer[LINE_LENGTH];

      while( fgets( buffer, LINE_LENGTH, file ) != null ) {
        String name = buffer;
        name = name.trim();

        if( !name.isEmpty() ) {
          names.add( name );
        }
      }
      fclose( file );

      listPositions.add( names.length() );

      log.printEnd( " OK" );
    }

    hard_assert( listPositions.length() == translator.nameLists.length() + 1 );

    log.unindent();
    log.println( "}" );
  }

  void NamePool::free()
  {
    names.clear();
    names.dealloc();
    listPositions.clear();
    listPositions.dealloc();
  }

}
