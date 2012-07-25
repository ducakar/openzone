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

#include "Map.hh"
#include "Exception.hh"
#include "System.hh"
#include "Log.hh"

#include <stdlib.h>

#define PARSE_EXCEPTION( charBias, message ) \
  throw Exception( "JSON: " message " at %s:%d:%d", pos.path, pos.line, pos.column + ( charBias ) );

namespace oz
{

struct JSON::Data
{};

struct JSON::BooleanData : JSON::Data
{
  bool value;

  OZ_HIDDEN
  explicit BooleanData( bool value_ ) :
    value( value_ )
  {}
};

struct JSON::NumberData : JSON::Data
{
  float value;
  int   intValue;

  OZ_HIDDEN
  explicit NumberData( double value_ ) :
    value( float( value_ ) ), intValue( int( value_ ) )
  {}

  OZ_HIDDEN
  explicit NumberData( int value_ ) :
    value( float( value_ ) ), intValue( value_ )
  {}

  OZ_HIDDEN
  explicit NumberData( float value_ ) :
    value( value_ ), intValue( int( value_ ) )
  {}
};

struct JSON::StringData : JSON::Data
{
  String value;

  OZ_HIDDEN
  explicit StringData( const String& value_ ) :
    value( value_ )
  {}

  OZ_HIDDEN
  explicit StringData( String&& value_ ) :
    value( static_cast<String&&>( value_ ) )
  {}

  OZ_HIDDEN
  explicit StringData( const char* value_ ) :
    value( value_ )
  {}
};

struct JSON::ArrayData : JSON::Data
{
  List<JSON> list;
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
      int          column;

      bool isAvailable();
      int available();
      char readChar();
      void back();
    };

    Position pos;

    Parser( InputStream* istream, const char* path );

    char skipBlanks();
    String parseString();
    JSON parseValue();
    JSON parseArray();
    JSON parseObject();

  public:

    void finish();

    static JSON parse( InputStream* istream, const char* path );

};

OZ_HIDDEN
inline bool JSON::Parser::Position::isAvailable()
{
  return istream->isAvailable();
}

OZ_HIDDEN
inline int JSON::Parser::Position::available()
{
  return istream->available();
}

OZ_HIDDEN
char JSON::Parser::Position::readChar()
{
  if( !istream->isAvailable() ) {
    const Position& pos = *this;
    PARSE_EXCEPTION( 0, "Unexpected end of file" );
  }

  char ch = istream->readChar();

  if( ch == '\n' ) {
    ++line;
    column = 0;
  }
  else {
    ++column;
  }
  return ch;
}

OZ_HIDDEN
inline void JSON::Parser::Position::back()
{
  hard_assert( istream->length() > 0 );

  istream->setPos( istream->getPos() - 1 );
}

OZ_HIDDEN
JSON::Parser::Parser( InputStream* istream, const char* path ) :
  pos( { istream, path, 1, 0 } )
{}

OZ_HIDDEN
char JSON::Parser::skipBlanks()
{
  char ch;
  do {
    ch = pos.readChar();
  }
  while( String::isBlank( ch ) );

  return ch;
}

