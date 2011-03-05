/*
 *  Config.cpp
 *
 *  List that reads configuration file
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Config.hpp"

#include "Exception.hpp"
#include "Log.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstdio>

namespace oz
{

  struct Elem
  {
    const char* key;
    const char* value;
#ifdef OZ_VERBOSE_CONFIG
    bool isUsed;

    explicit Elem() : isUsed( false ) {}
#endif

    bool operator < ( const Elem& e ) const
    {
      return String::compare( key, e.key ) < 0;
    }
  };

  Config config;

  Config::~Config()
  {
    clear();
  }

  void Config::add( const char* key, const char* value_ )
  {
    String* value = vars.find( key );
    if( value != null ) {
      *value = value_;
    }
    else {
      vars.add( key, value_ );
    }
  }

  void Config::include( const char* key, const char* value_ )
  {
    String* value = vars.find( key );
    if( value == null ) {
      vars.add( key, value_ );
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

  const String& Config::operator [] ( const char* key ) const
  {
#ifdef OZ_VERBOSE_CONFIG
    usedVars.add( key );
#endif
    return vars.get( key );
  }

  bool Config::loadConf( const char* file )
  {
    char buffer[BUFFER_SIZE];
    char ch;

    FILE* f = fopen( file, "r" );
    if( f == null ) {
      log.println( "Error reading variables from '%s' ... Cannot open file", file );
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
      if( String::isLetter( ch ) ) {
        buffer[0] = ch;
        ch = char( fgetc( f ) );

        int i = 1;
        while( i < BUFFER_SIZE - 1 &&
            ( String::isLetter( ch ) || String::isDigit( ch ) || ch == '.' ) )
        {
          buffer[i] = ch;
          ch = char( fgetc( f ) );
          ++i;
        }
        buffer[i] = '\0';
        name = buffer;
      }
      else {
        goto skipLine;
      }

      // skip spaced between name and value
      while( String::isSpace( ch ) ) {
        ch = char( fgetc( f ) );
      }

      if( ch == '"' ) {
        ch = char( fgetc( f ) );

        int i = 0;
        while( i < BUFFER_SIZE - 1 && ch != '"' && ch != '\n' && ch != EOF ) {
          buffer[i] = ch;
          ch = char( fgetc( f ) );
          ++i;
        }
        if( ch == '"' ) {
          buffer[i] = '\0';
          include( name, buffer );
        }
      }

      skipLine:;
      // find end of line/file
      while( ch != '\n' && ch != EOF ) {
        ch = char( fgetc( f ) );
      }
    }
    while( ch != EOF );

    fclose( f );

    return true;
  }

  bool Config::saveConf( const char* file )
  {
    log.print( "Writing variables to '%s' ...", file );

    // first we sort all the variables by key
    int size = vars.length();
    DArray<Elem> sortedVars( size );

    int i = 0;
    foreach( j, vars.citer() ) {
      sortedVars[i].key = j.key().cstr();
      sortedVars[i].value = j.value().cstr();
      ++i;
    }
    sortedVars.sort();

    FILE* f = fopen( file, "w" );
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

  bool Config::get( const char* name, bool defVal ) const
  {
    hard_assert( !vars.contains( name ) ||
            vars.get( name ).equals( "true" ) ||
            vars.get( name ).equals( "false" ) );

#ifdef OZ_VERBOSE_CONFIG
    if( !usedVars.contains( name ) ) {
      usedVars.add( name );
    }
#endif

    const String* value = vars.find( name );
    if( value != null ) {
      if( value->equals( "true" ) ) {
        return true;
      }
      else if( value->equals( "false" ) ) {
        return false;
      }
      else {
        throw Exception( "Invalid boolean value '" + *value + "'" );
      }
    }
    else {
      return defVal;
    }
  }

  int Config::get( const char* name, int defVal ) const
  {
#ifdef OZ_VERBOSE_CONFIG
    if( !usedVars.contains( name ) ) {
      usedVars.add( name );
    }
#endif

    const String* value = vars.find( name );
    if( value != null ) {
      errno = 0;
      char* end;
      int   num = int( strtol( *value, &end, 0 ) );

      if( errno != 0 || end == value->cstr() ) {
        throw Exception( "Invalid int value '" + *value + "'" );
      }
      else {
        return num;
      }
    }
    else {
      return defVal;
    }
  }

  float Config::get( const char* name, float defVal ) const
  {
#ifdef OZ_VERBOSE_CONFIG
    if( !usedVars.contains( name ) ) {
      usedVars.add( name );
    }
#endif

    const String* value = vars.find( name );
    if( value != null ) {
      errno = 0;
      char* end;
      float num = strtof( *value, &end );

      if( errno != 0 || end == value->cstr() ) {
        throw Exception( "Invalid float value '" + *value + "'" );
      }
      else {
        return num;
      }
    }
    else {
      return defVal;
    }
  }

  const char* Config::get( const char* name, const char* defVal ) const
  {
#ifdef OZ_VERBOSE_CONFIG
    if( !usedVars.contains( name ) ) {
      usedVars.add( name );
    }
#endif

    const String* value = vars.find( name );
    if( value != null ) {
      return *value;
    }
    else {
      return defVal;
    }
  }

  bool Config::getSet( const char* name, bool defVal )
  {
#ifdef OZ_VERBOSE_CONFIG
    if( !usedVars.contains( name ) ) {
      usedVars.add( name );
    }
#endif

    const String* value = vars.find( name );
    if( value != null ) {
      if( value->equals( "true" ) ) {
        return true;
      }
      else if( value->equals( "false" ) ) {
        return false;
      }
      else {
        throw Exception( "Invalid boolean value '" + *value + "'" );
      }
    }
    else {
      vars.add( name, String( defVal ) );
      return defVal;
    }
  }

  int Config::getSet( const char* name, int defVal )
  {
#ifdef OZ_VERBOSE_CONFIG
    if( !usedVars.contains( name ) ) {
      usedVars.add( name );
    }
#endif

    const String* value = vars.find( name );
    if( value != null ) {
      errno = 0;
      char* end;
      int   num = int( strtol( *value, &end, 0 ) );

      if( errno != 0 || end == value->cstr() ) {
        throw Exception( "Invalid int value '" + *value + "'" );
      }
      else {
        return num;
      }
    }
    else {
      vars.add( name, String( defVal ) );
      return defVal;
    }
  }

  float Config::getSet( const char* name, float defVal )
  {
#ifdef OZ_VERBOSE_CONFIG
    if( !usedVars.contains( name ) ) {
      usedVars.add( name );
    }
#endif

    const String* value = vars.find( name );
    if( value != null ) {
      errno = 0;
      char* end;
      float num = strtof( *value, &end );

      if( errno != 0 || end == value->cstr() ) {
        throw Exception( "Invalid float value '" + *value + "'" );
      }
      else {
        return num;
      }
    }
    else {
      vars.add( name, String( defVal ) );
      return defVal;
    }
  }

  const char* Config::getSet( const char* name, const char* defVal )
  {
#ifdef OZ_VERBOSE_CONFIG
    if( !usedVars.contains( name ) ) {
      usedVars.add( name );
    }
#endif

    const String* value = vars.find( name );
    if( value != null ) {
      return *value;
    }
    else {
      vars.add( name, defVal );
      return defVal;
    }
  }

  bool Config::load( const char* file )
  {
    const char* suffix = String::findLast( file, '.' );

    if( suffix != null ) {
      if( String::equals( suffix, ".rc" ) ) {
        return loadConf( file );
      }
    }

    log.println( "Unknown configuration file %s", file );
    return false;
  }

  bool Config::save( const char* file )
  {
    const char* suffix = String::findLast( file, '.' );

    if( suffix != null ) {
      if( String::equals( suffix, ".rc" ) ) {
        return saveConf( file );
      }
    }

    log.println( "Unknown configuration file %s", file );
    return false;
  }

  void Config::clear()
  {
#ifdef OZ_VERBOSE_CONFIG
    foreach( var, vars.citer() ) {
      if( !usedVars.contains( var.key() ) ) {
        log.println( "config: unused variable '%s'", var.key().cstr() );
      }
    }
    usedVars.clear();
    usedVars.dealloc();
#endif
    vars.clear();
    vars.dealloc();
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
      sortedVars[i].value = j.value().cstr();
      ++i;
    }
    sortedVars.sort();

    for( int i = 0; i < size; ++i ) {
      s = s + indentString + sortedVars[i].key + " = \"" + sortedVars[i].value + "\"\n";
    }
    return s;
  }

}
