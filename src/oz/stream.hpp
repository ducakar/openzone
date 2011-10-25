/*
 *  stream.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file oz/stream.hpp
 */

#include "Exception.hpp"
#include "Plane.hpp"
#include "Mat44.hpp"

namespace oz
{

/**
 * %Endian conversion.
 *
 * @ingroup oz
 */
class Endian
{
  private:

    /**
     * Singleton.
     */
    Endian();

  public:

    /**
     * Swap byte order if OZ_BIG_ENDIAN_ARCH does not match OZ_BIG_ENDIAN_STREAM.
     */
    OZ_ALWAYS_INLINE
    static short shuffle16( short s )
    {
#if defined( OZ_BIG_ENDIAN_STREAM ) == defined( OZ_BIG_ENDIAN_ARCH )
      return s;
#else
      return short( ushort( s ) << 8 | ushort( s ) >> 8 );
#endif
    }

    /**
     * Swap byte order if OZ_BIG_ENDIAN_ARCH does not match OZ_BIG_ENDIAN_STREAM.
     */
    OZ_ALWAYS_INLINE
    static int shuffle32( int i )
    {
#if defined( OZ_BIG_ENDIAN_STREAM ) == defined( OZ_BIG_ENDIAN_ARCH )
      return i;
#else
      return __builtin_bswap32( i );
#endif
    }

    /**
     * Swap byte order if OZ_BIG_ENDIAN_ARCH does not match OZ_BIG_ENDIAN_STREAM.
     */
    OZ_ALWAYS_INLINE
    static long64 shuffle64( long64 l )
    {
#if defined( OZ_BIG_ENDIAN_STREAM ) == defined( OZ_BIG_ENDIAN_ARCH )
      return l;
#else
      return __builtin_bswap64( l );
#endif
    }

};

/**
 * Read-only stream.
 *
 * @ingroup oz
 */
class InputStream
{
  private:

    const char* pos;   ///< Current position.
    const char* start; ///< Beginning.
    const char* end;   ///< End.

  public:

    /**
     * Create a stream with the given beginning and the end.
     */
    OZ_ALWAYS_INLINE
    explicit InputStream( const char* start_, const char* end_ ) :
        pos( start_ ), start( start_ ), end( end_ )
    {}

    /**
     * Number of bytes from the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return int( size_t( pos - start ) );
    }

    /**
     * Number of bytes left before end of the stream is reached.
     */
    OZ_ALWAYS_INLINE
    int available() const
    {
      return int( size_t( end - pos ) );
    }

    /**
     * True iff there is still some bytes left on the stream.
     */
    OZ_ALWAYS_INLINE
    bool isAvailable() const
    {
      return pos != end;
    }

    /**
     * Pointer to the current position.
     */
    OZ_ALWAYS_INLINE
    const char* getPos() const
    {
      hard_assert( start <= pos && pos <= end );

      return pos;
    }

    /**
     * Set the curent position.
     */
    OZ_ALWAYS_INLINE
    void setPos( const char* newPos )
    {
      hard_assert( start <= newPos && newPos <= end );

      pos = newPos;
    }

    /**
     * Rewind current position to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    void reset()
    {
      pos = start;
    }

    /**
     * Skip <tt>count</tt> bytes.
     *
     * @return Constant pointer to the beginning of the skipped bytes.
     */
    OZ_ALWAYS_INLINE
    const char* prepareRead( int count )
    {
      const char* oldPos = pos;
      pos += count;

#ifndef NDEBUG
      if( pos > end ) {
        pos -= count;
        throw Exception( "Buffer overrun for " + String( int( ptrdiff_t( pos + count - end ) ) ) +
                         " bytes during a read of " + String( count ) + " bytes" );
      }
#endif
      return oldPos;
    }

    /**
     * Read boolean.
     */
    OZ_ALWAYS_INLINE
    bool readBool()
    {
      const bool* data = reinterpret_cast<const bool*>( prepareRead( sizeof( bool ) ) );
      return *data;
    }

    /**
     * Read character.
     */
    OZ_ALWAYS_INLINE
    char readChar()
    {
      const char* data = reinterpret_cast<const char*>( prepareRead( sizeof( char ) ) );
      return *data;
    }

    /**
     * Read an array of characters.
     */
    OZ_ALWAYS_INLINE
    void readChars( char* array, int count )
    {
      const char* data = reinterpret_cast<const char*>( prepareRead( count ) );
      aCopy( array, data, count );
    }

