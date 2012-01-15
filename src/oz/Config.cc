/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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

void Config::loadConf( InputStream* istream )
{
  int  lineNum = 0;

  while( istream->isAvailable() ) {
    ++lineNum;

    char ch = istream->readChar();

    // Skip initial spaces.
    while( istream->isAvailable() && ( String::isSpace( ch ) || ch == '\n' ) ) {
      ch = istream->readChar();
    }

    // Skip comment.
    if( ch == '#' || ch == '/' ) {
      goto skipLine;
    }

    {
      // Key.
      const char* begin = istream->getPos() - 1;
      while( istream->isAvailable() &&
          ( String::isLetter( ch ) || String::isDigit( ch ) || ch == '_' || ch == '.' ) )
      {
        ch = istream->readChar();
      }
      const char* end = istream->getPos() - 1;

      if( begin == end ) {
        throw Exception( "%s:%d: Key expected", filePath.cstr(), lineNum );
      }

      String key = String( int( end - begin ), begin );

      // Spaces between name and value.
      while( istream->isAvailable() && String::isSpace( ch ) ) {
        ch = istream->readChar();
      }

      // Value.
      if( ch != '"' ) {
        throw Exception( "%s:%d: opening '\"' expected", filePath.cstr(), lineNum );
      }

      if( !istream->isAvailable() ) {
        throw Exception( "%s:%d: unexpected end of file", filePath.cstr(), lineNum );
      }

      ch = istream->readChar();

      begin = istream->getPos() - 1;
      while( istream->isAvailable() && ch != '"' && ch != '\n' ) {
        ch = istream->readChar();
      }
      end = istream->getPos() - 1;

      if( ch != '"' ) {
        throw Exception( "%s:%d: closing '\"' expected", filePath.cstr(), lineNum );
      }

      String value = String( int( end - begin ), begin );

      include( key, value );
    }

  skipLine:

    // End of line/file.
    while( istream->isAvailable() && ch != '\n' ) {
      ch = istream->readChar();
    }
  }
}

void Config::saveConf( BufferStream* bstream, const char* lineEnd )
{
  log.print( "Writing variables to '%s' ...", filePath.cstr() );

  // Sort all the variables by key.
  int size = vars.length();
  DArray<Elem> sortedVars( size );

  int i = 0;
  foreach( j, vars.citer() ) {
    sortedVars[i].key   = j.key().cstr();
    sortedVars[i].value = j.value().text.cstr();
    ++i;
  }
  sortedVars.sort();

  for( int i = 0; i < size; ++i ) {
    bstream->writeChars( sortedVars[i].key, String::length( sortedVars[i].key ) );

    int chars = ALIGNMENT - String::length( sortedVars[i].key );
    int tabs  = ( chars - 1 ) / 8 + 1;
    for( int j = 0; j < tabs; ++j ) {
      bstream->writeChar( '\t' );
    }

    bstream->writeChar( '"' );
    bstream->writeChars( sortedVars[i].value, String::length( sortedVars[i].value ) );
    bstream->writeChar( '"' );
    bstream->writeChars( lineEnd, String::length( lineEnd ) );
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
      throw Exception( "Invalid boolean value '%s' for %s", value->text.cstr(), key );
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
    int num = int( strtol( value->text, &end, 0 ) );

    if( errno != 0 || end == value->text.cstr() ) {
      throw Exception( "Invalid int value '%s' for %s", value->text.cstr(), key );
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
      throw Exception( "Invalid float value '%s' for %s", value->text.cstr(), key );
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
    int num = int( strtol( value->text, &end, 0 ) );

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

bool Config::save( File& file, const char* lineEnd )
{
  BufferStream bstream;
  saveConf( &bstream, lineEnd );

  return file.write( &bstream );
}

void Config::removeUnused()
{
  for( auto i = vars.citer(); i.isValid(); ) {
    auto var = i;
    ++i;

    if( !var.value().isUsed ) {
      vars.exclude( var.key() );
    }
  }
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

  // Sort all the variables by key.
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
