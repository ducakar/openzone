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
 * @file ozCore/InputStream.hh
 *
 * `InputStream` class.
 */

#pragma once

#include "String.hh"
#include "Mat44.hh"
#include "System.hh"
#include "Endian.hh"

namespace oz
{

/**
 * Fixed-size read-only stream.
 *
 * @sa `oz::OutputStream`
 */
class InputStream
{
  private:

    const char*   streamPos;   ///< Current position.
    const char*   streamBegin; ///< Beginning.
    const char*   streamEnd;   ///< End.
    Endian::Order order;       ///< Stream byte order.

  public:

    /**
     * Create a stream for reading a given memory range.
     */
    explicit InputStream( const char* start = nullptr, const char* end = nullptr,
                          Endian::Order order_ = Endian::NATIVE ) :
      streamPos( start ), streamBegin( start ), streamEnd( end ), order( order_ )
    {}

    /**
     * Length of the stream.
     */
    OZ_ALWAYS_INLINE
    int capacity() const
    {
      hard_assert( streamPos <= streamEnd );

      return int( streamEnd - streamBegin );
    }

    /**
     * Number of bytes left before end of the stream is reached.
     */
    OZ_ALWAYS_INLINE
    int available() const
    {
      hard_assert( streamPos <= streamEnd );

      return int( streamEnd - streamPos );
    }

    /**
     * True iff there is still some bytes left on the stream.
     */
    OZ_ALWAYS_INLINE
    bool isAvailable() const
    {
      hard_assert( streamPos <= streamEnd );

      return streamPos != streamEnd;
    }

    /**
     * Constant pointer to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    const char* begin() const
    {
      return streamBegin;
    }

    /**
     * Constant pointer to the eng of the stream.
     */
    OZ_ALWAYS_INLINE
    const char* end() const
    {
      return streamEnd;
    }

    /**
     * Pointer to the current position.
     */
    OZ_ALWAYS_INLINE
    const char* pos() const
    {
      hard_assert( streamBegin <= streamPos && streamPos <= streamEnd );

      return streamPos;
    }

    /**
     * Offset of the current position from the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    int tell() const
    {
      hard_assert( streamPos <= streamEnd );

      return int( streamPos - streamBegin );
    }

    /**
     * %Set stream position.
     */
    OZ_ALWAYS_INLINE
    void set( const char* newPos )
    {
      if( newPos < streamBegin || streamEnd < newPos ) {
        OZ_ERROR( "oz::InputStream overrun for %d B during setting stream position",
                  newPos < streamBegin ? int( newPos - streamBegin ) : int( newPos - streamEnd ) );
      }

      streamPos = newPos;
    }

    /**
     * %Set stream position relative to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    void seek( int offset )
    {
      if( offset < 0 || int( streamEnd - streamBegin ) < offset ) {
        OZ_ERROR( "oz::InputStream overrun for %d B during stream seek",
                  offset < 0 ? offset : offset - int( streamEnd - streamBegin ) );
      }

      streamPos = streamBegin + offset;
    }

    /**
     * Rewind current position to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    void rewind()
    {
      streamPos = streamBegin;
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
     * %Set byte order.
     */
    OZ_ALWAYS_INLINE
    void setEndian( Endian::Order order_ )
    {
      order = order_;
    }

    /**
     * Constant reference to the `i`-th byte from the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    const char& operator [] ( int i ) const
    {
      hard_assert( uint( i ) < uint( streamEnd - streamBegin ) );

      return streamBegin[i];
    }

    /**
     * Skip `count` bytes.
     *
     * @return Constant pointer to the beginning of the skipped bytes.
     */
    OZ_ALWAYS_INLINE
    const char* forward( int count )
    {
      const char* oldPos = streamPos;
      streamPos += count;

      if( streamPos > streamEnd ) {
        OZ_ERROR( "oz::InputStream overrun for %d B during a read of %d B",
                  int( streamPos - streamEnd ), count );
      }
      return oldPos;
    }

    /**
     * Read boolean.
     */
    OZ_ALWAYS_INLINE
    bool readBool()
    {
      const char* data = forward( int( sizeof( bool ) ) );
      return bool( *data );
    }

    /**
     * Read character.
     */
    OZ_ALWAYS_INLINE
    char readChar()
    {
      const char* data = forward( int( sizeof( char ) ) );
      return char( *data );
    }

    /**
     * Read an array of characters.
     */
    OZ_ALWAYS_INLINE
    void readChars( char* array, int count )
    {
      const char* data = forward( count * int( sizeof( char ) ) );
      mCopy( array, data, size_t( count ) );
    }