    /**
     * Read short integer.
     */
    OZ_ALWAYS_INLINE
    short readShort()
    {
      const short* data = reinterpret_cast<const short*>( prepareRead( sizeof( short ) ) );
      return Endian::shuffle16( *data );
    }

    /**
     * Read integer.
     */
    OZ_ALWAYS_INLINE
    int readInt()
    {
      const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( int ) ) );
      return Endian::shuffle32( *data );
    }

    /**
     * Read float.
     */
    OZ_ALWAYS_INLINE
    float readFloat()
    {
      const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( int ) ) );
      return Math::fromBits( Endian::shuffle32( *data ) );
    }

    /**
     * Read double.
     */
    OZ_ALWAYS_INLINE
    double readDouble()
    {
      union BitsToDouble
      {
        long64 b;
        double d;
      };

      const long64* data = reinterpret_cast<const long64*>( prepareRead( sizeof( long64 ) ) );
      BitsToDouble bd = { Endian::shuffle64( *data ) };
      return bd.d;
    }

    /**
     * Read string.
     */
    OZ_ALWAYS_INLINE
    String readString()
    {
      int length = 0;
      while( pos + length < end && pos[length] != '\0' ) {
        ++length;
      }
      if( pos + length == end ) {
        throw Exception( "End of buffer reached while looking for end of string." );
      }
      return String( prepareRead( length + 1 ), length );
    }

    /**
     * Read string, padded with null character till the length of <tt>size</tt> bytes.
     */
    OZ_ALWAYS_INLINE
    String readPaddedString( int size )
    {
      const char* data = reinterpret_cast<const char*>( prepareRead( size ) );
      if( data[size - 1] != '\0' ) {
        throw Exception( "Padded string does not end with NULL character." );
      }
      return String( data );
    }

    /**
     * Read 3D vector.
     */
    OZ_ALWAYS_INLINE
    Vec3 readVec3()
    {
      const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( float[3] ) ) );
      return Vec3( Math::fromBits( Endian::shuffle32( data[0] ) ),
                   Math::fromBits( Endian::shuffle32( data[1] ) ),
                   Math::fromBits( Endian::shuffle32( data[2] ) ) );
    }

    /**
     * Read 4-component vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 readVec4()
    {
      const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( float[4] ) ) );
      return Vec4( Math::fromBits( Endian::shuffle32( data[0] ) ),
                   Math::fromBits( Endian::shuffle32( data[1] ) ),
                   Math::fromBits( Endian::shuffle32( data[2] ) ),
                   Math::fromBits( Endian::shuffle32( data[3] ) ) );
    }

    /**
     * Read 3D point.
     */
    OZ_ALWAYS_INLINE
    Point3 readPoint3()
    {
      const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( float[3] ) ) );
      return Point3( Math::fromBits( Endian::shuffle32( data[0] ) ),
                     Math::fromBits( Endian::shuffle32( data[1] ) ),
                     Math::fromBits( Endian::shuffle32( data[2] ) ) );
    }

    /**
     * Read 3D plane.
     */
    OZ_ALWAYS_INLINE
    Plane readPlane()
    {
      const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( float[4] ) ) );
      return Plane( Math::fromBits( Endian::shuffle32( data[0] ) ),
                    Math::fromBits( Endian::shuffle32( data[1] ) ),
                    Math::fromBits( Endian::shuffle32( data[2] ) ),
                    Math::fromBits( Endian::shuffle32( data[3] ) ) );
    }

    /**
     * Read quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat readQuat()
    {
      const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( float[4] ) ) );
      return Quat( Math::fromBits( Endian::shuffle32( data[0] ) ),
                   Math::fromBits( Endian::shuffle32( data[1] ) ),
                   Math::fromBits( Endian::shuffle32( data[2] ) ),
                   Math::fromBits( Endian::shuffle32( data[3] ) ) );
    }

    /**
     * Read 4x4 matrix.
     */
    OZ_ALWAYS_INLINE
    Mat44 readMat44()
    {
      const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( float[16] ) ) );
      return Mat44( Math::fromBits( Endian::shuffle32( data[ 0] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 1] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 2] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 3] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 4] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 5] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 6] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 7] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 8] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 9] ) ),
                    Math::fromBits( Endian::shuffle32( data[10] ) ),
                    Math::fromBits( Endian::shuffle32( data[11] ) ),
                    Math::fromBits( Endian::shuffle32( data[12] ) ),
                    Math::fromBits( Endian::shuffle32( data[13] ) ),
                    Math::fromBits( Endian::shuffle32( data[14] ) ),
                    Math::fromBits( Endian::shuffle32( data[15] ) ) );
    }
};

