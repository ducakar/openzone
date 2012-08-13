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

Lingua::~Lingua()
{
  free();
}

String Lingua::detectLanguage( const char* language_ )
{
  String lang = language_;

  if( !lang.isEmpty() ) {
    if( !PFile( "lingua/" + lang ).stat() ) {
      lang = "";
    }
    return lang;
  }

  lang = SDL_getenv( "LANGUAGE" );

  if( !lang.isEmpty() && PFile( "lingua/" + lang ).stat() ) {
    return lang;
  }

  lang = SDL_getenv( "LC_MESSAGES" );

  int underscore = lang.index( '_' );
  if( underscore >= 2 ) {
    lang = lang.substring( 0, underscore );

    if( PFile( "lingua/" + lang ).stat() ) {
      return lang;
    }
  }

  lang = SDL_getenv( "LANG" );

  underscore = lang.index( '_' );
  if( underscore >= 2 ) {
    lang = lang.substring( 0, underscore );

    if( PFile( "lingua/" + lang ).stat() ) {
      return lang;
    }
  }

  lang = "";
  return lang;
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

bool Lingua::initMission( const char* mission )
{
  hard_assert( messages == null );

  PFile file( String::str( "mission/%s/lingua/%s.ozCat", mission, language.cstr() ) );

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
  hard_assert( messages == null );

  language = language_;

  PFile dir( "lingua/" + language );
  if( !dir.stat() ) {
    OZ_ERROR( "Invalid locale '%s', does not match any subdirectory in lingua/", language.cstr() );
  }

  DArray<PFile> files = dir.ls();

  foreach( file, files.iter() ) {
    if( !file->hasExtension( "ozCat" ) ) {
      continue;
    }

    if( !file->map() ) {
      OZ_ERROR( "Cannot read catalogue '%s'", file->path().cstr() );
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
      OZ_ERROR( "Cannot read catalogue '%s'", file->path().cstr() );
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
