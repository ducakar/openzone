/*
 *  Config.cpp
 *
 *  List that reads configuration file
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "base.hpp"

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

  void Config::add( const String &key, const String &value )
  {
    if( vars.contains( key ) ) {
      vars.cachedValue() = value;
    }
    else {
      vars.add( key, value );
    }
  }

  void Config::remove( const String &key )
  {
    vars.remove( key );
  }

  bool Config::contains( const String &key )
  {
    return vars.contains( key );
  }

  String &Config::operator [] ( const String &key )
  {
    return vars[key];
  }

  bool Config::load( const char *file )
  {
    logFile.print( "Reading variables from '%s' ...", file );

    xmlTextReader *reader = xmlReaderForFile( file, null, 0 );

    if( reader == null ) {
      xmlCleanupParser();
      logFile.printRaw( " Cannot open file\n" );
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
          error = -1;
          break;
        }
        add( (const char*) key, (const char*) value );

        // FIXME VC++ has problems with this WTF???
#ifndef WIN32
        ::free( key );
        ::free( value );
#endif
      }
      error = xmlTextReaderRead( reader );
    }
    xmlFreeTextReader( reader );
    xmlCleanupParser();

    if( error != 0 ) {
      logFile.printRaw( " Parse error\n" );
      return false;
    }
    else {
      logFile.printRaw( " OK\n" );
      return true;
    }
  }

  bool Config::save( const char *file )
  {
    logFile.print( "Writing variables to '%s' ...", file );

    // first we sort all the variables by key
    int size = vars.length();
    Elem sortedVars[size];

    HashString<String, SIZE>::Iterator j( vars );
    for( int i = 0; !j.isPassed(); i++, j++ ) {
      sortedVars[i].key = j.key().cstr();
      sortedVars[i].value = j.value().cstr();
      size = i;
    }
    size++;
    aSort( sortedVars, size );

    xmlTextWriter *writer = xmlNewTextWriterFilename( file, 0 );

    if( writer == null ) {
      xmlCleanupParser();
      logFile.printRaw( " Cannot open file\n" );
      return false;
    }

    if( xmlTextWriterStartDocument( writer, null, "UTF-8", null ) < 0 ||
        xmlTextWriterStartElement( writer, BAD_CAST "config" ) < 0 )
    {
      xmlFreeTextWriter( writer );
      xmlCleanupParser();
      logFile.printRaw( " Write error\n" );
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
        logFile.printRaw( " Write error\n" );
        return false;
      }
    }
    if( xmlTextWriterWriteString( writer, BAD_CAST "\n" ) < 0 ||
        xmlTextWriterEndElement( writer ) < 0 ||
        xmlTextWriterEndDocument( writer ) < 0 )
    {
      xmlFreeTextWriter( writer );
      xmlCleanupParser();
      logFile.printRaw( " Write error\n" );
      return false;
    }
    xmlFreeTextWriter( writer );
    xmlCleanupParser();
    logFile.printRaw( " OK\n" );
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

    for( int i = 0; i < size; i++ ) {
      s = s + indentString + sortedVars[i].key + " = \"" + sortedVars[i].value + "\"\n";
    }
    return s;
  }

}
