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
#else
# include <sys/mman.h>
#endif

#include <physfs.h>

namespace oz
{

static String specialDirs[10];
static String exePath;

static bool operator < (const File& a, const File& b)
{
  return String::compare(a.path(), b.path()) < 0;
}

#if defined(__native_client__)

static void initSpecialDirs()
{
  specialDirs[0] = "/";
  specialDirs[1] = "/config";
  specialDirs[2] = "/data";

  File::mkdir("/config");
  File::mkdir("/data");
}

static void initExecutablePath()
{
  exePath = "/";
}

#elif defined(_WIN32)

static void setSpecialDir(int dirId, int csidl)
{
  char path[MAX_PATH];
  path[0] = '\0';

  SHGetSpecialFolderPath(nullptr, path, csidl, false);

  for (int i = 0; i < MAX_PATH && path[i] != '\0'; ++i) {
    path[i] = path[i] == '\\' ? '/' : path[i];
  }

  specialDirs[dirId] = path;
}

static void initSpecialDirs()
{
  setSpecialDir(0, CSIDL_PROFILE         );
  setSpecialDir(1, CSIDL_APPDATA         );
  setSpecialDir(2, CSIDL_LOCAL_APPDATA   );
  setSpecialDir(3, CSIDL_DESKTOPDIRECTORY);
  setSpecialDir(4, CSIDL_PERSONAL        );
  setSpecialDir(5, CSIDL_PERSONAL        );
  setSpecialDir(6, CSIDL_MYMUSIC         );
  setSpecialDir(7, CSIDL_MYPICTURES      );
  setSpecialDir(8, CSIDL_MYVIDEO         );
}

static void initExecutablePath()
{
  char path[MAX_PATH];

  HMODULE module = GetModuleHandle(nullptr);
  int     length = GetModuleFileName(module, path, MAX_PATH);

  for (int i = 0; i < length; ++i) {
    path[i] = path[i] == '\\' ? '/' : path[i];
  }

  exePath = length == 0 || length == MAX_PATH ? String() : String(path, length);
}

#else

static void setSpecialDir(int dirId, const char* name, Map<String, String>* vars)
{
  const char* value = getenv(name);

  if (value == nullptr && vars != nullptr) {
    const String* defValue = vars->find(name);

    if (defValue == nullptr) {
      specialDirs[dirId] = "";
      return;
    }

    value = defValue->c();
  }

  specialDirs[dirId] = value;
}

static void loadXDGSettings(const File& file, Map<String, String>* vars)
{
  InputStream is = file.inputStream();

  String line;
  while (is.available() != 0) {
    line = is.readLine();

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
    String path = line.substring(firstQuote + 1, lastQuote);

    if (!name.beginsWith("XDG_") || !name.endsWith("_DIR")) {
      continue;
    }

    if (path.beginsWith("$HOME")) {
      path = File::HOME + path.substring(5);
    }

    vars->add(name, path);
  }
}

static void initSpecialDirs()
{
  setSpecialDir(0, "HOME", nullptr);

  if (File::HOME.isEmpty()) {
    OZ_ERROR("oz::File: Unable to determine home directory: HOME environment variable not set");
  }

  Map<String, String> vars;

  // Default locations.
  vars.add("XDG_CONFIG_HOME",   File::HOME + "/.config"     );
  vars.add("XDG_DATA_HOME",     File::HOME + "/.local/share");
  vars.add("XDG_DESKTOP_DIR",   File::HOME + "/Desktop"     );
  vars.add("XDG_DOCUMENTS_DIR", File::HOME + "/Documents"   );
  vars.add("XDG_DOWNLOAD_DIR",  File::HOME + "/Download"    );
  vars.add("XDG_MUSIC_DIR",     File::HOME + "/Music"       );
  vars.add("XDG_PICTURES_DIR",  File::HOME + "/Pictures"    );
  vars.add("XDG_VIDEOS_DIR",    File::HOME + "/Videos"      );

  // Override default locations with global settings, if exist.
  loadXDGSettings("/etc/xdg/user", &vars);

  // Override default locations with user settings, if exist.
  loadXDGSettings(*vars.find("XDG_CONFIG_HOME") + "/user-dirs.dirs", &vars);

  // Finally set special directories, environment variables override values from files.
  setSpecialDir(1, "XDG_CONFIG_HOME",   &vars);
  setSpecialDir(2, "XDG_DATA_HOME",     &vars);
  setSpecialDir(3, "XDG_DESKTOP_DIR",   &vars);
  setSpecialDir(4, "XDG_DOCUMENTS_DIR", &vars);
  setSpecialDir(5, "XDG_DOWNLOAD_DIR",  &vars);
  setSpecialDir(6, "XDG_MUSIC_DIR",     &vars);
  setSpecialDir(7, "XDG_PICTURES_DIR",  &vars);
  setSpecialDir(8, "XDG_VIDEOS_DIR",    &vars);
}

static void initExecutablePath()
{
  char pidPathBuffer[PATH_MAX];
  char exePathBuffer[PATH_MAX];

  pid_t pid = getpid();
  snprintf(pidPathBuffer, PATH_MAX, "/proc/%d/exe", pid);

  ptrdiff_t length = readlink(pidPathBuffer, exePathBuffer, PATH_MAX);
  exePath = length < 0 ? String() : String(exePathBuffer, int(length));
}

#endif

const String& File::HOME      = specialDirs[0];
const String& File::CONFIG    = specialDirs[1];
const String& File::DATA      = specialDirs[2];
const String& File::DESKTOP   = specialDirs[3];
const String& File::DOCUMENTS = specialDirs[4];
const String& File::DOWNLOAD  = specialDirs[5];
const String& File::MUSIC     = specialDirs[6];
const String& File::PICTURES  = specialDirs[7];
const String& File::VIDEOS    = specialDirs[8];

OZ_HIDDEN
File::File(const String& path, File::Type type, int size, long64 time) :
  filePath(path), fileType(type), fileSize(size), fileTime(time)
{}

File::File(const char* path) :
  filePath(path)
{
  stat();
}

File::File(const String& path) :
  filePath(path)
{
  stat();
}

File::~File()
{
  unmap();
}

File::File(const File& file) :
  filePath(file.filePath), fileType(file.fileType), fileSize(file.fileSize), fileTime(file.fileTime)
{}

File::File(File&& file) :
  filePath(static_cast<String&&>(file.filePath)), fileType(file.fileType), fileSize(file.fileSize),
  fileTime(file.fileTime), data(file.data)
{
  file.filePath = String::EMPTY;
  file.fileType = MISSING;
  file.fileSize = -1;
  file.fileTime = 0;
  file.data     = nullptr;
}

File& File::operator = (const File& file)
{
  if (&file != this) {
    unmap();

    filePath = file.filePath;
    fileType = file.fileType;
    fileSize = file.fileSize;
    fileTime = file.fileTime;
  }
  return *this;
}

File& File::operator = (File&& file)
{
  if (&file != this) {
    unmap();

    filePath = static_cast<String&&>(file.filePath);
    fileType = file.fileType;
    fileSize = file.fileSize;
    fileTime = file.fileTime;
    data     = file.data;

    file.filePath = String::EMPTY;
    file.fileType = MISSING;
    file.fileSize = -1;
    file.fileTime = 0;
    file.data     = nullptr;
  }
  return *this;
}

File& File::operator = (const char* path)
{
  unmap();

  filePath = path;
  stat();

  return *this;
}

File& File::operator = (const String& path)
{
  unmap();

  filePath = path;
  stat();

  return *this;
}

bool File::stat() const
{
  // Stat shouldn't be performed while the file is mapped. Changing fileSize may make a real mess
  // when unmapping file on Linux/Unix. True is returned since application should see the file as it
  // was when it has been mapped.
  if (data != nullptr) {
    return true;
  }

  fileType = MISSING;
  fileSize = -1;
  fileTime = 0;

  if (filePath.fileIsVirtual()) {
    if (PHYSFS_exists(&filePath[1])) {
      if (PHYSFS_isDirectory(&filePath[1])) {
        fileType = DIRECTORY;
        fileSize = -1;
        fileTime = PHYSFS_getLastModTime(&filePath[1]);
      }
      else {
        PHYSFS_File* file = PHYSFS_openRead(&filePath[1]);

        if (file == nullptr) {
          fileType = MISSING;
          fileSize = -1;
          fileTime = 0;
        }
        else {
          fileType = REGULAR;
          fileSize = int(PHYSFS_fileLength(file));
          fileTime = PHYSFS_getLastModTime(&filePath[1]);

          PHYSFS_close(file);
        }
      }
    }
  }
  else if (!filePath.isEmpty()) {
    struct stat info;

    if (::stat(filePath, &info) == 0) {
      if (S_ISREG(info.st_mode)) {
        fileType = REGULAR;
        fileSize = int(info.st_size);
        fileTime = max<long64>(info.st_ctime, info.st_mtime);
      }
      else if (S_ISDIR(info.st_mode)) {
        fileType = DIRECTORY;
        fileSize = -1;
        fileTime = max<long64>(info.st_ctime, info.st_mtime);
      }
    }
  }

  return fileType != MISSING;
}

String File::directory() const
{
  return filePath.fileDirectory();
}

String File::name() const
{
  return filePath.fileName();
}

String File::baseName() const
{
  return filePath.fileBaseName();
}

String File::extension() const
{
  return filePath.fileExtension();
}

bool File::hasExtension(const char* ext) const
{
  return filePath.fileHasExtension(ext);
}

String File::realDirectory() const
{
  if (filePath.fileIsVirtual()) {
    const char* realDir = PHYSFS_getRealDir(&filePath[1]);
    return realDir == nullptr ? "" : realDir;
  }
  else {
    return "";
  }
}

String File::realPath() const
{
  if (filePath.fileIsVirtual()) {
    const char* realDir = PHYSFS_getRealDir(&filePath[1]);
    realDir = realDir == nullptr ? "" : realDir;

    return String::format(String::last(realDir) == '/' ? "%s%s" : "%s/%s", realDir, &filePath[1]);
  }
  else {
    return filePath;
  }
}

bool File::read(char* buffer, int* size) const
{
  if (fileSize <= 0) {
    *size = 0;
    return fileSize == 0;
  }

  if (filePath.fileIsVirtual()) {
    PHYSFS_File* file = PHYSFS_openRead(&filePath[1]);
    if (file == nullptr) {
      *size = 0;
      return false;
    }

    int result = int(PHYSFS_read(file, buffer, 1, *size));
    PHYSFS_close(file);

    *size = result;
    return true;
  }
  else if (data != nullptr) {
    *size = min<int>(*size, fileSize);
    memcpy(buffer, data, *size);
    return true;
  }
  else {
#ifdef _WIN32
    int fd = open(filePath, O_RDONLY | O_BINARY);
#else
    int fd = open(filePath, O_RDONLY);
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

bool File::read(OutputStream* os) const
{
  int   size   = fileSize;
  char* buffer = os->skip(fileSize);
  bool  result = read(buffer, &size);

  os->seek(os->tell() - fileSize + size);
  return result;
}

Buffer File::read() const
{
  Buffer buffer;

  if (fileSize <= 0) {
    return buffer;
  }

  int size = fileSize;
  buffer.resize(size, true);

  read(buffer.begin(), &size);

  if (size != fileSize) {
    buffer.resize(size);
  }
  return buffer;
}

String File::readString() const
{
  InputStream is = inputStream();
  return String(is.begin(), is.available());
}

bool File::write(const char* buffer, int size) const
{
  if (filePath.fileIsVirtual()) {
    PHYSFS_File* file = PHYSFS_openWrite(&filePath[1]);
    if (file == nullptr) {
      return false;
    }

    int result = int(PHYSFS_write(file, buffer, 1, size));
    PHYSFS_close(file);
    stat();

    return result == size;
  }
  else if (data != nullptr) {
    OZ_ERROR("oz::File: Writing to a memory mapped file '%s'", filePath.c());
  }
  else {
#ifdef _WIN32
    int fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0644);
#else
    int fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
#endif
    if (fd < 0) {
      return false;
    }

    int result = int(::write(fd, buffer, size));
    close(fd);
    stat();

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

bool File::map() const
{
  if (fileSize <= 0) {
    return false;
  }
  if (data != nullptr) {
    return true;
  }

  if (filePath.fileIsVirtual()) {
    int size = fileSize;

    data = new char[size];
    read(data, &size);

    if (size != fileSize) {
      delete[] data;
      data = nullptr;
    }
    return true;
  }
  else {
#ifdef _WIN32

    HANDLE file = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == nullptr) {
      return false;
    }

    HANDLE mapping = CreateFileMapping(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (mapping == nullptr) {
      CloseHandle(file);
      return false;
    }

    data = static_cast<char*>(MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0));

    CloseHandle(mapping);
    CloseHandle(file);

    return data != nullptr;

#else

    int fd = open(filePath, O_RDONLY);
    if (fd < 0) {
      return false;
    }

    data = static_cast<char*>(mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, fd, 0));
    data = data == MAP_FAILED ? nullptr : data;

    close(fd);
    return data != nullptr;

#endif
  }
}

void File::unmap() const
{
  if (data == nullptr) {
    return;
  }

  if (filePath.fileIsVirtual()) {
    delete[] data;
  }
  else {
#ifdef _WIN32
    UnmapViewOfFile(data);
#else
    munmap(data, fileSize);
#endif
  }
  data = nullptr;
}

InputStream File::inputStream(Endian::Order order) const
{
  if (data == nullptr) {
    map();

    if (data == nullptr) {
      return InputStream(nullptr, nullptr, order);
    }
  }

  return InputStream(data, data + fileSize, order);
}

List<File> File::ls(const char* extension) const
{
  List<File> list;

  if (fileType != DIRECTORY) {
    return list;
  }

  if (filePath.fileIsVirtual()) {
    char** entities = PHYSFS_enumerateFiles(&filePath[1]);
    if (entities == nullptr) {
      return list;
    }

    String prefix = filePath.length() == 1 || filePath.last() == '/' ? filePath : filePath + "/";

    for (char** entity = entities; *entity != nullptr; ++entity) {
      if ((*entity)[0] != '.' &&
          (extension == nullptr || String::fileHasExtension(*entity, extension)))
      {
        list.add(prefix + *entity);
      }
    }

    PHYSFS_freeList(entities);
  }
  else {
    DIR* directory = opendir(filePath);
    if (directory == nullptr) {
      return list;
    }

    String  prefix = filePath.last() == '/' ? filePath : filePath + "/";
    // readlink() implementation is "usually" (required by POSIX) thread-safe as long as threads
    // don't access the same directory stream. Since opendir() is called for each thread separately
    // in our case, use of readdir() is safe here. This way we avoid readdir_r()-related issues with
    // portability and entity buffer size.
    dirent* entity = readdir(directory);

    while (entity != nullptr) {
      if (entity->d_name[0] != '.' &&
          (extension == nullptr || String::fileHasExtension(entity->d_name, extension)))
      {
        list.add(prefix + entity->d_name);
      }
      entity = readdir(directory);
    }

    closedir(directory);
  }

  list.sort();
  return list;
}

String File::cwd()
{
  char buffer[PATH_MAX];
  bool hasFailed = getcwd(buffer, PATH_MAX) == nullptr;
  return hasFailed ? "" : buffer;
}

bool File::chdir(const char* path)
{
  return ::chdir(path) == 0;
}

bool File::mkdir(const char* path)
{
  if (String::fileIsVirtual(path)) {
    return PHYSFS_mkdir(&path[1]) != 0;
  }
  else {
#ifdef _WIN32
    return ::mkdir(path) == 0;
#else
    return ::mkdir(path, 0755) == 0;
#endif
  }
}

bool File::cp(const File& src, const File& dest_)
{
  File dest = dest_;
  if (dest.type() == DIRECTORY) {
    dest = dest.path() + "/" + src.name();
  }

  InputStream is = src.inputStream();
  return is.available() == 0 ? false : dest.write(is.begin(), is.available());
}

bool File::mv(const File& src, const File& dest_)
{
  if (src.isVirtual()) {
    return false;
  }

  File dest = dest_;
  if (dest.type() == DIRECTORY) {
    dest = dest.path() + "/" + src.name();
  }

  return rename(src.path(), dest.path()) == 0;
}

bool File::rm(const char* path)
{
  if (String::fileIsVirtual(path)) {
    return PHYSFS_delete(&path[1]);
  }
  else {
    return remove(path) == 0;
  }
}

bool File::mount(const char* path, const char* mountPoint, bool append)
{
  return PHYSFS_mount(path, mountPoint, append) != 0;
}

bool File::mountLocal(const char* path, bool append)
{
  if (PHYSFS_setWriteDir(path) == 0) {
    return false;
  }
  if (PHYSFS_mount(path, nullptr, append) == 0) {
    PHYSFS_setWriteDir(nullptr);
    return false;
  }
  return true;
}

const String& File::executablePath()
{
  return exePath;
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

  Arrays::fill<String, String>(specialDirs, Arrays::length<String>(specialDirs), String::EMPTY);
  exePath = "";
}

}
