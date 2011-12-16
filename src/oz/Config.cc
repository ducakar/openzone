/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2011 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/Config.cc
 */

#include "Config.hh"

#include "System.hh"
#include "Log.hh"

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

// Size of buffer used when loading from file (maximum key/value length).
static const int LINE_BUFFER_SIZE = 1024;

// Internal buffer used during file parsing.
static OZ_THREAD_LOCAL char line[LINE_BUFFER_SIZE];

bool Config::loadConf( const char* path )
{
  filePath = path;

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
      line[0] = ch;
      ch = char( fgetc( f ) );

      int i = 1;
      while( i < LINE_BUFFER_SIZE - 1 &&
             ( String::isLetter( ch ) || String::isDigit( ch ) || ch == '_' || ch == '.' ) )
      {
        line[i] = ch;
        ch = char( fgetc( f ) );
        ++i;
      }
      line[i] = '\0';
      name = line;
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
      while( i < LINE_BUFFER_SIZE - 1 && ch != '"' && ch != '\n' && ch != EOF ) {
        line[i] = ch;
        ch = char( fgetc( f ) );
        ++i;
      }
      if( ch == '"' ) {
        line[i] = '\0';
        include( name, line );
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
    sortedVars[i].value = j.value().text.cstr();
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
  Value* value = vars.find( key );
  if( value != null ) {
    *value = Value( value_ );
  }
  else {
    vars.add( key, Value( value_ ) );
  }
}

void Config::include( const char* key, const char* value_ )
{
  Value* value = vars.find( key );
  if( value == null ) {
    vars.add( key, Value( value_ ) );
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
  const Value* value = vars.find( key );

  if( value != null ) {
    value->isUsed = true;

    if( value->text.equals( "true" ) ) {
      return true;
    }
    else if( value->text.equals( "false" ) ) {
      return false;
    }
    else {
      throw Exception( "Invalid boolean value '%s'", value->text.cstr() );
    }
  }
  else {
    return defVal;
  }
}

int Config::get( const char* key, int defVal ) const
{
  const Value* value = vars.find( key );

  if( value != null ) {
    value->isUsed = true;

    errno = 0;
    char* end;
    int   num = int( strtol( value->text, &end, 0 ) );

    if( errno != 0 || end == value->text.cstr() ) {
      throw Exception( "Invalid int value '%s'", value->text.cstr() );
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
  const Value* value = vars.find( key );

  if( value != null ) {
    value->isUsed = true;

    errno = 0;
    char* end;
    float num = strtof( value->text, &end );

    if( errno != 0 || end == value->text.cstr() ) {
      throw Exception( "Invalid float value '%s'", value->text.cstr() );
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
  const Value* value = vars.find( key );

  if( value != null ) {
    value->isUsed = true;

    return value->text;
  }
  else {
    return defVal;
  }
}

bool Config::getSet( const char* key, bool defVal )
{
  const Value* value = vars.find( key );

  if( value != null ) {
    value->isUsed = true;

    if( value->text.equals( "true" ) ) {
      return true;
    }
    else if( value->text.equals( "false" ) ) {
      return false;
    }
    else {
      throw Exception( "Invalid boolean value '%s'", value->text.cstr() );
    }
  }
  else {
    vars.add( key, Value( String( defVal ), true ) );
    return defVal;
  }
}

int Config::getSet( const char* key, int defVal )
{
  const Value* value = vars.find( key );

  if( value != null ) {
    value->isUsed = true;

    errno = 0;
    char* end;
    int   num = int( strtol( value->text, &end, 0 ) );

    if( errno != 0 || end == value->text.cstr() ) {
      throw Exception( "Invalid int value '%s'", value->text.cstr() );
    }
    else {
      return num;
    }
  }
  else {
    vars.add( key, Value( String( defVal ), true ) );
    return defVal;
  }
}

float Config::getSet( const char* key, float defVal )
{
  const Value* value = vars.find( key );

  if( value != null ) {
    value->isUsed = true;

    errno = 0;
    char* end;
    float num = strtof( value->text, &end );

    if( errno != 0 || end == value->text.cstr() ) {
      throw Exception( "Invalid float value '%s'", value->text.cstr() );
    }
    else {
      return num;
    }
  }
  else {
    vars.add( key, Value( String( defVal ), true ) );
    return defVal;
  }
}

const char* Config::getSet( const char* key, const char* defVal )
{
  const Value* value = vars.find( key );

  if( value != null ) {
    value->isUsed = true;

    return value->text;
  }
  else {
    vars.add( key, Value( defVal, true ) );
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
  if( issueWarnings ) {
    foreach( var, vars.citer() ) {
      if( !var.value().isUsed ) {
        log.println( "%s: unused variable '%s'", filePath.cstr(), var.key().cstr() );
        System::bell();
      }
    }
  }

  vars.clear();
  vars.dealloc();

  filePath = "";
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
    sortedVars[i].value = j.value().text.cstr();
    ++i;
  }
  sortedVars.sort();

  for( int i = 0; i < size; ++i ) {
    s = s + indentString + sortedVars[i].key + " = \"" + sortedVars[i].value + "\"\n";
  }
  return s;
}

}