    /**
     * Read byte.
     */
    OZ_ALWAYS_INLINE
    byte readByte()
    {
      const char* data = forward( int( sizeof( byte ) ) );
      return byte( *data );
    }

    /**
     * Read unsigned byte.
     */
    OZ_ALWAYS_INLINE
    ubyte readUByte()
    {
      const char* data = forward( int( sizeof( ubyte ) ) );
      return ubyte( *data );
    }

    /**
     * Read short integer.
     */
    OZ_ALWAYS_INLINE
    short readShort()
    {
      const char* data = forward( int( sizeof( short ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToShort value = { { data[0], data[1] } };

        return value.value;
      }
      else {
        Endian::BytesToShort value = { { data[1], data[0] } };

        return value.value;
      }
    }

    /**
     * Read unsigned short integer.
     */
    OZ_ALWAYS_INLINE
    ushort readUShort()
    {
      const char* data = forward( int( sizeof( ushort ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToUShort value = { { data[0], data[1] } };

        return value.value;
      }
      else {
        Endian::BytesToUShort value = { { data[1], data[0] } };

        return value.value;
      }
    }

    /**
     * Read integer.
     */
    OZ_ALWAYS_INLINE
    int readInt()
    {
      const char* data = forward( int( sizeof( int ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToInt value = { { data[0], data[1], data[2], data[3] } };

        return value.value;
      }
      else {
        Endian::BytesToInt value = { { data[3], data[2], data[1], data[0] } };

        return value.value;
      }
    }

    /**
     * Read unsigned integer.
     */
    OZ_ALWAYS_INLINE
    uint readUInt()
    {
      const char* data = forward( int( sizeof( uint ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToUInt value = { { data[0], data[1], data[2], data[3] } };

        return value.value;
      }
      else {
        Endian::BytesToUInt value = { { data[3], data[2], data[1], data[0] } };

        return value.value;
      }
    }

    /**
     * Read 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    long64 readLong64()
    {
      const char* data = forward( int( sizeof( long64 ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToLong64 value = {
          { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
        };

        return value.value;
      }
      else {
        Endian::BytesToLong64 value = {
          { data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0] }
        };

        return value.value;
      }
    }

    /**
     * Read unsigned 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    ulong64 readULong64()
    {
      const char* data = forward( int( sizeof( ulong64 ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToULong64 value = {
          { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
        };

        return value.value;
      }
      else {
        Endian::BytesToULong64 value = {
          { data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0] }
        };

        return value.value;
      }
    }

    /**
     * Read float.
     */
    OZ_ALWAYS_INLINE
    float readFloat()
    {
      const char* data = forward( int( sizeof( float ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToFloat value = { { data[0], data[1], data[2], data[3] } };

        return value.value;
      }
      else {
        Endian::BytesToFloat value = { { data[3], data[2], data[1], data[0] } };

        return value.value;
      }
    }

    /**
     * Read double.
     */
    OZ_ALWAYS_INLINE
    double readDouble()
    {
      const char* data = forward( int( sizeof( double ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToDouble value = {
          { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
        };

        return value.value;
      }
      else {
        Endian::BytesToDouble value = {
          { data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0] }
        };

        return value.value;
      }
    }

    /**
     * Read string.
     */
    const char* readString()
    {
      const char* begin = streamPos;

      while( streamPos < streamEnd && *streamPos != '\0' ) {
        ++streamPos;
      }
      if( streamPos == streamEnd ) {
        OZ_ERROR( "End of buffer reached while looking for the end of a string." );
      }

      ++streamPos;
      return begin;
    }

    /**
     * Read 3D vector.
     */
    OZ_ALWAYS_INLINE
    Vec3 readVec3()
    {
      const char* data = forward( int( sizeof( float[3] ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToFloat x = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
        Endian::BytesToFloat y = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
        Endian::BytesToFloat z = { { data[ 8], data[ 9], data[10], data[11] } };

        return Vec3( x.value, y.value, z.value );
      }
      else {
        Endian::BytesToFloat x = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
        Endian::BytesToFloat y = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
        Endian::BytesToFloat z = { { data[11], data[10], data[ 9], data[ 8] } };

        return Vec3( x.value, y.value, z.value );
      }
    }

    /**
     * Read 4-component vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 readVec4()
    {
      const char* data = forward( int( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToFloat x = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
        Endian::BytesToFloat y = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
        Endian::BytesToFloat z = { { data[ 8], data[ 9], data[10], data[11] } };
        Endian::BytesToFloat w = { { data[12], data[13], data[14], data[15] } };

        return Vec4( x.value, y.value, z.value, w.value );
      }
      else {
        Endian::BytesToFloat x = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
        Endian::BytesToFloat y = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
        Endian::BytesToFloat z = { { data[11], data[10], data[ 9], data[ 8] } };
        Endian::BytesToFloat w = { { data[15], data[14], data[13], data[12] } };

        return Vec4( x.value, y.value, z.value, w.value );
      }
    }

    /**
     * Read 3D point.
     */
    OZ_ALWAYS_INLINE
    Point readPoint()
    {
      const char* data = forward( int( sizeof( float[3] ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToFloat x = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
        Endian::BytesToFloat y = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
        Endian::BytesToFloat z = { { data[ 8], data[ 9], data[10], data[11] } };

        return Point( x.value, y.value, z.value );
      }
      else {
        Endian::BytesToFloat x = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
        Endian::BytesToFloat y = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
        Endian::BytesToFloat z = { { data[11], data[10], data[ 9], data[ 8] } };

        return Point( x.value, y.value, z.value );
      }
    }

    /**
     * Read 3D plane.
     */
    OZ_ALWAYS_INLINE
    Plane readPlane()
    {
      const char* data = forward( int( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToFloat nx = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
        Endian::BytesToFloat ny = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
        Endian::BytesToFloat nz = { { data[ 8], data[ 9], data[10], data[11] } };
        Endian::BytesToFloat d  = { { data[12], data[13], data[14], data[15] } };

        return Plane( nx.value, ny.value, nz.value, d.value );
      }
      else {
        Endian::BytesToFloat nx = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
        Endian::BytesToFloat ny = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
        Endian::BytesToFloat nz = { { data[11], data[10], data[ 9], data[ 8] } };
        Endian::BytesToFloat d  = { { data[15], data[14], data[13], data[12] } };

        return Plane( nx.value, ny.value, nz.value, d.value );
      }
    }

    /**
     * Read quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat readQuat()
    {
      const char* data = forward( int( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        Endian::BytesToFloat x = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
        Endian::BytesToFloat y = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
        Endian::BytesToFloat z = { { data[ 8], data[ 9], data[10], data[11] } };
        Endian::BytesToFloat w = { { data[12], data[13], data[14], data[15] } };

        return Quat( x.value, y.value, z.value, w.value );
      }
      else {
        Endian::BytesToFloat x = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
        Endian::BytesToFloat y = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
        Endian::BytesToFloat z = { { data[11], data[10], data[ 9], data[ 8] } };
        Endian::BytesToFloat w = { { data[15], data[14], data[13], data[12] } };

        return Quat( x.value, y.value, z.value, w.value );
      }
    }

    /**
     * Read 3x3 matrix.
     */
    OZ_ALWAYS_INLINE
    Mat33 readMat33()
    {
      const char* data = forward( int( sizeof( float[9] ) ) );

      Mat33 m;
      float* values = m;

      if( order == Endian::NATIVE ) {
        for( int i = 0; i < 9; ++i, data += 4, ++values ) {
          Endian::BytesToFloat value = { { data[0], data[1], data[2], data[3] } };

          *values = value.value;
        }
      }
      else {
        for( int i = 0; i < 9; ++i, data += 4, ++values ) {
          Endian::BytesToFloat value = { { data[3], data[2], data[1], data[0] } };

          *values = value.value;
        }
      }

      return m;
    }

    /**
     * Read 4x4 matrix.
     */
    OZ_ALWAYS_INLINE
    Mat44 readMat44()
    {
      const char* data = forward( int( sizeof( float[16] ) ) );

      Mat44 m;
      float* values = m;

      if( order == Endian::NATIVE ) {
        for( int i = 0; i < 16; ++i, data += 4, ++values ) {
          Endian::BytesToFloat value = { { data[0], data[1], data[2], data[3] } };

          *values = value.value;
        }
      }
      else {
        for( int i = 0; i < 16; ++i, data += 4, ++values ) {
          Endian::BytesToFloat value = { { data[3], data[2], data[1], data[0] } };

          *values = value.value;
        }
      }

      return m;
    }

    /**
     * Read a line.
     *
     * Line delimiter is read but not included in the returned string.
     */
    String readLine()
    {
      const char* begin = streamPos;

      while( streamPos < streamEnd && *streamPos != '\n' && *streamPos != '\r' ) {
        ++streamPos;
      }

      int length = int( streamPos - begin );

      streamPos += ( streamPos < streamEnd ) +
                   ( streamPos < streamEnd - 1 && streamPos[0] == '\r' && streamPos[1] == '\n' );
      return String( begin, length );
    }

};

}
