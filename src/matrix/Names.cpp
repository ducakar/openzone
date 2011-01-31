/*
 *  Names.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Names.hpp"

#include "matrix/Translator.hpp"

#include <dirent.h>
#include <sys/types.h>

namespace oz
{

  Names names;

  const String& Names::genName() const
  {
    assert( names.length() > 0 );

    if( names.length() == 1 ) {
      return names[0];
    }

    int index = Math::randn( names.length() - 1 ) + 1;
    return names[index];
  }

  void Names::init()
  {
    log.println( "Loading name databases {" );
    log.indent();

    // a default entry, we need it if nothing is read
    names.add( "" );

    for( int i = 0; i < translator.names.length(); ++i ) {
      log.print( "'%s' ...", translator.names[i].name.cstr() );

      FILE* file = fopen( translator.names[i].path, "r" );
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

      log.printEnd( " OK" );
    }

    log.unindent();
    log.println( "}" );
  }

  void Names::free()
  {
    names.clear();
  }

}
