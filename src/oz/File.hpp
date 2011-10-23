/*
 *  File.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file File.hpp
 */

#include "String.hpp"
#include "Buffer.hpp"

namespace oz
{

/**
 * Simple class for obtaining file type, base name etc. and basic directory operations.
 */
class File
{
  public:

    /**
     * File type classification.
     */
    enum Type
    {
      NONE,
      REGULAR,
      DIRECTORY,
      OTHER,
      MISSING
    };

  private:

    String filePath; ///< File path.
    Type   type;     ///< Cached file type.

  public:

    /**
     * Create an uninitialised instance.
     */
    File()
    {}

    /**
     * Create instance for the given path.
     */
    explicit File( const char* path );

    /**
     * Set a new file path.
     *
     * Cached file type is cleared to <tt>NONE</tt>.
     */
    void setPath( const char* path );

    /**
     * Stat file to get its type.
     *
     * File type is cached until you change the file path.
     */
    Type getType();

    /**
     * File path.
     */
    const char* path() const;

    /**
     * File name.
     */
    const char* name() const;

    /**
     * Extension (everything after the last dot in name.
     */
    const char* extension() const;

    /**
     * Name without the extension (and the dot).
     */
    String baseName() const;

    /**
     * True iff the extension is equal to the given string.
     *
     * @return True iff extension exists.
     */
    bool hasExtension( const char* ext ) const;

    /**
     * Make a new directory.
     *
     * @return True on success.
     */
    static bool mkdir( const char* path, uint mode = 0755 );

    /**
     * Generate a list of files in directory.
     *
     * Hidden files (in Unix means, so everything starting with '.') are skipped. On POSIX systems
     * file types are also set - for free.
     *
     * @return True on success.
     */
    bool ls( DArray<File>* fileList );

};

}
