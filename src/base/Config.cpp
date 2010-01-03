/*
 *  Config.cpp
 *
 *  List that reads configuration file
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "base.h"

#include <cstdlib>
#include <cstdio>

#ifdef OZ_XML_CONFIG
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#endif

namespace oz
{

  struct Elem
  {
    const char* key;
    const char* value;

    bool operator < ( const Elem& e ) const
    {
      return String::compare( key, e.key ) < 0;
    }
  };

  Config config;

  Config::~Config()
  {
    vars.clear();
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
          i++;
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
          i++;
        }
        if( ch == '"' ) {
          buffer[i] = '\0';
          add( name, buffer );
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
    Elem sortedVars[size];

    typeof( vars.iterator() ) j = vars.iterator();
    for( int i = 0; !j.isPassed(); ++i, ++j ) {
      sortedVars[i].key = j.key().cstr();
      sortedVars[i].value = j.value().cstr();
      size = i;
    }
    size++;
    aSort( sortedVars, size );

    FILE* f = fopen( file, "w" );
    if( f == null ) {
      log.printEnd( " Cannot open file" );
      return false;
    }

    for( int i = 0; i < size; i++ ) {
      int chars;
      fprintf( f, "%s%n", sortedVars[i].key, &chars );

      chars = ALIGNMENT - chars;
      while( chars > 0 ) {
        fprintf( f, " " );
        chars--;
      }
      fprintf( f, "\"%s\"\n", sortedVars[i].value );
    }

    fclose( f );

    log.printEnd( " OK" );
    return true;
  }

#ifdef OZ_XML_CONFIG
  bool Config::loadXML( const char* file )
  {
    xmlTextReader* reader = xmlReaderForFile( file, null, 0 );

    if( reader == null ) {
      xmlCleanupParser();
      log.println( "Error reading variables from '%s' ... Cannot open file", file );
      return false;
    }

    int error = xmlTextReaderRead( reader );
    while( error == 1 ) {
      const char* name = reinterpret_cast<const char*>( xmlTextReaderConstName( reader ) );

      // only check "var" nodes, ignore others
      if( name != null && String::equals( name, "var" ) ) {
        void* key = xmlTextReaderGetAttribute( reader, reinterpret_cast<const xmlChar*>( "name" ) );
        void* value = xmlTextReaderGetAttribute( reader, reinterpret_cast<const xmlChar*>( "value" ) );

        // error if "var" tag doesn't has "name" and "value" attributes
        if( key == null || value == null ) {
          xmlFree( key );
          xmlFree( value );

          error = -1;
          break;
        }
        add( reinterpret_cast<const char*>( key ), reinterpret_cast<const char*>( value ) );

        xmlFree( key );
        xmlFree( value );
      }
      error = xmlTextReaderRead( reader );
    }
    xmlFreeTextReader( reader );
    xmlCleanupParser();

    if( error != 0 ) {
      log.println( "Error reading variables from '%s' ... Parse error", file );
      return false;
    }
    return true;
  }

  bool Config::saveXML( const char* file )
  {
    log.print( "Writing variables to '%s' ...", file );

    // first we sort all the variables by key
    int size = vars.length();
    Elem sortedVars[size];

    typeof( vars.iterator() ) j = vars.iterator();
    for( int i = 0; !j.isPassed(); ++i, ++j ) {
      sortedVars[i].key = j.key().cstr();
      sortedVars[i].value = j.value().cstr();
      size = i;
    }
    size++;
    aSort( sortedVars, size );

    xmlTextWriter* writer = xmlNewTextWriterFilename( file, 0 );

    if( writer == null ) {
      xmlCleanupParser();
      log.printEnd( " Cannot open file" );
      return false;
    }

    if( xmlTextWriterStartDocument( writer, null, "UTF-8", null ) < 0 ||
        xmlTextWriterStartElement( writer, reinterpret_cast<const xmlChar*>( "config" ) ) < 0 )
    {
      xmlFreeTextWriter( writer );
      xmlCleanupParser();
      log.printEnd( " Write error" );
      return false;
    }

    for( int i = 0; i < size; i++ ) {
      if( xmlTextWriterWriteString( writer, reinterpret_cast<const xmlChar*>( "\n  " ) ) < 0 ||
          xmlTextWriterStartElement( writer, reinterpret_cast<const xmlChar*>( "var" ) ) < 0 ||
          xmlTextWriterWriteAttribute( writer, reinterpret_cast<const xmlChar*>( "name" ),
                                       reinterpret_cast<const xmlChar*>( sortedVars[i].key ) ) < 0 ||
          xmlTextWriterWriteAttribute( writer, reinterpret_cast<const xmlChar*>( "value" ),
                                       reinterpret_cast<const xmlChar*>( sortedVars[i].value ) ) < 0 ||
          xmlTextWriterEndElement( writer ) < 0 )
      {
        xmlFreeTextWriter( writer );
        xmlCleanupParser();
        log.printEnd( " Write error" );
        return false;
      }
    }
    if( xmlTextWriterWriteString( writer, reinterpret_cast<const xmlChar*>( "\n" ) ) < 0 ||
        xmlTextWriterEndElement( writer ) < 0 ||
        xmlTextWriterEndDocument( writer ) < 0 )
    {
      xmlFreeTextWriter( writer );
      xmlCleanupParser();
      log.printEnd( " Write error" );
      return false;
    }
    xmlFreeTextWriter( writer );
    xmlCleanupParser();
    log.printEnd( " OK" );
    return true;
  }
#endif

  bool Config::get( const char* name, bool defVal ) const
  {
    assert( !vars.contains( name ) ||
            vars.cachedValue().equals( "true" ) ||
            vars.cachedValue().equals( "false" ) );

    if( vars.contains( name ) ) {
      if( vars.cachedValue().equals( "true" ) ) {
        return true;
      }
      else if( vars.cachedValue().equals( "false" ) ) {
        return false;
      }
      else {
        throw Exception( "Invalid boolean value" );
      }
    }
    return defVal;
  }

  int Config::get( const char* name, int defVal ) const
  {
    if( vars.contains( name ) ) {
      return atoi( vars.cachedValue() );
    }
    else {
      return defVal;
    }
  }

  float Config::get( const char* name, float defVal ) const
  {
    if( vars.contains( name ) ) {
      return float( atof( vars.cachedValue() ) );
    }
    else {
      return defVal;
    }
  }

  double Config::get( const char* name, double defVal ) const
  {
    if( vars.contains( name ) ) {
      return atof( vars.cachedValue() );
    }
    else {
      return defVal;
    }
  }

  const char* Config::get( const char* name, const char* defVal ) const
  {
    if( vars.contains( name ) ) {
      return vars.cachedValue();
    }
    else {
      return defVal;
    }
  }

  bool Config::getSet( const char* name, bool defVal )
  {
    assert( !vars.contains( name ) ||
            vars.cachedValue().equals( "true" ) ||
            vars.cachedValue().equals( "false" ) );

    if( vars.contains( name ) ) {
      if( vars.cachedValue().equals( "true" ) ) {
        return true;
      }
      else if( vars.cachedValue().equals( "false" ) ) {
        return false;
      }
      else {
        throw Exception( "Invalid boolean value" );
      }
    }
    vars.add( name, defVal );
    return defVal;
  }

  int Config::getSet( const char* name, int defVal )
  {
    if( vars.contains( name ) ) {
      return atoi( vars.cachedValue() );
    }
    else {
      vars.add( name, defVal );
      return defVal;
    }
  }

  float Config::getSet( const char* name, float defVal )
  {
    if( vars.contains( name ) ) {
      return float( atof( vars.cachedValue() ) );
    }
    else {
      vars.add( name, defVal );
      return defVal;
    }
  }

  double Config::getSet( const char* name, double defVal )
  {
    if( vars.contains( name ) ) {
      return atof( vars.cachedValue() );
    }
    else {
      vars.add( name, defVal );
      return defVal;
    }
  }

  const char* Config::getSet( const char* name, const char* defVal )
  {
    if( vars.contains( name ) ) {
      return vars.cachedValue();
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
#ifdef OZ_XML_CONFIG
      else if( String::equals( suffix, ".xml" ) ) {
        return loadXML( file );
      }
#endif
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
#ifdef OZ_XML_CONFIG
      else if( String::equals( suffix, ".xml" ) ) {
        return saveXML( file );
      }
#endif
    }

    log.println( "Unknown configuration file %s", file );
    return false;
  }

  void Config::clear()
  {
    vars.clear();
  }

  void Config::deallocate()
  {
    vars.deallocate();
  }

  String Config::toString( const String& indentString )
  {
    String s = "";

    // first we sort all the variables by key
    int size = vars.length();
    Elem sortedVars[size];

    int i = 0;
    foreach( j, vars.iterator() ) {
      sortedVars[i].key = j.key().cstr();
      sortedVars[i].value = j.value().cstr();
      size = ++i;
    }
    aSort( sortedVars, size );

    for( i = 0; i < size; i++ ) {
      s = s + indentString + sortedVars[i].key + " = \"" + sortedVars[i].value + "\"\n";
    }
    return s;
  }

}
