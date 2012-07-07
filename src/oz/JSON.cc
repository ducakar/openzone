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

const char* JSON::TypeException::what() const noexcept
{
  return "oz::JSON::TypeException";
}

const char* JSON::ParseException::what() const noexcept
{
  return "oz::JSON::ParseException";
}

struct JSON::Value::Data
{};

struct JSON::Value::BooleanData : JSON::Value::Data
{
  bool value;

  explicit BooleanData( bool value_ ) :
    value( value_ )
  {}
};

struct JSON::Value::NumberData : JSON::Value::Data
{
  float value;
  int   intValue;

  explicit NumberData( double value_ ) :
    value( float( value_ ) ), intValue( int( value_ ) )
  {}
};

struct JSON::Value::StringData : JSON::Value::Data
{
  String value;

  explicit StringData( String&& value_ ) :
    value( static_cast<String&&>( value_ ) )
  {}
};

struct JSON::Value::ArrayData : JSON::Value::Data
{
  Vector<Value> array;
};

struct JSON::Value::ObjectData : JSON::Value::Data
{
  HashString<Value> table;
};

const JSON::Value JSON::Value::nil;

JSON::Value JSON::Value::parseValue( InputStream* is )
{
  char ch = skipBlanks( is );

  switch( ch ) {
    case 'n': {
      if( is->available() < 3 || is->readChar() != 'u' || is->readChar() != 'l' ||
          is->readChar() != 'l' )
      {
        throw ParseException();
      }

      return Value( null, NIL );
    }
    case 'f': {
      if( is->available() < 4 || is->readChar() != 'a' || is->readChar() != 'l' ||
          is->readChar() != 's' || is->readChar() != 'e' )
      {
        throw ParseException();
      }

      return Value( new BooleanData( false ), BOOLEAN );
    }
    case 't': {
      if( is->available() < 4 || is->readChar() != 'r' || is->readChar() != 'u' ||
          is->readChar() != 'e' )
      {
        throw ParseException();
      }

      return Value( new BooleanData( true ), BOOLEAN );
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

      return Value( new NumberData( number ), NUMBER );
    }
    case '"': {
      return Value( new StringData( parseString( is ) ), STRING );
    }
    case '{': {
      return parseObject( is );
    }
    case '[': {
      return parseArray( is );
    }
  }
}

String JSON::Value::parseString( oz::InputStream* is )
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

JSON::Value JSON::Value::parseArray( InputStream* is )
{
  Value arrayValue( new ArrayData(), ARRAY );
  Vector<Value>& array = static_cast<ArrayData*>( arrayValue.data )->array;

  char ch = skipBlanks( is );
  is->setPos( is->getPos() - 1 );

  while( ch != ']' ) {
    Value value = parseValue( is );
    array.add( static_cast<Value&&>( value ) );

    ch = skipBlanks( is );

    if( ch != ',' && ch != ']' ) {
      throw ParseException();
    }
  }

  return arrayValue;
}

JSON::Value JSON::Value::parseObject( InputStream* is )
{
  Value objectValue( new ObjectData(), OBJECT );
  HashString<Value>& table = static_cast<ObjectData*>( objectValue.data )->table;

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

    Value value = parseValue( is );
    table.add( static_cast<String&&>( key ), static_cast<Value&&>( value ) );

    ch = skipBlanks( is );

    if( ch != ',' && ch != '}' ) {
      throw ParseException();
    }
  }

  return objectValue;
}

inline JSON::Value::Value( Data* data_, Type valueType_ ) :
  data( data_ ), valueType( valueType_ )
{}

inline JSON::Value::Value() :
  data( null ), valueType( NIL )
{}

JSON::Value::~Value()
{
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
    case OBJECT: {
      delete static_cast<ObjectData*>( data );
      break;
    }
    case ARRAY: {
      delete static_cast<ArrayData*>( data );
      break;
    }
  }
}

