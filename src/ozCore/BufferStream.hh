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
 * @file ozCore/BufferStream.hh
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
 * @sa `oz::InputStream`, `oz::OutputStream`
 */
class BufferStream
{
  private:

    /// Capacity is always a multiple of `GRANULARITY`.
    static const int GRANULARITY = 4096;

    char*         streamPos;   ///< Current position.
    char*         streamBegin; ///< Beginning, also pointer to the buffer.
    const char*   streamEnd;   ///< End.
    Endian::Order order;       ///< Stream byte order.

  public:

    /**
     * Create a stream with the given size of the buffer.
     */
    explicit BufferStream( int size = 0, Endian::Order order_ = Endian::NATIVE ) :
      streamPos( size == 0 ? nullptr : new char[size] ), streamBegin( streamPos ),
      streamEnd( streamBegin + size ), order( order_ )
    {}

    /**
     * Destructor.
     */
    ~BufferStream()
    {
      delete[] streamBegin;
    }

    /**
     * Copy constructor, copies buffer.
     */
    BufferStream( const BufferStream& s )
    {
      int length = int( s.streamPos - s.streamBegin );
      int size   = int( s.streamEnd - s.streamBegin );

      streamBegin = size == 0 ? nullptr : new char[size];
      streamEnd   = streamBegin + size;
      streamPos   = streamBegin + length;
      order       = s.order;

      mCopy( streamBegin, s.streamBegin, size_t( size ) );
    }

    /**
     * Move constructor, moves buffer.
     */
    BufferStream( BufferStream&& s ) :
      streamPos( s.streamPos ), streamBegin( s.streamBegin ), streamEnd( s.streamEnd ),
      order( s.order )
    {
      s.streamPos   = nullptr;
      s.streamBegin = nullptr;
      s.streamEnd   = nullptr;
      s.order       = Endian::NATIVE;
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

      int length = int( s.streamPos - s.streamBegin );
      int size   = int( s.streamEnd - s.streamBegin );

      if( int( streamEnd - streamBegin ) < size ) {
        delete[] streamBegin;

        streamBegin = new char[size];
        streamEnd   = streamBegin + size;
      }

      streamPos = streamBegin + length;
      order     = s.order;

      mCopy( streamBegin, s.streamBegin, size_t( size ) );

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

      delete[] streamBegin;

      streamPos   = s.streamPos;
      streamBegin = s.streamBegin;
      streamEnd   = s.streamEnd;
      order       = s.order;

      s.streamPos   = nullptr;
      s.streamBegin = nullptr;
      s.streamEnd   = nullptr;
      s.order       = Endian::NATIVE;

      return *this;
    }

    /**
     * Create `InputStream` for reading this stream (position is not reset).
     */
    InputStream inputStream() const
    {
      InputStream is( streamBegin, streamEnd, order );

      is.streamPos = streamPos;
      return is;
    }

    /**
     * Create `OutputStream` for reading/writing to this stream (position is not reset).
     */
    OutputStream outputStream()
    {
      OutputStream os( streamBegin, streamEnd, order );

      os.streamPos = streamPos;
      return os;
    }

    /**
     * Number of bytes from the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      hard_assert( streamPos <= streamEnd );

      return int( streamPos - streamBegin );
    }

    /**
     * Maximum length of the stream.
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
     * True iff there is still some bytes left before end of the stream is reached.
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
     * Pointer to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    char* begin()
    {
      return streamBegin;
    }

    /**
     * Constant pointer to the beginning of the stream.
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
     * Pointer to the current position.
     */
    OZ_ALWAYS_INLINE
    char* pos()
    {
      hard_assert( streamBegin <= streamPos && streamPos <= streamEnd );

      return streamPos;
    }

    /**
     * %Set the current position.
     */
    OZ_ALWAYS_INLINE
    void setPos( char* newPos )
    {
      hard_assert( streamBegin <= newPos && newPos <= streamEnd );

      streamPos = newPos;
    }

