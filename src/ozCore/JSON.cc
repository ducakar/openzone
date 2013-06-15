/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <cstring>

#define OZ_PARSE_ERROR( charBias, message ) \
  OZ_ERROR( "JSON: " message " at %s:%d:%d", pos.path, pos.line, pos.column + ( charBias ) );

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
  double value;

  OZ_HIDDEN
  explicit NumberData( double value_ ) :
    value( value_ )
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
  HashMap<String, JSON> table;
};

JSON::ObjectCIterator::ObjectCIterator( const ObjectData* data ) :
  IteratorBase<const HashMap<String, JSON>::Elem>( nullptr ), objectIter( data->table.citer() )
{
  elem = objectIter;
}

JSON::ObjectCIterator& JSON::ObjectCIterator::operator ++ ()
{
  ++objectIter;
  elem = objectIter;
  return *this;
}

JSON::ObjectIterator::ObjectIterator( ObjectData* data ) :
  IteratorBase<HashMap<String, JSON>::Elem>( nullptr ), objectIter( data->table.iter() )
{
  elem = objectIter;
}

JSON::ObjectIterator& JSON::ObjectIterator::operator ++ ()
{
  ++objectIter;
  elem = objectIter;
  return *this;
}

struct JSON::Parser
{
  enum BlanksMode
  {
    WHITESPACE,
    LINE_COMMENT,
    MULTILINE_COMMENT
  };

  struct Position
  {
    InputStream* istream;
    const char*  path;
    int          line;
    int          column;
    int          oldLine;
    int          oldColumn;

    OZ_HIDDEN
    char readChar()
    {
      if( !istream->isAvailable() ) {
        const Position& pos = *this;
        OZ_PARSE_ERROR( 0, "Unexpected end of file" );
      }

      oldLine   = line;
      oldColumn = column;

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
    void backChar()
    {
      hard_assert( line != oldLine || column != oldColumn );

      istream->set( istream->pos() - 1 );

      line   = oldLine;
      column = oldColumn;
    }
  };

  Position pos;

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

