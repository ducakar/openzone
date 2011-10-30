/*
 *  Config.cpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

/**
 * @file oz/Config.cpp
 */

#include "Config.hpp"

#include "Log.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstdio>

namespace oz
{

Config config;

bool Config::loadConf( const char* file )
{
  char buffer[BUFFER_SIZE];
  char ch;

  FILE* f = fopen( file, "r" );
  if( f == null ) {
    log.println( "Error reading variables from '%s' ... Cannot open file", file );
    return false;
  }

  do {
    ch = char( fgetc( f ) );

    // skip initial spaces
    while( String::isSpace( ch ) ) {
      ch = char( fgetc( f ) );
    }

    // read variable
    String name;
    if( String::isLetter( ch ) || ch == '_' ) {
      buffer[0] = ch;
      ch = char( fgetc( f ) );

      int i = 1;
      while( i < BUFFER_SIZE - 1 &&
             ( String::isLetter( ch ) || String::isDigit( ch ) || ch == '_' || ch == '.' ) )
      {
        buffer[i] = ch;
        ch = char( fgetc( f ) );
        ++i;
      }
      buffer[i] = '\0';
      name = buffer;
    }
    else {
      goto skipLine;
    }

    // skip spaces between name and value
    while( String::isSpace( ch ) ) {
      ch = char( fgetc( f ) );
    }

    if( ch == '"' ) {
      ch = char( fgetc( f ) );

      int i = 0;
      while( i < BUFFER_SIZE - 1 && ch != '"' && ch != '\n' && ch != EOF ) {
        buffer[i] = ch;
        ch = char( fgetc( f ) );
        ++i;
      }
      if( ch == '"' ) {
        buffer[i] = '\0';
        include( name, buffer );
      }
    }

  skipLine:;

    // find end of line/file
    while( ch != '\n' && ch != EOF ) {
      ch = char( fgetc( f ) );
    }
  }
  while( ch != EOF );

  fclose( f );

  return true;
}

bool Config::saveConf( const char* file )
{
  log.print( "Writing variables to '%s' ...", file );

  // first we sort all the variables by key
  int size = vars.length();
  DArray<Elem> sortedVars( size );

  int i = 0;
  for( auto j : vars.citer() ) {
    sortedVars[i].key   = j.key().cstr();
    sortedVars[i].value = j.value().cstr();
    ++i;
  }
  sortedVars.sort();

  FILE* f = fopen( file, "w" );
  if( f == null ) {
    log.printEnd( " Cannot open file" );
    return false;
  }

  for( int i = 0; i < size; ++i ) {
    fprintf( f, "%s", sortedVars[i].key );

    int chars = ALIGNMENT - String::length( sortedVars[i].key );
    int tabs  = ( chars - 1 ) / 8 + 1;
    for( int j = 0; j < tabs; ++j ) {
      fprintf( f, "\t" );
    }
    fprintf( f, "\"%s\"\n", sortedVars[i].value );
  }

  fclose( f );

  log.printEnd( " OK" );
  return true;
}

Config::~Config()
{
  clear();
}

void Config::add( const char* key, const char* value_ )
{
  String* value = vars.find( key );
  if( value != null ) {
    *value = value_;
  }
  else {
    vars.add( key, value_ );
  }
}

void Config::include( const char* key, const char* value_ )
{
  String* value = vars.find( key );
  if( value == null ) {
    vars.add( key, value_ );
  }
}

void Config::exclude( const char* key )
{
  vars.exclude( key );
}

bool Config::contains( const char* key ) const
{
  return vars.contains( key );
}

bool Config::get( const char* key, bool defVal ) const
{
  const String* value = vars.find( key );
  if( value != null ) {
#ifndef NDEBUG
    usedVars.include( key );
#endif

    if( value->equals( "true" ) ) {
      return true;
    }
    else
      if( value->equals( "false" ) ) {
        return false;
      }
      else {
        throw Exception( "Invalid boolean value '" + *value + "'" );
      }
  }
  else {
    return defVal;
  }
}

int Config::get( const char* key, int defVal ) const
{
  const String* value = vars.find( key );
  if( value != null ) {
#ifndef NDEBUG
    usedVars.include( key );
#endif

    errno = 0;
    char* end;
    int   num = int( strtol( *value, &end, 0 ) );

    if( errno != 0 || end == value->cstr() ) {
      throw Exception( "Invalid int value '" + *value + "'" );
    }
    else {
      return num;
    }
  }
  else {
    return defVal;
  }
}

float Config::get( const char* key, float defVal ) const
{
  const String* value = vars.find( key );
  if( value != null ) {
#ifndef NDEBUG
    usedVars.include( key );
#endif

    errno = 0;
    char* end;
    float num = strtof( *value, &end );

    if( errno != 0 || end == value->cstr() ) {
      throw Exception( "Invalid float value '" + *value + "'" );
    }
    else {
      return num;
    }
  }
  else {
    return defVal;
  }
}

const char* Config::get( const char* key, const char* defVal ) const
{
  const String* value = vars.find( key );
  if( value != null ) {
#ifndef NDEBUG
    usedVars.include( key );
#endif

    return *value;
  }
  else {
    return defVal;
  }
}

bool Config::getSet( const char* key, bool defVal )
{
#ifndef NDEBUG
  usedVars.include( key );
#endif

  const String* value = vars.find( key );
  if( value != null ) {
    if( value->equals( "true" ) ) {
      return true;
    }
    else
      if( value->equals( "false" ) ) {
        return false;
      }
      else {
        throw Exception( "Invalid boolean value '" + *value + "'" );
      }
  }
  else {
    vars.add( key, String( defVal ) );
    return defVal;
  }
}

int Config::getSet( const char* key, int defVal )
{
#ifndef NDEBUG
  usedVars.include( key );
#endif

  const String* value = vars.find( key );
  if( value != null ) {
    errno = 0;
    char* end;
    int   num = int( strtol( *value, &end, 0 ) );

    if( errno != 0 || end == value->cstr() ) {
      throw Exception( "Invalid int value '" + *value + "'" );
    }
    else {
      return num;
    }
  }
  else {
    vars.add( key, String( defVal ) );
    return defVal;
  }
}

float Config::getSet( const char* key, float defVal )
{
#ifndef NDEBUG
  usedVars.include( key );
#endif

  const String* value = vars.find( key );
  if( value != null ) {
    errno = 0;
    char* end;
    float num = strtof( *value, &end );

    if( errno != 0 || end == value->cstr() ) {
      throw Exception( "Invalid float value '" + *value + "'" );
    }
    else {
      return num;
    }
  }
  else {
    vars.add( key, String( defVal ) );
    return defVal;
  }
}

const char* Config::getSet( const char* key, const char* defVal )
{
#ifndef NDEBUG
  usedVars.include( key );
#endif

  const String* value = vars.find( key );
  if( value != null ) {
    return *value;
  }
  else {
    vars.add( key, defVal );
    return defVal;
  }
}

bool Config::load( const char* file )
{
  const char* suffix = String::findLast( file, '.' );

  if( suffix != null ) {
    if( String::equals( suffix, ".rc" ) ) {
      return loadConf( file );
    }
  }

  log.println( "Unknown configuration file %s", file );
  return false;
}

bool Config::save( const char* file )
{
  const char* suffix = String::findLast( file, '.' );

  if( suffix != null ) {
    if( String::equals( suffix, ".rc" ) ) {
      return saveConf( file );
    }
  }

  log.println( "Unknown configuration file %s", file );
  return false;
}

void Config::clear()
{
#ifndef NDEBUG
  for( auto var : vars.citer() ) {
    if( !usedVars.contains( var.key() ) ) {
      log.println( "config: unused variable '%s'", var.key().cstr() );
    }
  }
  usedVars.clear();
  usedVars.dealloc();
#endif
  vars.clear();
  vars.dealloc();
}

String Config::toString( const String& indentString )
{
  String s = "";

  // first we sort all the variables by key
  int size = vars.length();
  DArray<Elem> sortedVars( size );

  int i = 0;
  for( auto j : vars.citer() ) {
    sortedVars[i].key = j.key().cstr();
    sortedVars[i].value = j.value().cstr();
    ++i;
  }
  sortedVars.sort();

  for( int i = 0; i < size; ++i ) {
    s = s + indentString + sortedVars[i].key + " = \"" + sortedVars[i].value + "\"\n";
  }
  return s;
}

}


