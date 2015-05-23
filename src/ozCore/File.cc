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
  Stream is = file.inputStream();

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
    File   dir  = line.substring(firstQuote + 1, lastQuote);

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

  if (File::HOME.isNil()) {
    OZ_ERROR("oz::File: Unable to determine home directory: HOME environment variable not set");
  }

  // Read global settings, if exist.
  loadXDGSettings(&vars, File("/etc/xdg/user"));

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
  executableFile = length < 0 ? File() : File(String(exePathBuffer, int(length)));
}

#endif

const File  File::NIL       = File();
const File& File::HOME      = specialDirs[0];
const File& File::CONFIG    = specialDirs[1];
const File& File::DATA      = specialDirs[2];
const File& File::DESKTOP   = specialDirs[3];
const File& File::DOCUMENTS = specialDirs[4];
const File& File::DOWNLOAD  = specialDirs[5];
const File& File::MUSIC     = specialDirs[6];
const File& File::PICTURES  = specialDirs[7];
const File& File::VIDEOS    = specialDirs[8];

File::File(const char* path) :
  String(path)
{}

File::File(const String& path) :
  String(path)
{}

File& File::operator = (const char* path)
{
  return static_cast<File&>(String::operator = (path));
}

File& File::operator = (const String& path)
{
  return static_cast<File&>(String::operator = (path));
}

File::Info File::stat() const
{
  Info info = { MISSING, -1, 0 };

  const char* path = begin();

  if (isVirtual()) {
    if (PHYSFS_exists(&path[1])) {
      if (PHYSFS_isDirectory(&path[1])) {
        info.type = DIRECTORY;
        info.time = PHYSFS_getLastModTime(&path[1]);
      }
      else {
        PHYSFS_File* file = PHYSFS_openRead(&path[1]);

        if (file != nullptr) {
          info.type = REGULAR;
          info.size = int(PHYSFS_fileLength(file));
          info.time = PHYSFS_getLastModTime(&path[1]);

          PHYSFS_close(file);
        }
      }
    }
  }
  else if (!String::isEmpty()) {
    struct stat statInfo;

    if (::stat(path, &statInfo) == 0) {
      if (S_ISDIR(statInfo.st_mode)) {
        info.type = DIRECTORY;
        info.time = max<long64>(statInfo.st_ctime, statInfo.st_mtime);
      }
      else if (S_ISREG(statInfo.st_mode)) {
        info.type = REGULAR;
        info.size = int(statInfo.st_size);
        info.time = max<long64>(statInfo.st_ctime, statInfo.st_mtime);
      }
    }
  }

  return info;
}

File File::directory() const
{
  int slash = lastIndex('/');

  return slash >= 0 ? File(substring(0, slash)) : File();
}

String File::name() const
{
  int slash = lastIndex('/');

  return slash >= 0 ? substring(slash + 1) : substring(isVirtual());
}

String File::baseName() const
{
  int begin = max<int>(lastIndex('/') + 1, isVirtual());
  int dot   = lastIndex('.');

  return begin <= dot ? substring(begin, dot) : substring(begin);
}

String File::extension() const
{
  int slash = lastIndex('/');
  int dot   = lastIndex('.');

  return slash < dot ? substring(dot + 1) : String();
}

bool File::hasExtension(const char* ext) const
{
  const char* slash = findLast('/');
  const char* dot   = findLast('.');

  if (slash < dot) {
    return String::compare(dot + 1, ext) == 0;
  }
  else {
    return String::isEmpty(ext);
  }
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

    return String::format(String::last(realDir) == '/' ? "%s%s" : "%s/%s", realDir, path);
  }
  else {
    return *this;
  }
}

File File::operator + (const String& pathElem) const
{
  return static_cast<File>(String(begin(), length(), pathElem, pathElem.length()));
}

File File::operator + (const char* pathElem) const
{
  return static_cast<File>(String(begin(), length(), pathElem, length(pathElem)));
}

File File::operator / (const String& pathElem) const
{
  int separator = length();

  String newPath(begin(), separator + 1, pathElem, pathElem.length());
  newPath[separator] = '/';

  return static_cast<File>(newPath);
}

File File::operator / (const char* pathElem) const
{
  int separator = length();

  String newPath(begin(), separator + 1, pathElem, length(pathElem));
  newPath[separator] = '/';

  return static_cast<File>(newPath);
}

File& File::operator += (const String& pathElem)
{
  *this = static_cast<File&&>(String(begin(), length(), pathElem, pathElem.length()));
  return *this;
}

File& File::operator += (const char* pathElem)
{
  *this = static_cast<File&&>(String(begin(), length(), pathElem, length(pathElem)));
  return *this;
}

File& File::operator /= (const String& pathElem)
{
  int separator = length();

  *this = static_cast<File&&>(String(begin(), length() + 1, pathElem, pathElem.length()));
  begin()[separator] = '/';

  return *this;
}

File& File::operator /= (const char* pathElem)
{
  int separator = length();

  *this = static_cast<File&&>(String(begin(), length() + 1, pathElem, length(pathElem)));
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

bool File::read(Stream* os) const
{
  int size = stat().size;
  return read(os->writeSkip(size), &size);
}

Buffer File::read() const
{
  Buffer buffer;

  int size = stat().size;
  buffer.resize(size, true);

  read(buffer.begin(), &size);
  return buffer;
}

String File::readString() const
{
  Buffer buffer = read();
  return String(buffer.begin(), buffer.length());
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

bool File::write(const Buffer& buffer) const
{
  return write(buffer.begin(), buffer.length());
}

bool File::writeString(const String& s) const
{
  return write(s.c(), s.length());
}

Stream File::inputStream(Endian::Order order) const
{
  int size = stat().size;
  Stream is(size < 0 ? 0 : size, order);

  if (size == 0 || !read(is.begin(), &size)) {
    is.free();
  }
  return is;
}

bool File::copyTo(const File& dest_) const
{
  File dest = dest_;
  if (dest.stat().type == DIRECTORY) {
    dest /= name();
  }

  Buffer buffer = read();
  return buffer.isEmpty() ? false : dest.write(buffer.begin(), buffer.length());
}

bool File::moveTo(const File& dest_) const
{
  if (isVirtual()) {
    return false;
  }

  File dest = dest_;
  if (dest.stat().type == DIRECTORY) {
    dest /= name();
  }

  return rename(begin(), dest) == 0;
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

List<File> File::ls(const char* extension) const
{
  List<File> list;

  if (isVirtual()) {
    char** entities = PHYSFS_enumerateFiles(begin() + 1);
    if (entities == nullptr) {
      return list;
    }

    String prefix = length() == 1 || last() == '/' ? *this : *this + "/";

    for (char** entity = entities; *entity != nullptr; ++entity) {
      File entry = prefix + *entity;

      if ((*entity)[0] != '.' && (extension == nullptr || entry.hasExtension(extension))) {
        list.add(entry);
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
      File entry = prefix + entity->d_name;

      if (entity->d_name[0] != '.' && (extension == nullptr || entry.hasExtension(extension))) {
        list.add(entry);
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

bool File::mkdir() const
{
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

  Arrays::fill<File, File>(specialDirs, Arrays::length<File>(specialDirs), File());
  executableFile = "";
}

}
