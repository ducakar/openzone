/*
 *  ozXmlrc.cc
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hh"

#include <SDL_main.h>

OZ_IMPORT()

int main( int argc, char** argv )
{
  log.init();

  if( argc != 2 ) {
    log.println( "usage: ozXmlrc file_to_convert" );
    log.println( "  if `file.xml' file is given, a new converted file `file.xml.rc' is created and "
        "vice versa" );
    return -1;
  }

  Config config;
  if( !config.load( argv[1] ) ) {
    return -1;
  }

  const char* suffix = String::findLast( argv[1], '.' );
  if( suffix == null ) {
    return -1;
  }

  if( String::equals( suffix, ".xml" ) ) {
    String newFile = argv[1] + String( ".rc" );
    config.save( newFile );
  }
  else if( String::equals( suffix, ".rc" ) ) {
    String newFile = argv[1] + String( ".xml" );
    config.save( newFile );
  }
  else {
    return -1;
  }
  return 0;
}