    /**
     * Rewind current position to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    void reset()
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
     * Reference to the `i`-th byte from the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    char& operator [] ( int i )
    {
      hard_assert( uint( i ) < uint( streamEnd - streamBegin ) );

      return streamBegin[i];
    }

    /**
     * Move position pointer for `count` bytes forward.
     *
     * @return Pointer to the beginning of the skipped bytes.
     */
    OZ_ALWAYS_INLINE
    char* forward( int count )
    {
      char* oldPos = streamPos;
      streamPos += count;

      if( streamPos > streamEnd ) {
        int length  = int( streamPos - streamBegin );
        int size    = int( streamEnd - streamBegin );
        int newSize = size == 0 ? GRANULARITY : 2 * size;

        if( newSize < length + count ) {
          newSize = ( ( length + count - 1 ) / GRANULARITY + 1 ) * GRANULARITY;
        }

        streamBegin  = aReallocate<char>( streamBegin, size, newSize );
        streamEnd    = streamBegin + newSize;
        streamPos    = streamBegin + length;
        oldPos = streamPos - count;
      }
      return oldPos;
    }

    /**
     * Read boolean.
     */
    OZ_ALWAYS_INLINE
    bool readBool()
    {
      const char* data = forward( sizeof( bool ) );
      return bool( *data );
    }

    /**
     * Write a boolean.
     */
    OZ_ALWAYS_INLINE
    void writeBool( bool b )
    {
      char* data = forward( sizeof( bool ) );
      *data = char( b );
    }

    /**
     * Read character.
     */
    OZ_ALWAYS_INLINE
    char readChar()
    {
      const char* data = forward( sizeof( char ) );
      return char( *data );
    }

