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
 * @file ozCore/OutputStream.hh
 *
 * `OutputStream` class.
 */

#pragma once

#include "InputStream.hh"

namespace oz
{

/**
 * Fixed-size or buffered read/write stream.
 *
 * @sa `oz::InputStream`
 */
class OutputStream
{
  private:

    /// Capacity is always a multiple of `GRANULARITY`.
    static const int GRANULARITY = 4096;

    char*         streamPos;   ///< Current position.
    char*         streamBegin; ///< Beginning.
    const char*   streamEnd;   ///< End.
    Endian::Order order;       ///< Stream byte order.
    bool          buffered;    ///< If stream writes to an internal buffer.

  public:

    /**
     * Create a fixed-size stream for reading/writing the given memory range.
     */
    explicit OutputStream( char* start = nullptr, const char* end = nullptr,
                           Endian::Order order_ = Endian::NATIVE ) :
      streamPos( start ), streamBegin( start ), streamEnd( end ), order( order_ ), buffered( false )
    {}

    /**
     * Create a buffered stream with dynamically growing buffer.
     */
    explicit OutputStream( int size, Endian::Order order_ = Endian::NATIVE ) :
      streamPos( size == 0 ? nullptr : new char[size] ), streamBegin( streamPos ),
      streamEnd( streamBegin + size ), order( order_ ), buffered( true )
    {}

    /**
     * Destructor.
     */
    ~OutputStream()
    {
      if( buffered ) {
        delete[] streamBegin;
      }
    }

    /**
     * Copy constructor, copies buffer if source stream is buffered.
     */
    OutputStream( const OutputStream& s ) :
      streamPos( s.streamPos ), streamBegin( s.streamBegin ), streamEnd( s.streamEnd ),
      order( s.order ), buffered( s.buffered )
    {
      if( s.buffered ) {
        int length = int( s.streamPos - s.streamBegin );
        int size   = int( s.streamEnd - s.streamBegin );

        streamBegin = size == 0 ? nullptr : new char[size];
        streamEnd   = streamBegin + size;
        streamPos   = streamBegin + length;

        mCopy( streamBegin, s.streamBegin, size_t( size ) );
      }
    }

    /**
     * Move constructor, moves buffer if source stream is buffered.
     */
    OutputStream( OutputStream&& s ) :
      streamPos( s.streamPos ), streamBegin( s.streamBegin ), streamEnd( s.streamEnd ),
      order( s.order ), buffered( s.buffered )
    {
      if( s.buffered ) {
        s.streamPos   = nullptr;
        s.streamBegin = nullptr;
        s.streamEnd   = nullptr;
        s.order       = Endian::NATIVE;
        s.buffered    = false;
      }
    }

    /**
     * Copy operator, copies buffer if source stream is buffered.
     *
     * Existing storage is reused if its size matches.
     */
    OutputStream& operator = ( const OutputStream& s )
    {
      if( &s == this ) {
        return *this;
      }

      if( s.buffered ) {
        int  length      = int( s.streamPos - s.streamBegin );
        int  size        = int( s.streamEnd - s.streamBegin );
        bool sizeMatches = int( streamEnd - streamBegin ) == size;

        if( buffered && !sizeMatches ) {
          delete[] streamBegin;
        }
        if( !buffered || !sizeMatches ) {
          streamBegin = new char[size];
          streamEnd   = streamBegin + size;
        }

        streamPos = streamBegin + length;
        order     = s.order;
        buffered  = s.buffered;

        mCopy( streamBegin, s.streamBegin, size_t( size ) );
      }
      else {
        if( buffered ) {
          delete[] streamBegin;
        }

        streamPos   = s.streamPos;
        streamBegin = s.streamBegin;
        streamEnd   = s.streamEnd;
        order       = s.order;
        buffered    = s.buffered;
      }

      return *this;
    }