        foreach( i, list.iter() ) {
          setAccessed( i );
        }
        break;
      }
      case OBJECT: {
        HashMap<String, JSON>& table = static_cast<ObjectData*>( value->data )->table;

        foreach( i, table.iter() ) {
          setAccessed( &i->value );
        }
        break;
      }
    }
  }

  OZ_HIDDEN
  static JSON parse( InputStream* istream, const char* path )
  {
    Parser parser( istream, path );

    JSON root = parser.parseValue();

    parser.finish();
    return root;
  }

  OZ_HIDDEN
  explicit Parser( InputStream* istream, const char* path ) :
    pos( { istream, path, 1, 0, 1, 0 } )
  {}

  OZ_HIDDEN
  char skipBlanks()
  {
    char ch1, ch2;

    do {
      do {
        ch2 = pos.readChar();
      }
      while( String::isBlank( ch2 ) );

      if( ch2 == '/' ) {
        ch1 = ch2;
        ch2 = pos.readChar();

        if( ch2 == '/' ) {
          // Skip a line comment.
          do {
            ch2 = pos.readChar();
          }
          while( ch2 != '\n' );

          continue;
        }
        else if( ch2 == '*' ) {
          // Skip a multi-line comment.
          ch2 = pos.readChar();

          do {
            ch1 = ch2;
            ch2 = pos.readChar();
          }
          while( ch1 != '*' || ch2 != '/' );

          continue;
        }
        else {
          ch2 = ch1;
          pos.backChar();
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
    while( pos.istream->isAvailable() );

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
        if( pos.istream->available() < 3 || pos.readChar() != 'u' || pos.readChar() != 'l' ||
            pos.readChar() != 'l' )
        {
          OZ_PARSE_ERROR( -3, "Unknown value type" );
        }

        return JSON( nullptr, NIL );
      }
      case 'f': {
        if( pos.istream->available() < 4 || pos.readChar() != 'a' || pos.readChar() != 'l' ||
            pos.readChar() != 's' || pos.readChar() != 'e' )
        {
          OZ_PARSE_ERROR( -4, "Unknown value type" );
        }

        return JSON( new BooleanData( false ), BOOLEAN );
      }
      case 't': {
        if( pos.istream->available() < 4 || pos.readChar() != 'r' || pos.readChar() != 'u' ||
            pos.readChar() != 'e' )
        {
          OZ_PARSE_ERROR( -3, "Unknown value type" );
        }

        return JSON( new BooleanData( true ), BOOLEAN );
      }
      default: { // Number.
        SList<char, 32> chars;
        chars.add( ch );

        while( pos.istream->isAvailable() ) {
          ch = pos.readChar();

          if( String::isBlank( ch ) || ch == ',' || ch == '}' || ch == ']' ) {
            pos.backChar();
            break;
          }
          if( chars.length() == 32 ) {
            OZ_PARSE_ERROR( -chars.length(), "Too long " );
          }
          chars.add( ch );
        }
        chars.add( '\0' );

        const char* end;
        double number = String::parseDouble( chars.begin(), &end );

        if( end == chars.begin() ) {
          OZ_PARSE_ERROR( -chars.length(), "Unknown value type" );
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
  JSON parseArray()
  {
    JSON arrayValue( new ArrayData(), ARRAY );
    List<JSON>& list = static_cast<ArrayData*>( arrayValue.data )->list;

    char ch = skipBlanks();
    if( ch != ']' ) {
      pos.backChar();
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
      pos.backChar();
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
    while( pos.istream->isAvailable() ) {
      char ch = pos.readChar();

      if( !String::isBlank( ch ) ) {
        OZ_PARSE_ERROR( 0, "End of file expected but some content found after" );
      }
    }
  }
};

struct JSON::Formatter
{
  static const int ALIGNMENT_COLUMN   = 32;
  static const int SIGNIFICANT_DIGITS = 17;

  OutputStream* ostream;
  const char*   lineEnd;
  int           lineEndLength;
  int           indentLevel;

  OZ_HIDDEN
  int writeString( const String& string )
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
  void writeValue( const JSON& value )
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

        String s = String( numberData->value, SIGNIFICANT_DIGITS );
        ostream->writeChars( s, s.length() );
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

  OZ_HIDDEN
  void writeObject( const JSON& value )
  {
    const HashMap<String, JSON>& table = static_cast<const ObjectData*>( value.data )->table;

    if( table.isEmpty() ) {
      ostream->writeChars( "{}", 2 );
      return;
    }

    ostream->writeChar( '{' );
    ostream->writeChars( lineEnd, lineEndLength );

    ++indentLevel;

    Map<String, const JSON*> sortedEntries;

    foreach( entry, table.citer() ) {
      sortedEntries.add( entry->key, &entry->value );
    }

    for( int i = 0; i < sortedEntries.length(); ++i ) {
      if( i != 0 ) {
        ostream->writeChar( ',' );
        ostream->writeChars( lineEnd, lineEndLength );
      }

      for( int j = 0; j < indentLevel; ++j ) {
        ostream->writeChars( "  ", 2 );
      }

      const String& entryKey   = sortedEntries[i].key;
      const JSON*   entryValue = sortedEntries[i].value;

      int keyLength = writeString( entryKey );
      ostream->writeChar( ':' );

      if( entryValue->valueType == ARRAY || entryValue->valueType == OBJECT ) {
        ostream->writeChars( lineEnd, lineEndLength );

        for( int j = 0; j < indentLevel; ++j ) {
          ostream->writeChars( "  ", 2 );
        }
      }
      else {
        int column = indentLevel * 2 + keyLength + 1;

        // Align to 24-th column.
        for( int j = column; j < ALIGNMENT_COLUMN; ++j ) {
          ostream->writeChar( ' ' );
        }
      }

      writeValue( *entryValue );
    }

    sortedEntries.clear();
    sortedEntries.deallocate();

    ostream->writeChars( lineEnd, lineEndLength );

    --indentLevel;
    for( int j = 0; j < indentLevel; ++j ) {
      ostream->writeChars( "  ", 2 );
    }

    ostream->writeChar( '}' );
  }
};

OZ_HIDDEN
const JSON JSON::NIL_VALUE;

OZ_HIDDEN
JSON::JSON( Data* data_, Type valueType_ ) :
  data( data_ ), valueType( valueType_ ), wasAccessed( false )
{}

JSON::JSON( Type type ) :
  data( nullptr ), valueType( type ), wasAccessed( true )
{
  switch( type ) {
    case NIL: {
      break;
    }
    case BOOLEAN: {
      data = new BooleanData( false );
      break;
    }
    case NUMBER: {
      data = new NumberData( 0.0 );
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

JSON::JSON( bool value ) :
  data( new BooleanData( value ) ), valueType( BOOLEAN ), wasAccessed( true )
{}

JSON::JSON( int value ) :
  data( new NumberData( value ) ), valueType( NUMBER ), wasAccessed( true )
{}

JSON::JSON( float value ) :
  data( new NumberData( value ) ), valueType( NUMBER ), wasAccessed( true )
{}

JSON::JSON( double value ) :
  data( new NumberData( value ) ), valueType( NUMBER ), wasAccessed( true )
{}

JSON::JSON( const String& value ) :
  data( new StringData( value ) ), valueType( STRING ), wasAccessed( true )
{}

JSON::JSON( const char* value ) :
  data( new StringData( value ) ), valueType( STRING ), wasAccessed( true )
{}

JSON::JSON( const Vec3& v ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( true )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 3; ++i ) {
    list.add( JSON( new NumberData( v[i] ), NUMBER ) );
  }
}

JSON::JSON( const Vec4& v ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( true )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 4; ++i ) {
    list.add( JSON( new NumberData( v[i] ), NUMBER ) );
  }
}

JSON::JSON( const Point& p ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( true )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 3; ++i ) {
    list.add( JSON( new NumberData( p[i] ), NUMBER ) );
  }
}

JSON::JSON( const Plane& p ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( true )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 3; ++i ) {
    list.add( JSON( new NumberData( p.n[i] ), NUMBER ) );
  }
  list.add( JSON( new NumberData( p.d ), NUMBER ) );
}

JSON::JSON( const Quat& q ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( true )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 3; ++i ) {
    list.add( JSON( new NumberData( q[i] ), NUMBER ) );
  }
}

JSON::JSON( const Mat33& m ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( true )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 3; ++i ) {
    for( int j = 0; j < 3; ++j ) {
      list.add( JSON( new NumberData( m[i][j] ), NUMBER ) );
    }
  }
}

JSON::JSON( const Mat44& m ) :
  data( new ArrayData() ), valueType( ARRAY ), wasAccessed( true )
{
  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  for( int i = 0; i < 4; ++i ) {
    for( int j = 0; j < 4; ++j ) {
      list.add( JSON( new NumberData( m[i][j] ), NUMBER ) );
    }
  }
}

JSON::JSON( const File& file ) :
  data( nullptr ), valueType( NIL ), wasAccessed( true )
{
  load( file );
}

JSON::~JSON()
{
  clear();
}

JSON::JSON( const JSON& v ) :
  data( nullptr ), valueType( v.valueType ), wasAccessed( v.wasAccessed )
{
  switch( valueType ) {
    case NIL: {
      break;
    }
    case BOOLEAN: {
      data = new BooleanData( *static_cast<const BooleanData*>( v.data ) );
      break;
    }
    case NUMBER: {
      data = new NumberData( *static_cast<const NumberData*>( v.data ) );
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
  data( v.data ), valueType( v.valueType ), wasAccessed( v.wasAccessed )
{
  v.data        = nullptr;
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
      data = new BooleanData( *static_cast<const BooleanData*>( v.data ) );
      break;
    }
    case NUMBER: {
      data = new NumberData( *static_cast<const NumberData*>( v.data ) );
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

  data        = v.data;
  valueType   = v.valueType;
  wasAccessed = v.wasAccessed;

  v.data        = nullptr;
  v.valueType   = NIL;
  v.wasAccessed = true;

  return *this;
}

int JSON::length() const
{
  switch( valueType ) {
    default: {
      return -1;
    }
    case ARRAY: {
      return static_cast<const ArrayData*>( data )->list.length();
    }
    case OBJECT: {
      return static_cast<const ObjectData*>( data )->table.length();
    }
  }
}

int JSON::isEmpty() const
{
  switch( valueType ) {
    default: {
      return true;
    }
    case ARRAY: {
      return static_cast<const ArrayData*>( data )->list.isEmpty();
    }
    case OBJECT: {
      return static_cast<const ObjectData*>( data )->table.isEmpty();
    }
  }
}

JSON::ArrayCIterator JSON::arrayCIter() const
{
  if( valueType == ARRAY ) {
    const ArrayData* arrayData = static_cast<const ArrayData*>( data );

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

    return ObjectCIterator( objectData );
  }
  else {
    return ObjectCIterator();
  }
}

JSON::ObjectIterator JSON::objectIter()
{
  if( valueType == OBJECT ) {
    ObjectData* objectData = static_cast<ObjectData*>( data );

    return ObjectIterator( objectData );
  }
  else {
    return ObjectIterator();
  }
}

const JSON& JSON::operator [] ( int i ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return NIL_VALUE;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( uint( i ) >= uint( list.length() ) ) {
    return NIL_VALUE;
  }

  list[i].wasAccessed = true;
  return list[i];
}

const JSON& JSON::operator [] ( const char* key ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return NIL_VALUE;
  }
  else if( valueType != OBJECT ) {
    OZ_ERROR( "JSON value accessed as an object: %s", toString().cstr() );
  }

  const HashMap<String, JSON>& table = static_cast<const ObjectData*>( data )->table;
  const JSON* value = table.find( key );

  if( value == nullptr ) {
    return NIL_VALUE;
  }

  value->wasAccessed = true;
  return *value;
}

bool JSON::contains( const char* key )
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != OBJECT ) {
    OZ_ERROR( "JSON value accessed as an object: %s", toString().cstr() );
  }

  const HashMap<String, JSON>& table = static_cast<const ObjectData*>( data )->table;
  const JSON* value = table.find( key );

  if( value == nullptr ) {
    return false;
  }

  value->wasAccessed = true;
  return true;
}

bool JSON::asBool() const
{
  wasAccessed = true;

  if( valueType != BOOLEAN ) {
    OZ_ERROR( "JSON value accessed as a boolean: %s", toString().cstr() );
  }
  return static_cast<const BooleanData*>( data )->value;
}

int JSON::asInt() const
{
  return int( asDouble() );
}

float JSON::asFloat() const
{
  return float( asDouble() );
}

double JSON::asDouble() const
{
  wasAccessed = true;

  if( valueType != NUMBER ) {
    OZ_ERROR( "JSON value accessed as a number: %s", toString().cstr() );
  }
  return static_cast<const NumberData*>( data )->value;
}

const String& JSON::asString() const
{
  wasAccessed = true;

  if( valueType != STRING ) {
    OZ_ERROR( "JSON value accessed as a string: %s", toString().cstr() );
  }
  return static_cast<const StringData*>( data )->value;
}

Vec3 JSON::asVec3() const
{
  Vec3 v;
  asArray( v, 3 );
  return v;
}

Vec4 JSON::asVec4() const
{
  Vec4 v;
  asArray( v, 4 );
  return v;
}

Point JSON::asPoint() const
{
  Point p;
  asArray( p, 3 );
  return p;
}

Plane JSON::asPlane() const
{
  Plane p;
  asArray( p.n, 4 );
  return p;
}

Quat JSON::asQuat() const
{
  Quat q;
  asArray( q, 4 );
  return q;
}

Mat33 JSON::asMat33() const
{
  Mat33 m;
  asArray( m, 9 );
  return m;
}

Mat44 JSON::asMat44() const
{
  Mat44 m;
  asArray( m, 16 );
  return m;
}

void JSON::asArray( bool* array, int count ) const
{
  wasAccessed = true;

  if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asBool();
  }
}

void JSON::asArray( int* array, int count ) const
{
  wasAccessed = true;

  if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asInt();
  }
}

