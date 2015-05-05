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
 * @file ozCore/Log.cc
 */

#include "Log.hh"

#include "Alloc.hh"
#include "Profiler.hh"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define OZ_VAARGS_BUFFER(buffer) \
  char buffer[OUT_BUFFER_SIZE]; \
  va_list ap; \
  va_start(ap, s); \
  vsnprintf(buffer, OUT_BUFFER_SIZE, s, ap); \
  va_end(ap);

#define OZ_PRINT_BOTH(code) \
  { \
    auto lambda = [&](FILE* stream) code; \
    if (!verboseMode || showVerbose || logFile == nullptr) { lambda(stdout); } \
    if (logFile != nullptr) { lambda(logFile); } \
  }

namespace oz
{

static const int         OUT_BUFFER_SIZE      = 4096;
static const int         INDENT_SPACES        = 2;
static const char        INDENT_BUFFER[49]    = "                                                ";
static const int         INDENT_BUFFER_LENGTH = sizeof(INDENT_BUFFER) - 1;
static const char* const SIGNALS[][2]         =
{
  { "SIG???",    "[invalid signal number]"    },
  { "SIGHUP",    "Hangup"                     }, //  1
  { "SIGINT",    "Interrupt"                  }, //  2
  { "SIGQUIT",   "Quit"                       }, //  3
  { "SIGILL",    "Illegal instruction"        }, //  4
  { "SIGTRAP",   "Trace trap"                 }, //  5
  { "SIGABRT",   "Abort"                      }, //  6
  { "SIGBUS",    "BUS error"                  }, //  7
  { "SIGFPE",    "Floating-point exception"   }, //  8
  { "SIGKILL",   "Kill, unblockable"          }, //  9
  { "SIGUSR1",   "User-defined signal 1"      }, // 10
  { "SIGSEGV",   "Segmentation violation"     }, // 11
  { "SIGUSR2",   "User-defined signal 2"      }, // 12
  { "SIGPIPE",   "Broken pipe"                }, // 13
  { "SIGALRM",   "Alarm clock"                }, // 14
  { "SIGTERM",   "Termination"                }, // 15
  { "SIGSTKFLT", "Stack fault"                }, // 16
  { "SIGCHLD",   "Child status has changed"   }, // 17
  { "SIGCONT",   "Continue"                   }, // 18
  { "SIGSTOP",   "Stop, unblockable"          }, // 19
  { "SIGTSTP",   "Keyboard stop"              }, // 20
  { "SIGTTIN",   "Background read from tty"   }, // 21
  { "SIGTTOU",   "Background write to tty"    }, // 22
  { "SIGURG",    "Urgent condition on socket" }, // 23
  { "SIGXCPU",   "CPU limit exceeded"         }, // 24
  { "SIGXFSZ",   "File size limit exceeded"   }, // 25
  { "SIGVTALRM", "Virtual alarm clock"        }, // 26
  { "SIGPROF",   "Profiling alarm clock"      }, // 27
  { "SIGWINCH",  "Window size change"         }, // 28
  { "SIGIO",     "I/O now possible"           }, // 29
  { "SIGPWR",    "Power failure restart"      }, // 30
  { "SIGSYS",    "Bad system call"            }  // 31
};

static char  path[256]   = "";
static FILE* logFile     = nullptr;
static int   indentLevel = 0;

bool Log::showVerbose    = false;
bool Log::verboseMode    = false;

static inline const char* getIndent()
{
  hard_assert(indentLevel >= 0);

  int bias = max<int>(INDENT_BUFFER_LENGTH - indentLevel * INDENT_SPACES, 0);
  return &INDENT_BUFFER[bias];
}

Log::~Log()
{
  printRaw("\n");
}

const char* Log::filePath()
{
  return path;
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
  OZ_PRINT_BOTH({
    fputs(s, stream);
    fflush(stream);
  });
}

void Log::vprintRaw(const char* s, va_list ap)
{
  char buffer[OUT_BUFFER_SIZE];
  vsnprintf(buffer, OUT_BUFFER_SIZE, s, ap);

  OZ_PRINT_BOTH({
    fputs(buffer, stream);
    fflush(stream);
  });
}

void Log::printRaw(const char* s, ...)
{
  OZ_VAARGS_BUFFER(buffer);

  OZ_PRINT_BOTH({
    fputs(buffer, stream);
    fflush(stream);
  });
}

void Log::print(const char* s, ...)
{
  OZ_VAARGS_BUFFER(buffer);

  const char* indent = getIndent();

  OZ_PRINT_BOTH({
    fputs(indent, stream);
    fputs(buffer, stream);
    fflush(stream);
  });
}

void Log::printEnd(const char* s, ...)
{
  OZ_VAARGS_BUFFER(buffer);

  OZ_PRINT_BOTH({
    fputs(buffer, stream);
    fputc('\n', stream);
    fflush(stream);
  });
}

void Log::printEnd()
{
  OZ_PRINT_BOTH({
    fputc('\n', stream);
    fflush(stream);
  });
}

void Log::println(const char* s, ...)
{
  OZ_VAARGS_BUFFER(buffer);

  const char* indent = getIndent();

  OZ_PRINT_BOTH({
    fputs(indent, stream);
    fputs(buffer, stream);
    fputc('\n', stream);
    fflush(stream);
  });
}

void Log::println()
{
  OZ_PRINT_BOTH({
    fputc('\n', stream);
    fflush(stream);
  });
}

void Log::printTrace(const StackTrace& st)
{
  const char* threadName = String::isEmpty(st.threadName) ? "?" : st.threadName;

  OZ_PRINT_BOTH({
    fputs("  thread: ", stream);
    fputs(threadName, stream);
    fputs("\n  stack trace:\n", stream);
  });

  if (st.nFrames == 0) {
    OZ_PRINT_BOTH({
      fputs("    [no stack trace]\n", stream);
    });
  }
  else {
    char** entries = st.symbols();

    for (int i = 0; i < st.nFrames; ++i) {
      OZ_PRINT_BOTH({
        fputs("    ", stream);
        fputs(entries[i], stream);
        fputc('\n', stream);
      });
    }

    free(entries);
  }

  OZ_PRINT_BOTH({
    fflush(stream);
  });
}

void Log::printSignal(int sigNum)
{
  char buffer[OUT_BUFFER_SIZE];
  int  index = uint(sigNum) >= uint(Arrays::length(SIGNALS)) ? 0 : sigNum;

  snprintf(buffer, OUT_BUFFER_SIZE, "\n\nSignal %d %s (%s)\n",
           sigNum, SIGNALS[index][0], SIGNALS[index][1]);

  OZ_PRINT_BOTH({
    fputs(buffer, stream);
    fflush(stream);
  });
}

bool Log::printMemorySummary()
{
  if (Alloc::count == 0) {
    return false;
  }

  println("Alloc summary {");
  indent();

  println("current chunks    %7d", Alloc::count);
  println("current amount    %7.2f MiB (%lu B)",
          float(Alloc::amount) / (1024.0f * 1024.0f), ulong(Alloc::amount));
  println("maximum chunks    %7d", Alloc::maxCount);
  println("maximum amount    %7.2f MiB (%lu B)",
          float(Alloc::maxAmount) / (1024.0f * 1024.0f), ulong(Alloc::maxAmount));
  println("cumulative chunks %7d", Alloc::sumCount);
  println("cumulative amount %7.2f MiB (%lu B)",
          float(Alloc::sumAmount) / (1024.0f * 1024.0f), ulong(Alloc::sumAmount));

  unindent();
  println("}");

  return true;
}

bool Log::printMemoryLeaks()
{
  bool hasOutput = false;

  for (const auto& ci : Alloc::objectCIter()) {
    println("Leaked object at %p of size %lu B allocated",
            static_cast<const void*>(&ci), ulong(ci.size));
    indent();
    printTrace(ci.stackTrace);
    unindent();

    hasOutput = true;
  }

  for (const auto& ci : Alloc::arrayCIter()) {
    println("Leaked array at %p of size %lu B allocated",
            static_cast<const void*>(&ci), ulong(ci.size));
    indent();
    printTrace(ci.stackTrace);
    unindent();

    hasOutput = true;
  }

  return hasOutput;
}

void Log::printProfilerStatistics()
{
  if (Profiler::citerator().isValid()) {
    println("Profiler statistics {");
    indent();

    for (const auto& i : Profiler::citerator()) {
      println("%.6f s\t %s", double(i.value) / 1e6, i.key.cstr());
    }

    unindent();
    println("}");
  }
}

bool Log::init(const char* filePath, bool clearFile)
{
  destroy();

  indentLevel = 0;

#if defined(__ANDROID__) || defined(__native_client__)

  static_cast<void>(filePath);
  static_cast<void>(clearFile);

  return false;

#else

  if (filePath != nullptr) {
    strncpy(path, filePath, 255);
    path[255] = '\0';
  }

  if (path[0] != '\0') {
    logFile = fopen(path, clearFile ? "w" : "a");
  }
  return logFile != nullptr;

#endif
}

void Log::destroy()
{
#if !defined(__ANDROID__) && !defined(__native_client__)

  if (logFile != nullptr) {
    fclose(logFile);
    logFile = nullptr;
  }

  path[0] = '\0';

#endif
}

const Log& Log::operator << (bool b) const
{
  putsRaw(b ? "true" : "false");
  return *this;
}

const Log& Log::operator << (char c) const
{
  printRaw("%c", c);
  return *this;
}

const Log& Log::operator << (byte b) const
{
  printRaw("%d", b);
  return *this;
}

const Log& Log::operator << (ubyte b) const
{
  printRaw("%u", b);
  return *this;
}

const Log& Log::operator << (short s) const
{
  printRaw("%hd", s);
  return *this;
}

const Log& Log::operator << (ushort s) const
{
  printRaw("%hu", s);
  return *this;
}

const Log& Log::operator << (int i) const
{
  printRaw("%d", i);
  return *this;
}

const Log& Log::operator << (uint i) const
{
  printRaw("%u", i);
  return *this;
}

const Log& Log::operator << (long l) const
{
  printRaw("%ld", l);
  return *this;
}

const Log& Log::operator << (ulong l) const
{
  printRaw("%lu", l);
  return *this;
}

const Log& Log::operator << (long64 l) const
{
#ifdef _WIN32
  printRaw("%ld", long(l));
#else
  printRaw("%lld", l);
#endif
  return *this;
}

const Log& Log::operator << (ulong64 l) const
{
#ifdef _WIN32
  printRaw("%lu", ulong(l));
#else
  printRaw("%llu", l);
#endif
  return *this;
}

const Log& Log::operator << (float f) const
{
  printRaw("%g", f);
  return *this;
}

const Log& Log::operator << (double d) const
{
  printRaw("%g", d);
  return *this;
}

const Log& Log::operator << (const String& s) const
{
  putsRaw(s);
  return *this;
}

const Log& Log::operator << (const char* s) const
{
  putsRaw(s);
  return *this;
}

const Log& Log::operator << (const Vec3& v) const
{
  printRaw("(%g %g %g)", v.x, v.y, v.z);
  return *this;
}

const Log& Log::operator << (const Vec4& v) const
{
  printRaw("(%g %g %g %g)", v.x, v.y, v.z, v.w);
  return *this;
}

const Log& Log::operator << (const Point& p) const
{
  printRaw("[%g %g %g]", p.x, p.y, p.z);
  return *this;
}

const Log& Log::operator << (const Plane& p) const
{
  printRaw("(%g %g %g; %g)", p.n.x, p.n.y, p.n.z, p.d);
  return *this;
}

const Log& Log::operator << (const Quat& q) const
{
  printRaw("[%g %g %g %g]", q.x, q.y, q.z, q.w);
  return *this;
}

const Log& Log::operator << (const Mat3& m) const
{
  printRaw("[%g %g %g; %g %g %g; %g %g %g]",
           m.x.x, m.x.y, m.x.z,
           m.y.x, m.y.y, m.y.z,
           m.z.x, m.z.y, m.z.z);
  return *this;
}

const Log& Log::operator << (const Mat4& m) const
{
  printRaw("[%g %g %g %g; %g %g %g %g; %g %g %g %g; %g %g %g %g]",
           m.x.x, m.x.y, m.x.z, m.x.w,
           m.y.x, m.y.y, m.y.z, m.y.w,
           m.z.x, m.z.y, m.z.z, m.z.w,
           m.w.x, m.w.y, m.w.z, m.w.w);
  return *this;
}

const Log& Log::operator << (const InputStream& is) const
{
  OZ_PRINT_BOTH({
    fwrite(is.begin(), 1, is.tell(), stream);
    fputc('\n', stream);
    fflush(stream);
  });
  return *this;
}

const Log& Log::operator << (const Buffer& buffer) const
{
  OZ_PRINT_BOTH({
    fwrite(buffer.begin(), 1, buffer.length(), stream);
    fputc('\n', stream);
    fflush(stream);
  });
  return *this;
}

const Log& Log::operator << (const File& file) const
{
  printRaw("%s", file.path().cstr());
  return *this;
}

const Log& Log::operator << (const Time& time) const
{
  printRaw("%s", time.toString().cstr());
  return *this;
}

const Log& Log::operator << (const Json& json) const
{
  printRaw("%s", json.toString().cstr());
  return *this;
}

const Log& Log::operator << (volatile const void* p) const
{
  printRaw("%p", p);
  return *this;
}

}
