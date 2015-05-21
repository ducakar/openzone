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
 * @file ozCore/String.cc
 */

#include "String.hh"

#include "Math.hh"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <clocale>

namespace oz
{

static const int LOCAL_BUFFER_SIZE = 4096;

const String String::EMPTY;

OZ_INTERNAL
String::String(const char* s, int sLength, const char* t, int tLength)
{
  char* begin = resize(sLength + tLength, false);

  memcpy(begin, s, sLength);
  memcpy(begin + sLength, t, tLength + 1);
}

OZ_INTERNAL
char* String::resize(int newCount, bool keepContents)
{
  hard_assert(count >= 0 && newCount >= 0);
  hard_assert(!keepContents || newCount > count);

  if (newCount < BUFFER_SIZE) {
    if (count >= BUFFER_SIZE) {
      delete[] buffer ;
    }

    count = newCount;
    return baseBuffer;
  }

  if (newCount != count) {
    char* oldBuffer = data();
    char* newBuffer = new char[newCount + 1];

    if (keepContents) {
      memcpy(newBuffer, oldBuffer, min<int>(count, newCount) + 1);
    }
    if (count >= BUFFER_SIZE) {
      delete[] buffer;
    }

    buffer = newBuffer;
    count  = newCount;
  }

  return buffer;
}

int String::index(const char* s, char ch, int start)
{
  for (int i = start; s[i] != '\0'; ++i) {
    if (s[i] == ch) {
      return i;
    }
  }
  return -1;
}

int String::lastIndex(const char* s, char ch, int end)
{
  for (int i = end - 1; i >= 0; --i) {
    if (s[i] == ch) {
      return i;
    }
  }
  return -1;
}

int String::lastIndex(const char* s, char ch)
{
  int last = -1;

  for (int i = 0; s[i] != '\0'; ++i) {
    if (s[i] == ch) {
      last = i;
    }
  }
  return last;
}

const char* String::find(const char* s, char ch, int start)
{
  return strchr(s + start, ch);
}

const char* String::findLast(const char* s, char ch, int end)
{
#ifndef _GNU_SOURCE
  static_cast<void>(end);

  return strrchr(s, ch);
#else
  return static_cast<const char*>(memrchr(s, ch, end));
#endif
}

const char* String::findLast(const char* s, char ch)
{
  return strrchr(s, ch);
}

bool String::beginsWith(const char* s, const char* sub)
{
  while (*sub != '\0' && *sub == *s) {
    ++sub;
    ++s;
  }
  return *sub == '\0';
}

bool String::endsWith(const char* s, const char* sub)
{
  int len    = length(s);
  int subLen = length(sub);

  if (subLen > len) {
    return false;
  }

  const char* end    = s   + len    - 1;
  const char* subEnd = sub + subLen - 1;

  while (subEnd >= sub && *subEnd == *end) {
    --subEnd;
    --end;
  }
  return subEnd < sub;
}

String String::substring(const char* s, int start)
{
  hard_assert(0 <= start && start <= length(s));

  return String(s + start, length(s) - start);
}

String String::substring(const char* s, int start, int end)
{
  hard_assert(0 <= start && start <= end && end <= length(s));

  return String(s + start, end - start);
}

String String::trim(const char* s)
{
  int         count = length(s);
  const char* begin = s;
  const char* end   = s + count;

  while (begin < end && isBlank(*begin)) {
    ++begin;
  }
  while (begin < end && isBlank(*(end - 1))) {
    --end;
  }

  return String(begin, int(end - begin));
}

String String::replace(const char* s, char whatChar, char withChar)
{
  String r;

  int         count  = length(s);
  const char* oBegin = s;
  char*       rBegin = r.resize(count, false);

  for (int i = 0; i < count; ++i) {
    rBegin[i] = oBegin[i] == whatChar ? withChar : oBegin[i];
  }
  rBegin[count] = '\0';

  return r;
}

List<String> String::split(const char* s, char delimiter)
{
  List<String> list;

  int begin = 0;
  int end   = index(s, delimiter);

  // Count substrings first.
  while (end >= 0) {
    list.add(substring(s, begin, end));

    begin = end + 1;
    end   = index(s, delimiter, begin);
  }
  list.add(substring(s, begin));

  return list;
}

String String::fileDirectory(const char* s)
{
  int slash = lastIndex(s, '/');

  return slash >= 0 ? substring(s, 0, slash) : String();
}

String String::fileName(const char* s)
{
  int slash = lastIndex(s, '/');

  return slash >= 0 ? substring(s, slash + 1) : substring(s, fileIsVirtual(s));
}

String String::fileBaseName(const char* s)
{
  int begin = max<int>(lastIndex(s, '/') + 1, fileIsVirtual(s));
  int dot   = lastIndex(s, '.');

  return begin <= dot ? substring(s, begin, dot) : substring(s, begin);
}

String String::fileExtension(const char* s)
{
  int slash = lastIndex(s, '/');
  int dot   = lastIndex(s, '.');

  return slash < dot ? substring(s, dot + 1) : String();
}

bool String::fileHasExtension(const char* s, const char* ext)
{
  const char* slash = findLast(s, '/');
  const char* dot   = findLast(s, '.');

  if (slash < dot) {
    return compare(dot + 1, ext) == 0;
  }
  else {
    return isEmpty(ext);
  }
}

bool String::parseBool(const char* s, const char** end)
{
  if (s[0] == 't' && s[1] == 'r' && s[2] == 'u' && s[3] == 'e') {
    if (end != nullptr) {
      *end = s + 4;
    }
    return true;
  }
  else {
    if (end != nullptr) {
      if (s[0] == 'f' && s[1] == 'a' && s[2] == 'l' && s[3] == 's' && s[4] == 'e') {
        *end = s + 5;
      }
      else {
        *end = s;
      }
    }
    return false;
  }
}

int String::parseInt(const char* s, const char** end, int base)
{
  return int(strtol(s, const_cast<char**>(end), base));
}

double String::parseDouble(const char* s, const char** end)
{
  return strtod(s, const_cast<char**>(end));
}

String::String(const char* s, int nChars)
{
  char* begin = resize(nChars, false);

  memcpy(begin, s, nChars);
  begin[count] = '\0';
}

String::String(const char* s, const char* t) :
  String(s, length(s), t, length(t))
{}

String::String(bool b) :
  String(b ? "true" : "false", 5)
{}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"

String::String(int i, const char* format)
{
  count = snprintf(baseBuffer, BUFFER_SIZE, format, i);
}

String::String(double d, const char* format)
{
  count = snprintf(baseBuffer, BUFFER_SIZE, format, d);
}

#pragma GCC diagnostic pop

String::~String()
{
  if (count >= BUFFER_SIZE) {
    delete[] buffer;
  }
}

String::String(const String& s) :
  String(s.data(), s.count)
{}

String::String(String&& s) :
  count(s.count)
{
  memcpy(this, &s, sizeof(String));

  s.count         = 0;
  s.baseBuffer[0] = '\0';
}

String& String::operator = (const String& s)
{
  if (&s != this) {
    char* begin = resize(s.count, false);

    memcpy(begin, s.data(), count + 1);
  }
  return *this;
}

String& String::operator = (String&& s)
{
  if (&s != this) {
    if (count >= BUFFER_SIZE) {
      delete[] buffer;
    }

    memcpy(this, &s, sizeof(String));

    s.count         = 0;
    s.baseBuffer[0] = '\0';
  }
  return *this;
}

String& String::operator = (const char* s)
{
  if (s != data()) {
    int   nChars = length(s);
    char* begin  = resize(nChars, false);

    memcpy(begin, s, nChars + 1);
  }
  return *this;
}

String String::format(const char* s, ...)
{
  va_list ap;
  va_start(ap, s);

  char localBuffer[LOCAL_BUFFER_SIZE];
  int  length = vsnprintf(localBuffer, LOCAL_BUFFER_SIZE, s, ap);

  va_end(ap);

  return String(localBuffer, length);
}

String String::si(double e, const char* format)
{
  char prefixes[] = "m kMG";

  int nGroups = 0;
  if (e < 1.0) {
    e *= 1000.0;
    --nGroups;
  }
  else {
    for (; nGroups < 3 && e >= 1000.0; ++nGroups) {
      e /= 1000.0;
    }
  }

  char suffix[] = " \0";
  if (nGroups != 0) {
    suffix[1] = prefixes[nGroups + 1];
  }

  return String(e, format) + suffix;
}

int String::index(char ch, int start) const
{
  return index(data(), ch, start);
}

int String::lastIndex(char ch, int end) const
{
  return lastIndex(data(), ch, end);
}

int String::lastIndex(char ch) const
{
  return lastIndex(data(), ch, count);
}

const char* String::find(char ch, int start) const
{
  return find(data(), ch, start);
}

const char* String::findLast(char ch, int end) const
{
  return findLast(data(), ch, end);
}

const char* String::findLast(char ch) const
{
  return findLast(data(), ch, count);
}

bool String::beginsWith(const char* sub) const
{
  return beginsWith(data(), sub);
}

bool String::endsWith(const char* sub) const
{
  int subLen = length(sub);

  if (subLen > count) {
    return false;
  }

  const char* end    = data() + count  - 1;
  const char* subEnd = sub    + subLen - 1;

  while (subEnd >= sub && *subEnd == *end) {
    --subEnd;
    --end;
  }
  return subEnd < sub;
}

bool String::parseBool(const char** end) const
{
  return parseBool(data(), end);
}

int String::parseInt(const char** end) const
{
  return parseInt(data(), end);
}

double String::parseDouble(const char** end) const
{
  return parseDouble(data(), end);
}

String String::operator + (const String& s) const
{
  return String(data(), count, s.data(), s.count);
}

String String::operator + (const char* s) const
{
  return String(data(), count, s, length(s));
}

String operator + (const char* s, const String& t)
{
  return String(s, String::length(s), t.data(), t.count);
}

String& String::operator += (const String& s)
{
  int oCount = count;

  resize(count + s.count, true);
  memcpy(data() + oCount, s.data(), s.count + 1);

  return *this;
}

String& String::operator += (const char* s)
{
  int oCount  = count;
  int sLength = length(s);

  resize(count + sLength, true);
  memcpy(data() + oCount, s, sLength + 1);

  return *this;
}

String String::substring(int start) const
{
  hard_assert(0 <= start && start <= count);

  return String(data() + start, count - start);
}

String String::substring(int start, int end) const
{
  hard_assert(0 <= start && start <= count && start <= end && end <= count);

  return String(data() + start, end - start);
}

String String::trim() const
{
  const char* begin = data();
  const char* end   = begin + count;

  while (begin < end && isBlank(*begin)) {
    ++begin;
  }
  while (begin < end && isBlank(*(end - 1))) {
    --end;
  }

  return String(begin, int(end - begin));
}

String String::replace(char whatChar, char withChar) const
{
  String r;

  const char* oBegin = data();
  char*       rBegin = r.resize(count, false);

  for (int i = 0; i < count; ++i) {
    rBegin[i] = oBegin[i] == whatChar ? withChar : oBegin[i];
  }
  rBegin[count] = '\0';

  return r;
}

List<String> String::split(char delimiter) const
{
  List<String> list;

  int begin = 0;
  int end   = index(delimiter);

  // Count substrings first.
  while (end >= 0) {
    list.add(substring(begin, end));

    begin = end + 1;
    end   = index(delimiter, begin);
  }
  list.add(substring(begin));

  return list;
}

String String::fileDirectory() const
{
  int slash = lastIndex('/');

  return slash >= 0 ? substring(0, slash) : String();
}

String String::fileName() const
{
  int slash = lastIndex('/');

  return slash >= 0 ? substring(slash + 1) : substring(fileIsVirtual());
}

String String::fileBaseName() const
{
  int begin = max<int>(lastIndex('/') + 1, fileIsVirtual());
  int dot   = lastIndex('.');

  return begin <= dot ? substring(begin, dot) : substring(begin);
}

String String::fileExtension() const
{
  int slash = lastIndex('/');
  int dot   = lastIndex('.');

  return slash < dot ? substring(dot + 1) : String();
}

bool String::fileHasExtension(const char* ext) const
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

}