void JSON::asArray( float* array, int count ) const
{
  wasAccessed = true;

  if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asFloat();
  }
}

void JSON::asArray( double* array, int count ) const
{
  wasAccessed = true;

  if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asDouble();
  }
}

void JSON::asArray( String* array, int count ) const
{
  wasAccessed = true;

  if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asString();
  }
}

void JSON::asArray( Vec3* array, int count ) const
{
  wasAccessed = true;

  if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asVec3();
  }
}

void JSON::asArray( Vec4* array, int count ) const
{
  wasAccessed = true;

  if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asVec4();
  }
}

void JSON::asArray( Point* array, int count ) const
{
  wasAccessed = true;

  if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asPoint();
  }
}

void JSON::asArray( Plane* array, int count ) const
{
  wasAccessed = true;

  if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asPlane();
  }
}

void JSON::asArray( Quat* array, int count ) const
{
  wasAccessed = true;

  if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asQuat();
  }
}

void JSON::asArray( Mat33* array, int count ) const
{
  wasAccessed = true;

  if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asMat33();
  }
}

void JSON::asArray( Mat44* array, int count ) const
{
  wasAccessed = true;

  if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asMat44();
  }
}

bool JSON::get( bool defaultValue ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return defaultValue;
  }
  else if( valueType != BOOLEAN ) {
    OZ_ERROR( "JSON value accessed as a boolean: %s", toString().cstr() );
  }

  return static_cast<const BooleanData*>( data )->value;
}

