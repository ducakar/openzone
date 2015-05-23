/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/File.hh
 *
 * `File` class.
 */

#pragma once

#include "Stream.hh"

namespace oz
{

/**
 * Class for basic file and directory operations.
 *
 * This class provides two back-ends: the native file system back-end and virtual file system (VFS)
 * back-end implemented via PhysicsFS.
 *
 * Paths inside VFS are always absolute and should not begin with '/', they use '/' as a path
 * separator and are prefixed by "@".
 */
class File : public String
{
public:

  /// Invalid file.
  static const File NIL;

  /// User's home/profile directory.
  static const File& HOME;

  /// Directory for configuration (i.e. `~/.config`).
  static const File& CONFIG;

  /// Directory for program data (i.e. `~/.local/share`).
  static const File& DATA;

  /// User's desktop directory.
  static const File& DESKTOP;

  /// User's directory for documents.
  static const File& DOCUMENTS;

  /// User's directory for downloads.
  static const File& DOWNLOAD;

  /// User's directory for music.
  static const File& MUSIC;

  /// User's directory for pictures.
  static const File& PICTURES;

  /// User's directory for videos.
  static const File& VIDEOS;

  /**
   * %File type classification.
   */
  enum Type
  {
    MISSING,
    DIRECTORY,
    REGULAR
  };

  /// File information returned by `stat()`.
  struct Info
  {
    Type   type;
    int    size;
    long64 time;
  };

public:

  /**
   * Empty/invalid instance.
   */
  File() = default;

  /**
   * Create an instance for a given path.
   *
   * `stat()` is automatically called on construction unless the path is empty.
   */
  File(const char* path);

  /**
   * Create an instance for a given path.
   *
   * `stat()` is automatically called on construction unless the path is empty.
   */
  File(const String& path);

  /**
   * Recreate instance for a given path.
   *
   * `stat()` is automatically called on construction unless the new path is empty.
   */
  File& operator = (const char* path);

  /**
   * Recreate instance for a given path.
   *
   * `stat()` is automatically called on construction unless the new path is empty.
   */
  File& operator = (const String& path);

  /**
   * True iff path is not initialised (i.e. an empty string or "@").
   */
  OZ_ALWAYS_INLINE
  bool isNil() const
  {
    const char* buffer = begin();
    return buffer[0] == '\0' || (buffer[1] == '\0' && buffer[0] == '@');
  }

  /**
   * True iff VFS file path.
   */
  OZ_ALWAYS_INLINE
  bool isVirtual() const
  {
    return begin()[0] == '@';
  }

  /**
   * Stat file and return its type, size and modification/creation time.
   */
  Info stat() const;

  /**
   * Extract directory from the path (substring before the last `/`).
   */
  File directory() const;

  /**
   * Extract file name from the path (substring after the last `/`).
   */
  String name() const;

  /**
   * Extract base file name from the path (substring after the last `/` till the last dot following
   * it).
   */
  String baseName() const;

  /**
   * Extract file extension from the path (substring after the last dot in file name or "" if no
   * extension).
   */
  String extension() const;

  /**
   * True iff file name has a given extension.
   *
   * Empty string matches both no extension and files names ending with dot.
   */
  bool hasExtension(const char* ext) const;

  /**
   * %Path to the archive or mount-point of a VFS, "" for native files.
   */
  String realDirectory() const;

  /**
   * %Path to file in native file system.
   *
   * This function does not work correctly for VFS files from archives and that whose directory was
   * not mounted to the root of VFS. This function is identical to `path()` for files with native
   * file system path.
   */
  String realPath() const;

  /**
   * Return file with a given string appended to its path.
   */
  File operator + (const String& pathElem) const;

  /**
   * Return file with a given string appended to its path.
   */
  File operator + (const char* pathElem) const;

  /**
   * Return file with path separator and a given token appended to its path.
   */
  File operator / (const String& pathElem) const;

  /**
   * Return file with path separator and a given token appended to its path.
   */
  File operator / (const char* pathElem) const;

