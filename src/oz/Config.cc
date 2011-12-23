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

void Config::loadConf( InputStream* istream )
{
  int  lineNum = 0;
  char ch;

  while( istream->isAvailable() ) {
    ++lineNum;

    ch = istream->readChar();

    // skip initial spaces
    while( String::isSpace( ch ) ) {
      ch = istream->readChar();
    }

    // read variable
    String name;

    if( String::isLetter( ch ) || ch == '_' ) {
      line[0] = ch;
      ch = istream->readChar();

      int i = 1;
      while( i < LINE_BUFFER_SIZE - 1 &&
             ( String::isLetter( ch ) || String::isDigit( ch ) || ch == '_' || ch == '.' ) )
      {
        line[i] = ch;
        ch = istream->readChar();
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
      ch = istream->readChar();
    }

    if( ch == '"' ) {
      ch = istream->readChar();

      int i = 0;
      while( i < LINE_BUFFER_SIZE - 1 && ch != '"' && ch != '\n' && ch != EOF ) {
        line[i] = ch;
        ch = istream->readChar();
        ++i;
      }
      if( ch == '"' ) {
        line[i] = '\0';
        include( name, line );
      }
      else {
        throw Exception( "%s:%d: Unterminated value string", filePath.cstr(), lineNum );
      }
    }

  skipLine:;

    // find end of line/file
    while( ch != '\n' && istream->isAvailable() ) {
      ch = istream->readChar();
    }
  }
}

void Config::saveConf( BufferStream* bstream )
{
  log.print( "Writing variables to '%s' ...", filePath.cstr() );

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

  char buffer[1024];

  for( int i = 0; i < size; ++i ) {
    int length = snprintf( buffer, 1024, "%s", sortedVars[i].key );
    bstream->writeChars( buffer, length );

    int chars = ALIGNMENT - String::length( sortedVars[i].key );
    int tabs  = ( chars - 1 ) / 8 + 1;
    for( int j = 0; j < tabs; ++j ) {
      bstream->writeChar( '\t' );
    }

    length = snprintf( buffer, 1024, "\"%s\"\n", sortedVars[i].value );
    bstream->writeChars( buffer, length );
  }

  log.printEnd( " OK" );
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

bool Config::load( File& file )
{
  filePath = file.path();
  if( !file.map() ) {
    return false;
  }

  InputStream istream = file.inputStream();
  loadConf( &istream );

  file.unmap();
  return true;
}

bool Config::load( PhysFile& file )
{
  filePath = file.path();
  if( !file.map() ) {
    return false;
  }

  InputStream istream = file.inputStream();
  loadConf( &istream );

  file.unmap();
  return true;
}

bool Config::save( File& file )
{
  BufferStream bstream;
  saveConf( &bstream );

  return file.write( &bstream );
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
