/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <stable.hh>
#include <common/Lingua.hh>

namespace oz
{
namespace common
{

String Lingua::language;

Lingua::Lingua() :
  messages( nullptr ), nMessages( 0 )
{}

Lingua::~Lingua()
{
  destroy();
}

String Lingua::detectLanguage( const char* language_ )
{
  String lang = language_;

  if( !lang.isEmpty() ) {
    if( File( File::VIRTUAL, "lingua/" + lang ).type() == File::MISSING ) {
      lang = "";
    }
    return lang;
  }

  lang = getenv( "LANGUAGE" );

  if( !lang.isEmpty() && File( File::VIRTUAL, "lingua/" + lang ).type() != File::MISSING ) {
    return lang;
  }

  lang = getenv( "LC_MESSAGES" );

  int underscore = lang.index( '_' );
  if( underscore >= 2 ) {
    lang = lang.substring( 0, underscore );

    if( File( File::VIRTUAL, "lingua/" + lang ).type() != File::MISSING ) {
      return lang;
    }
  }

  lang = getenv( "LANG" );

  underscore = lang.index( '_' );
  if( underscore >= 2 ) {
    lang = lang.substring( 0, underscore );

    if( File( File::VIRTUAL, "lingua/" + lang ).type() != File::MISSING ) {
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

  uint index = uint( hash( message ) ) % uint( nMessages );
  Message* m = messages[index];

  while( m != nullptr ) {
    if( m->original.equals( message ) ) {
      return m->translation;
    }

    m = m->next;
  }

  return message;
}

bool Lingua::initMission( const char* mission )
{
  hard_assert( messages == nullptr );

  File file( File::VIRTUAL, String::str( "mission/%s/lingua/%s.ozCat", mission, language.cstr() ) );

  Buffer buffer = file.read();
  if( buffer.isEmpty() ) {
    return false;
  }

  InputStream is = buffer.inputStream();

  int length = is.readInt();

  nMessages = ( 4 * length ) / 3;
  messages = new Message*[nMessages];
  aFill<Message*, Message*>( messages, nullptr, nMessages );

  for( int i = 0; i < length; ++i ) {
    uint index = uint( is.readInt() ) % uint( nMessages );

    Message* msg = new( msgPool ) Message();

    msg->original    = is.readString();
    msg->translation = is.readString();
    msg->next        = messages[index];

    messages[index] = msg;
  }

  return true;
}

bool Lingua::init( const char* language_ )
{
  hard_assert( messages == nullptr );

  language = language_;

  File dir( File::VIRTUAL, "lingua/" + language );
  if( dir.type() == File::MISSING ) {
    OZ_ERROR( "Invalid locale '%s', does not match any subdirectory in lingua/", language.cstr() );
  }

  DArray<File> files = dir.ls();

  foreach( file, files.iter() ) {
    if( !file->hasExtension( "ozCat" ) ) {
      continue;
    }

    Buffer buffer = file->read();
    if( buffer.isEmpty() ) {
      OZ_ERROR( "Cannot read catalogue '%s'", file->path().cstr() );
    }

    InputStream is = buffer.inputStream();

    nMessages += is.readInt();
  }

  if( nMessages == 0 ) {
    return false;
  }

  nMessages = ( 4 * nMessages ) / 3;
  messages = new Message*[nMessages];
  aFill<Message*, Message*>( messages, nullptr, nMessages );

  foreach( file, files.iter() ) {
    if( !file->hasExtension( "ozCat" ) ) {
      continue;
    }

    Buffer buffer = file->read();
    if( buffer.isEmpty() ) {
      OZ_ERROR( "Cannot read catalogue '%s'", file->path().cstr() );
    }

    InputStream is = buffer.inputStream();

    int length = is.readInt();

    for( int i = 0; i < length; ++i ) {
      uint index = uint( is.readInt() ) % uint( nMessages );

      Message* msg = new( msgPool ) Message();

      msg->original    = is.readString();
      msg->translation = is.readString();
      msg->next        = messages[index];

      messages[index] = msg;
    }
  }

  return true;
}

void Lingua::destroy()
{
  for( int i = 0; i < nMessages; ++i ) {
    Message* chain = messages[i];

    while( chain != nullptr ) {
      Message* next = chain->next;

      chain->~Message();
      msgPool.deallocate( chain );

      chain = next;
    }
  }

  delete[] messages;
  msgPool.free();

  messages  = nullptr;
  nMessages = 0;
}

Lingua lingua;

}
}
