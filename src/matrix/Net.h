/*
 *  Net.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  class Net
  {
    public:

      static const int BUFFER_SIZE = 4096;

      enum ObjectType
      {
        PARTICLE,
        OBJECT,
        STRUCTURE,
        BSP
      };

      enum ActionType
      {
        ADD,
        REMOVE,
        PUT,
        CUT,
        CLEAR,
        USE,
        TAKE,
        ACTION
      };

      struct Action
      {
        ActionType type;
        int        index;
        int        param;

        Action() {}
        Action( ActionType type_ ) : type( type_ ) {}
        Action( ActionType type_, int index_ ) : type( type_ ), index( index_ ) {}
        Action( ActionType type_, int index_, int param_ ) :
            type( type_ ), index( index_ ), param( param_ )
        {}
      };

      struct Packet
      {
        char *pos;
        char data[BUFFER_SIZE];

        bool readBool()
        {
          void *p = pos;
          pos += sizeof( bool );
          return *(bool*) p;
        }

        void writeBool( bool b )
        {
          *(bool*) pos = b;
          pos += sizeof( bool );
        }

        int readInt()
        {
          void *p = pos;
          pos += sizeof( int );
          return SDL_SwapBE32( *(int*) p );
        }

        void writeInt( int i )
        {
          *(int*) pos = SDL_SwapBE32( i );
          pos += sizeof( int );
        }

        float readFloat()
        {
          pos += sizeof( float );
          return *(float*) ( pos - sizeof( float ) );
        }

        void writeFloat( float f )
        {
          *(float*) pos = f;
          pos += sizeof( float );
        }

        String readString()
        {
          // check for buffer overruns
          char *p = pos;
          char *end = data + BUFFER_SIZE;
          while( p < end && *p != '\0' ) {
            p++;
          }
          if( p < end ) {
            return String( pos );
          }
          else {
            return String( "" );
          }
        }

        void writeString( const String &s )
        {
          //TODO read/write-String
          strcpy( pos, s );
        }

        Vec3 readVec3()
        {
          pos += sizeof( Vec3 );
          return Vec3( (float*) ( pos - sizeof( Vec3 ) ) );
        }

        void writeVec3( const Vec3 &v )
        {
          *(Vec3*) pos = v;
          pos += sizeof( Vec3 );
        }
      };

    public:

      bool isClient;

      Vector<Action> particles;
      Vector<Action> objects;
      Vector<Action> structs;
      Vector<Action> bsps;
      Vector<Action> world;

      Net();

  };

  extern Net net;

}
