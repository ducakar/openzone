/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file oz/BufferStream.hh
 *
 * BufferStream class.
 */

#pragma once

#include "OutputStream.hh"

namespace oz
{

/**
 * Buffered read/write stream.
 *
 * Stream uses an underlying buffer that is dynamically growing.
 *
 * @ingroup oz
 */
class BufferStream
{
  private:

    /// Capacity is always a multiple of <tt>GRANULARITY</tt>.
    static const int GRANULARITY = 4096;

    char*         pos;   ///< Current position.
    char*         start; ///< Beginning, also pointer to the buffer.
    const char*   end;   ///< End.
    Endian::Order order; ///< Stream byte order.

  public:

    /**
     * Create an empty stream.
     */
    BufferStream( Endian::Order order_ = Endian::NATIVE ) :
      pos( null ), start( null ), end( null ), order( order_ )
    {}

    /**
     * Destructor.
     */
    ~BufferStream()
    {
      delete[] start;
    }

    /**
     * Copy constructor, copies buffer.
     */
    BufferStream( const BufferStream& s )
    {
      int length = int( s.pos - s.start );
      int size   = int( s.end - s.start );

      start = size == 0 ? null : new char[size];
      end   = start + size;
      pos   = start + length;
      order = s.order;

      aCopy<char>( start, s.start, size );
    }

    /**
     * Move constructor, moves buffer.
     */
    BufferStream( BufferStream&& s ) :
      pos( s.pos ), start( s.start ), end( s.end ), order( s.order )
    {
      s.pos   = null;
      s.start = null;
      s.end   = null;
      s.order = Endian::NATIVE;
    }

    /**
     * Copy operator, copies buffer.
     *
     * Reuse existing buffer if it suffices.
     */
    BufferStream& operator = ( const BufferStream& s )
    {
      if( &s == this ) {
        return *this;
      }

      int length = int( s.pos - s.start );
      int size   = int( s.end - s.start );

      if( int( end - start ) < size ) {
        delete[] start;

        start = new char[size];
        end   = start + size;
      }

      pos = start + length;
      order = s.order;
      aCopy<char>( start, s.start, size );

      return *this;
    }

    /**
     * Move operator, moves buffer.
     */
    BufferStream& operator = ( BufferStream&& s )
    {
      if( &s == this ) {
        return *this;
      }

      delete[] start;

      pos   = s.pos;
      start = s.start;
      end   = s.end;
      order = s.order;

      s.pos   = null;
      s.start = null;
      s.end   = null;
      s.order = Endian::NATIVE;

      return *this;
    }

    /**
     * Create a stream with the given size of the buffer.
     */
    explicit BufferStream( int size ) :
      pos( new char[size] ), start( pos ), end( start + size ), order( Endian::NATIVE )
    {}

    /**
     * Create <tt>InputStream</tt> for reading this stream (position is not reset).
     */
    OZ_ALWAYS_INLINE
    InputStream inputStream() const
    {
      InputStream is( start, end, order );

      is.pos = pos;
      return is;
    }

    /**
     * Create <tt>OutputStream</tt> for reading/writing to this stream (position is not reset).
     */
    OZ_ALWAYS_INLINE
    OutputStream outputStream()
    {
      OutputStream os( start, end, order );

      os.pos = pos;
      return os;
    }

    /**
     * Number of bytes from the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      hard_assert( pos <= end );

      return int( pos - start );
    }

    /**
     * Maximum length of the stream.
     */
    OZ_ALWAYS_INLINE
    int capacity() const
    {
      hard_assert( pos <= end );

      return int( end - start );
    }

    /**
     * Number of bytes left before end of the stream is reached.
     */
    OZ_ALWAYS_INLINE
    int available() const
    {
      hard_assert( pos <= end );

      return int( end - pos );
    }

    /**
     * True iff there is still some bytes left before end of the stream is reached.
     */
    OZ_ALWAYS_INLINE
    bool isAvailable() const
    {
      hard_assert( pos <= end );

      return pos != end;
    }

