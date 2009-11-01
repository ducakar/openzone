/*
 *  ozXmlrc.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

using namespace oz;

int main( int argc, char **argv )
{
  log.init( null, false, "  " );

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

  const char *suffix = String::lastIndex( argv[1], '.' );
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
    config.clear();
    return -1;
  }

  config.clear();
  return 0;
}
