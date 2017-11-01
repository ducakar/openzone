/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include "Opt.hh"
#include "Stream.hh"

namespace oz
{

/**
 * Class for basic file and directory operations.
 *
 * This class provides two back-ends: the native file system back-end and virtual file system (VFS)
 * back-end implemented via PhysicsFS.
 *
 * Paths inside VFS are always absolute and use '@' in place the leading '/'.
 */
class File : public String
{
public:

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

  /// Application's executable path ("/" on NaCl).
  static const File& EXECUTABLE;

private:

  using String::String;

public:

  /**
   * Empty/invalid instance.
   */
  File() = default;

  /**
   * Create an instance for a given path.
   */
  File(const String& path);

  /**
   * Create an instance for a given path.
   */
  File(String&& path) noexcept;

  /**
   * Create an instance for a given path.
   */
  File(const char* path)
    : String(path)
  {}

  /**
   * Recreate instance for a given path.
   */
  File& operator=(const String& path);

  /**
   * Recreate instance for a given path.
   */
  File& operator=(String&& path) noexcept;

  /**
   * Recreate instance for a given path.
   */
  File& operator=(const char* path)
  {
    return static_cast<File&>(String::operator=(path));
  }

  /**
   * True iff VFS file path.
   */
  OZ_ALWAYS_INLINE
  bool isVirtual() const
  {
    return first() == '@';
  }

  /**
   * True iff filesystem of VFS root (i.e. equals "/" or "@").
   */
  OZ_ALWAYS_INLINE
  bool isRoot() const
  {
    return length() == 1 && (first() == '/' || first() == '@');
  }

  /**
   * True iff file is a directory or regular file.
   */
  bool exists() const;

  /**
   * True iff file is a regular file.
   */
  bool isFile() const;

  /**
   * True iff file is a directory.
   */
  bool isDirectory() const;

  /**
   * File size in bytes.
   */
  int64 size() const;

  /**
   * File modification or creation time (which is newer).
   */
  int64 time() const;

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
   * Path with file extension removed (if there is any).
   */
  File stripExtension() const;

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
   * Return de-virtualised path (the leading '@' is stripped if exists).
   */
  File toNative() const;

  /**
   * Return "virtualised" path ('@' is prepended not already there).
   */
  File toVirtual() const;

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
  File operator+(const String& pathElem) const;

  /**
   * Return file with a given string appended to its path.
   */
  File operator+(const char* pathElem) const;

  /**
   * Return file with path separator and a given token appended to its path.
   */
  File operator/(const String& pathElem) const;

  /**
   * Return file with path separator and a given token appended to its path.
   */
  File operator/(const char* pathElem) const;

  /**
   * Append a given string to the path.
   */
  File& operator+=(const String& pathElem);

  /**
   * Append a given string to the path.
   */
  File& operator+=(const char* pathElem);

  /**
   * Append path separator and a given token to the path.
   */
  File& operator/=(const String& pathElem);

  /**
   * Append path separator and a given token to the path.
   */
  File& operator/=(const char* pathElem);

  /**
   * Read at most `*size` bytes from file and update `*size` to the number of bytes read.
   *
   * @return true iff read operation succeeded (it is not necessary the whole file was read).
   */
  bool read(char* data_, int64* length) const;

  /**
   * Create a buffered stream that contains file contents.
   *
   * An invalid (empty) stream is returned on error.
   */
  Opt<Stream> read(Endian::Order order = Endian::NATIVE) const;

  /**
   * Write buffer contents to the file.
   */
  bool write(const char* data_, int64 length) const;

  /**
   * Write contents of a stream, equivalent to `write(is.begin(), is.tell())`.
   */
  bool write(const Stream& is) const;

  /**
   * Copy a file.
   *
   * If destination is an existing directory, the file is copied into it and its name is preserved.
   *
   * @param dest destination file or an existing directory.
   */
  bool copyTo(const File& dest) const;

  /**
   * Recursively copy a directory and it contents.
   *
   * If destination is an existing directory, the tree is copied into it and the root's name is
   * preserved.
   *
   * @param dest destination path or an existing directory.
   * @param ext if not null, skip all files with non-matching extensions.
   */
  bool copyTreeTo(const File& dest, const char* ext = nullptr) const;

  /**
   * Move/rename a file or directory.
   *
   * If destination is an existing directory, the file is moved into it and its name is preserved.
   *
   * @param dest destination path or an existing directory.
   */
  bool moveTo(const File& dest) const;

  /**
   * Delete a file or an empty directory.
   */
  bool remove() const;

  /**
   * Recursively delete a file or directory and its contents.
   *
   * @param ext if not null, skip all files with non-matching extensions.
   */
  bool removeTree(const char* ext = nullptr) const;

  /**
   * Generate a list of files in directory.
   *
   * An empty array is returned on error.
   *
   * @param ext if not null, skip all files with non-matching extensions.
   */
  List<File> list(const char* ext = nullptr) const;

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
   *
   * @param makeParents create any non-existent parent directories.
   */
  bool mkdir(bool makeParents = false) const;

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
   * Initialise VFS, determine user directories and executable path.
   */
  static void init();

  /**
   * Deinitialise file systems.
   */
  static void destroy();

};

/**
 * `Less` function object is the same as for C strings.
 */
template <>
struct Less<File> : Less<const char*>
{};

/**
 * `Hash` function object is the same as for C strings.
 */
template <>
struct Hash<File> : Hash<const char*>
{};

}
