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
 * @file ozCore/File.cc
 */

#include "File.hh"

#include "Map.hh"
#include "Pepper.hh"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef _WIN32
# include <windows.h>
# include <shlobj.h>
#endif

#include <physfs.h>

namespace oz
{

struct Stat
{
  enum Type
  {
    MISSING,
    FILE,
    DIRECTORY
  };

  Type   type = MISSING;
  int    size = -1;
  long64 time = 0;

  OZ_HIDDEN
  explicit Stat(const char* path)
  {
    if (path[0] == '@') {
      ++path;

      if (PHYSFS_exists(path)) {
        if (PHYSFS_isDirectory(path)) {
          type = DIRECTORY;
          time = PHYSFS_getLastModTime(path);
        }
        else {
          PHYSFS_File* file = PHYSFS_openRead(path);

          if (file != nullptr) {
            type = FILE;
            size = int(PHYSFS_fileLength(file));
            time = PHYSFS_getLastModTime(path);

            PHYSFS_close(file);
          }
        }
      }
    }
    else if (path[0] != '\0') {
      struct stat statInfo;

      if (::stat(path, &statInfo) == 0) {
        if (S_ISDIR(statInfo.st_mode)) {
          type = DIRECTORY;
          time = max<long64>(statInfo.st_ctime, statInfo.st_mtime);
        }
        else if (S_ISREG(statInfo.st_mode)) {
          type = FILE;
          size = int(statInfo.st_size);
          time = max<long64>(statInfo.st_ctime, statInfo.st_mtime);
        }
      }
    }
  }
};

static File specialDirs[10];
static File executableFile;

#if defined(__native_client__)

static void initSpecialDirs()
{
  specialDirs[0] = "/";
  specialDirs[1] = "/config";
  specialDirs[2] = "/data";

  File::CONFIG.mkdir();
  File::DATA.mkdir();
}

static void initExecutablePath()
{
  executableFile = "/";
}

#elif defined(_WIN32)

static void setSpecialDir(int id, int csidl)
{
  char path[MAX_PATH];
  path[0] = '\0';

  SHGetSpecialFolderPath(nullptr, path, csidl, false);

  specialDirs[id] = String(path).replace('\\', '/');
}

static void initSpecialDirs()
{
  setSpecialDir(0, CSIDL_PROFILE);
  setSpecialDir(1, CSIDL_APPDATA);
  setSpecialDir(2, CSIDL_LOCAL_APPDATA);
  setSpecialDir(3, CSIDL_DESKTOPDIRECTORY);
  setSpecialDir(4, CSIDL_PERSONAL);
  setSpecialDir(5, CSIDL_PERSONAL);
  setSpecialDir(6, CSIDL_MYMUSIC);
  setSpecialDir(7, CSIDL_MYPICTURES);
  setSpecialDir(8, CSIDL_MYVIDEO);
}

static void initExecutablePath()
{
  char path[MAX_PATH];

  HMODULE module = GetModuleHandle(nullptr);
  int     length = GetModuleFileName(module, path, MAX_PATH);

  if (length == 0 || length == MAX_PATH) {
    executableFile = String(path, length).replace('\\', '/');
  }
}

#else

static void setSpecialDir(Map<String, File>* vars, int id, const char* name, const File& defValue)
{
  const char* value = getenv(name);

  if (value != nullptr) {
    specialDirs[id] = value;
  }
  else {
    const File* configValue = vars->find(name);

    specialDirs[id] = configValue == nullptr ? defValue : *configValue;
  }
}

static void loadXDGSettings(Map<String, File>* vars, const File& file)
{
  Stream is = file.read();

  while (is.available() != 0) {
    String line = is.readLine();

    if (line[0] == '#') {
      continue;
    }

    int equal      = line.index('=');
    int firstQuote = line.index('"');
    int lastQuote  = line.lastIndex('"');

    if (equal <= 0 || firstQuote <= equal || lastQuote <= firstQuote) {
      continue;
    }

    String name = line.substring(0, equal).trim();
    File   dir  = File(line + firstQuote + 1, lastQuote - firstQuote - 1);

    if (!name.beginsWith("XDG_") || !name.endsWith("_DIR")) {
      continue;
    }

    if (dir.beginsWith("$HOME")) {
      dir = File::HOME + dir.substring(5);
    }

    vars->add(name, dir);
  }
}

static void initSpecialDirs()
{
  Map<String, File> vars;

  setSpecialDir(&vars, 0, "HOME", File());

  if (File::HOME.isEmpty()) {
    OZ_ERROR("oz::File: Unable to determine home directory: HOME environment variable not set");
  }

  // Read global settings, if exist.
  loadXDGSettings(&vars, "/etc/xdg/user");

  // Read config and data locations from environment.
  setSpecialDir(&vars, 1, "XDG_CONFIG_HOME",   File::HOME / ".config");
  setSpecialDir(&vars, 2, "XDG_DATA_HOME",     File::HOME / ".local/share");

  // Override default locations with user settings, if exist.
  loadXDGSettings(&vars, File::CONFIG / "user-dirs.dirs");

  // Finally set special directories, environment variables override values from files.
  setSpecialDir(&vars, 3, "XDG_DESKTOP_DIR",   File::HOME / "Desktop");
  setSpecialDir(&vars, 4, "XDG_DOCUMENTS_DIR", File::HOME / "Documents");
  setSpecialDir(&vars, 5, "XDG_DOWNLOAD_DIR",  File::HOME / "Download");
  setSpecialDir(&vars, 6, "XDG_MUSIC_DIR",     File::HOME / "Music");
  setSpecialDir(&vars, 7, "XDG_PICTURES_DIR",  File::HOME / "Pictures");
  setSpecialDir(&vars, 8, "XDG_VIDEOS_DIR",    File::HOME / "Videos");
}

static void initExecutablePath()
{
  char pidPathBuffer[PATH_MAX];
  char exePathBuffer[PATH_MAX];

  pid_t pid = getpid();
  snprintf(pidPathBuffer, PATH_MAX, "/proc/%d/exe", pid);

  ptrdiff_t length = readlink(pidPathBuffer, exePathBuffer, PATH_MAX);
  executableFile = File(exePathBuffer, length < 0 ? 0 : int(length));
}

#endif

const File& File::HOME      = specialDirs[0];
const File& File::CONFIG    = specialDirs[1];
const File& File::DATA      = specialDirs[2];
const File& File::DESKTOP   = specialDirs[3];
const File& File::DOCUMENTS = specialDirs[4];
const File& File::DOWNLOAD  = specialDirs[5];
const File& File::MUSIC     = specialDirs[6];
const File& File::PICTURES  = specialDirs[7];
const File& File::VIDEOS    = specialDirs[8];

File::File(const String& path) :
  String(path)
{}

File::File(String&& path) :
  String(static_cast<String&&>(path))
{}

File& File::operator = (const String& path)
{
  return static_cast<File&>(String::operator = (path));
}

File& File::operator = (String&& path)
{
  return static_cast<File&>(String::operator = (static_cast<String&&>(path)));
}

bool File::exists() const
{
  return Stat(begin()).type != Stat::MISSING;
}

bool File::isFile() const
{
  return Stat(begin()).type == Stat::FILE;
}

bool File::isDirectory() const
{
  return Stat(begin()).type == Stat::DIRECTORY;
}

int File::size() const
{
  return Stat(begin()).size;
}

long64 File::time() const
{
  return Stat(begin()).time;
}

File File::directory() const
{
  int slash = lastIndex('/', length() - 1);
  int end   = slash < 0 ? length() != 1 && first() == '@' : slash + (slash == 0 && first() == '/');

  return File(begin(), end); // = substring(0, end), but prevents String -> File ctor.
}

String File::name() const
{
  int start = lastIndex('/', length() - 1) + 1;
  int end   = length() - (last() == '/');

  start = start == 0 ? isVirtual() : start;

  return substring(start, end);
}

String File::baseName() const
{
  int start = lastIndex('/', length() - 1) + 1;
  int end   = lastIndex('.');

  start = start == 0 ? isVirtual() : start;
  end   = end < start ? length() - (last() == '/') : end;

  return substring(start, end);
}

File File::stripExtension() const
{
  int slash = lastIndex('/', length() - 1);
  int start = lastIndex('.');
  int end   = start <= slash ? length() : start;

  return File(begin(), end); // = substring(0, end), but prevents String -> File ctor.
}

String File::extension() const
{
  int slash = lastIndex('/', length() - 1);
  int start = lastIndex('.');
  int end   = start <= slash ? start + 1 : length() - (last() == '/');

  return substring(start + 1, end);
}

bool File::hasExtension(const char* ext) const
{
  const char* slash = findLast('/');
  const char* dot   = findLast('.');

  if (slash < dot) {
    return compare(dot + 1, ext) == 0;
  }
  else {
    return isEmpty(ext);
  }
}

File File::toNative() const
{
  int start = (first() == '@');
  return File(begin() + start, length() - start);
}

File File::toVirtual() const
{
  return File("@", first() == '@', begin(), length());
}

String File::realDirectory() const
{
  if (isVirtual()) {
    const char* realDir = PHYSFS_getRealDir(begin() + 1);
    return realDir == nullptr ? "" : realDir;
  }
  else {
    return "";
  }
}

String File::realPath() const
{
  if (isVirtual()) {
    const char* path    = begin() + 1;
    const char* realDir = PHYSFS_getRealDir(path);
    realDir = realDir == nullptr ? "" : realDir;

    return format(last(realDir) == '/' ? "%s%s" : "%s/%s", realDir, path);
  }
  else {
    return *this;
  }
}

File File::operator + (const String& pathElem) const
{
  return File(begin(), length(), pathElem, pathElem.length());
}

File File::operator + (const char* pathElem) const
{
  return File(begin(), length(), pathElem, length(pathElem));
}

File File::operator / (const String& pathElem) const
{
  int separator = length();

  File newPath(begin(), separator + 1, pathElem, pathElem.length());
  newPath[separator] = '/';

  return newPath;
}

File File::operator / (const char* pathElem) const
{
  int separator = length();

  File newPath(begin(), separator + 1, pathElem, length(pathElem));
  newPath[separator] = '/';

  return newPath;
}

File& File::operator += (const String& pathElem)
{
  *this = File(begin(), length(), pathElem, pathElem.length());
  return *this;
}

File& File::operator += (const char* pathElem)
{
  *this = File(begin(), length(), pathElem, length(pathElem));
  return *this;
}

File& File::operator /= (const String& pathElem)
{
  int separator = length();

  *this = File(begin(), length() + 1, pathElem, pathElem.length());
  begin()[separator] = '/';

  return *this;
}

File& File::operator /= (const char* pathElem)
{
  int separator = length();

  *this = File(begin(), length() + 1, pathElem, length(pathElem));
  begin()[separator] = '/';

  return *this;
}

bool File::read(char* buffer, int* size) const
{
  if (isVirtual()) {
    PHYSFS_File* file = PHYSFS_openRead(begin() + 1);
    if (file == nullptr) {
      *size = 0;
      return false;
    }

    int result = int(PHYSFS_read(file, buffer, 1, *size));
    PHYSFS_close(file);

    *size = result;
    return true;
  }
  else {
#ifdef _WIN32
    int fd = open(begin(), O_RDONLY | O_BINARY);
#else
    int fd = open(begin(), O_RDONLY);
#endif
    if (fd < 0) {
      *size = 0;
      return false;
    }

    int nBytes = int(::read(fd, buffer, *size));
    close(fd);

    if (nBytes != *size) {
      *size = max<int>(0, nBytes);
      return false;
    }
    return true;
  }
}

Stream File::read(Endian::Order order) const
{
  int size = Stat(begin()).size;
  Stream is(size < 0 ? 0 : size, order);

  if (size == 0 || !read(is.begin(), &size)) {
    is.free();
  }
  return is;
}

bool File::write(const char* buffer, int size) const
{
  if (isVirtual()) {
    PHYSFS_File* file = PHYSFS_openWrite(begin() + 1);
    if (file == nullptr) {
      return false;
    }

    int result = int(PHYSFS_write(file, buffer, 1, size));
    PHYSFS_close(file);

    return result == size;
  }
  else {
#ifdef _WIN32
    int fd = open(begin(), O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0644);
#else
    int fd = open(begin(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
#endif
    if (fd < 0) {
      return false;
    }

    int result = int(::write(fd, buffer, size));
    close(fd);

    return result == size;
  }
}

bool File::copyTo(const File& dest) const
{
  Stream stream = read();
  if (stream.available() == 0) {
    return false;
  }

  File destFile = Stat(dest).type == Stat::DIRECTORY ? dest / name() : dest;
  return destFile.write(stream.begin(), stream.capacity());
}

bool File::copyTreeTo(const File& dest, const char* ext) const
{
  bool success = true;
  Stat stat    = Stat(begin());

  if (stat.type == Stat::FILE) {
    if (ext == nullptr || hasExtension(ext)) {
      success &= copyTo(dest);
    }
  }
  else if (stat.type == Stat::DIRECTORY) {
    File destDir = Stat(dest).type == Stat::DIRECTORY ? dest / name() : dest;

    destDir.mkdir();

    for (const File& file : list()) {
      success &= file.copyTreeTo(destDir, ext);
    }
  }
  return success;
}

bool File::moveTo(const File& dest) const
{
  if (isVirtual()) {
    return false;
  }
  else {
    File destFile = Stat(dest).type == Stat::DIRECTORY ? dest / name() : dest;
    return rename(begin(), destFile) == 0;
  }
}

bool File::remove() const
{
  if (isVirtual()) {
    return PHYSFS_delete(begin() + 1);
  }
  else {
    return ::remove(begin()) == 0;
  }
}

bool File::removeTree(const char* ext) const
{
  bool success = true;
  Stat stat    = Stat(begin());

  if (stat.type == Stat::FILE) {
    if (ext == nullptr || hasExtension(ext)) {
      success &= remove();
    }
  }
  else if (stat.type == Stat::DIRECTORY) {
    for (const File& file : list()) {
      success &= file.removeTree(ext);
    }

    // Delete directory if empty.
    if (list().isEmpty()) {
      success &= remove();
    }
  }
  return success;
}

List<File> File::list(const char* ext) const
{
  List<File> list;

  if (isVirtual()) {
    char** entities = PHYSFS_enumerateFiles(begin() + 1);
    if (entities == nullptr) {
      return list;
    }

    String prefix = length() == 1 || last() == '/' ? *this : *this + "/";

    for (char** entity = entities; *entity != nullptr; ++entity) {
      if (!equals(*entity, ".") && !equals(*entity, "..")) {
        File entry(prefix, prefix.length(), *entity, length(*entity));

        if (ext == nullptr || entry.hasExtension(ext)) {
          list.add(entry);
        }
      }
    }

    PHYSFS_freeList(entities);
  }
  else {
    DIR* directory = opendir(begin());
    if (directory == nullptr) {
      return list;
    }

    String prefix = last() == '/' ? *this : *this + "/";
    // readlink() implementation is "usually" (required by POSIX) thread-safe as long as threads
    // don't access the same directory stream. Since opendir() is called for each thread separately
    // in our case, use of readdir() is safe here. This way we avoid readdir_r()-related issues with
    // portability and entity buffer size.
    dirent* entity = readdir(directory);

    while (entity != nullptr) {
      if (!equals(entity->d_name, ".") && !equals(entity->d_name, "..")) {
        File entry(prefix, prefix.length(), entity->d_name, length(entity->d_name));

        if (ext == nullptr || entry.hasExtension(ext)) {
          list.add(entry);
        }
      }
      entity = readdir(directory);
    }

    closedir(directory);
  }

  list.sort();
  return list;
}

File File::cwd()
{
  char buffer[PATH_MAX];
  bool hasFailed = getcwd(buffer, PATH_MAX) == nullptr;
  return hasFailed ? "" : buffer;
}

bool File::chdir() const
{
  return ::chdir(begin()) == 0;
}

bool File::mkdir(bool makeParents) const
{
  if (makeParents) {
    File parent = directory();

    if (!parent.isEmpty() && !parent.isRoot()) {
      parent.mkdir(true);
    }
  }

  if (isVirtual()) {
    return PHYSFS_mkdir(begin() + 1) != 0;
  }
  else {
#ifdef _WIN32
    return ::mkdir(begin()) == 0;
#else
    return ::mkdir(begin(), 0755) == 0;
#endif
  }
}

bool File::mountAt(const char* mountPoint, bool append) const
{
  return PHYSFS_mount(begin(), mountPoint, append) != 0;
}

bool File::mountLocalAt(bool append) const
{
  if (PHYSFS_setWriteDir(begin()) == 0) {
    return false;
  }
  if (PHYSFS_mount(begin(), nullptr, append) == 0) {
    PHYSFS_setWriteDir(nullptr);
    return false;
  }
  return true;
}

const File& File::executable()
{
  return executableFile;
}

void File::init(const char* argv0)
{
  initSpecialDirs();
  initExecutablePath();

  if (PHYSFS_init(argv0) == 0) {
    OZ_ERROR("oz::File: PhysicsFS initialisation failed: %s", PHYSFS_getLastError());
  }
}

void File::destroy()
{
  PHYSFS_deinit();

  Arrays::clear<File>(specialDirs, Arrays::length<File>(specialDirs));
  executableFile = "";
}

}
