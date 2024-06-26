/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include "Log.hh"

#include "Profiler.hh"

#include <cstdio>

#ifdef __GLIBC__
# include <execinfo.h>
# include <unistd.h>
#endif

#define OZ_VAARGS_BUFFER(buffer) \
  va_list ap; \
  va_start(ap, s); \
  char buffer[OUT_BUFFER_SIZE]; \
  vsnprintf(buffer, OUT_BUFFER_SIZE, s, ap); \
  va_end(ap);

#define OZ_PRINT_BOTH(code) \
  { \
    auto lambda = [&](FILE* stream) { code }; \
    if (!verboseMode || showVerbose || logFileStream == nullptr) { lambda(stdout); } \
    if (logFileStream != nullptr) { lambda(logFileStream); } \
  }

namespace oz
{

namespace
{

constexpr int  OUT_BUFFER_SIZE      = 4096;
constexpr int  INDENT_SPACES        = 2;
constexpr char INDENT_BUFFER[49]    = "                                                ";
constexpr int  INDENT_BUFFER_LENGTH = sizeof(INDENT_BUFFER) - 1;

File  logFile;
FILE* logFileStream = nullptr;
int   indentLevel   = 0;

inline const char* getIndent()
{
  OZ_ASSERT(indentLevel >= 0);

  int bias = max<int>(INDENT_BUFFER_LENGTH - indentLevel * INDENT_SPACES, 0);
  return &INDENT_BUFFER[bias];
}

}

bool Log::showVerbose = false;
bool Log::verboseMode = false;

Log::Log()
{
  print();
}

Log::~Log()
{
  println();
}

const File& Log::file()
{
  return logFile;
}

void Log::resetIndent()
{
  indentLevel = 0;
}

void Log::indent()
{
  ++indentLevel;
}

void Log::unindent()
{
  if (indentLevel > 0) {
    --indentLevel;
  }
}

void Log::putsRaw(const char* s)
{
  OZ_PRINT_BOTH(
    fputs(s, stream);
  )
}

void Log::vprintRaw(const char* s, va_list ap)
{
  char buffer[OUT_BUFFER_SIZE];
  vsnprintf(buffer, OUT_BUFFER_SIZE, s, ap);

  putsRaw(buffer);
}

void Log::printRaw(const char* s, ...)
{
  OZ_VAARGS_BUFFER(buffer)

  putsRaw(buffer);
}

void Log::print(const char* s, ...)
{
  OZ_VAARGS_BUFFER(buffer)

  const char* indent = getIndent();

  OZ_PRINT_BOTH(
    fputs(indent, stream);
    fputs(buffer, stream);
  )
}

void Log::print()
{
  const char* indent = getIndent();

  OZ_PRINT_BOTH(
    fputs(indent, stream);
  )
}

void Log::printEnd(const char* s, ...)
{
  OZ_VAARGS_BUFFER(buffer)

  OZ_PRINT_BOTH(
    fputs(buffer, stream);
    fputc('\n', stream);
    fflush(stream);
  )
}

void Log::println(const char* s, ...)
{
  OZ_VAARGS_BUFFER(buffer)

  const char* indent = getIndent();

  OZ_PRINT_BOTH(
    fputs(indent, stream);
    fputs(buffer, stream);
    fputc('\n', stream);
    fflush(stream);
  )
}

void Log::println()
{
  OZ_PRINT_BOTH(
    fputc('\n', stream);
    fflush(stream);
  )
}

void Log::printTrace(const StackTrace& st)
{
  printEnd("  on %s", String::isEmpty(st.thread) ? "?" : st.thread);

#ifdef __GLIBC__

  printEnd("  stack trace:");

  for (int i = 0; i < st.nFrames; ++i) {
    write(STDOUT_FILENO, "    ", 4);
    backtrace_symbols_fd(&st.frames[i], 1, STDOUT_FILENO);
  }

  if (logFileStream != nullptr) {
    int fd = fileno(logFileStream);

    for (int i = 0; i < st.nFrames; ++i) {
      write(fd, "    ", 4);
      backtrace_symbols_fd(&st.frames[i], 1, fd);
    }
  }

#endif
}

void Log::printProfilerStatistics()
{
  auto range = Profiler::crange();

  if (range.begin() != range.end()) {
    println("Profiler statistics {");
    indent();

    for (const auto& i : range) {
      println("%.6f s\t %s", double(i.value.ns()) / 1e6, i.key.c());
    }

    unindent();
    println("}");
  }
}

bool Log::init(const File& file, bool clearFile)
{
  destroy();

  logFile     = file;
  indentLevel = 0;

  if (!file.isEmpty()) {
    logFileStream = fopen(file, clearFile ? "w" : "a");
  }
  return logFile.isEmpty();
}

void Log::destroy()
{
  if (!logFile.isEmpty()) {
    fclose(logFileStream);

    logFileStream = nullptr;
    logFile = "";
  }
}

const Log& Log::operator<<(bool b) const
{
  printRaw(b ? "true" : "false");
  return *this;
}

const Log& Log::operator<<(char c) const
{
  printRaw("%c", c);
  return *this;
}

const Log& Log::operator<<(byte b) const
{
  printRaw("%d", b);
  return *this;
}

const Log& Log::operator<<(ubyte b) const
{
  printRaw("%u", b);
  return *this;
}

const Log& Log::operator<<(int16 s) const
{
  printRaw("%hd", s);
  return *this;
}

const Log& Log::operator<<(uint16 s) const
{
  printRaw("%hu", s);
  return *this;
}

const Log& Log::operator<<(int i) const
{
  printRaw("%d", i);
  return *this;
}

const Log& Log::operator<<(uint i) const
{
  printRaw("%u", i);
  return *this;
}

const Log& Log::operator<<(long l) const
{
  printRaw("%li", l);
  return *this;
}

const Log& Log::operator<<(ulong l) const
{
  printRaw("%lu", l);
  return *this;
}

const Log& Log::operator<<(int64 l) const
{
  printRaw("%lld", l);
  return *this;
}

const Log& Log::operator<<(uint64 l) const
{
  printRaw("%llu", l);
  return *this;
}

const Log& Log::operator<<(float f) const
{
  printRaw("%g", f);
  return *this;
}

const Log& Log::operator<<(double d) const
{
  printRaw("%g", d);
  return *this;
}

const Log& Log::operator<<(volatile const void* p) const
{
  printRaw("%p", p);
  return *this;
}

const Log& Log::operator<<(const char* s) const
{
  printRaw("%s", s);
  return *this;
}

const Log& Log::operator<<(const String& s) const
{
  printRaw("%s", s.c());
  return *this;
}

const Log& Log::operator<<(const Vec3& v) const
{
  printRaw("(%g %g %g)", v.x, v.y, v.z);
  return *this;
}

const Log& Log::operator<<(const Point& p) const
{
  printRaw("[%g %g %g]", p.x, p.y, p.z);
  return *this;
}

const Log& Log::operator<<(const Plane& p) const
{
  printRaw("(%g %g %g; %g)", p.n.x, p.n.y, p.n.z, p.d);
  return *this;
}

const Log& Log::operator<<(const Vec4& v) const
{
  printRaw("(%g %g %g %g)", v.x, v.y, v.z, v.w);
  return *this;
}

const Log& Log::operator<<(const Quat& q) const
{
  printRaw("[%g %g %g %g]", q.x, q.y, q.z, q.w);
  return *this;
}

const Log& Log::operator<<(const Mat3& m) const
{
  printRaw("[%g %g %g; %g %g %g; %g %g %g]",
           m.x.x, m.x.y, m.x.z,
           m.y.x, m.y.y, m.y.z,
           m.z.x, m.z.y, m.z.z);
  return *this;
}

const Log& Log::operator<<(const Mat4& m) const
{
  printRaw("[%g %g %g %g; %g %g %g %g; %g %g %g %g; %g %g %g %g]",
           m.x.x, m.x.y, m.x.z, m.x.w,
           m.y.x, m.y.y, m.y.z, m.y.w,
           m.z.x, m.z.y, m.z.z, m.z.w,
           m.w.x, m.w.y, m.w.z, m.w.w);
  return *this;
}

const Log& Log::operator<<(const Stream& is) const
{
  OZ_PRINT_BOTH(
    fwrite(is.begin(), 1, is.capacity(), stream);
  )
  return *this;
}

const Log& Log::operator<<(const Time& time) const
{
  printRaw("%s", time.toString().c());
  return *this;
}

const Log& Log::operator<<(const Json& json) const
{
  printRaw("%s", json.toString().c());
  return *this;
}

}
