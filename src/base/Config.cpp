/*
 *  Config.cpp
 *
 *  List that reads configuration file
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "base.h"

#include <stdlib.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

namespace oz
{

  struct Elem
  {
    const char *key;
    const char *value;

    bool operator < ( const Elem &e ) const
    {
      return String::compare( key, e.key ) < 0;
    }
  };

  Config config;

  bool Config::get( const char *name, bool defVal ) const
  {
    assert( !vars.contains( name ) ||
            vars.cachedValue() == "true" ||
            vars.cachedValue() == "false" );

    if( vars.contains( name ) ) {
      if( vars.cachedValue() == "true" ) {
        return true;
      }
      else if( vars.cachedValue() == "false" ) {
        return false;
      }
      else {
        return defVal;
      }
    }
    else {
      return defVal;
    }
  }

  int Config::get( const char *name, int defVal ) const
  {
    if( vars.contains( name ) ) {
      return atoi( vars.cachedValue() );
    }
    else {
      return defVal;
    }
  }

  float Config::get( const char *name, float defVal ) const
  {
    if( vars.contains( name ) ) {
      return atof( vars.cachedValue() );
    }
    else {
      return defVal;
    }
  }

  double Config::get( const char *name, double defVal ) const
  {
    if( vars.contains( name ) ) {
      return atof( vars.cachedValue() );
    }
    else {
      return defVal;
    }
  }

  const char *Config::get( const char *name, const char *defVal ) const
  {
    if( vars.contains( name ) ) {
      return vars.cachedValue();
    }
    else {
      return defVal;
    }
  }

  bool Config::load( const char *path )
  {
    xmlTextReader *reader = xmlReaderForFile( path, null, 0 );

    if( reader == null ) {
      xmlCleanupParser();
      log.printEnd( "Error reading variables from '%s' ... Cannot open file", path );
      return false;
    }

    int error = xmlTextReaderRead( reader );
    while( error == 1 ) {
      const char *name = (const char*) xmlTextReaderConstName( reader );

      // only check "var" nodes, ignore others
      if( name != null && String::equals( name, "var" ) ) {
        void *key = xmlTextReaderGetAttribute( reader, BAD_CAST "name" );
        void *value = xmlTextReaderGetAttribute( reader, BAD_CAST "value" );

        // error if "var" tag doesn't has "name" and "value" attributes
        if( key == null || value == null ) {
          free( key );
          free( value );

          error = -1;
          break;
        }
        add( (const char*) key, (const char*) value );

        free( key );
        free( value );
      }
      error = xmlTextReaderRead( reader );
    }
    xmlFreeTextReader( reader );
    xmlCleanupParser();

    if( error != 0 ) {
      log.printEnd( "Error reading variables from '%s' ... Parse error", path );
      return false;
    }
    return true;
  }

  bool Config::save( const char *file )
  {
    log.print( "Writing variables to '%s' ...", file );

    // first we sort all the variables by key
    int size = vars.length();
    Elem sortedVars[size];

    typeof( vars.iterator() ) j( vars );
    for( int i = 0; !j.isPassed(); ++i, ++j ) {
      sortedVars[i].key = j.key().cstr();
      sortedVars[i].value = j.value().cstr();
      size = i;
    }
    size++;
    aSort( sortedVars, size );

    xmlTextWriter *writer = xmlNewTextWriterFilename( file, 0 );

    if( writer == null ) {
      xmlCleanupParser();
      log.printEnd( " Cannot open file" );
      return false;
    }

    if( xmlTextWriterStartDocument( writer, null, "UTF-8", null ) < 0 ||
        xmlTextWriterStartElement( writer, BAD_CAST "config" ) < 0 )
    {
      xmlFreeTextWriter( writer );
      xmlCleanupParser();
      log.printEnd( " Write error" );
      return false;
    }

    for( int i = 0; i < size; i++ ) {
      if( xmlTextWriterWriteString( writer, BAD_CAST "\n  " ) < 0 ||
          xmlTextWriterStartElement( writer, BAD_CAST "var" ) < 0 ||
          xmlTextWriterWriteAttribute( writer, BAD_CAST "name", BAD_CAST sortedVars[i].key ) < 0 ||
          xmlTextWriterWriteAttribute( writer, BAD_CAST "value", BAD_CAST sortedVars[i].value ) < 0 ||
          xmlTextWriterEndElement( writer ) < 0 )
      {
        xmlFreeTextWriter( writer );
        xmlCleanupParser();
        log.printEnd( " Write error" );
        return false;
      }
    }
    if( xmlTextWriterWriteString( writer, BAD_CAST "\n" ) < 0 ||
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

  void Config::clear()
  {
    vars.clear();
  }

  String Config::toString( const String &indentString )
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
