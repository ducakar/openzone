/*
 *  ozXmlrc.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <SDL_main.h>

using namespace oz;

bool Alloc::isLocked = true;

int main( int argc, char** argv )
{
  StackTrace::init();

  Alloc::isLocked = false;
  onleave( []() {
    Alloc::isLocked = true;
    Alloc::printLeaks();
  } );

  if( argc != 2 ) {
    oz::log.println( "usage: ozXmlrc file_to_convert" );
    oz::log.println( "  if `file.xml' file is given, a new converted file `file.xml.rc' is created "
        "and vice versa" );
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
