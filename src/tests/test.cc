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
 * @file tests/test.cc
 */

#include <ozCore/ozCore.hh>
#include <ozEngine/ozEngine.hh>
#include <ozFactory/ozFactory.hh>

#include <SDL_ttf.h>

using namespace oz;

static const char* rwError = "";

static Sint64 rwSize( struct SDL_RWops* context )
{
  return context->hidden.mem.stop - context->hidden.mem.base;
}

static Sint64 rwSeek( struct SDL_RWops* context, Sint64 offset, int whence )
{
  Uint8* newHere;

  switch( whence ) {
    case RW_SEEK_SET: {
      newHere = context->hidden.mem.base + offset;
      break;
    }
    case RW_SEEK_CUR: {
      newHere = context->hidden.mem.here + offset;
      break;
    }
    case RW_SEEK_END: {
      newHere = context->hidden.mem.stop + offset;
      break;
    }
    default: {
      rwError = "RW seek invalid whence";
      return -1;
    }
  }

  if( newHere < context->hidden.mem.base ) {
    newHere = context->hidden.mem.base;
  }
  if( newHere > context->hidden.mem.stop ) {
    newHere = context->hidden.mem.stop;
  }

  context->hidden.mem.here = newHere;
  return newHere - context->hidden.mem.base;
}

static size_t rwRead( struct SDL_RWops* context, void* ptr, size_t size, size_t maxnum )
{
  size_t nBytes   = maxnum * size;
  size_t maxBytes = size_t( context->hidden.mem.stop - context->hidden.mem.here );

  if( nBytes > maxBytes ) {
    maxnum = maxBytes / size;
    nBytes = maxnum * size;
  }

  memcpy( ptr, context->hidden.mem.here, nBytes );
  context->hidden.mem.here += nBytes;

  return maxnum;
}

static size_t rwWrite( struct SDL_RWops*, const void*, size_t, size_t )
{
  hard_assert( false );
  rwError = "RW write not implemented";
  return 0;
}

static int rwClose( struct SDL_RWops* context )
{
  if( context != nullptr ) {
    free( context );
  }
  return 0;
}

extern "C"
const char* SDL_GetError()
{
  return rwError;
}

extern "C"
SDL_RWops* SDL_RWFromConstMem( const void* mem, int size )
{
  struct SDL_RWops* rwOps = (SDL_RWops*) malloc( sizeof *rwOps );

  rwOps->size            = rwSize;
  rwOps->seek            = rwSeek;
  rwOps->read            = rwRead;
  rwOps->write           = rwWrite;
  rwOps->close           = rwClose;
  rwOps->type            = SDL_RWOPS_MEMORY_RO;
  rwOps->hidden.mem.base = (Uint8*) mem;
  rwOps->hidden.mem.here = (Uint8*) mem;
  rwOps->hidden.mem.stop = (Uint8*) mem + size;

  return rwOps;
}

int main()
{
  System::init();
  File::init();
  TTF_Init();

  const char* name   = "DroidSansMono";
  const int   height = 12;

  File   file   = String::str( "/usr/share/fonts/TTF/%s.ttf", name );
  Buffer buffer = file.read();

  if( buffer.isEmpty() ) {
    OZ_ERROR( "Failed to read font file '%s'", file.path().cstr() );
  }

  SDL_RWops* rwOps  = SDL_RWFromConstMem( buffer.begin(), buffer.length() );
  TTF_Font*  handle = TTF_OpenFontRW( rwOps, true, height );

  if( handle == nullptr ) {
    OZ_ERROR( "TTF Error: %s", TTF_GetError() );
  }

  TTF_CloseFont( handle );

  File::destroy();
  return 0;
}