int JSON::get( int defaultValue ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return defaultValue;
  }
  else if( valueType != NUMBER ) {
    OZ_ERROR( "JSON value accessed as a number: %s", toString().cstr() );
  }

  return int( static_cast<const NumberData*>( data )->value );
}

float JSON::get( float defaultValue ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return defaultValue;
  }
  else if( valueType != NUMBER ) {
    OZ_ERROR( "JSON value accessed as a number: %s", toString().cstr() );
  }

  return float( static_cast<const NumberData*>( data )->value );
}

double JSON::get( double defaultValue ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return defaultValue;
  }
  else if( valueType != NUMBER ) {
    OZ_ERROR( "JSON value accessed as a number: %s", toString().cstr() );
  }

  return static_cast<const NumberData*>( data )->value;
}

const String& JSON::get( const String& defaultValue ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return defaultValue;
  }
  else if( valueType != STRING ) {
    OZ_ERROR( "JSON value accessed as a string: %s", toString().cstr() );
  }

  return static_cast<const StringData*>( data )->value;
}

const char* JSON::get( const char* defaultValue ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return defaultValue;
  }
  else if( valueType != STRING ) {
    OZ_ERROR( "JSON value accessed as a string: %s", toString().cstr() );
  }

  return static_cast<const StringData*>( data )->value;
}

