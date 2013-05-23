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
 * @file ozCore/Gettext.cc
 */

#include "Gettext.hh"

namespace oz
{

static const uint GETTEXT_MAGIC = 0x950412de;
static const int  MAX_MESSAGES  = 1 << 16;

struct Gettext::Message
{
  int      original;
  int      translation;
  Message* next;
};

Gettext::Gettext() :
  table( nullptr ), messages( nullptr ), strings( nullptr ), nMessages( 0 ), stringsSize( 0 )
{}

Gettext::Gettext( const File& file ) :
  table( nullptr ), messages( nullptr ), strings( nullptr ), nMessages( 0 ), stringsSize( 0 )
{
  import( file );
}

Gettext::~Gettext()
{
  clear();
}

Gettext::Gettext( Gettext&& gt ) :
  table( gt.table ), messages( gt.messages ), strings( gt.strings ), nBuckets( gt.nBuckets ),
  nMessages( gt.nMessages ), stringsSize( gt.stringsSize )
{
  gt.table       = nullptr;
  gt.messages    = nullptr;
  gt.strings     = nullptr;
  gt.nBuckets    = 0;
  gt.nMessages   = 0;
  gt.stringsSize = 0;
}

Gettext& Gettext::operator = ( Gettext&& gt )
{
  if( &gt == this ) {
    return *this;
  }

  clear();

  table       = gt.table;
  messages    = gt.messages;
  strings     = gt.strings;
  nBuckets    = gt.nBuckets;
  nMessages   = gt.nMessages;
  stringsSize = gt.stringsSize;

  gt.table       = nullptr;
  gt.messages    = nullptr;
  gt.strings     = nullptr;
  gt.nBuckets    = 0;
  gt.nMessages   = 0;
  gt.stringsSize = 0;

  return *this;
}

bool Gettext::exists( const char* message ) const
{
  if( nMessages == 0 || String::isEmpty( message ) ) {
    return false;
  }

  uint index = uint( hash( message ) ) % uint( nBuckets );

  for( const Message* m = table[index]; m != nullptr; m = m->next ) {
    if( String::equals( strings + m->original, message ) ) {
      return true;
    }
  }
  return false;
}

const char* Gettext::get( const char* message ) const
{
  if( nMessages == 0 || String::isEmpty( message ) ) {
    return message;
  }

  uint index = uint( hash( message ) ) % uint( nBuckets );

  for( const Message* m = table[index]; m != nullptr; m = m->next ) {
    if( String::equals( strings + m->original, message ) ) {
      return strings + m->translation;
    }
  }
  return message;
}

bool Gettext::import( const File& file )
{
  Buffer buffer = file.read();
  if( buffer.isEmpty() ) {
    return false;
  }

  InputStream istream = buffer.inputStream();

  // Header.
  uint magic = istream.readUInt();
  if( magic != GETTEXT_MAGIC ) {
    if( Endian::bswap32( magic ) == GETTEXT_MAGIC ) {
      istream.setEndian( Endian::Order( !istream.endian() ) );
    }
    else {
      return false;
    }
  }

  istream.readInt();
  int nNewMessages = istream.readInt() - 1;
  if( nNewMessages <= 0 || nNewMessages > MAX_MESSAGES ) {
    return nNewMessages == 0;
  }

  const char* originals    = istream.begin() + istream.readInt() + sizeof( int[2] );
  const char* translations = istream.begin() + istream.readInt() + sizeof( int[2] );

  // Determine size of string chunks, excluding the empty string with its "translation".
  int newStringsSize = 0;

  istream.set( originals );
  for( int i = 0; i < nNewMessages; ++i ) {
    newStringsSize += istream.readInt() + 1;
    istream.readInt();
  }

  istream.set( translations );
  for( int i = 0; i < nNewMessages; ++i ) {
    newStringsSize += istream.readInt() + 1;
    istream.readInt();
  }

  // Expand messages and strings arrays.
  messages = aReallocate<Message>( messages, nMessages, nMessages + nNewMessages );
  strings  = aReallocate<char>( strings, stringsSize, stringsSize + newStringsSize );

  // Add new strings.
  char* stringsPos = strings + stringsSize;

  for( int i = 0; i < nNewMessages; ++i ) {
    istream.set( originals + size_t( i ) * sizeof( int[2] ) );

    int size   = istream.readInt() + 1;
    int offset = istream.readInt();

    messages[nMessages + i].original = int( stringsPos - strings );

    mCopy( stringsPos, istream.begin() + offset, size_t( size ) );
    stringsPos += size;

    istream.set( translations + size_t( i ) * sizeof( int[2] ) );

    size   = istream.readInt() + 1;
    offset = istream.readInt();

    messages[nMessages + i].translation = int( stringsPos - strings );

    mCopy( stringsPos, istream.begin() + offset, size_t( size ) );
    stringsPos += size;
  }

  nMessages   += nNewMessages;
  stringsSize += newStringsSize;

  // Rebuild hashtable.
  nBuckets = ( 4 * nMessages ) / 3;

  delete[] table;
  table = new Message*[nBuckets] {};

  for( int i = 0; i < nMessages; ++i ) {
    uint index = uint( hash( strings + messages[i].original ) ) % uint( nBuckets );

    messages[i].next = table[index];
    table[index] = &messages[i];
  }

  return true;
}

void Gettext::clear()
{
  delete[] table;
  delete[] messages;
  delete[] strings;

  table       = nullptr;
  messages    = nullptr;
  strings     = nullptr;
  nBuckets    = 0;
  nMessages   = 0;
  stringsSize = 0;
}

}
