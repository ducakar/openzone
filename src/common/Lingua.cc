/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file common/Lingua.cc
 */

#include "common/Lingua.hh"

#include <SDL.h>

namespace oz
{

Lingua lingua;

String Lingua::language;

Lingua::Lingua() :
  messages( null ), nMessages( 0 )
{}

String Lingua::detectLanguage( const char* language_ )
{
  String language = language_;

  if( !language.isEmpty() ) {
    if( !PFile( "lingua/" + language + "/main" ).stat() ) {
      language = "";
    }
    return language;
  }

  language = SDL_getenv( "LANGUAGE" );

  if( !language.isEmpty() && PFile( "lingua/" + language + "/main" ).stat() ) {
    return language;
  }

  language = SDL_getenv( "LC_MESSAGES" );

  if( language.length() >= 5 ) {
    language = language.substring( 0, 2 );

    if( PFile( "lingua/" + language + "/main" ).stat() ) {
      return language;
    }
  }

  language = SDL_getenv( "LANG" );

  if( language.length() >= 5 ) {
    language = language.substring( 0, 2 );

    if( PFile( "lingua/" + language + "/main" ).stat() ) {
      return language;
    }
  }

  language = "";
  return language;
}

const char* Lingua::get( const char* message ) const
{
  if( nMessages == 0 || String::isEmpty( message ) ) {
    return message;
  }

  uint index = uint( String::hash( message ) ) % uint( nMessages );
  Message* m = messages[index];

  while( m != null ) {
    if( m->original.equals( message ) ) {
      return m->translation;
    }

    m = m->next;
  }

  return message;
}

bool Lingua::initDomain( const char* domain )
{
  free();

  PFile file( String::str( "lingua/%s/domain/%s.ozCat", language.cstr(), domain ) );

  if( !file.map() ) {
    return false;
  }

  InputStream is = file.inputStream();

  int length = is.readInt();

  nMessages = ( 4 * length ) / 3;
  messages = new Message*[nMessages];
  aSet<Message*, Message*>( messages, null, nMessages );

  for( int i = 0; i < length; ++i ) {
    uint index = uint( is.readInt() ) % uint( nMessages );

    Message* msg = new( msgPool ) Message();

    msg->original    = is.readString();
    msg->translation = is.readString();
    msg->next        = messages[index];

    messages[index] = msg;
  }

  file.unmap();

  return true;
}

bool Lingua::init( const char* language_ )
{
  free();

  language = language_;

  PFile dir( "lingua/" + language + "/main" );
  if( !dir.stat() ) {
    throw Exception( "Invalid locale '%s', does not match any subdirectory in lingua/",
                     language.cstr() );
  }

  DArray<PFile> files = dir.ls();

  foreach( file, files.iter() ) {
    if( !file->hasExtension( "ozCat" ) ) {
      continue;
    }

    if( !file->map() ) {
      throw Exception( "Cannot read catalogue '%s'", file->path().cstr() );
    }

    InputStream is = file->inputStream();

    nMessages += is.readInt();

    file->unmap();
  }

  if( nMessages == 0 ) {
    return false;
  }

  nMessages = ( 4 * nMessages ) / 3;
  messages = new Message*[nMessages];
  aSet<Message*, Message*>( messages, null, nMessages );

  foreach( file, files.iter() ) {
    if( !file->hasExtension( "ozCat" ) ) {
      continue;
    }

    if( !file->map() ) {
      throw Exception( "Cannot read catalogue '%s'", file->path().cstr() );
    }

    InputStream is = file->inputStream();

    int length = is.readInt();

    for( int i = 0; i < length; ++i ) {
      uint index = uint( is.readInt() ) % uint( nMessages );

      Message* msg = new( msgPool ) Message();

      msg->original    = is.readString();
      msg->translation = is.readString();
      msg->next        = messages[index];

      messages[index] = msg;
    }

    file->unmap();
  }

  return true;
}

void Lingua::free()
{
  for( int i = 0; i < nMessages; ++i ) {
    Message* chain = messages[i];

    while( chain != null ) {
      Message* next = chain->next;

      chain->~Message();
      msgPool.dealloc( chain );

      chain = next;
    }
  }

  delete[] messages;
  msgPool.free();

  messages  = null;
  nMessages = 0;
}

}