/**
 * Write-only stream.
 *
 * @ingroup oz
 */
class OutputStream
{
  private:

    char*       pos;   ///< Current position.
    char*       start; ///< Beginning.
    const char* end;   ///< End.

  public:

    /**
     * Create a stream with the given beginning and the end.
     */
    OZ_ALWAYS_INLINE
    explicit OutputStream( char* start_, const char* end_ ) :
        pos( start_ ), start( start_ ), end( end_ )
    {}

    /**
     * Number of bytes from the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return int( size_t( pos - start ) );
    }

    /**
     * Number of bytes left before end of the stream is reached.
     */
    OZ_ALWAYS_INLINE
    int available() const
    {
      return int( size_t( end - pos ) );
    }

    /**
     * True iff there is still some bytes left before end of the stream is reached.
     */
    OZ_ALWAYS_INLINE
    bool isAvailable() const
    {
      return pos != end;
    }

    /**
     * Pointer to the current position.
     */
    OZ_ALWAYS_INLINE
    char* getPos() const
    {
      hard_assert( start <= pos && pos <= end );

      return pos;
    }

    /**
     * Set the current position.
     */
    OZ_ALWAYS_INLINE
    void setPos( char* newPos )
    {
      hard_assert( start <= newPos && newPos <= end );

      pos = newPos;
    }

    /**
     * Rewind current position to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    void reset()
    {
      pos = start;
    }

    /**
     * Move position pointer for <tt>count</tt> bytes forward.
     *
     * @return Pointer to the beginning of the skipped bytes.
     */
    OZ_ALWAYS_INLINE
    char* prepareWrite( int count )
    {
      char* oldPos = pos;
      pos += count;

      if( pos > end ) {
        pos -= count;
        throw Exception( "Buffer overrun for " + String( int( ptrdiff_t( pos + count - end ) ) ) +
                         " bytes during a write of " + String( count ) + " bytes" );
      }
      return oldPos;
    }

    /**
     * Write a boolean.
     */
    OZ_ALWAYS_INLINE
    void writeBool( bool b )
    {
      bool* data = reinterpret_cast<bool*>( prepareWrite( sizeof( bool ) ) );
      *data = b;
    }

    /**
     * Write a character.
     */
    OZ_ALWAYS_INLINE
    void writeChar( char c )
    {
      char* data = reinterpret_cast<char*>( prepareWrite( sizeof( char ) ) );
      *data = c;
    }

    /**
     * Write an array of characters.
     */
    OZ_ALWAYS_INLINE
    void writeChars( char *array, int count )
    {
      char* data = reinterpret_cast<char*>( prepareWrite( count * int( sizeof( char ) ) ) );
      aCopy( data, array, count );
    }

    /**
     * Write short integer.
     */
    OZ_ALWAYS_INLINE
    void writeShort( short s )
    {
      short* data = reinterpret_cast<short*>( prepareWrite( sizeof( short ) ) );
      *data = Endian::shuffle16( s );
    }

    /**
     * Write integer.
     */
    OZ_ALWAYS_INLINE
    void writeInt( int i )
    {
      int* data = reinterpret_cast<int*>( prepareWrite( sizeof( int ) ) );
      *data = Endian::shuffle32( i );
    }