    /**
     * Write a character.
     */
    OZ_ALWAYS_INLINE
    void writeChar( char c )
    {
      char* data = forward( sizeof( char ) );
      *data = char( c );
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
     * Write an array of characters.
     */
    OZ_ALWAYS_INLINE
    void writeChars( const char* array, int count )
    {
      char* data = forward( count * int( sizeof( char ) ) );
      mCopy( data, array, size_t( count ) );
    }

    /**
     * Read byte.
     */
    OZ_ALWAYS_INLINE
    byte readByte()
    {
      const char* data = forward( sizeof( byte ) );
      return byte( *data );
    }

    /**
     * Write byte.
     */
    OZ_ALWAYS_INLINE
    void writeByte( byte b )
    {
      char* data = forward( sizeof( byte ) );
      *data = char( b );
    }

    /**
     * Read unsigned byte.
     */
    OZ_ALWAYS_INLINE
    ubyte readUByte()
    {
      const char* data = forward( sizeof( ubyte ) );
      return ubyte( *data );
    }

    /**
     * Write unsigned byte.
     */
    OZ_ALWAYS_INLINE
    void writeUByte( ubyte b )
    {
      char* data = forward( sizeof( ubyte ) );
      *data = char( b );
    }

    /**
     * Read short integer.
     */
    OZ_ALWAYS_INLINE
    short readShort()
    {
      const char* data = forward( sizeof( short ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToShort value = { { data[0], data[1] } };

        return value.value;
      }
      else {
        Endian::BitsToShort value = { { data[1], data[0] } };

        return value.value;
      }
    }

    /**
     * Write short integer.
     */
    OZ_ALWAYS_INLINE
    void writeShort( short s )
    {
      char* data = forward( sizeof( short ) );

      Endian::ShortToBits value = { s };

      if( order == Endian::NATIVE ) {
        data[0] = value.data[0];
        data[1] = value.data[1];
      }
      else {
        data[0] = value.data[1];
        data[1] = value.data[0];
      }
    }

    /**
     * Read unsigned short integer.
     */
    OZ_ALWAYS_INLINE
    ushort readUShort()
    {
      const char* data = forward( sizeof( ushort ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToUShort value = { { data[0], data[1] } };

        return value.value;
      }
      else {
        Endian::BitsToUShort value = { { data[1], data[0] } };

        return value.value;
      }
    }

    /**
     * Write unsigned short integer.
     */
    OZ_ALWAYS_INLINE
    void writeUShort( ushort s )
    {
      char* data = forward( sizeof( ushort ) );

      Endian::UShortToBits value = { s };

      if( order == Endian::NATIVE ) {
        data[0] = value.data[0];
        data[1] = value.data[1];
      }
      else {
        data[0] = value.data[1];
        data[1] = value.data[0];
      }
    }

    /**
     * Read integer.
     */
    OZ_ALWAYS_INLINE
    int readInt()
    {
      const char* data = forward( sizeof( int ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToInt value = { { data[0], data[1], data[2], data[3] } };

        return value.value;
      }
      else {
        Endian::BitsToInt value = { { data[3], data[2], data[1], data[0] } };

        return value.value;
      }
    }

    /**
     * Write integer.
     */
    OZ_ALWAYS_INLINE
    void writeInt( int i )
    {
      char* data = forward( sizeof( int ) );

      Endian::IntToBits value = { i };

      if( order == Endian::NATIVE ) {
        data[0] = value.data[0];
        data[1] = value.data[1];
        data[2] = value.data[2];
        data[3] = value.data[3];
      }
      else {
        data[0] = value.data[3];
        data[1] = value.data[2];
        data[2] = value.data[1];
        data[3] = value.data[0];
      }
    }

    /**
     * Read unsigned integer.
     */
    OZ_ALWAYS_INLINE
    uint readUInt()
    {
      const char* data = forward( sizeof( uint ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToUInt value = { { data[0], data[1], data[2], data[3] } };

        return value.value;
      }
      else {
        Endian::BitsToUInt value = { { data[3], data[2], data[1], data[0] } };

        return value.value;
      }
    }

    /**
     * Write unsigned integer.
     */
    OZ_ALWAYS_INLINE
    void writeUInt( uint i )
    {
      char* data = forward( sizeof( uint ) );

      Endian::UIntToBits value = { i };

      if( order == Endian::NATIVE ) {
        data[0] = value.data[0];
        data[1] = value.data[1];
        data[2] = value.data[2];
        data[3] = value.data[3];
      }
      else {
        data[0] = value.data[3];
        data[1] = value.data[2];
        data[2] = value.data[1];
        data[3] = value.data[0];
      }
    }

    /**
     * Read 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    long64 readLong64()
    {
      const char* data = forward( sizeof( long64 ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToLong64 value = {
          { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
        };

        return value.value;
      }
      else {
        Endian::BitsToLong64 value = {
          { data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0] }
        };

        return value.value;
      }
    }

    /**
     * Write 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    void writeLong64( long64 l )
    {
      char* data = forward( sizeof( long64 ) );

      Endian::Long64ToBits value = { l };

      if( order == Endian::NATIVE ) {
        data[0] = value.data[0];
        data[1] = value.data[1];
        data[2] = value.data[2];
        data[3] = value.data[3];
        data[4] = value.data[4];
        data[5] = value.data[5];
        data[6] = value.data[6];
        data[7] = value.data[7];
      }
      else {
        data[0] = value.data[7];
        data[1] = value.data[6];
        data[2] = value.data[5];
        data[3] = value.data[4];
        data[4] = value.data[3];
        data[5] = value.data[2];
        data[6] = value.data[1];
        data[7] = value.data[0];
      }
    }

    /**
     * Read unsigned 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    ulong64 readULong64()
    {
      const char* data = forward( sizeof( ulong64 ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToULong64 value = {
          { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
        };

        return value.value;
      }
      else {
        Endian::BitsToULong64 value = {
          { data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0] }
        };

        return value.value;
      }
    }

    /**
     * Write unsigned 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    void writeULong64( ulong64 l )
    {
      char* data = forward( sizeof( ulong64 ) );

      Endian::ULong64ToBits value = { l };

      if( order == Endian::NATIVE ) {
        data[0] = value.data[0];
        data[1] = value.data[1];
        data[2] = value.data[2];
        data[3] = value.data[3];
        data[4] = value.data[4];
        data[5] = value.data[5];
        data[6] = value.data[6];
        data[7] = value.data[7];
      }
      else {
        data[0] = value.data[7];
        data[1] = value.data[6];
        data[2] = value.data[5];
        data[3] = value.data[4];
        data[4] = value.data[3];
        data[5] = value.data[2];
        data[6] = value.data[1];
        data[7] = value.data[0];
      }
    }

    /**
     * Read float.
     */
    OZ_ALWAYS_INLINE
    float readFloat()
    {
      const char* data = forward( sizeof( float ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToFloat value = { { data[0], data[1], data[2], data[3] } };

        return value.value;
      }
      else {
        Endian::BitsToFloat value = { { data[3], data[2], data[1], data[0] } };

        return value.value;
      }
    }

    /**
     * Write float.
     */
    OZ_ALWAYS_INLINE
    void writeFloat( float f )
    {
      char* data = forward( sizeof( float ) );

      Endian::FloatToBits value = { f };

      if( order == Endian::NATIVE ) {
        data[0] = value.data[0];
        data[1] = value.data[1];
        data[2] = value.data[2];
        data[3] = value.data[3];
      }
      else {
        data[0] = value.data[3];
        data[1] = value.data[2];
        data[2] = value.data[1];
        data[3] = value.data[0];
      }
    }

    /**
     * Read double.
     */
    OZ_ALWAYS_INLINE
    double readDouble()
    {
      const char* data = forward( sizeof( double ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToDouble value = {
          { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
        };

        return value.value;
      }
      else {
        Endian::BitsToDouble value = {
          { data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0] }
        };

        return value.value;
      }
    }

    /**
     * Write double.
     */
    OZ_ALWAYS_INLINE
    void writeDouble( double d )
    {
      char* data = forward( sizeof( double ) );

      Endian::DoubleToBits value = { d };

      if( order == Endian::NATIVE ) {
        data[0] = value.data[0];
        data[1] = value.data[1];
        data[2] = value.data[2];
        data[3] = value.data[3];
        data[4] = value.data[4];
        data[5] = value.data[5];
        data[6] = value.data[6];
        data[7] = value.data[7];
      }
      else {
        data[0] = value.data[7];
        data[1] = value.data[6];
        data[2] = value.data[5];
        data[3] = value.data[4];
        data[4] = value.data[3];
        data[5] = value.data[2];
        data[6] = value.data[1];
        data[7] = value.data[0];
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
     * Write string.
     */
    void writeString( const String& s )
    {
      int   size = s.length() + 1;
      char* data = forward( size );

      mCopy( data, s.cstr(), size_t( size ) );
    }

    /**
     * Write C string.
     */
    void writeString( const char* s )
    {
      int   size = String::length( s ) + 1;
      char* data = forward( size );

      mCopy( data, s, size_t( size ) );
    }

    /**
     * Read 3D vector.
     */
    OZ_ALWAYS_INLINE
    Vec3 readVec3()
    {
      const char* data = forward( sizeof( float[3] ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToFloat x = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
        Endian::BitsToFloat y = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
        Endian::BitsToFloat z = { { data[ 8], data[ 9], data[10], data[11] } };

        return Vec3( x.value, y.value, z.value );
      }
      else {
        Endian::BitsToFloat x = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
        Endian::BitsToFloat y = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
        Endian::BitsToFloat z = { { data[11], data[10], data[ 9], data[ 8] } };

        return Vec3( x.value, y.value, z.value );
      }
    }

    /**
     * Write 3D vector.
     */
    OZ_ALWAYS_INLINE
    void writeVec3( const Vec3& v )
    {
      char* data = forward( sizeof( float[3] ) );

      Endian::FloatToBits x = { v.x };
      Endian::FloatToBits y = { v.y };
      Endian::FloatToBits z = { v.z };

      if( order == Endian::NATIVE ) {
        data[ 0] = x.data[0];
        data[ 1] = x.data[1];
        data[ 2] = x.data[2];
        data[ 3] = x.data[3];
        data[ 4] = y.data[0];
        data[ 5] = y.data[1];
        data[ 6] = y.data[2];
        data[ 7] = y.data[3];
        data[ 8] = z.data[0];
        data[ 9] = z.data[1];
        data[10] = z.data[2];
        data[11] = z.data[3];
      }
      else {
        data[ 0] = x.data[3];
        data[ 1] = x.data[2];
        data[ 2] = x.data[1];
        data[ 3] = x.data[0];
        data[ 4] = y.data[3];
        data[ 5] = y.data[2];
        data[ 6] = y.data[1];
        data[ 7] = y.data[0];
        data[ 8] = z.data[3];
        data[ 9] = z.data[2];
        data[10] = z.data[1];
        data[11] = z.data[0];
      }
    }

    /**
     * Read 4-component vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 readVec4()
    {
      const char* data = forward( sizeof( float[4] ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToFloat x = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
        Endian::BitsToFloat y = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
        Endian::BitsToFloat z = { { data[ 8], data[ 9], data[10], data[11] } };
        Endian::BitsToFloat w = { { data[12], data[13], data[14], data[15] } };

        return Vec4( x.value, y.value, z.value, w.value );
      }
      else {
        Endian::BitsToFloat x = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
        Endian::BitsToFloat y = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
        Endian::BitsToFloat z = { { data[11], data[10], data[ 9], data[ 8] } };
        Endian::BitsToFloat w = { { data[15], data[14], data[13], data[12] } };

        return Vec4( x.value, y.value, z.value, w.value );
      }
    }

    /**
     * Write 4-component vector.
     */
    OZ_ALWAYS_INLINE
    void writeVec4( const Vec4& v )
    {
      char* data = forward( sizeof( float[4] ) );

      Endian::FloatToBits x = { v.x };
      Endian::FloatToBits y = { v.y };
      Endian::FloatToBits z = { v.z };
      Endian::FloatToBits w = { v.w };

      if( order == Endian::NATIVE ) {
        data[ 0] = x.data[0];
        data[ 1] = x.data[1];
        data[ 2] = x.data[2];
        data[ 3] = x.data[3];
        data[ 4] = y.data[0];
        data[ 5] = y.data[1];
        data[ 6] = y.data[2];
        data[ 7] = y.data[3];
        data[ 8] = z.data[0];
        data[ 9] = z.data[1];
        data[10] = z.data[2];
        data[11] = z.data[3];
        data[12] = w.data[0];
        data[13] = w.data[1];
        data[14] = w.data[2];
        data[15] = w.data[3];
      }
      else {
        data[ 0] = x.data[3];
        data[ 1] = x.data[2];
        data[ 2] = x.data[1];
        data[ 3] = x.data[0];
        data[ 4] = y.data[3];
        data[ 5] = y.data[2];
        data[ 6] = y.data[1];
        data[ 7] = y.data[0];
        data[ 8] = z.data[3];
        data[ 9] = z.data[2];
        data[10] = z.data[1];
        data[11] = z.data[0];
        data[12] = w.data[3];
        data[13] = w.data[2];
        data[14] = w.data[1];
        data[15] = w.data[0];
      }
    }

    /**
     * Read 3D point.
     */
    OZ_ALWAYS_INLINE
    Point readPoint()
    {
      const char* data = forward( sizeof( float[3] ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToFloat x = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
        Endian::BitsToFloat y = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
        Endian::BitsToFloat z = { { data[ 8], data[ 9], data[10], data[11] } };

        return Point( x.value, y.value, z.value );
      }
      else {
        Endian::BitsToFloat x = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
        Endian::BitsToFloat y = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
        Endian::BitsToFloat z = { { data[11], data[10], data[ 9], data[ 8] } };

        return Point( x.value, y.value, z.value );
      }
    }

    /**
     * Write 3D point.
     */
    OZ_ALWAYS_INLINE
    void writePoint( const Point& p )
    {
      char* data = forward( sizeof( float[3] ) );

      Endian::FloatToBits x = { p.x };
      Endian::FloatToBits y = { p.y };
      Endian::FloatToBits z = { p.z };

      if( order == Endian::NATIVE ) {
        data[ 0] = x.data[0];
        data[ 1] = x.data[1];
        data[ 2] = x.data[2];
        data[ 3] = x.data[3];
        data[ 4] = y.data[0];
        data[ 5] = y.data[1];
        data[ 6] = y.data[2];
        data[ 7] = y.data[3];
        data[ 8] = z.data[0];
        data[ 9] = z.data[1];
        data[10] = z.data[2];
        data[11] = z.data[3];
      }
      else {
        data[ 0] = x.data[3];
        data[ 1] = x.data[2];
        data[ 2] = x.data[1];
        data[ 3] = x.data[0];
        data[ 4] = y.data[3];
        data[ 5] = y.data[2];
        data[ 6] = y.data[1];
        data[ 7] = y.data[0];
        data[ 8] = z.data[3];
        data[ 9] = z.data[2];
        data[10] = z.data[1];
        data[11] = z.data[0];
      }
    }

    /**
     * Read 3D plane.
     */
    OZ_ALWAYS_INLINE
    Plane readPlane()
    {
      const char* data = forward( sizeof( float[4] ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToFloat nx = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
        Endian::BitsToFloat ny = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
        Endian::BitsToFloat nz = { { data[ 8], data[ 9], data[10], data[11] } };
        Endian::BitsToFloat d  = { { data[12], data[13], data[14], data[15] } };

        return Plane( nx.value, ny.value, nz.value, d.value );
      }
      else {
        Endian::BitsToFloat nx = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
        Endian::BitsToFloat ny = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
        Endian::BitsToFloat nz = { { data[11], data[10], data[ 9], data[ 8] } };
        Endian::BitsToFloat d  = { { data[15], data[14], data[13], data[12] } };

        return Plane( nx.value, ny.value, nz.value, d.value );
      }
    }

    /**
     * Write 3D plane.
     */
    OZ_ALWAYS_INLINE
    void writePlane( const Plane& p )
    {
      char* data = forward( sizeof( float[4] ) );

      Endian::FloatToBits nx = { p.n.x };
      Endian::FloatToBits ny = { p.n.y };
      Endian::FloatToBits nz = { p.n.z };
      Endian::FloatToBits d  = { p.d };

      if( order == Endian::NATIVE ) {
        data[ 0] = nx.data[0];
        data[ 1] = nx.data[1];
        data[ 2] = nx.data[2];
        data[ 3] = nx.data[3];
        data[ 4] = ny.data[0];
        data[ 5] = ny.data[1];
        data[ 6] = ny.data[2];
        data[ 7] = ny.data[3];
        data[ 8] = nz.data[0];
        data[ 9] = nz.data[1];
        data[10] = nz.data[2];
        data[11] = nz.data[3];
        data[12] = d.data[0];
        data[13] = d.data[1];
        data[14] = d.data[2];
        data[15] = d.data[3];
      }
      else {
        data[ 0] = nx.data[3];
        data[ 1] = nx.data[2];
        data[ 2] = nx.data[1];
        data[ 3] = nx.data[0];
        data[ 4] = ny.data[3];
        data[ 5] = ny.data[2];
        data[ 6] = ny.data[1];
        data[ 7] = ny.data[0];
        data[ 8] = nz.data[3];
        data[ 9] = nz.data[2];
        data[10] = nz.data[1];
        data[11] = nz.data[0];
        data[12] = d.data[3];
        data[13] = d.data[2];
        data[14] = d.data[1];
        data[15] = d.data[0];
      }
    }

    /**
     * Read quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat readQuat()
    {
      const char* data = forward( sizeof( float[4] ) );

      if( order == Endian::NATIVE ) {
        Endian::BitsToFloat x = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
        Endian::BitsToFloat y = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
        Endian::BitsToFloat z = { { data[ 8], data[ 9], data[10], data[11] } };
        Endian::BitsToFloat w = { { data[12], data[13], data[14], data[15] } };

        return Quat( x.value, y.value, z.value, w.value );
      }
      else {
        Endian::BitsToFloat x = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
        Endian::BitsToFloat y = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
        Endian::BitsToFloat z = { { data[11], data[10], data[ 9], data[ 8] } };
        Endian::BitsToFloat w = { { data[15], data[14], data[13], data[12] } };

        return Quat( x.value, y.value, z.value, w.value );
      }
    }

    /**
     * Write quaternion.
     */
    OZ_ALWAYS_INLINE
    void writeQuat( const Quat& q )
    {
      char* data = forward( sizeof( float[4] ) );

      Endian::FloatToBits x = { q.x };
      Endian::FloatToBits y = { q.y };
      Endian::FloatToBits z = { q.z };
      Endian::FloatToBits w = { q.w };

      if( order == Endian::NATIVE ) {
        data[ 0] = x.data[0];
        data[ 1] = x.data[1];
        data[ 2] = x.data[2];
        data[ 3] = x.data[3];
        data[ 4] = y.data[0];
        data[ 5] = y.data[1];
        data[ 6] = y.data[2];
        data[ 7] = y.data[3];
        data[ 8] = z.data[0];
        data[ 9] = z.data[1];
        data[10] = z.data[2];
        data[11] = z.data[3];
        data[12] = w.data[0];
        data[13] = w.data[1];
        data[14] = w.data[2];
        data[15] = w.data[3];
      }
      else {
        data[ 0] = x.data[3];
        data[ 1] = x.data[2];
        data[ 2] = x.data[1];
        data[ 3] = x.data[0];
        data[ 4] = y.data[3];
        data[ 5] = y.data[2];
        data[ 6] = y.data[1];
        data[ 7] = y.data[0];
        data[ 8] = z.data[3];
        data[ 9] = z.data[2];
        data[10] = z.data[1];
        data[11] = z.data[0];
        data[12] = w.data[3];
        data[13] = w.data[2];
        data[14] = w.data[1];
        data[15] = w.data[0];
      }
    }

    /**
     * Read 3x3 matrix.
     */
    OZ_ALWAYS_INLINE
    Mat33 readMat33()
    {
      const char* data = forward( sizeof( float[9] ) );

      Mat33 m;
      float* values = m;

      if( order == Endian::NATIVE ) {
        for( int i = 0; i < 9; ++i, data += 4, ++values ) {
          Endian::BitsToFloat value = { { data[0], data[1], data[2], data[3] } };

          *values = value.value;
        }
      }
      else {
        for( int i = 0; i < 9; ++i, data += 4, ++values ) {
          Endian::BitsToFloat value = { { data[3], data[2], data[1], data[0] } };

          *values = value.value;
        }
      }

      return m;
    }

    /**
     * Write 3x3 matrix.
     */
    OZ_ALWAYS_INLINE
    void writeMat33( const Mat44& m )
    {
      char* data = forward( sizeof( float[9] ) );
      const float* values = m;

      if( order == Endian::NATIVE ) {
        for( int i = 0; i < 9; ++i, data += 4, ++values ) {
          Endian::FloatToBits value = { *values };

          data[0] = value.data[0];
          data[1] = value.data[1];
          data[2] = value.data[2];
          data[3] = value.data[3];
        }
      }
      else {
        for( int i = 0; i < 9; ++i, data += 4, ++values ) {
          Endian::FloatToBits value = { *values };

          data[0] = value.data[3];
          data[1] = value.data[2];
          data[2] = value.data[1];
          data[3] = value.data[0];
        }
      }
    }

    /**
     * Read 4x4 matrix.
     */
    OZ_ALWAYS_INLINE
    Mat44 readMat44()
    {
      const char* data = forward( sizeof( float[16] ) );

      Mat44 m;
      float* values = m;

      if( order == Endian::NATIVE ) {
        for( int i = 0; i < 16; ++i, data += 4, ++values ) {
          Endian::BitsToFloat value = { { data[0], data[1], data[2], data[3] } };

          *values = value.value;
        }
      }
      else {
        for( int i = 0; i < 16; ++i, data += 4, ++values ) {
          Endian::BitsToFloat value = { { data[3], data[2], data[1], data[0] } };

          *values = value.value;
        }
      }

      return m;
    }

    /**
     * Write 4x4 matrix.
     */
    OZ_ALWAYS_INLINE
    void writeMat44( const Mat44& m )
    {
      char* data = forward( sizeof( float[16] ) );
      const float* values = m;

      if( order == Endian::NATIVE ) {
        for( int i = 0; i < 16; ++i, data += 4, ++values ) {
          Endian::FloatToBits value = { *values };

          data[0] = value.data[0];
          data[1] = value.data[1];
          data[2] = value.data[2];
          data[3] = value.data[3];
        }
      }
      else {
        for( int i = 0; i < 16; ++i, data += 4, ++values ) {
          Endian::FloatToBits value = { *values };

          data[0] = value.data[3];
          data[1] = value.data[2];
          data[2] = value.data[1];
          data[3] = value.data[0];
        }
      }
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

    /**
     * Write a line replacing terminating null byte with UNIX newline.
     */
    OZ_ALWAYS_INLINE
    void writeLine( const String& s )
    {
      int   length = s.length();
      char* data   = forward( length + 1 );

      mCopy( data, s, size_t( length ) );
      data[length] = '\n';
    }

    /**
     * Write a line replacing terminating null byte with UNIX newline.
     */
    OZ_ALWAYS_INLINE
    void writeLine( const char* s )
    {
      int   length = String::length( s );
      char* data   = forward( length + 1 );

      mCopy( data, s, size_t( length ) );
      data[length] = '\n';
    }

    /**
     * Deallocate allocated buffer.
     */
    void deallocate()
    {
      delete[] streamBegin;

      streamPos   = nullptr;
      streamBegin = nullptr;
      streamEnd   = nullptr;
    }

};

}
