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
 * @file oz/JSON.cc
 */

#include "JSON.hh"

#include "System.hh"
#include "Log.hh"

#include <stdlib.h>

namespace oz
{

static char skipBlanks( InputStream* is )
{
  char ch;

  do {
    if( !is->isAvailable() ) {
      throw JSON::ParseException();
    }

    ch = is->readChar();
  }
  while( String::isBlank( ch ) );

  return ch;
}

const char* JSON::ParseException::what() const noexcept
{
  return "oz::JSON::ParseException";
}

struct JSON::Data
{};

struct JSON::BooleanData : JSON::Data
{
  bool value;

  explicit BooleanData( bool value_ ) :
    value( value_ )
  {}
};

struct JSON::NumberData : JSON::Data
{
  float value;
  int   intValue;

  explicit NumberData( double value_ ) :
    value( float( value_ ) ), intValue( int( value_ ) )
  {}
};

struct JSON::StringData : JSON::Data
{
  String value;

  explicit StringData( String&& value_ ) :
    value( static_cast<String&&>( value_ ) )
  {}
};

struct JSON::ArrayData : JSON::Data
{
  Vector<JSON> array;
};

struct JSON::ObjectData : JSON::Data
{
  HashString<JSON> table;
};

const JSON JSON::nil;

JSON JSON::parseValue( InputStream* is )
{
  char ch = skipBlanks( is );

  switch( ch ) {
    case 'n': {
      if( is->available() < 3 || is->readChar() != 'u' || is->readChar() != 'l' ||
          is->readChar() != 'l' )
      {
        throw ParseException();
      }

      return JSON( null, NIL );
    }
    case 'f': {
      if( is->available() < 4 || is->readChar() != 'a' || is->readChar() != 'l' ||
          is->readChar() != 's' || is->readChar() != 'e' )
      {
        throw ParseException();
      }

      return JSON( new BooleanData( false ), BOOLEAN );
    }
    case 't': {
      if( is->available() < 4 || is->readChar() != 'r' || is->readChar() != 'u' ||
          is->readChar() != 'e' )
      {
        throw ParseException();
      }

      return JSON( new BooleanData( true ), BOOLEAN );
    }
    default: { // number
      Vector<char> chars;
      chars.add( ch );

      while( is->isAvailable() ) {
        ch = is->readChar();

        if( String::isBlank( ch ) || ch == ',' || ch == '}' || ch == ']' ) {
          is->setPos( is->getPos() - 1 );
          break;
        }
        chars.add( ch );
      }
      chars.add( '\0' );

      char* end;
      double number = strtod( chars, &end );

      if( end != &chars.last() ) {
        throw ParseException();
      }

      return JSON( new NumberData( number ), NUMBER );
    }
    case '"': {
      return JSON( new StringData( parseString( is ) ), STRING );
    }
    case '{': {
      return parseObject( is );
    }
    case '[': {
      return parseArray( is );
    }
  }
}

String JSON::parseString( oz::InputStream* is )
{
  Vector<char> chars;
  char prevChar;
  char ch = '"';

  if( !is->isAvailable() ) {
    throw ParseException();
  }

  do {
    prevChar = ch;
    ch = is->readChar();

    if( ch == '\\' ) {
      continue;
    }
    if( prevChar == '\\' ) {
      switch( ch ) {
        case 'b': {
          ch = '\b';
          break;
        }
        case 'f': {
          ch = '\f';
          break;
        }
        case 'n': {
          ch = '\n';
          break;
        }
        case 'r': {
          ch = '\r';
          break;
        }
        case 't': {
          ch = '\t';
          break;
        }
        default: {
          break;
        }
      }
    }
    else if( ch == '"' ) {
      break;
    }
    chars.add( ch );
  }
  while( is->isAvailable() );

  if( ch != '"' ) {
    throw ParseException();
  }
  chars.add( '\0' );

  return String( chars.length() - 1, chars );
}

JSON JSON::parseArray( InputStream* is )
{
  JSON arrayValue( new ArrayData(), ARRAY );
  Vector<JSON>& array = static_cast<ArrayData*>( arrayValue.data )->array;

  char ch = skipBlanks( is );
  is->setPos( is->getPos() - 1 );

  while( ch != ']' ) {
    JSON value = parseValue( is );
    array.add( static_cast<JSON&&>( value ) );

    ch = skipBlanks( is );

    if( ch != ',' && ch != ']' ) {
      throw ParseException();
    }
  }

  return arrayValue;
}

JSON JSON::parseObject( InputStream* is )
{
  JSON objectValue( new ObjectData(), OBJECT );
  HashString<JSON>& table = static_cast<ObjectData*>( objectValue.data )->table;

  char ch = skipBlanks( is );
  is->setPos( is->getPos() - 1 );

  while( ch != '}' ) {
    ch = skipBlanks( is );
    if( ch != '"' ) {
      throw ParseException();
    }

    String key = parseString( is );

    ch = skipBlanks( is );
    if( ch != ':' ) {
      throw ParseException();
    }

    JSON value = parseValue( is );

    if( key.beginsWith( "//" ) ) {
      value.wasAccessed = true;
    }

    table.add( static_cast<String&&>( key ), static_cast<JSON&&>( value ) );

    ch = skipBlanks( is );

    if( ch != ',' && ch != '}' ) {
      throw ParseException();
    }
  }

  return objectValue;
}

inline JSON::JSON( Data* data_, Type valueType_ ) :
  data( data_ ), valueType( valueType_ ), wasAccessed( false )
{}

inline JSON::JSON() :
  data( null ), valueType( NIL ), wasAccessed( true )
{}

JSON::~JSON()
{
  clear();
}

inline JSON::JSON( JSON&& v ) :
  data( v.data ), valueType( v.valueType ), wasAccessed( v.wasAccessed )
{
  v.data        = null;
  v.valueType   = NIL;
  v.wasAccessed = true;
}

inline JSON& JSON::operator = ( JSON&& v )
{
  clear();

  data        = v.data;
  valueType   = v.valueType;
  wasAccessed = v.wasAccessed;

  v.data        = null;
  v.valueType   = NIL;
  v.wasAccessed = true;

  return *this;
}

int JSON::length() const
{
  switch( valueType ) {
    case ARRAY: {
      return static_cast<const ArrayData*>( data )->array.length();
    }
    case OBJECT: {
      return static_cast<const ObjectData*>( data )->table.length();
    }
    default: {
      return -1;
    }
  }
}

bool JSON::asBool() const
{
  if( valueType == BOOLEAN ) {
    wasAccessed = true;
    return static_cast<BooleanData*>( data )->value;
  }
  else {
    throw Exception( "JSON value %s accessed as a BOOLEAN", toString().cstr() );
  }
}

int JSON::asInt() const
{
  if( valueType == NUMBER ) {
    wasAccessed = true;
    return static_cast<NumberData*>( data )->intValue;
  }
  else {
    throw Exception( "JSON value %s accessed as a NUMBER", toString().cstr() );
  }
}

float JSON::asFloat() const
{
  if( valueType == NUMBER ) {
    wasAccessed = true;
    return static_cast<NumberData*>( data )->value;
  }
  else {
    throw Exception( "JSON value %s accessed as a NUMBER", toString().cstr() );
  }
}

const String& JSON::asString() const
{
  if( valueType == STRING ) {
    wasAccessed = true;
    return static_cast<StringData*>( data )->value;
  }
  else {
    throw Exception( "JSON value %s accessed as a STRING", toString().cstr() );
  }
}

bool JSON::get( bool defaultValue ) const
{
  if( valueType == BOOLEAN ) {
    wasAccessed = true;
    return static_cast<BooleanData*>( data )->value;
  }
  else if( valueType == NIL ) {
    return defaultValue;
  }
  else {
    throw Exception( "JSON value %s accessed as a BOOLEAN", toString().cstr() );
  }
}

int JSON::get( int defaultValue ) const
{
  if( valueType == NUMBER ) {
    wasAccessed = true;
    return static_cast<NumberData*>( data )->intValue;
  }
  else if( valueType == NIL ) {
    return defaultValue;
  }
  else {
    throw Exception( "JSON value %s accessed as a NUMBER", toString().cstr() );
  }
}

float JSON::get( float defaultValue ) const
{
  if( valueType == NUMBER ) {
    wasAccessed = true;
    return static_cast<NumberData*>( data )->value;
  }
  else if( valueType == NIL ) {
    return defaultValue;
  }
  else {
    throw Exception( "JSON value %s accessed as a NUMBER", toString().cstr() );
  }
}

const String& JSON::get( const String& defaultValue ) const
{
  if( valueType == STRING ) {
    wasAccessed = true;
    return static_cast<StringData*>( data )->value;
  }
  else if( valueType == NIL ) {
    return defaultValue;
  }
  else {
    throw Exception( "JSON value %s accessed as a NUMBER", toString().cstr() );
  }
}

const char* JSON::get( const char* defaultValue ) const
{
  if( valueType == STRING ) {
    wasAccessed = true;
    return static_cast<StringData*>( data )->value;
  }
  else if( valueType == NIL ) {
    return defaultValue;
  }
  else {
    throw Exception( "JSON value %s accessed as a STRING", toString().cstr() );
  }
}

const JSON& JSON::operator [] ( int i ) const
{
  if( valueType != ARRAY ) {
    if( valueType == NIL ) {
      return nil;
    }
    throw Exception( "JSON value %s accessed as an ARRAY", toString().cstr() );
  }

  wasAccessed = true;

  const Vector<JSON>& array = static_cast<ArrayData*>( data )->array;

  if( uint( i ) >= uint( array.length() ) ) {
    return nil;
  }

  array[i].wasAccessed = true;
  return array[i];
}

const JSON& JSON::operator [] ( const char* key ) const
{
  if( valueType != OBJECT ) {
    if( valueType == NIL ) {
      return nil;
    }
    throw Exception( "JSON value %s accessed as an OBJECT", toString().cstr() );
  }

  wasAccessed = true;

  const HashString<JSON>& table = static_cast<ObjectData*>( data )->table;
  const JSON* value = table.find( key );

  if( value == null ) {
    return nil;
  }

  value->wasAccessed = true;
  return *value;
}

String JSON::toString() const
{
  switch( valueType ) {
    default:
    case NIL: {
      return "null";
    }
    case BOOLEAN: {
      return String( static_cast<BooleanData*>( data )->value );
    }
    case NUMBER: {
      return String( static_cast<NumberData*>( data )->value );
    }
    case STRING: {
      return String::str( "\"%s\"", static_cast<StringData*>( data )->value.cstr() );
    }
    case ARRAY: {
      const Vector<JSON>& array = static_cast<ArrayData*>( data )->array;

      if( array.isEmpty() ) {
        return "[]";
      }

      String s = "[ ";

      for( int i = 0; i < array.length(); ++i ) {
        if( i != 0 ) {
          s += ", ";
        }

        s += array[i].toString();
      }

      return s + " ]";
    }
    case OBJECT: {
      const HashString<JSON>& table = static_cast<ObjectData*>( data )->table;

      if( table.isEmpty() ) {
        return "{}";
      }

      String s = "{ ";

      bool isFirst = true;
      foreach( i, table.citer() ) {
        s += String::str( isFirst ? "\"%s\": %s" : ", \"%s\": %s",
                          i.key().cstr(), i.value().toString().cstr() );
        isFirst = false;
      }

      return s + " }";
    }
  }
}

bool JSON::load( File file )
{
  if( !file.map() ) {
    return false;
  }

  clear();

  InputStream is = file.inputStream();

  try {
    *this = parseValue( &is );
  }
  catch( const ParseException& ) {
    throw Exception( "Failed to parse JSON file '%s'", file.path().cstr() );
  }
  return true;
}

bool JSON::load( PFile file )
{
  if( !file.map() ) {
    return false;
  }

  clear();

  InputStream is = file.inputStream();

  try {
    *this = parseValue( &is );
  }
  catch( const ParseException& ) {
    throw Exception( "Failed to parse JSON file '%s'", file.path().cstr() );
  }
  return true;
}

void JSON::clear( bool unusedWarnings )
{
  if( unusedWarnings && !wasAccessed ) {
    Log::println( "Unused JSON value %s", toString().cstr() );
    System::bell();
  }

  switch( valueType ) {
    case NIL: {
      hard_assert( data == null );
      break;
    }
    case BOOLEAN: {
      delete static_cast<BooleanData*>( data );
      break;
    }
    case NUMBER: {
      delete static_cast<NumberData*>( data );
      break;
    }
    case STRING: {
      delete static_cast<StringData*>( data );
      break;
    }
    case ARRAY: {
      ArrayData* arrayData = static_cast<ArrayData*>( data );

      if( unusedWarnings ) {
        foreach( i, arrayData->array.iter() ) {
          i->clear( true );
        }
      }

      delete arrayData;
      break;
    }
    case OBJECT: {
      ObjectData* objectData = static_cast<ObjectData*>( data );

      if( unusedWarnings ) {
        foreach( i, objectData->table.iter() ) {
          i.value().clear( true );
        }
      }

      delete objectData;
      break;
    }
  }

  data        = null;
  valueType   = NIL;
  wasAccessed = true;
}

}
