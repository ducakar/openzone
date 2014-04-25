/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/JSON.cc
 */

#include "JSON.hh"

#include "List.hh"
#include "SList.hh"
#include "Map.hh"
#include "Log.hh"

#define OZ_PARSE_ERROR( charBias, message ) \
  OZ_ERROR( "oz::JSON: " message " at %s:%d:%d", path, line, column + ( charBias ) );

namespace oz
{

static const JSON NIL_VALUE;

struct JSON::Data
{};

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
  HashMap<String, JSON> table;
};

struct JSON::Parser
{
  enum BlanksMode
  {
    WHITESPACE,
    LINE_COMMENT,
    MULTILINE_COMMENT
  };

  InputStream* is;
  const char*  path;
  int          line;
  int          column;
  int          oldLine;
  int          oldColumn;

  OZ_HIDDEN
  char readChar()
  {
    if( !is->isAvailable() ) {
      OZ_PARSE_ERROR( 0, "Unexpected end of file" );
    }

    oldLine   = line;
    oldColumn = column;

    char ch = is->readChar();

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
  void backChar()
  {
    hard_assert( line != oldLine || column != oldColumn );

    is->set( is->pos() - 1 );

    line   = oldLine;
    column = oldColumn;
  }

  OZ_HIDDEN
  static void setAccessed( JSON* value )
  {
    value->wasAccessed = true;

    switch( value->valueType ) {
      default: {
        break;
      }
      case ARRAY: {
        List<JSON>& list = static_cast<ArrayData*>( value->data )->list;

        for( JSON& i : list ) {
          setAccessed( &i );
        }
        break;
      }
      case OBJECT: {
        HashMap<String, JSON>& table = static_cast<ObjectData*>( value->data )->table;

        for( auto& i : table ) {
          setAccessed( &i.value );
        }
        break;
      }
    }
  }

  OZ_HIDDEN
  static JSON parse( InputStream* is, const char* path )
  {
    Parser parser( is, path );

    JSON root = parser.parseValue();

    parser.finish();
    return root;
  }

  OZ_HIDDEN
  explicit Parser( InputStream* is_, const char* path_ ) :
    is( is_ ), path( path_ ), line( 1 ), column( 0 ), oldLine( 1 ), oldColumn( 0 )
  {}

  OZ_HIDDEN
  char skipBlanks()
  {
    char ch1, ch2;

    do {
      do {
        ch2 = readChar();
      }
      while( String::isBlank( ch2 ) );

      if( ch2 == '/' ) {
        ch1 = ch2;
        ch2 = readChar();

        if( ch2 == '/' ) {
          // Skip a line comment.
          do {
            ch2 = readChar();
          }
          while( ch2 != '\n' );

          continue;
        }
        else if( ch2 == '*' ) {
          // Skip a multi-line comment.
          ch2 = readChar();

          do {
            ch1 = ch2;
            ch2 = readChar();
          }
          while( ch1 != '*' || ch2 != '/' );

          continue;
        }
        else {
          ch2 = ch1;
          backChar();
        }
      }

      return ch2;
    }
    while( true );
  }

  OZ_HIDDEN
  String parseString()
  {
    List<char> chars;
    char ch = '"';

    do {
      ch = readChar();

      if( ch == '\n' || ch == '\r' ) {
        continue;
      }

      if( ch == '\\' ) {
        ch = readChar();

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
      OZ_PARSE_ERROR( 0, "End of file while looking for end of string (Is ending \" missing?)" );
    }
    chars.add( '\0' );

    return String( chars.begin(), chars.length() - 1 );
  }

  OZ_HIDDEN
  JSON parseValue()
  {
    char ch = skipBlanks();

    switch( ch ) {
      case 'n': {
        if( is->available() < 3 || readChar() != 'u' || readChar() != 'l' || readChar() != 'l' ) {
          OZ_PARSE_ERROR( -3, "Unknown value type" );
        }

        return JSON( nullptr, NIL );
      }
      case 'f': {
        if( is->available() < 4 || readChar() != 'a' || readChar() != 'l' || readChar() != 's' ||
            readChar() != 'e' )
        {
          OZ_PARSE_ERROR( -4, "Unknown value type" );
        }

        return JSON( false );
      }
      case 't': {
        if( is->available() < 4 || readChar() != 'r' || readChar() != 'u' || readChar() != 'e' ) {
          OZ_PARSE_ERROR( -3, "Unknown value type" );
        }

        return JSON( true );
      }
      default: { // Number.
        SList<char, 32> chars;
        chars.add( ch );

        while( is->isAvailable() ) {
          ch = readChar();

          if( String::isBlank( ch ) || ch == ',' || ch == '}' || ch == ']' ) {
            backChar();
            break;
          }
          if( chars.length() >= 31 ) {
            OZ_PARSE_ERROR( -chars.length(), "Too long number" );
          }
          chars.add( ch );
        }
        chars.add( '\0' );

        const char* end;
        double number = String::parseDouble( chars.begin(), &end );

        if( end == chars.begin() ) {
          OZ_PARSE_ERROR( -chars.length(), "Unknown value type" );
        }

        return JSON( number );
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
  JSON parseArray()
  {
    JSON arrayValue( new ArrayData(), ARRAY );
    List<JSON>& list = static_cast<ArrayData*>( arrayValue.data )->list;

    char ch = skipBlanks();
    if( ch != ']' ) {
      backChar();
    }

    while( ch != ']' ) {
      JSON value = parseValue();
      list.add( static_cast<JSON&&>( value ) );

      ch = skipBlanks();

      if( ch != ',' && ch != ']' ) {
        OZ_PARSE_ERROR( 0, "Expected ',' or ']' while parsing array (Is ',' is missing?)" );
      }
    }

    return arrayValue;
  }

  OZ_HIDDEN
  JSON parseObject()
  {
    JSON objectValue( new ObjectData(), OBJECT );
    HashMap<String, JSON>& table = static_cast<ObjectData*>( objectValue.data )->table;

    char ch = skipBlanks();
    if( ch != '}' ) {
      backChar();
    }

    while( ch != '}' ) {
      ch = skipBlanks();
      if( ch != '"' ) {
        OZ_PARSE_ERROR( 0, "Expected key while parsing object (Is there ',' after last entry?)" );
      }

      String key = parseString();

      ch = skipBlanks();
      if( ch != ':' ) {
        OZ_PARSE_ERROR( 0, "Expected ':' after key in object entry" );
      }

      JSON value = parseValue();
      table.add( static_cast<String&&>( key ), static_cast<JSON&&>( value ) );

      ch = skipBlanks();

      if( ch != ',' && ch != '}' ) {
        OZ_PARSE_ERROR( 0, "Expected ',' or '}' while parsing object entry" );
      }
    }

    return objectValue;
  }

  OZ_HIDDEN
  void finish()
  {
    while( is->isAvailable() ) {
      char ch = readChar();

      if( !String::isBlank( ch ) ) {
        OZ_PARSE_ERROR( 0, "End of file expected but some content found after" );
      }
    }
  }
};

struct JSON::Formatter
{
  OutputStream* os;
  const Format* format;
  int           lineEndLength;
  int           indentLevel;

  OZ_HIDDEN
  int writeString( const String& string )
  {
    int length = string.length() + 2;

    os->writeChar( '"' );

    for( int i = 0; i < string.length(); ++i ) {
      char ch = string[i];

      switch( ch ) {
        case '\\': {
          os->writeChars( "\\\\", 2 );
          ++length;
          break;
        }
        case '"': {
          os->writeChars( "\\\"", 2 );
          ++length;
          break;
        }
        case '\b': {
          os->writeChars( "\\b", 2 );
          ++length;
          break;
        }
        case '\f': {
          os->writeChars( "\\f", 2 );
          ++length;
          break;
        }
        case '\n': {
          os->writeChars( "\\n", 2 );
          ++length;
          break;
        }
        case '\r': {
          os->writeChars( "\\r", 2 );
          ++length;
          break;
        }
        case '\t': {
          os->writeChars( "\\t", 2 );
          ++length;
          break;
        }
        default: {
            os->writeChar( ch );
          break;
        }
      }
    }

    os->writeChar( '"' );

    return length;
  }

  OZ_HIDDEN
  void writeValue( const JSON& value )
  {
    switch( value.valueType ) {
      case NIL: {
        os->writeChars( "null", 4 );
        break;
      }
      case BOOLEAN: {
        if( value.boolean ) {
          os->writeChars( "true", 4 );
        }
        else {
          os->writeChars( "false", 5 );
        }
        break;
      }
      case NUMBER: {
        String s = String( value.number, format->significantDigits );
        os->writeChars( s, s.length() );
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

  OZ_HIDDEN
  void writeArray( const JSON& value )
  {
    const List<JSON>& list = static_cast<const ArrayData*>( value.data )->list;

    if( list.isEmpty() ) {
      os->writeChars( "[]", 2 );
      return;
    }

    os->writeChar( '[' );
    os->writeChars( format->lineEnd, lineEndLength );

    ++indentLevel;

    for( int i = 0; i < list.length(); ++i ) {
      if( i != 0 ) {
        os->writeChar( ',' );
        os->writeChars( format->lineEnd, lineEndLength );
      }

      for( int j = 0; j < indentLevel; ++j ) {
        os->writeChars( "  ", 2 );
      }

      writeValue( list[i] );
    }

    os->writeChars( format->lineEnd, lineEndLength );

    --indentLevel;
    for( int j = 0; j < indentLevel; ++j ) {
      os->writeChars( "  ", 2 );
    }

    os->writeChar( ']' );
  }

  OZ_HIDDEN
  void writeObject( const JSON& value )
  {
    const HashMap<String, JSON>& table = static_cast<const ObjectData*>( value.data )->table;

    if( table.isEmpty() ) {
      os->writeChars( "{}", 2 );
      return;
    }

    os->writeChar( '{' );
    os->writeChars( format->lineEnd, lineEndLength );

    ++indentLevel;

    Map<String, const JSON*> sortedEntries;

    for( const auto& entry : table ) {
      sortedEntries.add( entry.key, &entry.value );
    }

    for( int i = 0; i < sortedEntries.length(); ++i ) {
      if( i != 0 ) {
        os->writeChar( ',' );
        os->writeChars( format->lineEnd, lineEndLength );
      }

      for( int j = 0; j < indentLevel; ++j ) {
        os->writeChars( "  ", 2 );
      }

      const String& entryKey   = sortedEntries[i].key;
      const JSON*   entryValue = sortedEntries[i].value;

      int keyLength = writeString( entryKey );
      os->writeChar( ':' );

      if( entryValue->valueType == ARRAY || entryValue->valueType == OBJECT ) {
        os->writeChars( format->lineEnd, lineEndLength );

        for( int j = 0; j < indentLevel; ++j ) {
          os->writeChars( "  ", 2 );
        }
      }
      else {
        int column = indentLevel * 2 + keyLength + 1;

        // Align to 24-th column.
        for( int j = column; j < format->alignmentColumn; ++j ) {
          os->writeChar( ' ' );
        }
      }

      writeValue( *entryValue );
    }

    sortedEntries.clear();
    sortedEntries.deallocate();

    os->writeChars( format->lineEnd, lineEndLength );

    --indentLevel;
    for( int j = 0; j < indentLevel; ++j ) {
      os->writeChars( "  ", 2 );
    }

    os->writeChar( '}' );
  }
};

const JSON::Format JSON::DEFAULT_FORMAT = { 2, 32, 9, "\n" };

OZ_HIDDEN
JSON::JSON( Data* data_, Type valueType_ ) :
  data( data_ ), valueType( valueType_ ), wasAccessed( false )
{}

OZ_HIDDEN
bool JSON::getVector( float* vector, int count ) const
{
  if( valueType != ARRAY ) {
    return false;
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    return false;
  }

  for( int i = 0; i < count; ++i ) {
    vector[i] = float( list[i].get( 0.0 ) );
  }
  return true;
}

JSON::JSON( Type type ) :
  number( 0.0 ), valueType( type ), wasAccessed( false )
{
  switch( type ) {
    default: {
      break;
    }
    case STRING: {
      data = new StringData( "" );
      break;
    }
    case ARRAY: {
      data = new ArrayData();
      break;
    }
    case OBJECT: {
      data = new ObjectData();
      break;
    }
  }
}

JSON::JSON( nullptr_t ) :
  number( 0.0 ), valueType( NIL ), wasAccessed( false )
{}

JSON::JSON( bool value ) :
  boolean( value ), valueType( BOOLEAN ), wasAccessed( false )
{}

JSON::JSON( int value ) :
  number( value ), valueType( NUMBER ), wasAccessed( false )
{}

JSON::JSON( float value ) :
  number( value ), valueType( NUMBER ), wasAccessed( false )
{}

JSON::JSON( double value ) :
  number( value ), valueType( NUMBER ), wasAccessed( false )
{}

JSON::JSON( const String& value ) :
  data( new StringData( value ) ), valueType( STRING ), wasAccessed( false )
{}

JSON::JSON( const char* value ) :
  data( new StringData( value ) ), valueType( STRING ), wasAccessed( false )
{}

JSON::JSON( const Vec3& v ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( false )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 3; ++i ) {
    list.add( JSON( v[i] ) );
  }
}

JSON::JSON( const Vec4& v ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( false )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 4; ++i ) {
    list.add( JSON( v[i] ) );
  }
}

JSON::JSON( const Point& p ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( false )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 3; ++i ) {
    list.add( JSON( p[i] ) );
  }
}

JSON::JSON( const Plane& p ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( false )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 3; ++i ) {
    list.add( JSON( p.n[i] ) );
  }
  list.add( JSON( p.d ) );
}

JSON::JSON( const Quat& q ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( false )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 4; ++i ) {
    list.add( JSON( q[i] ) );
  }
}

JSON::JSON( const Mat3& m ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( false )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 3; ++i ) {
    for( int j = 0; j < 3; ++j ) {
      list.add( JSON( m[i][j] ) );
    }
  }
}

JSON::JSON( const Mat4& m ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( false )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 4; ++i ) {
    for( int j = 0; j < 4; ++j ) {
      list.add( JSON( m[i][j] ) );
    }
  }
}

JSON::JSON( const File& file ) :
  valueType( NIL ), wasAccessed( false )
{
  load( file );
}

JSON::~JSON()
{
  clear();
}

JSON::JSON( const JSON& v ) :
  valueType( v.valueType ), wasAccessed( v.wasAccessed )
{
  switch( valueType ) {
    default: {
      break;
    }
    case BOOLEAN: {
      boolean = v.boolean;
      break;
    }
    case NUMBER: {
      number = v.number;
      break;
    }
    case STRING: {
      data = new StringData( *static_cast<const StringData*>( v.data ) );
      break;
    }
    case ARRAY: {
      data = new ArrayData( *static_cast<const ArrayData*>( v.data ) );
      break;
    }
    case OBJECT: {
      data = new ObjectData( *static_cast<const ObjectData*>( v.data ) );
      break;
    }
  }
}

JSON::JSON( JSON&& v ) :
  valueType( v.valueType ), wasAccessed( v.wasAccessed )
{
  boolean = v.boolean;
  number  = v.number;
  data    = v.data;

  v.valueType   = NIL;
  v.wasAccessed = true;
}

JSON& JSON::operator = ( const JSON& v )
{
  if( &v == this ) {
    return *this;
  }

  clear();

  valueType   = v.valueType;
  wasAccessed = v.wasAccessed;

  switch( valueType ) {
    case NIL: {
      data = nullptr;
      break;
    }
    case BOOLEAN: {
      boolean = v.boolean;
      break;
    }
    case NUMBER: {
      number = v.number;
      break;
    }
    case STRING: {
      data = new StringData( *static_cast<const StringData*>( v.data ) );
      break;
    }
    case ARRAY: {
      data = new ArrayData( *static_cast<const ArrayData*>( v.data ) );
      break;
    }
    case OBJECT: {
      data = new ObjectData( *static_cast<const ObjectData*>( v.data ) );
      break;
    }
  }

  return *this;
}

JSON& JSON::operator = ( JSON&& v )
{
  if( &v == this ) {
    return *this;
  }

  clear();

  boolean     = v.boolean;
  number      = v.number;
  data        = v.data;
  valueType   = v.valueType;
  wasAccessed = v.wasAccessed;

  v.valueType   = NIL;
  v.wasAccessed = true;

  return *this;
}

JSON::ArrayCIterator JSON::arrayCIter() const
{
  if( valueType == ARRAY ) {
    const ArrayData* arrayData = static_cast<const ArrayData*>( data );

    wasAccessed = true;
    return ArrayCIterator( arrayData->list.begin(), arrayData->list.end() );
  }
  else {
    return ArrayCIterator();
  }
}

JSON::ArrayIterator JSON::arrayIter()
{
  if( valueType == ARRAY ) {
    ArrayData* arrayData = static_cast<ArrayData*>( data );

    wasAccessed = true;
    return ArrayIterator( arrayData->list.begin(), arrayData->list.end() );
  }
  else {
    return ArrayIterator();
  }
}

JSON::ObjectCIterator JSON::objectCIter() const
{
  if( valueType == OBJECT ) {
    const ObjectData* objectData = static_cast<const ObjectData*>( data );

    wasAccessed = true;
    return objectData->table.citer();
  }
  else {
    return ObjectCIterator();
  }
}

JSON::ObjectIterator JSON::objectIter()
{
  if( valueType == OBJECT ) {
    ObjectData* objectData = static_cast<ObjectData*>( data );

    wasAccessed = true;
    return objectData->table.iter();
  }
  else {
    return ObjectIterator();
  }
}

int JSON::length() const
{
  switch( valueType ) {
    default: {
      return -1;
    }
    case ARRAY: {
      const ArrayData* arrayData = static_cast<const ArrayData*>( data );

      wasAccessed = true;
      return arrayData->list.length();
    }
    case OBJECT: {
      const ObjectData* objectData = static_cast<const ObjectData*>( data );

      wasAccessed = true;
      return objectData->table.length();
    }
  }
}

const JSON& JSON::operator [] ( int i ) const
{
  if( valueType != ARRAY ) {
    return NIL_VALUE;
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  wasAccessed = true;

  if( uint( i ) >= uint( list.length() ) ) {
    return NIL_VALUE;
  }

  list[i].wasAccessed = true;
  return list[i];
}

const JSON& JSON::operator [] ( const char* key ) const
{
  if( valueType != OBJECT ) {
    return NIL_VALUE;
  }

  const HashMap<String, JSON>& table = static_cast<const ObjectData*>( data )->table;
  const JSON* value = table.find( key );

  wasAccessed = true;

  if( value == nullptr ) {
    return NIL_VALUE;
  }

  value->wasAccessed = true;
  return *value;
}

bool JSON::contains( const char* key ) const
{
  if( valueType != OBJECT ) {
    return false;
  }

  const HashMap<String, JSON>& table = static_cast<const ObjectData*>( data )->table;
  const JSON* value = table.find( key );

  wasAccessed = true;

  if( value == nullptr ) {
    return false;
  }

  value->wasAccessed = true;
  return true;
}

bool JSON::get( bool defaultValue ) const
{
  if( valueType != BOOLEAN ) {
    return defaultValue;
  }

  wasAccessed = true;
  return boolean;
}

double JSON::get( double defaultValue ) const
{
  if( valueType != NUMBER ) {
    return defaultValue;
  }

  wasAccessed = true;
  return number;
}

const String& JSON::get( const String& defaultValue ) const
{
  if( valueType != STRING ) {
    return defaultValue;
  }

  wasAccessed = true;
  return static_cast<const StringData*>( data )->value;
}

const char* JSON::get( const char* defaultValue ) const
{
  if( valueType != STRING ) {
    return defaultValue;
  }

  wasAccessed = true;
  return static_cast<const StringData*>( data )->value.cstr();
}

Vec3 JSON::get( const Vec3& defaultValue ) const
{
  Vec3 v;
  return getVector( v, 3 ) ? v : defaultValue;
}

Vec4 JSON::get( const Vec4& defaultValue ) const
{
  Vec4 v;
  return getVector( v, 4 ) ? v : defaultValue;
}

Point JSON::get( const Point& defaultValue ) const
{
  Point p;
  return getVector( p, 3 ) ? p : defaultValue;
}

Plane JSON::get( const Plane& defaultValue ) const
{
  Plane p;
  return getVector( p.n, 4 ) ? p : defaultValue;
}

Quat JSON::get( const Quat& defaultValue ) const
{
  Quat q;
  return getVector( q, 4 ) ? q : defaultValue;
}

Mat3 JSON::get( const Mat3& defaultValue ) const
{
  Mat3 m;
  return getVector( m, 9 ) ? m : defaultValue;
}

Mat4 JSON::get( const Mat4& defaultValue ) const
{
  Mat4 m;
  return getVector( m, 16 ) ? m : defaultValue;
}

JSON& JSON::add( const JSON& json )
{
  if( valueType != ARRAY ) {
    OZ_ERROR( "oz::JSON: Tried to add a value to a non-array JSON value: %s", toString().cstr() );
  }

  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  list.add( json );
  return list.last();
}

JSON& JSON::add( JSON&& json )
{
  if( valueType != ARRAY ) {
    OZ_ERROR( "oz::JSON: Tried to add a value to a non-array JSON value: %s", toString().cstr() );
  }

  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  list.add( json );
  return list.last();
}

JSON& JSON::add( const char* key, const JSON& json )
{
  if( valueType != OBJECT ) {
    OZ_ERROR( "oz::JSON: Tried to add a key-value pair '%s' to a non-object JSON value: %s",
              key, toString().cstr() );
  }

  HashMap<String, JSON>& table = static_cast<ObjectData*>( data )->table;
  return table.add( key, json );
}

JSON& JSON::add( const char* key, JSON&& json )
{
  if( valueType != OBJECT ) {
    OZ_ERROR( "oz::JSON: Tried to add a key-value pair '%s' to a non-object JSON value: %s",
              key, toString().cstr() );
  }

  HashMap<String, JSON>& table = static_cast<ObjectData*>( data )->table;
  return table.add( key, json );
}

JSON& JSON::include( const char* key, const JSON& json )
{
  if( valueType != OBJECT ) {
    OZ_ERROR( "oz::JSON: Tried to include a key-value pair '%s' in a non-object JSON value: %s",
              key, toString().cstr() );
  }

  ObjectData* table = static_cast<ObjectData*>( data );
  JSON* entry = table->table.find( key );

  if( entry == nullptr ) {
    entry = &table->table.add( key, json );
  }
  return *entry;
}

JSON& JSON::include( const char* key, JSON&& json )
{
  if( valueType != OBJECT ) {
    OZ_ERROR( "oz::JSON: Tried to include a key-value pair '%s' in a non-object JSON value: %s",
              key, toString().cstr() );
  }

  ObjectData* table = static_cast<ObjectData*>( data );
  JSON* entry = table->table.find( key );

  if( entry == nullptr ) {
    entry = &table->table.add( key, json );
  }
  return *entry;
}

bool JSON::erase( int index )
{
  if( valueType != ARRAY ) {
    OZ_ERROR( "oz::JSON: Tried to erase a value from a non-array JSON value: %s",
              toString().cstr() );
  }

  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  if( uint( index ) >= uint( list.length() ) ) {
    return false;
  }

  list.erase( index );
  return true;
}

bool JSON::exclude( const char* key )
{
  if( valueType != OBJECT ) {
    OZ_ERROR( "oz::JSON: Tried to exclude and entry form a non-object JSON value: %s",
              toString().cstr() );
  }

  HashMap<String, JSON>& table = static_cast<ObjectData*>( data )->table;

  return table.exclude( key );
}

bool JSON::clear( bool warnUnused )
{
  bool hasUnused = false;

  if( warnUnused && !wasAccessed ) {
    Log::println( "oz::JSON: unused value: %s", toString().cstr() );
    System::bell();

    hasUnused  = true;
    warnUnused = false;
  }

  switch( valueType ) {
    default: {
      break;
    }
    case STRING: {
      delete static_cast<StringData*>( data );
      break;
    }
    case ARRAY: {
      ArrayData* arrayData = static_cast<ArrayData*>( data );

      if( warnUnused ) {
        for( JSON& i : arrayData->list ) {
          hasUnused |= i.clear( true );
        }
      }

      delete arrayData;
      break;
    }
    case OBJECT: {
      ObjectData* objectData = static_cast<ObjectData*>( data );

      if( warnUnused ) {
        for( auto& i : objectData->table ) {
          hasUnused |= i.value.clear( true );
        }
      }

      delete objectData;
      break;
    }
  }

  valueType   = NIL;
  wasAccessed = true;

  return hasUnused;
}

String JSON::toString() const
{
  switch( valueType ) {
    default: {
      return "null";
    }
    case BOOLEAN: {
      return boolean ? "true" : "false";
    }
    case NUMBER: {
      return String( number );
    }
    case STRING: {
      const StringData* stringData = static_cast<const StringData*>( data );

      return "\"" + stringData->value + "\"";
    }
    case ARRAY: {
      const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

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
      const HashMap<String, JSON>& table = static_cast<const ObjectData*>( data )->table;

      if( table.isEmpty() ) {
        return "{}";
      }

      String s = "{ ";

      bool isFirst = true;
      for( const auto& i : table ) {
        s += String::str( isFirst ? "\"%s\": %s" : ", \"%s\": %s",
                          i.key.cstr(), i.value.toString().cstr() );
        isFirst = false;
      }

      return s + " }";
    }
  }
}

String JSON::toFormattedString( const Format& format ) const
{
  OutputStream os( 0 );
  Formatter formatter = { &os, &format, String::length( format.lineEnd ), 0 };

  formatter.writeValue( *this );
  os.writeChars( format.lineEnd, formatter.lineEndLength );

  return String( os.begin(), os.tell() );
}

bool JSON::load( const File& file )
{
  InputStream is = file.inputStream();

  if( !is.isAvailable() ) {
    return false;
  }

  *this = Parser::parse( &is, file.path() );
  return true;
}

bool JSON::save( const File& file, const Format& format ) const
{
  OutputStream os( 0 );
  Formatter formatter = { &os, &format, String::length( format.lineEnd ), 0 };

  formatter.writeValue( *this );
  os.writeChars( format.lineEnd, formatter.lineEndLength );

  return file.write( os.begin(), os.tell() );
}

}
