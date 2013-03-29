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
 * @file ozCore/Log.hh
 *
 * `Log` class.
 */

#pragma once

#include "StackTrace.hh"
#include "String.hh"
#include "Mat44.hh"

namespace oz
{

/**
 * %Log writing utility.
 *
 * Logging service, can write to terminal and into a file. After each operation stream is flushed,
 * so no data is lost on a crash.
 *
 * Two spaces are used for indentation.
 */
class Log
{
  public:

    static bool showVerbose; ///< Do not suppress verbose messages on stdout. Off by default.
    static bool verboseMode; ///< While `verboseMode` is on log output is only written to log file
                             ///< unless `isVerbose` is also on. Off by default.

  public:

    /**
     * Return log file path or an empty string if log is printed to stdout only.
     */
    static const char* logFile();

    /**
     * %Set indent to zero.
     */
    static void resetIndent();

    /**
     * Increase indent.
     */
    static void indent();

    /**
     * Decrease indent.
     */
    static void unindent();

    /**
     * Print the raw text (without indentation or newline).
     */
    static void putsRaw( const char* s );

    /**
     * Print the raw text (without indentation or newline).
     */
    OZ_PRINTF_FORMAT( 1, 0 )
    static void vprintRaw( const char* s, va_list ap );

    /**
     * Print the raw text (without indentation or newline).
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static void printRaw( const char* s, ... );

    /**
     * Indent and print the text.
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static void print( const char* s, ... );

    /**
     * Print the text and terminate the line.
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static void printEnd( const char* s, ... );

    /**
     * Terminate the line.
     */
    static void printEnd();

    /**
     * Indent, print the text and terminate the line.
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static void println( const char* s, ... );

    /**
     * Print a blank line.
     */
    static void println();

    /**
     * Print current thread's name and stack trace.
     */
    static void printTrace( const StackTrace& st );

    /**
     * Print "Caught signal ..." and signal description.
     *
     * This method is only required internally by signal handler since it has issues with functions
     * that take variable arguments.
     */
    static void printSignal( int sigNum );

    /**
     * Print summary about memory usage (`Alloc`'s -`count` and -`size` fields).
     *
     * @sa `oz::Alloc`
     */
    static void printMemorySummary();

    /**
     * Print memory chunks allocated via `new` and `new[]` operators, tracked by `Alloc` class.
     *
     * @return true iff there is any allocated memory chunk.
     *
     * @sa `oz::Alloc`
     */
    static bool printMemoryLeaks();

    /**
     * First parameter is file path (if `nullptr` or "", it only writes to terminal), the other
     * tells whether to clear its content if the file already exists.
     */
    static bool init( const char* filePath = nullptr, bool clearFile = true );

    /**
     * Close log file.
     */
    static void destroy();

    /**
     * Same as `putsRaw( b ? "true" : "false" )`.
     */
    const Log& operator << ( bool b ) const;

    /**
     * Same as `printRaw( "%c", c )`.
     */
    const Log& operator << ( char c ) const;

    /**
     * Same as `printRaw( "%d", b )`.
     */
    const Log& operator << ( byte b ) const;

    /**
     * Same as `printRaw( "%ud", b )`.
     */
    const Log& operator << ( ubyte b ) const;

    /**
     * Same as `printRaw( "%d", s )`.
     */
    const Log& operator << ( short s ) const;

    /**
     * Same as `printRaw( "%ud", s )`.
     */
    const Log& operator << ( ushort s ) const;

    /**
     * Same as `printRaw( "%d", i )`.
     */
    const Log& operator << ( int i ) const;

    /**
     * Same as `printRaw( "%ud", i )`.
     */
    const Log& operator << ( uint i ) const;

    /**
     * Same as `printRaw( "%ld", l )`.
     */
    const Log& operator << ( long l ) const;

    /**
     * Same as `printRaw( "%lud", l )`.
     */
    const Log& operator << ( ulong l ) const;

    /**
     * Same as `printRaw( "%lld", l )`.
     */
    const Log& operator << ( long64 l ) const;

    /**
     * Same as `printRaw( "%llud", l )`.
     */
    const Log& operator << ( ulong64 l ) const;

    /**
     * Same as `printRaw( "%g", f )`.
     */
    const Log& operator << ( float f ) const;

    /**
     * Same as `printRaw( "%g", d )`.
     */
    const Log& operator << ( double d ) const;

    /**
     * Same as `putsRaw( s )`.
     */
    const Log& operator << ( const String& s ) const;

    /**
     * Same as `putsRaw( s )`.
     */
    const Log& operator << ( const char* s ) const;

    /**
     * Same as `printRaw( "(%g %g %g)", v.x, v.y, v.z )`.
     */
    const Log& operator << ( const Vec3& v ) const;

    /**
     * Same as `printRaw( "(%g %g %g %g)", v.x, v.y, v.z, v.z )`.
     */
    const Log& operator << ( const Vec4& v ) const;

    /**
     * Same as `printRaw( "[%g %g %g]", p.x, p.y, p.z )`.
     */
    const Log& operator << ( const Point& p ) const;

    /**
     * Same as `printRaw( "(%g %g %g; %g)", p.n.x, p.n.y, p.n.z, p.d )`.
     */
    const Log& operator << ( const Plane& p ) const;

    /**
     * Same as `printRaw( "[%g %g %g %g]", q.x, q.y, q.z, q.w )`.
     */
    const Log& operator << ( const Quat& q ) const;

    /**
     * Print 3x3 matrix as `printRaw( "[%g %g %g; %g %g %g; %g %g %g]", ... )` in row-major format.
     */
    const Log& operator << ( const Mat33& m ) const;

    /**
     * Print 4x4 matrix as `printRaw( "[%g %g %g %g; %g %g %g %g; %g %g %g %g; %g %g %g %g]", ... )`
     * in row-major format.
     */
    const Log& operator << ( const Mat44& m ) const;

    /**
     * Same as `printfRaw( "%p", p )`.
     */
    const Log& operator << ( volatile const void* p ) const;

};

}