OZ_HIDDEN
String JSON::Parser::parseString()
{
  List<char> chars;
  char ch = '"';

  do {
    ch = pos.readChar();

    if( ch == '\n' || ch == '\r' ) {
      continue;
    }

    if( ch == '\\' ) {
      ch = pos.readChar();

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
    PARSE_EXCEPTION( 0, "End of file while looking for end of string (Is ending \" missing?)" );
  }
  chars.add( '\0' );

  return String( chars.length() - 1, chars );
}

OZ_HIDDEN
JSON JSON::Parser::parseValue()
{
  char ch = skipBlanks();

  switch( ch ) {
    case 'n': {
      if( pos.available() < 3 || pos.readChar() != 'u' || pos.readChar() != 'l' ||
          pos.readChar() != 'l' )
      {
        PARSE_EXCEPTION( -3, "Unknown value type" );
      }

      return JSON( null, NIL );
    }
    case 'f': {
      if( pos.available() < 4 || pos.readChar() != 'a' || pos.readChar() != 'l' ||
          pos.readChar() != 's' || pos.readChar() != 'e' )
      {
        PARSE_EXCEPTION( -4, "Unknown value type" );
      }

      return JSON( new BooleanData( false ), BOOLEAN );
    }
    case 't': {
      if( pos.available() < 4 || pos.readChar() != 'r' || pos.readChar() != 'u' ||
          pos.readChar() != 'e' )
      {
        PARSE_EXCEPTION( -3, "Unknown value type" );
      }

      return JSON( new BooleanData( true ), BOOLEAN );
    }
    default: { // number
      List<char> chars;
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
        PARSE_EXCEPTION( -chars.length(), "Unknown value type" );
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

OZ_HIDDEN
JSON JSON::Parser::parseArray()
{
  JSON arrayValue( new ArrayData(), ARRAY );
  List<JSON>& list = static_cast<ArrayData*>( arrayValue.data )->list;

  char ch = skipBlanks();
  if( ch != ']' ) {
    pos.back();
  }

  while( ch != ']' ) {
    JSON value = parseValue();
    list.add( static_cast<JSON&&>( value ) );

    ch = skipBlanks();

    if( ch != ',' && ch != ']' ) {
      PARSE_EXCEPTION( 0, "Expected ',' or ']' while parsing array (Is ',' is missing?)" );
    }
  }

  return arrayValue;
}

OZ_HIDDEN
JSON JSON::Parser::parseObject()
{
  JSON objectValue( new ObjectData(), OBJECT );
  HashString<JSON>& table = static_cast<ObjectData*>( objectValue.data )->table;

  char ch = skipBlanks();
  if( ch != '}' ) {
    pos.back();
  }

  while( ch != '}' ) {
    ch = skipBlanks();
    if( ch != '"' ) {
      PARSE_EXCEPTION( 0, "Expected key while parsing object (Is there ',' after last entry?)" );
    }

    String key = parseString();

    ch = skipBlanks();
    if( ch != ':' ) {
      PARSE_EXCEPTION( 0, "Expected ':' after key in object entry" );
    }

    JSON value = parseValue();

    if( key.beginsWith( "//" ) ) {
      value.wasAccessed = true;
    }

    table.add( static_cast<String&&>( key ), static_cast<JSON&&>( value ) );

    ch = skipBlanks();

    if( ch != ',' && ch != '}' ) {
      PARSE_EXCEPTION( 0, "Expected ',' or '}' while parsing object entry" );
    }
  }

  return objectValue;
}

OZ_HIDDEN
void JSON::Parser::finish()
{
  while( pos.isAvailable() ) {
    char ch = pos.readChar();

    if( !String::isBlank( ch ) ) {
      PARSE_EXCEPTION( 0, "End of file expected but some content found after" );
    }
  }
}

OZ_HIDDEN
JSON JSON::Parser::parse( InputStream* istream, const char* path )
{
  Parser parser( istream, path );

  JSON root = parser.parseValue();

  parser.finish();
  return root;
}

class JSON::Formatter
{
  public:

    static const int ALIGNMENT_COLUMN = 32;

    BufferStream* ostream;
    const char*   lineEnd;
    int           lineEndLength;
    int           indentLevel;

    int writeString( const String& string );
    void writeValue( const JSON& value );
    void writeArray( const JSON& value );
    void writeObject( const JSON& value );

};

OZ_HIDDEN
int JSON::Formatter::writeString( const String& string )
{
  int length = string.length() + 2;

  ostream->writeChar( '"' );

  for( int i = 0; i < string.length(); ++i ) {
    char ch = string[i];

    switch( ch ) {
      case '\\': {
        ostream->writeChars( "\\\\", 2 );
        ++length;
        break;
      }
      case '"': {
        ostream->writeChars( "\\\"", 2 );
        ++length;
        break;
      }
      case '\b': {
        ostream->writeChars( "\\b", 2 );
        ++length;
        break;
      }
      case '\f': {
        ostream->writeChars( "\\f", 2 );
        ++length;
        break;
      }
      case '\n': {
        ostream->writeChars( "\\n", 2 );
        ++length;
        break;
      }
      case '\r': {
        ostream->writeChars( "\\r", 2 );
        ++length;
        break;
      }
      case '\t': {
        ostream->writeChars( "\\t", 2 );
        ++length;
        break;
      }
      default: {
        ostream->writeChar( ch );
        break;
      }
    }
  }

  ostream->writeChar( '"' );

  return length;
}

OZ_HIDDEN
void JSON::Formatter::writeValue( const JSON& value )
{
  switch( value.valueType ) {
    case NIL: {
      ostream->writeChars( "null", 4 );
      break;
    }
    case BOOLEAN: {
      const BooleanData* booleanData = static_cast<const BooleanData*>( value.data );

      if( booleanData->value ) {
        ostream->writeChars( "true", 4 );
      }
      else {
        ostream->writeChars( "false", 5 );
      }
      break;
    }
    case NUMBER: {
      const NumberData* numberData = static_cast<const NumberData*>( value.data );

      String sNumber = String( numberData->value );
      ostream->writeChars( sNumber, sNumber.length() );
      break;
    }
    case STRING: {
      const StringData* stringData = static_cast<const StringData*>( value.data );

      writeString( stringData->value );
      break;
    }
    case ARRAY: {
      writeArray( value );
      break;
    }
    case OBJECT: {
      writeObject( value );
      break;
    }
  }
}

void JSON::Formatter::writeArray( const JSON& value )
{
  const List<JSON>& list = static_cast<const ArrayData*>( value.data )->list;

  if( list.isEmpty() ) {
    ostream->writeChars( "[]", 2 );
    return;
  }

  ostream->writeChar( '[' );
  ostream->writeChars( lineEnd, lineEndLength );

  ++indentLevel;

  for( int i = 0; i < list.length(); ++i ) {
    if( i != 0 ) {
      ostream->writeChar( ',' );
      ostream->writeChars( lineEnd, lineEndLength );
    }

    for( int j = 0; j < indentLevel; ++j ) {
      ostream->writeChars( "  ", 2 );
    }

    writeValue( list[i] );
  }

  ostream->writeChars( lineEnd, lineEndLength );

  --indentLevel;
  for( int j = 0; j < indentLevel; ++j ) {
    ostream->writeChars( "  ", 2 );
  }

  ostream->writeChar( ']' );
}

void JSON::Formatter::writeObject( const JSON& value )
{
  const HashString<JSON>& table = static_cast<const ObjectData*>( value.data )->table;

  if( table.isEmpty() ) {
    ostream->writeChars( "{}", 2 );
    return;
  }

  ostream->writeChar( '{' );
  ostream->writeChars( lineEnd, lineEndLength );

  ++indentLevel;

  Map<String, const JSON*> sortedEntries;

  foreach( entry, table.citer() ) {
    sortedEntries.add( entry.key(), &entry.value() );
  }

  for( int i = 0; i < sortedEntries.length(); ++i ) {
    if( i != 0 ) {
      ostream->writeChar( ',' );
      ostream->writeChars( lineEnd, lineEndLength );
    }

    for( int j = 0; j < indentLevel; ++j ) {
      ostream->writeChars( "  ", 2 );
    }

    const String& key   = sortedEntries[i];
    const JSON*   value = sortedEntries.value( i );

    int keyLength = writeString( key );
    ostream->writeChar( ':' );

    if( value->valueType == ARRAY || value->valueType == OBJECT ) {
      ostream->writeChars( lineEnd, lineEndLength );

      for( int j = 0; j < indentLevel; ++j ) {
        ostream->writeChars( "  ", 2 );
      }
    }
    else {
      int column = indentLevel * 2 + keyLength + 1;

      // Align to 24-th column.
      for( int i = column; i < ALIGNMENT_COLUMN; ++i ) {
        ostream->writeChar( ' ' );
      }
    }

    writeValue( *value );
  }

  sortedEntries.clear();
  sortedEntries.dealloc();

  ostream->writeChars( lineEnd, lineEndLength );

  --indentLevel;
  for( int j = 0; j < indentLevel; ++j ) {
    ostream->writeChars( "  ", 2 );
  }

  ostream->writeChar( '}' );
}

OZ_HIDDEN
const JSON JSON::nil;

OZ_HIDDEN
inline JSON::JSON( Data* data_, Type valueType_ ) :
  data( data_ ), valueType( valueType_ ), wasAccessed( false )
{}

JSON::JSON() :
  data( null ), valueType( NIL ), wasAccessed( true )
{}

JSON::~JSON()
{
  clear();
}

JSON::JSON( JSON&& v ) :
  data( v.data ), valueType( v.valueType ), wasAccessed( v.wasAccessed )
{
  v.data        = null;
  v.valueType   = NIL;
  v.wasAccessed = true;
}

JSON& JSON::operator = ( JSON&& v )
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
      return static_cast<const ArrayData*>( data )->list.length();
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

const JSON& JSON::operator [] ( int i ) const
{
  if( valueType != ARRAY ) {
    if( valueType == NIL ) {
      return nil;
    }
    throw Exception( "JSON value accessed as an array: %s", toString().cstr() );
  }

  wasAccessed = true;

  const List<JSON>& list = static_cast<ArrayData*>( data )->list;

  if( uint( i ) >= uint( list.length() ) ) {
    return nil;
  }

  list[i].wasAccessed = true;
  return list[i];
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

void JSON::setNull()
{
  clear();
}

void JSON::set( bool value )
{
  clear();

  data      = new BooleanData( value );
  valueType = BOOLEAN;
}

void JSON::set( int value )
{
  clear();

  data      = new NumberData( value );
  valueType = NUMBER;
}

void JSON::set( float value )
{
  clear();

  data      = new NumberData( value );
  valueType = NUMBER;
}

void JSON::set( const String& value )
{
  clear();

  data      = new StringData( value );
  valueType = STRING;
}

void JSON::set( const char* value )
{
  clear();

  data      = new StringData( value );
  valueType = STRING;
}

void JSON::setArray()
{
  clear();

  data      = new ArrayData();
  valueType = ARRAY;
}

void JSON::setObject()
{
  clear();

  data      = new ObjectData();
  valueType = OBJECT;
}

JSON& JSON::addNull()
{
  if( valueType != ARRAY ) {
    throw Exception( "Tried to add a value to a non-array JSON value: %s", toString().cstr() );
  }

  ArrayData* array = static_cast<ArrayData*>( data );

  array->list.add( JSON( null, NIL ) );
  return array->list.last();
}

JSON& JSON::addNull( const char* key )
{
  if( valueType != OBJECT ) {
    throw Exception( "Tried to add a key-value pair '%s' to a non-object JSON value: %s",
                     key, toString().cstr() );
  }

  ObjectData* table = static_cast<ObjectData*>( data );

  return *table->table.add( key, JSON( null, NIL ) );
}

JSON& JSON::includeNull( const char* key )
{
  if( valueType != OBJECT ) {
    throw Exception( "Tried to include a key-value pair '%s' in a non-object JSON value: %s",
                     key, toString().cstr() );
  }

  ObjectData* table = static_cast<ObjectData*>( data );

  JSON* entry = table->table.find( key );

  if( entry == null ) {
    entry = table->table.add( key, JSON( null, NIL ) );
  }

  return *entry;
}

JSON& JSON::include( const char* key, bool value )
{
  if( valueType != OBJECT ) {
    throw Exception( "Tried to include a key-value '%s' pair in a non-object JSON value: %s",
                     key, toString().cstr() );
  }

  ObjectData* table = static_cast<ObjectData*>( data );

  JSON* entry = table->table.find( key );

  if( entry == null ) {
    entry = table->table.add( key, JSON( new BooleanData( value ), BOOLEAN ) );
  }

  return *entry;
}

JSON& JSON::include( const char* key, int value )
{
  if( valueType != OBJECT ) {
    throw Exception( "Tried to include a key-value pair '%s' in a non-object JSON value: %s",
                     key, toString().cstr() );
  }

  ObjectData* table = static_cast<ObjectData*>( data );

  JSON* entry = table->table.find( key );

  if( entry == null ) {
    entry = table->table.add( key, JSON( new NumberData( value ), NUMBER ) );
  }

  return *entry;
}

JSON& JSON::include( const char* key, float value )
{
  if( valueType != OBJECT ) {
    throw Exception( "Tried to include a key-value pair '%s' in a non-object JSON value: %s",
                     key, toString().cstr() );
  }

  ObjectData* table = static_cast<ObjectData*>( data );

  JSON* entry = table->table.find( key );

  if( entry == null ) {
    entry = table->table.add( key, JSON( new NumberData( value ), NUMBER ) );
  }

  return *entry;
}

JSON& JSON::include( const char* key, const String& value )
{
  if( valueType != OBJECT ) {
    throw Exception( "Tried to include a key-value pair '%s' in a non-object JSON value: %s",
                     key, toString().cstr() );
  }

  ObjectData* table = static_cast<ObjectData*>( data );

  JSON* entry = table->table.find( key );

  if( entry == null ) {
    entry = table->table.add( key, JSON( new StringData( value ), STRING ) );
  }

  return *entry;
}

JSON& JSON::include( const char* key, const char* value )
{
  if( valueType != OBJECT ) {
    throw Exception( "Tried to include a key-value pair '%s' in a non-object JSON value: %s",
                     key, toString().cstr() );
  }

  ObjectData* table = static_cast<ObjectData*>( data );

  JSON* entry = table->table.find( key );

  if( entry == null ) {
    entry = table->table.add( key, JSON( new StringData( value ), STRING ) );
  }

  return *entry;
}

JSON& JSON::includeArray( const char* key )
{
  if( valueType != OBJECT ) {
    throw Exception( "Tried to include a key-value pair '%s' in a non-object JSON value: %s",
                     key, toString().cstr() );
  }

  ObjectData* table = static_cast<ObjectData*>( data );

  JSON* entry = table->table.find( key );

  if( entry == null ) {
    entry = table->table.add( key, JSON( new ArrayData(), STRING ) );
  }

  return *entry;
}

JSON& JSON::includeObject( const char* key )
{
  if( valueType != OBJECT ) {
    throw Exception( "Tried to include a key-value pair '%s' in a non-object JSON value: %s",
                     key, toString().cstr() );
  }

  ObjectData* table = static_cast<ObjectData*>( data );

  JSON* entry = table->table.find( key );

  if( entry == null ) {
    entry = table->table.add( key, JSON( new ObjectData(), STRING ) );
  }

  return *entry;
}

bool JSON::remove( int index )
{
  if( valueType != ARRAY ) {
    throw Exception( "Tried to remove a value from a non-array JSON value: %s", toString().cstr() );
  }

  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  if( uint( index ) >= uint( list.length() ) ) {
    return false;
  }

  list.remove( index );
  return true;
}

bool JSON::exclude( const char* key )
{
  if( valueType != OBJECT ) {
    throw Exception( "Tried to exclude and entry form a non-object JSON value: %s",
                     toString().cstr() );
  }

  HashString<JSON>& table = static_cast<ObjectData*>( data )->table;

  return table.exclude( key );
}

void JSON::clear( bool unusedWarnings )
{
  if( unusedWarnings && !wasAccessed ) {
    Log::println( "JSON: unused value: %s", toString().cstr() );
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
        foreach( i, arrayData->list.iter() ) {
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
      const List<JSON>& list = static_cast<ArrayData*>( data )->list;

      if( list.isEmpty() ) {
        return "[]";
      }

      String s = "[ ";

      for( int i = 0; i < list.length(); ++i ) {
        if( i != 0 ) {
          s += ", ";
        }

        s += list[i].toString();
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

void JSON::read( InputStream* istream, const char* path )
{
  *this = Parser::parse( istream, path );
}

void JSON::write( BufferStream* ostream, const char* lineEnd )
{
  Formatter formatter = { ostream, lineEnd, String::length( lineEnd ), 0 };

  formatter.writeValue( *this );
  ostream->writeChars( lineEnd, formatter.lineEndLength );
}

bool JSON::load( File* file )
{
  if( !file->map() ) {
    return false;
  }

  InputStream is = file->inputStream();

  read( &is, file->path() );

  file->unmap();
  return true;
}

bool JSON::load( PFile* file )
{
  if( !file->map() ) {
    return false;
  }

  InputStream is = file->inputStream();

  read( &is, file->path() );

  file->unmap();
  return true;
}

bool JSON::save( File* file, const char* lineEnd )
{
  BufferStream ostream;

  write( &ostream, lineEnd );

  return file->write( ostream.begin(), ostream.length() );
}

bool JSON::save( PFile* file, const char* lineEnd )
{
  BufferStream ostream;

  write( &ostream, lineEnd );

  return file->write( ostream.begin(), ostream.length() );
}

}