    /**
     * Constant reference to the i-th byte from the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    const char& operator [] ( int i ) const
    {
      hard_assert( uint( i ) < uint( end - start ) );

      return start[i];
    }

    /**
     * Reference to the i-th byte from the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    char& operator [] ( int i )
    {
      hard_assert( uint( i ) < uint( end - start ) );

      return start[i];
    }

    /**
     * Constant Pointer to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    const char* begin() const
    {
      return start;
    }

    /**
     * Pointer to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    char* begin()
    {
      return start;
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
     * Pointer to the current position.
     */
    OZ_ALWAYS_INLINE
    char* getPos()
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
     * Get byte order.
     */
    OZ_ALWAYS_INLINE
    Endian::Order endian() const
    {
      return order;
    }

    /**
     * Set byte order.
     */
    OZ_ALWAYS_INLINE
    void setEndian( Endian::Order order_ )
    {
      order = order_;
    }

    /**
     * Move position pointer for <tt>count</tt> bytes forward.
     *
     * @return Pointer to the beginning of the skipped bytes.
     */
    OZ_ALWAYS_INLINE
    char* forward( int count )
    {
      char* oldPos = pos;
      pos += count;

      if( pos > end ) {
        int length  = int( pos - start );
        int size    = int( end - start );
        int newSize = size == 0 ? GRANULARITY : 2 * size;

        if( newSize < length + count ) {
          newSize = ( ( length + count - 1 ) / GRANULARITY + 1 ) * GRANULARITY;
        }

        start  = aRealloc<char>( start, size, newSize );
        end    = start + newSize;
        pos    = start + length;
        oldPos = pos - count;
      }
      return oldPos;
    }

    /**
     * Read boolean.
     */
    OZ_ALWAYS_INLINE
    bool readBool()
    {
      const bool* data = reinterpret_cast<const bool*>( forward( sizeof( bool ) ) );
      return *data;
    }

    /**
     * Write a boolean.
     */
    OZ_ALWAYS_INLINE
    void writeBool( bool b )
    {
      bool* data = reinterpret_cast<bool*>( forward( sizeof( bool ) ) );
      *data = b;
    }

    /**
     * Read character.
     */
    OZ_ALWAYS_INLINE
    char readChar()
    {
      const char* data = reinterpret_cast<const char*>( forward( sizeof( char ) ) );
      return *data;
    }

    /**
     * Write a character.
     */
    OZ_ALWAYS_INLINE
    void writeChar( char c )
    {
      char* data = reinterpret_cast<char*>( forward( sizeof( char ) ) );
      *data = c;
    }

    /**
     * Read an array of characters.
     */
    OZ_ALWAYS_INLINE
    void readChars( char* array, int count )
    {
      const char* data = reinterpret_cast<const char*>( forward( count * int( sizeof( char ) ) ) );
      aCopy<char>( array, data, count );
    }

    /**
     * Write an array of characters.
     */
    OZ_ALWAYS_INLINE
    void writeChars( const char* array, int count )
    {
      char* data = reinterpret_cast<char*>( forward( count * int( sizeof( char ) ) ) );
      aCopy<char>( data, array, count );
    }

    /**
     * Read byte.
     */
    OZ_ALWAYS_INLINE
    byte readByte()
    {
      const byte* data = reinterpret_cast<const byte*>( forward( sizeof( byte ) ) );
      return *data;
    }

    /**
     * Write byte.
     */
    OZ_ALWAYS_INLINE
    void writeByte( byte b )
    {
      byte* data = reinterpret_cast<byte*>( forward( sizeof( byte ) ) );
      *data = b;
    }

    /**
     * Read unsigned byte.
     */
    OZ_ALWAYS_INLINE
    ubyte readUByte()
    {
      const ubyte* data = reinterpret_cast<const ubyte*>( forward( sizeof( ubyte ) ) );
      return *data;
    }

    /**
     * Write unsigned byte.
     */
    OZ_ALWAYS_INLINE
    void writeUByte( ubyte b )
    {
      ubyte* data = reinterpret_cast<ubyte*>( forward( sizeof( ubyte ) ) );
      *data = b;
    }

    /**
     * Read short integer.
     */
    OZ_ALWAYS_INLINE
    short readShort()
    {
      const short* data = reinterpret_cast<const short*>( forward( sizeof( short ) ) );

      if( order == Endian::NATIVE ) {
        return *data;
      }
      else {
        return Endian::bswap16( *data );
      }
    }