    /**
     * Move operator, moves buffer if source stream is buffered.
     */
    OutputStream& operator = ( OutputStream&& s )
    {
      if( &s == this ) {
        return *this;
      }

      if( buffered ) {
        delete[] streamBegin;
      }

      streamPos   = s.streamPos;
      streamBegin = s.streamBegin;
      streamEnd   = s.streamEnd;
      order       = s.order;
      buffered    = s.buffered;

      if( s.buffered ) {
        s.streamPos   = nullptr;
        s.streamBegin = nullptr;
        s.streamEnd   = nullptr;
        s.order       = Endian::NATIVE;
        s.buffered    = false;
      }

      return *this;
    }

    /**
     * Create `InputStream` for reading this stream (position is not reset).
     */
    InputStream inputStream() const
    {
      InputStream istream( streamBegin, streamEnd, order );

      istream.set( streamPos );
      return istream;
    }

    /**
     * Iff stream uses internal buffer instead of given storage.
     */
    OZ_ALWAYS_INLINE
    bool isBuffered()
    {
      return buffered;
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
     * Constant pointer to the end of the stream.
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
    void set( char* newPos )
    {
      if( newPos < streamBegin || streamEnd < newPos ) {
        OZ_ERROR( "oz::OutputStream overrun for %d B during setting stream position",
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
        OZ_ERROR( "oz::OutputStream overrun for %d B during stream seek",
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
        if( buffered ) {
          int length  = int( streamPos - streamBegin );
          int size    = int( streamEnd - streamBegin );
          int reqSize = length + count;
          int newSize = size == 0 ? GRANULARITY : 2 * size;

          if( newSize < 0 || reqSize < 0 ) {
            OZ_ERROR( "oz::OutputStream capacity overflow" );
          }
          else if( newSize < reqSize ) {
            newSize = ( reqSize + GRANULARITY - 1 ) & ~( GRANULARITY - 1 );
          }

          streamBegin  = aReallocate<char>( streamBegin, size, newSize );
          streamEnd    = streamBegin + newSize;
          streamPos    = streamBegin + length;
          oldPos = streamPos - count;
        }
        else {
          OZ_ERROR( "oz::OutputStream overrun for %d B during a read or write of %d B",
                    int( streamPos - streamEnd ), count );
        }
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
     * Write a boolean.
     */
    OZ_ALWAYS_INLINE
    void writeBool( bool b )
    {
      char* data = forward( int( sizeof( bool ) ) );
      *data = char( b );
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
     * Write a character.
     */
    OZ_ALWAYS_INLINE
    void writeChar( char c )
    {
      char* data = forward( int( sizeof( char ) ) );
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
      const char* data = forward( int( sizeof( byte ) ) );
      return byte( *data );
    }

    /**
     * Write byte.
     */
    OZ_ALWAYS_INLINE
    void writeByte( byte b )
    {
      char* data = forward( int( sizeof( byte ) ) );
      *data = char( b );
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
     * Write unsigned byte.
     */
    OZ_ALWAYS_INLINE
    void writeUByte( ubyte b )
    {
      char* data = forward( int( sizeof( ubyte ) ) );
      *data = char( b );
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
     * Write short integer.
     */
    OZ_ALWAYS_INLINE
    void writeShort( short s )
    {
      char* data = forward( int( sizeof( short ) ) );

      Endian::ShortToBytes value = { s };

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
     * Write unsigned short integer.
     */
    OZ_ALWAYS_INLINE
    void writeUShort( ushort s )
    {
      char* data = forward( int( sizeof( ushort ) ) );

      Endian::UShortToBytes value = { s };

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
     * Write integer.
     */
    OZ_ALWAYS_INLINE
    void writeInt( int i )
    {
      char* data = forward( int( sizeof( int ) ) );

      Endian::IntToBytes value = { i };

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
     * Write unsigned integer.
     */
    OZ_ALWAYS_INLINE
    void writeUInt( uint i )
    {
      char* data = forward( int( sizeof( uint ) ) );

      Endian::UIntToBytes value = { i };

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
     * Write 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    void writeLong64( long64 l )
    {
      char* data = forward( int( sizeof( long64 ) ) );

      Endian::Long64ToBytes value = { l };

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
     * Write unsigned 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    void writeULong64( ulong64 l )
    {
      char* data = forward( int( sizeof( ulong64 ) ) );

      Endian::ULong64ToBytes value = { l };

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
     * Write float.
     */
    OZ_ALWAYS_INLINE
    void writeFloat( float f )
    {
      char* data = forward( int( sizeof( float ) ) );

      Endian::FloatToBytes value = { f };

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
     * Write double.
     */
    OZ_ALWAYS_INLINE
    void writeDouble( double d )
    {
      char* data = forward( int( sizeof( double ) ) );

      Endian::DoubleToBytes value = { d };

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
     * Write 3D vector.
     */
    OZ_ALWAYS_INLINE
    void writeVec3( const Vec3& v )
    {
      char* data = forward( int( sizeof( float[3] ) ) );

      Endian::FloatToBytes x = { v.x };
      Endian::FloatToBytes y = { v.y };
      Endian::FloatToBytes z = { v.z };

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
     * Write 4-component vector.
     */
    OZ_ALWAYS_INLINE
    void writeVec4( const Vec4& v )
    {
      char* data = forward( int( sizeof( float[4] ) ) );

      Endian::FloatToBytes x = { v.x };
      Endian::FloatToBytes y = { v.y };
      Endian::FloatToBytes z = { v.z };
      Endian::FloatToBytes w = { v.w };

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
     * Write 3D point.
     */
    OZ_ALWAYS_INLINE
    void writePoint( const Point& p )
    {
      char* data = forward( int( sizeof( float[3] ) ) );

      Endian::FloatToBytes x = { p.x };
      Endian::FloatToBytes y = { p.y };
      Endian::FloatToBytes z = { p.z };

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
     * Write 3D plane.
     */
    OZ_ALWAYS_INLINE
    void writePlane( const Plane& p )
    {
      char* data = forward( int( sizeof( float[4] ) ) );

      Endian::FloatToBytes nx = { p.n.x };
      Endian::FloatToBytes ny = { p.n.y };
      Endian::FloatToBytes nz = { p.n.z };
      Endian::FloatToBytes d  = { p.d };

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
     * Write quaternion.
     */
    OZ_ALWAYS_INLINE
    void writeQuat( const Quat& q )
    {
      char* data = forward( int( sizeof( float[4] ) ) );

      Endian::FloatToBytes x = { q.x };
      Endian::FloatToBytes y = { q.y };
      Endian::FloatToBytes z = { q.z };
      Endian::FloatToBytes w = { q.w };

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
     * Write 3x3 matrix.
     */
    OZ_ALWAYS_INLINE
    void writeMat33( const Mat44& m )
    {
      char* data = forward( int( sizeof( float[9] ) ) );
      const float* values = m;

      if( order == Endian::NATIVE ) {
        for( int i = 0; i < 9; ++i, data += 4, ++values ) {
          Endian::FloatToBytes value = { *values };

          data[0] = value.data[0];
          data[1] = value.data[1];
          data[2] = value.data[2];
          data[3] = value.data[3];
        }
      }
      else {
        for( int i = 0; i < 9; ++i, data += 4, ++values ) {
          Endian::FloatToBytes value = { *values };

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
     * Write 4x4 matrix.
     */
    OZ_ALWAYS_INLINE
    void writeMat44( const Mat44& m )
    {
      char* data = forward( int( sizeof( float[16] ) ) );
      const float* values = m;

      if( order == Endian::NATIVE ) {
        for( int i = 0; i < 16; ++i, data += 4, ++values ) {
          Endian::FloatToBytes value = { *values };

          data[0] = value.data[0];
          data[1] = value.data[1];
          data[2] = value.data[2];
          data[3] = value.data[3];
        }
      }
      else {
        for( int i = 0; i < 16; ++i, data += 4, ++values ) {
          Endian::FloatToBytes value = { *values };

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
     * Deallocate internal buffer if stream is buffered.
     */
    void deallocate()
    {
      if( buffered ) {
        delete[] streamBegin;

        streamPos   = nullptr;
        streamBegin = nullptr;
        streamEnd   = nullptr;
      }
    }

};

}