    /**
     * Write float.
     */
    OZ_ALWAYS_INLINE
    void writeFloat( float f )
    {
      int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float ) ) );
      *data = Endian::shuffle32( Math::toBits( f ) );
    }

    /**
     * Write double.
     */
    OZ_ALWAYS_INLINE
    void writeDouble( double d )
    {
      union DoubleToBits
      {
        double d;
        long64 b;
      };
      DoubleToBits db = { d };

      long64* data = reinterpret_cast<long64*>( prepareWrite( sizeof( double ) ) );
      *data = Endian::shuffle64( db.b );
    }

    /**
     * Write string.
     */
    OZ_ALWAYS_INLINE
    void writeString( const String& s )
    {
      int size = s.length() + 1;
      char* data = prepareWrite( size );

      aCopy( data, s.cstr(), size );
    }

    /**
     * Write C string.
     */
    OZ_ALWAYS_INLINE
    void writeString( const char* s )
    {
      int size = String::length( s ) + 1;
      char* data = prepareWrite( size );

      aCopy( data, s, size );
    }

    /**
     * Write string padded with null characters to the given number of bytes.
     */
    OZ_ALWAYS_INLINE
    void writePaddedString( const String& s, int size )
    {
      int length = s.length();
      char* data = prepareWrite( size );

      aCopy( data, s.cstr(), length );
      aSet( data + length, '\0', size - length );
    }

    /**
     * Write C string padded with null characters to the given number of bytes.
     */
    OZ_ALWAYS_INLINE
    void writePaddedString( const char* s, int size )
    {
      int length = String::length( s );
      char* data = prepareWrite( size );

      aCopy( data, s, length );
      aSet( data + length, '\0', size - length );
    }

    /**
     * Write 3D vector.
     */
    OZ_ALWAYS_INLINE
    void writeVec3( const Vec3& v )
    {
      int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float[3] ) ) );

      data[0] = Endian::shuffle32( Math::toBits( v.x ) );
      data[1] = Endian::shuffle32( Math::toBits( v.y ) );
      data[2] = Endian::shuffle32( Math::toBits( v.z ) );
    }

    /**
     * Write 4-component vector.
     */
    OZ_ALWAYS_INLINE
    void writeVec4( const Vec4& v )
    {
      int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float[4] ) ) );

      data[0] = Endian::shuffle32( Math::toBits( v.x ) );
      data[1] = Endian::shuffle32( Math::toBits( v.y ) );
      data[2] = Endian::shuffle32( Math::toBits( v.z ) );
      data[3] = Endian::shuffle32( Math::toBits( v.w ) );
    }

    /**
     * Write 3D point.
     */
    OZ_ALWAYS_INLINE
    void writePoint3( const Point3& p )
    {
      int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float[3] ) ) );

      data[0] = Endian::shuffle32( Math::toBits( p.x ) );
      data[1] = Endian::shuffle32( Math::toBits( p.y ) );
      data[2] = Endian::shuffle32( Math::toBits( p.z ) );
    }

    /**
     * Write 3D plane.
     */
    OZ_ALWAYS_INLINE
    void writePlane( const Plane& p )
    {
      int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float[4] ) ) );

      data[0] = Endian::shuffle32( Math::toBits( p.nx ) );
      data[1] = Endian::shuffle32( Math::toBits( p.ny ) );
      data[2] = Endian::shuffle32( Math::toBits( p.nz ) );
      data[3] = Endian::shuffle32( Math::toBits( p.d ) );
    }

    /**
     * Write quaternion.
     */
    OZ_ALWAYS_INLINE
    void writeQuat( const Quat& q )
    {
      int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float[4] ) ) );

      data[0] = Endian::shuffle32( Math::toBits( q.x ) );
      data[1] = Endian::shuffle32( Math::toBits( q.y ) );
      data[2] = Endian::shuffle32( Math::toBits( q.z ) );
      data[3] = Endian::shuffle32( Math::toBits( q.w ) );
    }

    /**
     * Write 4x4 matrix.
     */
    OZ_ALWAYS_INLINE
    void writeMat44( const Mat44& m )
    {
      int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float[16] ) ) );

      data[ 0] = Endian::shuffle32( Math::toBits( m.x.x ) );
      data[ 1] = Endian::shuffle32( Math::toBits( m.x.y ) );
      data[ 2] = Endian::shuffle32( Math::toBits( m.x.z ) );
      data[ 3] = Endian::shuffle32( Math::toBits( m.x.w ) );
      data[ 4] = Endian::shuffle32( Math::toBits( m.y.x ) );
      data[ 5] = Endian::shuffle32( Math::toBits( m.y.y ) );
      data[ 6] = Endian::shuffle32( Math::toBits( m.y.z ) );
      data[ 7] = Endian::shuffle32( Math::toBits( m.y.w ) );
      data[ 8] = Endian::shuffle32( Math::toBits( m.z.x ) );
      data[ 9] = Endian::shuffle32( Math::toBits( m.z.y ) );
      data[10] = Endian::shuffle32( Math::toBits( m.z.z ) );
      data[11] = Endian::shuffle32( Math::toBits( m.z.w ) );
      data[12] = Endian::shuffle32( Math::toBits( m.w.x ) );
      data[13] = Endian::shuffle32( Math::toBits( m.w.y ) );
      data[14] = Endian::shuffle32( Math::toBits( m.w.z ) );
      data[15] = Endian::shuffle32( Math::toBits( m.w.w ) );
    }
};

}