    /**
     * Write short integer.
     */
    OZ_ALWAYS_INLINE
    void writeShort( short s )
    {
      short* data = reinterpret_cast<short*>( forward( sizeof( short ) ) );

      if( order == Endian::NATIVE ) {
        *data = s;
      }
      else {
        *data = Endian::bswap16( s );
      }
    }

    /**
     * Read unsigned short integer.
     */
    OZ_ALWAYS_INLINE
    ushort readUShort()
    {
      const ushort* data = reinterpret_cast<const ushort*>( forward( sizeof( ushort ) ) );

      if( order == Endian::NATIVE ) {
        return *data;
      }
      else {
        return Endian::bswap16( *data );
      }
    }

    /**
     * Write unsigned short integer.
     */
    OZ_ALWAYS_INLINE
    void writeUShort( ushort s )
    {
      ushort* data = reinterpret_cast<ushort*>( forward( sizeof( ushort ) ) );

      if( order == Endian::NATIVE ) {
        *data = s;
      }
      else {
        *data = Endian::bswap16( s );
      }
    }

    /**
     * Read integer.
     */
    OZ_ALWAYS_INLINE
    int readInt()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( int ) ) );

      if( order == Endian::NATIVE ) {
        return *data;
      }
      else {
        return Endian::bswap32( *data );
      }
    }

    /**
     * Write integer.
     */
    OZ_ALWAYS_INLINE
    void writeInt( int i )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( int ) ) );

      if( order == Endian::NATIVE ) {
        *data = i;
      }
      else {
        *data = Endian::bswap32( i );
      }
    }

    /**
     * Read unsigned integer.
     */
    OZ_ALWAYS_INLINE
    uint readUInt()
    {
      const uint* data = reinterpret_cast<const uint*>( forward( sizeof( uint ) ) );

      if( order == Endian::NATIVE ) {
        return *data;
      }
      else {
        return Endian::bswap32( *data );
      }
    }

    /**
     * Write unsigned integer.
     */
    OZ_ALWAYS_INLINE
    void writeUInt( uint i )
    {
      uint* data = reinterpret_cast<uint*>( forward( sizeof( uint ) ) );

      if( order == Endian::NATIVE ) {
        *data = i;
      }
      else {
        *data = Endian::bswap32( i );
      }
    }

    /**
     * Read 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    long64 readLong64()
    {
      const long64* data = reinterpret_cast<const long64*>( forward( sizeof( long64 ) ) );

      if( order == Endian::NATIVE ) {
        return *data;
      }
      else {
        return Endian::bswap64( *data );
      }
    }

    /**
     * Write 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    void writeLong64( long64 l )
    {
      long64* data = reinterpret_cast<long64*>( forward( sizeof( long64 ) ) );

      if( order == Endian::NATIVE ) {
        *data = l;
      }
      else {
        *data = Endian::bswap64( l );
      }
    }

    /**
     * Read unsigned 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    ulong64 readULong64()
    {
      const ulong64* data = reinterpret_cast<const ulong64*>( forward( sizeof( ulong64 ) ) );

      if( order == Endian::NATIVE ) {
        return *data;
      }
      else {
        return Endian::bswap64( *data );
      }
    }

    /**
     * Write unsigned 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    void writeULong64( ulong64 l )
    {
      ulong64* data = reinterpret_cast<ulong64*>( forward( sizeof( ulong64 ) ) );

      if( order == Endian::NATIVE ) {
        *data = l;
      }
      else {
        *data = Endian::bswap64( l );
      }
    }

    /**
     * Read float.
     */
    OZ_ALWAYS_INLINE
    float readFloat()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( int ) ) );

      if( order == Endian::NATIVE ) {
        return Math::fromBits( *data );
      }
      else {
        return Math::fromBits( Endian::bswap32( *data ) );
      }
    }

    /**
     * Write float.
     */
    OZ_ALWAYS_INLINE
    void writeFloat( float f )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( int ) ) );

      if( order == Endian::NATIVE ) {
        *data = Math::toBits( f );
      }
      else {
        *data = Endian::bswap32( Math::toBits( f ) );
      }
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

      const long64* data = reinterpret_cast<const long64*>( forward( sizeof( long64 ) ) );

      if( order == Endian::NATIVE ) {
        BitsToDouble bd = { *data };
        return bd.d;
      }
      else {
        BitsToDouble bd = { Endian::bswap64( *data ) };
        return bd.d;
      }
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

      long64* data = reinterpret_cast<long64*>( forward( sizeof( long64 ) ) );

      if( order == Endian::NATIVE ) {
        *data = db.b;
      }
      else {
        *data = Endian::bswap64( db.b );
      }
    }

    /**
     * Read string.
     */
    String readString()
    {
      const char* begin = pos;

      while( pos < end && *pos != '\0' ) {
        ++pos;
      }
      if( pos == end ) {
        throw Exception( "End of buffer reached while looking for the end of a string." );
      }

      int length = int( pos - begin );

      ++pos;
      return String( length, begin );
    }

    /**
     * Write string.
     */
    void writeString( const String& s )
    {
      int size = s.length() + 1;
      char* data = forward( size );

      aCopy<char>( data, s.cstr(), size );
    }

    /**
     * Write C string.
     */
    void writeString( const char* s )
    {
      int size = String::length( s ) + 1;
      char* data = forward( size );

      aCopy<char>( data, s, size );
    }

    /**
     * Read 3D vector.
     */
    OZ_ALWAYS_INLINE
    Vec3 readVec3()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[3] ) ) );

      if( order == Endian::NATIVE ) {
        return Vec3( Math::fromBits( data[0] ),
                     Math::fromBits( data[1] ),
                     Math::fromBits( data[2] ) );
      }
      else {
        return Vec3( Math::fromBits( Endian::bswap32( data[0] ) ),
                     Math::fromBits( Endian::bswap32( data[1] ) ),
                     Math::fromBits( Endian::bswap32( data[2] ) ) );
      }
    }

    /**
     * Write 3D vector.
     */
    OZ_ALWAYS_INLINE
    void writeVec3( const Vec3& v )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[3] ) ) );

      if( order == Endian::NATIVE ) {
        data[0] = Math::toBits( v.x );
        data[1] = Math::toBits( v.y );
        data[2] = Math::toBits( v.z );
      }
      else {
        data[0] = Endian::bswap32( Math::toBits( v.x ) );
        data[1] = Endian::bswap32( Math::toBits( v.y ) );
        data[2] = Endian::bswap32( Math::toBits( v.z ) );
      }
    }

    /**
     * Read 4-component vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 readVec4()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        return Vec4( Math::fromBits( data[0] ),
                     Math::fromBits( data[1] ),
                     Math::fromBits( data[2] ),
                     Math::fromBits( data[3] ) );
      }
      else {
        return Vec4( Math::fromBits( Endian::bswap32( data[0] ) ),
                     Math::fromBits( Endian::bswap32( data[1] ) ),
                     Math::fromBits( Endian::bswap32( data[2] ) ),
                     Math::fromBits( Endian::bswap32( data[3] ) ) );
      }
    }

    /**
     * Write 4-component vector.
     */
    OZ_ALWAYS_INLINE
    void writeVec4( const Vec4& v )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        data[0] = Math::toBits( v.x );
        data[1] = Math::toBits( v.y );
        data[2] = Math::toBits( v.z );
        data[3] = Math::toBits( v.w );
      }
      else {
        data[0] = Endian::bswap32( Math::toBits( v.x ) );
        data[1] = Endian::bswap32( Math::toBits( v.y ) );
        data[2] = Endian::bswap32( Math::toBits( v.z ) );
        data[3] = Endian::bswap32( Math::toBits( v.w ) );
      }
    }

    /**
     * Read 3D point.
     */
    OZ_ALWAYS_INLINE
    Point readPoint()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[3] ) ) );

      if( order == Endian::NATIVE ) {
        return Point( Math::fromBits( data[0] ),
                      Math::fromBits( data[1] ),
                      Math::fromBits( data[2] ) );
      }
      else {
        return Point( Math::fromBits( Endian::bswap32( data[0] ) ),
                      Math::fromBits( Endian::bswap32( data[1] ) ),
                      Math::fromBits( Endian::bswap32( data[2] ) ) );
      }
    }

    /**
     * Write 3D point.
     */
    OZ_ALWAYS_INLINE
    void writePoint( const Point& p )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[3] ) ) );

      if( order == Endian::NATIVE ) {
        data[0] = Math::toBits( p.x );
        data[1] = Math::toBits( p.y );
        data[2] = Math::toBits( p.z );
      }
      else {
        data[0] = Endian::bswap32( Math::toBits( p.x ) );
        data[1] = Endian::bswap32( Math::toBits( p.y ) );
        data[2] = Endian::bswap32( Math::toBits( p.z ) );
      }
    }

    /**
     * Read 3D plane.
     */
    OZ_ALWAYS_INLINE
    Plane readPlane()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        return Plane( Math::fromBits( data[0] ),
                      Math::fromBits( data[1] ),
                      Math::fromBits( data[2] ),
                      Math::fromBits( data[3] ) );
      }
      else {
        return Plane( Math::fromBits( Endian::bswap32( data[0] ) ),
                      Math::fromBits( Endian::bswap32( data[1] ) ),
                      Math::fromBits( Endian::bswap32( data[2] ) ),
                      Math::fromBits( Endian::bswap32( data[3] ) ) );
      }
    }

    /**
     * Write 3D plane.
     */
    OZ_ALWAYS_INLINE
    void writePlane( const Plane& p )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        data[0] = Math::toBits( p.n.x );
        data[1] = Math::toBits( p.n.y );
        data[2] = Math::toBits( p.n.z );
        data[3] = Math::toBits( p.d );
      }
      else {
        data[0] = Endian::bswap32( Math::toBits( p.n.x ) );
        data[1] = Endian::bswap32( Math::toBits( p.n.y ) );
        data[2] = Endian::bswap32( Math::toBits( p.n.z ) );
        data[3] = Endian::bswap32( Math::toBits( p.d ) );
      }
    }

    /**
     * Read quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat readQuat()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        return Quat( Math::fromBits( data[0] ),
                     Math::fromBits( data[1] ),
                     Math::fromBits( data[2] ),
                     Math::fromBits( data[3] ) );
      }
      else {
        return Quat( Math::fromBits( Endian::bswap32( data[0] ) ),
                     Math::fromBits( Endian::bswap32( data[1] ) ),
                     Math::fromBits( Endian::bswap32( data[2] ) ),
                     Math::fromBits( Endian::bswap32( data[3] ) ) );
      }
    }

    /**
     * Write quaternion.
     */
    OZ_ALWAYS_INLINE
    void writeQuat( const Quat& q )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        data[0] = Math::toBits( q.x );
        data[1] = Math::toBits( q.y );
        data[2] = Math::toBits( q.z );
        data[3] = Math::toBits( q.w );
      }
      else {
        data[0] = Endian::bswap32( Math::toBits( q.x ) );
        data[1] = Endian::bswap32( Math::toBits( q.y ) );
        data[2] = Endian::bswap32( Math::toBits( q.z ) );
        data[3] = Endian::bswap32( Math::toBits( q.w ) );
      }
    }

    /**
     * Read 4x4 matrix.
     */
    OZ_ALWAYS_INLINE
    Mat44 readMat44()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[16] ) ) );

      if( order == Endian::NATIVE ) {
        return Mat44( Math::fromBits( data[ 0] ),
                      Math::fromBits( data[ 1] ),
                      Math::fromBits( data[ 2] ),
                      Math::fromBits( data[ 3] ),
                      Math::fromBits( data[ 4] ),
                      Math::fromBits( data[ 5] ),
                      Math::fromBits( data[ 6] ),
                      Math::fromBits( data[ 7] ),
                      Math::fromBits( data[ 8] ),
                      Math::fromBits( data[ 9] ),
                      Math::fromBits( data[10] ),
                      Math::fromBits( data[11] ),
                      Math::fromBits( data[12] ),
                      Math::fromBits( data[13] ),
                      Math::fromBits( data[14] ),
                      Math::fromBits( data[15] ) );
      }
      else {
        return Mat44( Math::fromBits( Endian::bswap32( data[ 0] ) ),
                      Math::fromBits( Endian::bswap32( data[ 1] ) ),
                      Math::fromBits( Endian::bswap32( data[ 2] ) ),
                      Math::fromBits( Endian::bswap32( data[ 3] ) ),
                      Math::fromBits( Endian::bswap32( data[ 4] ) ),
                      Math::fromBits( Endian::bswap32( data[ 5] ) ),
                      Math::fromBits( Endian::bswap32( data[ 6] ) ),
                      Math::fromBits( Endian::bswap32( data[ 7] ) ),
                      Math::fromBits( Endian::bswap32( data[ 8] ) ),
                      Math::fromBits( Endian::bswap32( data[ 9] ) ),
                      Math::fromBits( Endian::bswap32( data[10] ) ),
                      Math::fromBits( Endian::bswap32( data[11] ) ),
                      Math::fromBits( Endian::bswap32( data[12] ) ),
                      Math::fromBits( Endian::bswap32( data[13] ) ),
                      Math::fromBits( Endian::bswap32( data[14] ) ),
                      Math::fromBits( Endian::bswap32( data[15] ) ) );
      }
    }

    /**
     * Write 4x4 matrix.
     */
    OZ_ALWAYS_INLINE
    void writeMat44( const Mat44& m )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[16] ) ) );

      if( order == Endian::NATIVE ) {
        data[ 0] = Math::toBits( m.x.x );
        data[ 1] = Math::toBits( m.x.y );
        data[ 2] = Math::toBits( m.x.z );
        data[ 3] = Math::toBits( m.x.w );
        data[ 4] = Math::toBits( m.y.x );
        data[ 5] = Math::toBits( m.y.y );
        data[ 6] = Math::toBits( m.y.z );
        data[ 7] = Math::toBits( m.y.w );
        data[ 8] = Math::toBits( m.z.x );
        data[ 9] = Math::toBits( m.z.y );
        data[10] = Math::toBits( m.z.z );
        data[11] = Math::toBits( m.z.w );
        data[12] = Math::toBits( m.w.x );
        data[13] = Math::toBits( m.w.y );
        data[14] = Math::toBits( m.w.z );
        data[15] = Math::toBits( m.w.w );
      }
      else {
        data[ 0] = Endian::bswap32( Math::toBits( m.x.x ) );
        data[ 1] = Endian::bswap32( Math::toBits( m.x.y ) );
        data[ 2] = Endian::bswap32( Math::toBits( m.x.z ) );
        data[ 3] = Endian::bswap32( Math::toBits( m.x.w ) );
        data[ 4] = Endian::bswap32( Math::toBits( m.y.x ) );
        data[ 5] = Endian::bswap32( Math::toBits( m.y.y ) );
        data[ 6] = Endian::bswap32( Math::toBits( m.y.z ) );
        data[ 7] = Endian::bswap32( Math::toBits( m.y.w ) );
        data[ 8] = Endian::bswap32( Math::toBits( m.z.x ) );
        data[ 9] = Endian::bswap32( Math::toBits( m.z.y ) );
        data[10] = Endian::bswap32( Math::toBits( m.z.z ) );
        data[11] = Endian::bswap32( Math::toBits( m.z.w ) );
        data[12] = Endian::bswap32( Math::toBits( m.w.x ) );
        data[13] = Endian::bswap32( Math::toBits( m.w.y ) );
        data[14] = Endian::bswap32( Math::toBits( m.w.z ) );
        data[15] = Endian::bswap32( Math::toBits( m.w.w ) );
      }
    }

    /**
     * Read line from a text file.
     *
     * Line delimiting character is not part of the returned string.
     */
    String readLine()
    {
      const char* begin = pos;

      while( pos < end && *pos != '\n' && *pos != '\r' ) {
        ++pos;
      }

      int length = int( pos - begin );

      pos += ( pos < end ) + ( pos < end - 1 && pos[0] == '\r' && pos[1] == '\n' );
      return String( length, begin );
    }

    /**
     * Write line in a text file.
     *
     * This writes the given string but replaces terminating null character with a UNIX newline.
     */
    OZ_ALWAYS_INLINE
    void writeLine( const String& s )
    {
      int   length = s.length();
      char* data   = forward( length + 1 );

      aCopy<char>( data, s, length );
      data[length] = '\n';
    }

    /**
     * Write line in a text file.
     *
     * This writes the given string but replaces terminating null character with a UNIX newline.
     */
    OZ_ALWAYS_INLINE
    void writeLine( const char* s )
    {
      int   length = String::length( s );
      char* data   = forward( length + 1 );

      aCopy<char>( data, s, length );
      data[length] = '\n';
    }

    /**
     * Deallocate allocated buffer.
     */
    void dealloc()
    {
      delete[] start;

      pos   = null;
      start = null;
      end   = null;
    }

};

}
