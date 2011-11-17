/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/Config.cpp
 */

#include "Config.hpp"

#include "System.hpp"
#include "Log.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstdio>

namespace oz
{

Config config;

// Needed when sorting entries for printing.
struct Elem
{
  const char* key;
  const char* value;

  bool operator < ( const Elem& e ) const
  {
    return String::compare( key, e.key ) < 0;
  }
};

bool Config::loadConf( const char* path )
{
#ifndef NDEBUG
  filePath = path;
#endif

  char buffer[BUFFER_SIZE];
  int  lineNum = 1;
  char ch;

  FILE* f = fopen( path, "r" );
  if( f == null ) {
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
      else {
        throw Exception( "%s:%d: Unterminated value string", path, lineNum );
      }
    }

  skipLine:;

    // find end of line/file
    while( ch != '\n' && ch != EOF ) {
      ch = char( fgetc( f ) );
    }

    ++lineNum;
  }
  while( ch != EOF );

  fclose( f );

  return true;
}

bool Config::saveConf( const char* path )
{
  log.print( "Writing variables to '%s' ...", path );

  // first we sort all the variables by key
  int size = vars.length();
  DArray<Elem> sortedVars( size );

  int i = 0;
  foreach( j, vars.citer() ) {
    sortedVars[i].key   = j.key().cstr();
    sortedVars[i].value = j.value().cstr();
    ++i;
  }
  sortedVars.sort();

  FILE* f = fopen( path, "w" );
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
    else if( value->equals( "false" ) ) {
      return false;
    }
    else {
      throw Exception( "Invalid boolean value '%s'", value->cstr() );
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
      throw Exception( "Invalid int value '%s'", value->cstr() );
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
      throw Exception( "Invalid float value '%s'", value->cstr() );
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
    else if( value->equals( "false" ) ) {
      return false;
    }
    else {
      throw Exception( "Invalid boolean value '%s'", value->cstr() );
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
      throw Exception( "Invalid int value '%s'", value->cstr() );
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
      throw Exception( "Invalid float value '%s'", value->cstr() );
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

bool Config::load( const char* path )
{
  const char* suffix = String::findLast( path, '.' );

  if( suffix != null && String::equals( suffix, ".rc" ) ) {
    return loadConf( path );
  }

  log.println( "Unknown configuration file %s", path );
  return false;
}

bool Config::save( const char* path )
{
  const char* suffix = String::findLast( path, '.' );

  if( suffix != null && String::equals( suffix, ".rc" ) ) {
    return saveConf( path );
  }

  log.println( "Unknown configuration file %s", path );
  return false;
}

void Config::clear( bool issueWarnings )
{
#ifndef NDEBUG
  if( issueWarnings ) {
    foreach( var, vars.citer() ) {
      if( !usedVars.contains( var.key() ) ) {
        System::bell();
        log.println( "%s: unused variable '%s'", filePath.cstr(), var.key().cstr() );
      }
    }
  }

  usedVars.clear();
  usedVars.dealloc();

  filePath = "";
#else
  static_cast<void>( issueWarnings );
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
  foreach( j, vars.citer() ) {
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
