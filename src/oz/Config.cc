/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file oz/Config.cc
 */

#include "Config.hh"

#include "System.hh"
#include "Log.hh"

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
    while( istream->isAvailable() && String::isBlank( ch ) ) {
      ch = istream->readChar();
    }

    // Skip comment.
    if( ch == '#' || ch == '/' ) {
      while( istream->isAvailable() && ch != '\n' ) {
        ch = istream->readChar();
      }
      continue;
    }

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
    while( istream->isAvailable() && ch != '"' ) {
      ch = istream->readChar();
    }
    end = istream->getPos() - 1;

    if( ch != '"' ) {
      throw Exception( "%s:%d: closing '\"' expected", filePath.cstr(), lineNum );
    }

    String value = String( int( end - begin ), begin );

    include( key, value );

    while( istream->isAvailable() ) {
      ch = istream->readChar();

      if( ch == '\n' ) {
        break;
      }
      else if( !String::isSpace( ch ) && ch != '\r' ) {
        throw Exception( "%s:%d: newline expected", filePath.cstr(), lineNum );
      }
    }
  }
}

void Config::saveConf( BufferStream* bstream, const char* lineEnd )
{
  Log::print( "Writing variables to '%s' ...", filePath.cstr() );

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

  Log::printEnd( " OK" );
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

    try {
      return value->text.parseBool();
    }
    catch( const String::ParseException& ) {
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

    try {
      return value->text.parseInt();
    }
    catch( const String::ParseException& ) {
      throw Exception( "Invalid integer value '%s' for %s", value->text.cstr(), key );
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

    try {
      return value->text.parseFloat();
    }
    catch( const String::ParseException& ) {
      throw Exception( "Invalid float value '%s' for %s", value->text.cstr(), key );
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

    try {
      return value->text.parseBool();
    }
    catch( const String::ParseException& ) {
      throw Exception( "Invalid boolean value '%s' for %s", value->text.cstr(), key );
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

    try {
      return value->text.parseInt();
    }
    catch( const String::ParseException& ) {
      throw Exception( "Invalid integer value '%s' for %s", value->text.cstr(), key );
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

    try {
      return value->text.parseFloat();
    }
    catch( const String::ParseException& ) {
      throw Exception( "Invalid float value '%s' for %s", value->text.cstr(), key );
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

bool Config::load( PFile& file )
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

  return file.write( bstream.begin(), bstream.length() );
}

bool Config::save( PFile& file, const char* lineEnd )
{
  BufferStream bstream;
  saveConf( &bstream, lineEnd );

  return file.write( bstream.begin(), bstream.length() );
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
    bool verboseMode = Log::verboseMode;
    Log::verboseMode = false;

    foreach( var, vars.citer() ) {
      if( !var.value().isUsed ) {
        Log::println( "%s: unused variable '%s'", filePath.cstr(), var.key().cstr() );
        System::bell();
      }
    }

    Log::verboseMode = verboseMode;
  }

  vars.clear();
  vars.dealloc();

  filePath = "";
}

String Config::toString( const char* indentString )
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
    s += String::str( "%s%s = \"%s\"\n", indentString, sortedVars[i].key, sortedVars[i].value );
  }
  return s;
}

}