inline JSON::Value::Value( Value&& v ) :
  data( v.data ), valueType( v.valueType )
{
  v.data      = null;
  v.valueType = NIL;
}

inline JSON::Value& JSON::Value::operator = ( Value&& v )
{
  this->~Value();

  data      = v.data;
  valueType = v.valueType;

  v.data      = null;
  v.valueType = NIL;

  return *this;
}

int JSON::Value::length() const
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

bool JSON::Value::asBool() const
{
  if( valueType == BOOLEAN ) {
    return static_cast<BooleanData*>( data )->value;
  }
  else {
    throw TypeException();
  }
}

int JSON::Value::asInt() const
{
  if( valueType == NUMBER ) {
    return static_cast<NumberData*>( data )->intValue;
  }
  else {
    throw TypeException();
  }
}

float JSON::Value::asFloat() const
{
  if( valueType == NUMBER ) {
    return static_cast<NumberData*>( data )->value;
  }
  else {
    throw TypeException();
  }
}

const String JSON::Value::asString() const
{
  if( valueType == STRING ) {
    return static_cast<StringData*>( data )->value;
  }
  else {
    throw TypeException();
  }
}

bool JSON::Value::get( bool defaultValue ) const
{
  if( valueType == BOOLEAN ) {
    return static_cast<BooleanData*>( data )->value;
  }
  else if( valueType == NIL ) {
    return defaultValue;
  }
  else {
    throw TypeException();
  }
}

int JSON::Value::get( int defaultValue ) const
{
  if( valueType == NUMBER ) {
    return static_cast<NumberData*>( data )->intValue;
  }
  else if( valueType == NIL ) {
    return defaultValue;
  }
  else {
    throw TypeException();
  }
}

float JSON::Value::get( float defaultValue ) const
{
  if( valueType == NUMBER ) {
    return static_cast<NumberData*>( data )->value;
  }
  else if( valueType == NIL ) {
    return defaultValue;
  }
  else {
    throw TypeException();
  }
}

const String& JSON::Value::get( const String& defaultValue ) const
{
  if( valueType == STRING ) {
    return static_cast<StringData*>( data )->value;
  }
  else if( valueType == NIL ) {
    return defaultValue;
  }
  else {
    throw TypeException();
  }
}

const char* JSON::Value::get( const char* defaultValue ) const
{
  if( valueType == STRING ) {
    return static_cast<StringData*>( data )->value;
  }
  else if( valueType == NIL ) {
    return defaultValue;
  }
  else {
    throw TypeException();
  }
}

const JSON::Value& JSON::Value::operator [] ( int i ) const
{
  if( valueType != ARRAY ) {
    throw TypeException();
  }

  const Vector<Value>& array = static_cast<ArrayData*>( data )->array;

  if( uint( i ) >= uint( array.length() ) ) {
    return nil;
  }

  return array[i];
}

const JSON::Value& JSON::Value::operator[]( const char* key ) const
{
  if( valueType != OBJECT ) {
    throw TypeException();
  }

  const HashString<Value>& table = static_cast<ObjectData*>( data )->table;
  const Value* value = table.find( key );

  if( value == null ) {
    return nil;
  }

  return *value;
}

String JSON::Value::toString() const
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
      const Vector<Value>& array = static_cast<ArrayData*>( data )->array;

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
      const HashString<Value>& table = static_cast<ObjectData*>( data )->table;

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

  InputStream is = file.inputStream();

  try {
    root = Value::parseValue( &is );
  }
  catch( const ParseException& ) {
    return false;
  }
  return true;
}

bool JSON::load( PFile file )
{
  if( !file.map() ) {
    return false;
  }

  InputStream is = file.inputStream();

  try {
    root = Value::parseValue( &is );
  }
  catch( const ParseException& ) {
    return false;
  }
  return true;
}

void JSON::clear()
{
  root = Value();
}

}
