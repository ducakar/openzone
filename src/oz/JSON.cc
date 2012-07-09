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

#include "Exception.hh"
#include "System.hh"
#include "Log.hh"

#include <stdlib.h>

#define PARSE_EXCEPTION( message ) \
  throw Exception( "JSON: " message " at %s:%d", pos.path, pos.line );

namespace oz
{

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

class JSON::Parser
{
  private:

    struct Position
    {
      InputStream* istream;
      const char*  path;
      int          line;

      bool isAvailable();
      int  available();
      char readChar();
      void back();
    };

    Position pos;

    Parser( InputStream* istream, const char* file );

    char skipBlanks();
    String parseString();
    JSON parseValue();
    JSON parseArray();
    JSON parseObject();

  public:

    static JSON parse( InputStream* istream, const char* file );

};

inline bool JSON::Parser::Position::isAvailable()
{
  return istream->isAvailable();
}

int JSON::Parser::Position::available()
{
  return istream->available();
}

inline char JSON::Parser::Position::readChar()
{
  if( !istream->isAvailable() ) {
    const Position& pos = *this;
    PARSE_EXCEPTION( "Unexpected end of file" );
  }

  char ch = istream->readChar();

  if( ch == '\n' ) {
    ++line;
  }
  return ch;
}

void JSON::Parser::Position::back()
{
  hard_assert( istream->length() > 0 );

  istream->setPos( istream->getPos() - 1 );
}

JSON::Parser::Parser( InputStream* istream, const char* file ) :
  pos( { istream, file, 1 } )
{}

char JSON::Parser::skipBlanks()
{
  char ch;
  do {
    ch = pos.readChar();
  }
  while( String::isBlank( ch ) );

  return ch;
}

String JSON::Parser::parseString()
{
  Vector<char> chars;
  char prevChar;
  char ch = '"';

  do {
    prevChar = ch;
    ch = pos.readChar();

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
  while( pos.isAvailable() );

  if( ch != '"' ) {
    PARSE_EXCEPTION( "End of string expected" );
  }
  chars.add( '\0' );

  return String( chars.length() - 1, chars );
}

JSON JSON::Parser::parseValue()
{
  char ch = skipBlanks();

  switch( ch ) {
    case 'n': {
      if( pos.available() < 3 || pos.readChar() != 'u' || pos.readChar() != 'l' ||
          pos.readChar() != 'l' )
      {
        PARSE_EXCEPTION( "Unknown value type" );
      }

      return JSON( null, NIL );
    }
    case 'f': {
      if( pos.available() < 4 || pos.readChar() != 'a' || pos.readChar() != 'l' ||
          pos.readChar() != 's' || pos.readChar() != 'e' )
      {
        PARSE_EXCEPTION( "Unknown value type" );
      }

      return JSON( new BooleanData( false ), BOOLEAN );
    }
    case 't': {
      if( pos.available() < 4 || pos.readChar() != 'r' || pos.readChar() != 'u' ||
          pos.readChar() != 'e' )
      {
        PARSE_EXCEPTION( "Unknown value type" );
      }

      return JSON( new BooleanData( true ), BOOLEAN );
    }
    default: { // number
      Vector<char> chars;
      chars.add( ch );

      while( pos.isAvailable() ) {
        ch = pos.readChar();

        if( String::isBlank( ch ) || ch == ',' || ch == '}' || ch == ']' ) {
          pos.back();
          break;
        }
        chars.add( ch );
      }
      chars.add( '\0' );

      char* end;
      double number = strtod( chars, &end );

      if( end != &chars.last() ) {
        PARSE_EXCEPTION( "Unknown value type" );
      }

      return JSON( new NumberData( number ), NUMBER );
    }
    case '"': {
      return JSON( new StringData( parseString() ), STRING );
    }
    case '{': {
      return parseObject();
    }
    case '[': {
      return parseArray();
    }
  }
}

JSON JSON::Parser::parseArray()
{
  JSON arrayValue( new ArrayData(), ARRAY );
  Vector<JSON>& array = static_cast<ArrayData*>( arrayValue.data )->array;

  char ch = skipBlanks();
  pos.back();

  while( ch != ']' ) {
    JSON value = parseValue();
    array.add( static_cast<JSON&&>( value ) );

    ch = skipBlanks();

    if( ch != ',' && ch != ']' ) {
      PARSE_EXCEPTION( "Expected ',' or ']' while parsing array" );
    }
  }

  return arrayValue;
}

JSON JSON::Parser::parseObject()
{
  JSON objectValue( new ObjectData(), OBJECT );
  HashString<JSON>& table = static_cast<ObjectData*>( objectValue.data )->table;

  char ch = skipBlanks();
  pos.back();

  while( ch != '}' ) {
    ch = skipBlanks();
    if( ch != '"' ) {
      PARSE_EXCEPTION( "Expected string key while parsing object" );
    }

    String key = parseString();

    ch = skipBlanks();
    if( ch != ':' ) {
      PARSE_EXCEPTION( "Expected ':' after key in object" );
    }

    JSON value = parseValue();

    if( key.beginsWith( "//" ) ) {
      value.wasAccessed = true;
    }

    table.add( static_cast<String&&>( key ), static_cast<JSON&&>( value ) );

    ch = skipBlanks();

    if( ch != ',' && ch != '}' ) {
      PARSE_EXCEPTION( "Expected ',' or '}' while parsing object" );
    }
  }

  return objectValue;
}

JSON JSON::Parser::parse( InputStream* istream, const char* file )
{
  Parser parser( istream, file );
  return parser.parseValue();
}

const JSON JSON::nil;

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
    throw Exception( "JSON value accessed as a boolean: %s", toString().cstr() );
  }
}

int JSON::asInt() const
{
  if( valueType == NUMBER ) {
    wasAccessed = true;
    return static_cast<NumberData*>( data )->intValue;
  }
  else {
    throw Exception( "JSON value accessed as a number: %s", toString().cstr() );
  }
}

float JSON::asFloat() const
{
  if( valueType == NUMBER ) {
    wasAccessed = true;
    return static_cast<NumberData*>( data )->value;
  }
  else {
    throw Exception( "JSON value accessed as a number: %s", toString().cstr() );
  }
}

const String& JSON::asString() const
{
  if( valueType == STRING ) {
    wasAccessed = true;
    return static_cast<StringData*>( data )->value;
  }
  else {
    throw Exception( "JSON value accessed as a string: %s", toString().cstr() );
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
    throw Exception( "JSON value accessed as a boolean: %s", toString().cstr() );
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
    throw Exception( "JSON value accessed as a number: %s", toString().cstr() );
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
    throw Exception( "JSON value accessed as a number: %s", toString().cstr() );
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
    throw Exception( "JSON value accessed as a string: %s", toString().cstr() );
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
    throw Exception( "JSON value accessed as a string: %s", toString().cstr() );
  }
}

const JSON& JSON::operator [] ( int i ) const
{
  if( valueType != ARRAY ) {
    if( valueType == NIL ) {
      return nil;
    }
    throw Exception( "JSON value accessed as an array: %s", toString().cstr() );
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
    throw Exception( "JSON value accessed as an object: %s", toString().cstr() );
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

  *this = Parser::parse( &is, file.path() );

  file.unmap();
  return true;
}

bool JSON::load( PFile file )
{
  if( !file.map() ) {
    return false;
  }

  clear();

  InputStream is = file.inputStream();

  *this = Parser::parse( &is, file.path() );

  file.unmap();
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