  /**
   * Append a given string to the path.
   */
  File& operator += (const String& pathElem);

  /**
   * Append a given string to the path.
   */
  File& operator += (const char* pathElem);

  /**
   * Append path separator and a given token to the path.
   */
  File& operator /= (const String& pathElem);

  /**
   * Append path separator and a given token to the path.
   */
  File& operator /= (const char* pathElem);

  /**
   * Read at most `*size` bytes from file and update `*size` to the number of bytes read.
   *
   * @return true iff read operation succeeded (it is not necessary the whole file was read).
   */
  bool read(char* buffer, int* size) const;

  /**
   * Read file and write it to a given stream's current position.
   *
   * If there's not enough space on stream, the rest of the file contents is skipped.
   *
   * @return true iff read operation succeeded (it is not necessary the whole file was read).
   */
  bool read(Stream* os) const;

  /**
   * Read file into a buffer.
   *
   * @return true iff read operation succeeded (it is not necessary the whole file was read).
   */
  Buffer read() const;

  /**
   * Read file as a string.
   *
   * Terminating null character is always assured.
   */
  String readString() const;

  /**
   * Write buffer contents to the file.
   *
   * @note
   * - Write operation is not possible while the file is mapped.
   */
  bool write(const char* data, int size) const;

  /**
   * Write buffer contents into the file.
   *
   * @note
   * - Write operation is not possible while the file is mapped.
   */
  bool write(const Buffer& buffer) const;

  /**
   * Write string into the file (omitting the terminating null character).
   *
   * @note
   * - Write operation is not possible while the file is mapped.
   */
  bool writeString(const String& s) const;

  /**
   * Create a buffered stream that contains file contents.
   *
   * An invalid (empty) stream is returned on error.
   */
  Stream inputStream(Endian::Order order = Endian::NATIVE) const;

  /**
   * Copy a file.
   *
   * @param dest destination directory or file.
   */
  bool copyTo(const File& dest) const;

  /**
   * Move/rename a file.
   *
   * @param dest destination directory or file.
   */
  bool moveTo(const File& dest) const;

  /**
   * Delete a file or an empty directory.
   */
  bool remove() const;

  /**
   * Generate a list of files in directory.
   *
   * Hidden files (in Unix means, so everything starting with '.') are skipped.
   * On error, an empty array is returned.
   *
   * Directory listing is not supported on NaCl, so this function always returns an empty list.
   *
   * @param extension if not null, filter out all files that don't have the specified extension.
   */
  List<File> ls(const char* extension = nullptr) const;

  /**
   * Return the current directory in native file system.
   *
   * Empty string is returned on failure. This function always fails on NaCl.
   */
  static File cwd();

  /**
   * Change current directory in native file system.
   *
   * Always fails on NaCl.
   */
  bool chdir() const;

  /**
   * Make a new directory.
   */
  bool mkdir() const;

  /**
   * Mount read-only directory or archive into VFS.
   *
   * @param mountPoint mount point in VFS, `nullptr` or "" equals root of VFS.
   * @param append true to append to the end instead to the beginning of the search path.
   */
  bool mountAt(const char* mountPoint, bool append = true) const;

  /**
   * Mount writeable local resource directory to the root of VFS.
   *
   * Only physical files under this directory can be changed in VFS. Any new files or directories
   * created in VFS will be physically written here.
   *
   * @note
   * If more than one directory is mounted via `mountLocal()` the last one will be used for writing.
   *
   * @param append true to append to the end instead to the beginning of the search path.
   */
  bool mountLocalAt(bool append = true) const;

  /**
   * Get executable file path.
   *
   * On NaCl or on an error, an empty string is returned.
   */
  static const File& executable();

  /**
   * Initialise VFS and NaCl file system, determine user directories and executable path.
   *
   * @param argv0 argv[0] argument passed to `main()` function, used by PhysicsFS initialisation on
   *              certain platforms (e.g. NaCl).
   */
  static void init(const char* argv0);

  /**
   * Deinitialise file systems.
   */
  static void destroy();

};

}