Vec3 JSON::get( const Vec3& defaultValue ) const
{
  Vec3 v;
  return get( v, 3 ) ? v : defaultValue;
}

Vec4 JSON::get( const Vec4& defaultValue ) const
{
  Vec4 v;
  return get( v, 4 ) ? v : defaultValue;
}

Point JSON::get( const Point& defaultValue ) const
{
  Point p;
  return get( p, 3 ) ? p : defaultValue;
}

Plane JSON::get( const Plane& defaultValue ) const
{
  Plane p;
  return get( p.n, 4 ) ? p : defaultValue;
}

Quat JSON::get( const Quat& defaultValue ) const
{
  Quat q;
  return get( q, 4 ) ? q : defaultValue;
}

Mat33 JSON::get( const Mat33& defaultValue ) const
{
  Mat33 m;
  return get( m, 9 ) ? m : defaultValue;
}

Mat44 JSON::get( const Mat44& defaultValue ) const
{
  Mat44 m;
  return get( m, 16 ) ? m : defaultValue;
}

bool JSON::get( bool* array, int count ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asBool();
  }
  return true;
}

bool JSON::get( int* array, int count ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asInt();
  }
  return false;
}

bool JSON::get( float* array, int count ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asFloat();
  }
  return true;
}

bool JSON::get( double* array, int count ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asDouble();
  }
  return true;
}

bool JSON::get( String* array, int count ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asString();
  }
  return true;
}

bool JSON::get( Vec3* array, int count ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asVec3();
  }
  return true;
}

bool JSON::get( Vec4* array, int count ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asVec4();
  }
  return true;
}

bool JSON::get( Point* array, int count ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asPoint();
  }
  return true;
}

bool JSON::get( Plane* array, int count ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asPlane();
  }
  return true;
}

bool JSON::get( Quat* array, int count ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asQuat();
  }
  return true;
}

bool JSON::get( Mat33* array, int count ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asMat33();
  }
  return true;
}

bool JSON::get( Mat44* array, int count ) const
{
  wasAccessed = true;

  if( valueType == NIL ) {
    return false;
  }
  else if( valueType != ARRAY ) {
    OZ_ERROR( "JSON value accessed as an array: %s", toString().cstr() );
  }

  const List<JSON>& list = static_cast<const ArrayData*>( data )->list;

  if( list.length() != count ) {
    OZ_ERROR( "JSON array has %d elements but %d expected: %s",
              list.length(), count, toString().cstr() );
  }

  for( int i = 0; i < count; ++i ) {
    array[i] = list[i].asMat44();
  }
  return true;
}

JSON& JSON::add( const JSON& json )
{
  if( valueType != ARRAY ) {
    OZ_ERROR( "Tried to add a value to a non-array JSON value: %s", toString().cstr() );
  }

  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  list.add( json );
  return list.last();
}

JSON& JSON::add( JSON&& json )
{
  if( valueType != ARRAY ) {
    OZ_ERROR( "Tried to add a value to a non-array JSON value: %s", toString().cstr() );
  }

  List<JSON>& list = static_cast<ArrayData*>( data )->list;

  list.add( json );
  return list.last();
}

JSON& JSON::add( const char* key, const JSON& json )
{
  if( valueType != OBJECT ) {
    OZ_ERROR( "Tried to add a key-value pair '%s' to a non-object JSON value: %s",
              key, toString().cstr() );
  }

  HashMap<String, JSON>& table = static_cast<ObjectData*>( data )->table;
  return table.add( key, json );
}

JSON& JSON::add( const char* key, JSON&& json )
{
  if( valueType != OBJECT ) {
    OZ_ERROR( "Tried to add a key-value pair '%s' to a non-object JSON value: %s",
              key, toString().cstr() );
  }

  HashMap<String, JSON>& table = static_cast<ObjectData*>( data )->table;
  return table.add( key, json );
}

JSON& JSON::include( const char* key, const JSON& json )
{
  if( valueType != OBJECT ) {
    OZ_ERROR( "Tried to include a key-value pair '%s' in a non-object JSON value: %s",
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
    OZ_ERROR( "Tried to include a key-value pair '%s' in a non-object JSON value: %s",
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
    OZ_ERROR( "Tried to erase a value from a non-array JSON value: %s", toString().cstr() );
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
    OZ_ERROR( "Tried to exclude and entry form a non-object JSON value: %s", toString().cstr() );
  }

  HashMap<String, JSON>& table = static_cast<ObjectData*>( data )->table;

  return table.exclude( key );
}

bool JSON::clear( bool warnUnused )
{
  bool hasUnused = false;

  if( warnUnused && !wasAccessed ) {
    Log::println( "JSON: unused value: %s", toString().cstr() );
    System::bell();
    hasUnused = true;
  }

  switch( valueType ) {
    case NIL: {
      hard_assert( data == nullptr );
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

      if( warnUnused ) {
        foreach( i, arrayData->list.iter() ) {
          hasUnused |= i->clear( true );
        }
      }

      delete arrayData;
      break;
    }
    case OBJECT: {
      ObjectData* objectData = static_cast<ObjectData*>( data );

      if( warnUnused ) {
        foreach( i, objectData->table.iter() ) {
          hasUnused |= i->value.clear( true );
        }
      }

      delete objectData;
      break;
    }
  }

  data        = nullptr;
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
      const BooleanData* booleanData = static_cast<const BooleanData*>( data );

      return booleanData->value ? "true" : "false";
    }
    case NUMBER: {
      const NumberData* numberData = static_cast<const NumberData*>( data );

      return String( numberData->value, Formatter::SIGNIFICANT_DIGITS );
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
      foreach( i, table.citer() ) {
        s += String::str( isFirst ? "\"%s\": %s" : ", \"%s\": %s",
                          i->key.cstr(), i->value.toString().cstr() );
        isFirst = false;
      }

      return s + " }";
    }
  }
}

String JSON::toFormattedString( const char* lineEnd ) const
{
  OutputStream ostream( 0 );
  Formatter formatter = { &ostream, lineEnd, String::length( lineEnd ), 0 };

  formatter.writeValue( *this );
  ostream.writeChars( lineEnd, formatter.lineEndLength );

  return String( ostream.begin(), ostream.tell() );
}

bool JSON::load( const File& file )
{
  Buffer buffer = file.read();
  if( buffer.isEmpty() ) {
    return false;
  }

  InputStream istream = buffer.inputStream();

  *this = Parser::parse( &istream, file.path() );
  return true;
}

bool JSON::save( const File& file, const char* lineEnd ) const
{
  OutputStream ostream( 0 );
  Formatter formatter = { &ostream, lineEnd, String::length( lineEnd ), 0 };

  formatter.writeValue( *this );
  ostream.writeChars( lineEnd, formatter.lineEndLength );

  return file.write( ostream.begin(), ostream.tell() );
}

}