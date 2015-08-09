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
 * @file ozCore/Json.cc
 */

#include "Json.hh"

#include "SList.hh"
#include "Log.hh"

#define OZ_PARSE_ERROR(charBias, message) \
  OZ_ERROR("oz::Json: " message " at %s:%d:%d", path, line, column + (charBias));

static_assert(sizeof(double) >= sizeof(void*),
              "Pointer must fit into double for internal oz::Json union to work properly");

namespace oz
{

static const Json NIL_VALUE = Json();

struct StringData
{
  String value;
};

struct ArrayData
{
  List<Json> list;
};

struct ObjectData
{
  Map<String, Json> map;
};

struct Json::Parser
{
  enum BlanksMode
  {
    WHITESPACE,
    LINE_COMMENT,
    MULTILINE_COMMENT
  };

  Stream*     is;
  const char* path;
  int         line;
  int         column;
  int         oldLine;
  int         oldColumn;

  OZ_INTERNAL
  char readChar()
  {
    if (is->available() == 0) {
      OZ_PARSE_ERROR(0, "Unexpected end of file");
    }

    oldLine   = line;
    oldColumn = column;

    char ch = is->readChar();

    if (ch == '\n') {
      ++line;
      column = 0;
    }
    else {
      ++column;
    }
    return ch;
  }

  OZ_INTERNAL
  void backChar()
  {
    OZ_ASSERT(line != oldLine || column != oldColumn);

    is->seek(is->tell() - 1);

    line   = oldLine;
    column = oldColumn;
  }

  OZ_INTERNAL
  static void setAccessed(Json* value)
  {
    value->wasAccessed = true;

    switch (value->valueType) {
      default: {
        break;
      }
      case ARRAY: {
        List<Json>& list = static_cast<ArrayData*>(value->data)->list;

        for (Json& i : list) {
          setAccessed(&i);
        }
        break;
      }
      case OBJECT: {
        Map<String, Json>& map = static_cast<ObjectData*>(value->data)->map;

        for (auto& i : map) {
          setAccessed(&i.value);
        }
        break;
      }
    }
  }

  OZ_INTERNAL
  static Json parse(Stream* is, const char* path)
  {
    Parser parser(is, path);

    Json root = parser.parseValue();

    parser.finish();
    return root;
  }

  OZ_INTERNAL
  explicit Parser(Stream* is_, const char* path_) :
    is(is_), path(path_), line(1), column(0), oldLine(1), oldColumn(0)
  {}

  OZ_INTERNAL
  char skipBlanks()
  {
    char ch1, ch2;

    do {
      do {
        ch2 = readChar();
      }
      while (String::isBlank(ch2));

      if (ch2 == '/') {
        ch1 = ch2;
        ch2 = readChar();

        if (ch2 == '/') {
          // Skip a line comment.
          do {
            ch2 = readChar();
          }
          while (ch2 != '\n');

          continue;
        }
        else if (ch2 == '*') {
          // Skip a multi-line comment.
          ch2 = readChar();

          do {
            ch1 = ch2;
            ch2 = readChar();
          }
          while (ch1 != '*' || ch2 != '/');

          continue;
        }
        else {
          ch2 = ch1;
          backChar();
        }
      }

      return ch2;
    }
    while (true);
  }

  OZ_INTERNAL
  String parseString()
  {
    List<char> chars;
    char ch = '"';

    do {
      ch = readChar();

      if (ch == '\n' || ch == '\r') {
        continue;
      }

      if (ch == '\\') {
        ch = readChar();

        switch (ch) {
          case 'b': {
            ch = '\b';
            break;
          }
          case 'f': {
            ch = '\f';
            break;
          }
          case 'n': {
            ch = '\n';
            break;
          }
          case 'r': {
            ch = '\r';
            break;
          }
          case 't': {
            ch = '\t';
            break;
          }
          default: {
            break;
          }
        }
      }
      else if (ch == '"') {
        break;
      }

      chars.add(ch);
    }
    while (is->available() != 0);

    if (ch != '"') {
      OZ_PARSE_ERROR(0, "End of file while looking for end of string (Is ending \" missing?)");
    }
    chars.add('\0');

    return String(chars.begin(), chars.length() - 1);
  }

  OZ_INTERNAL
  Json parseValue()
  {
    char ch = skipBlanks();

    switch (ch) {
      case 'n': {
        if (is->available() < 3 || readChar() != 'u' || readChar() != 'l' || readChar() != 'l') {
          OZ_PARSE_ERROR(-3, "Unknown value type");
        }

        return Json();
      }
      case 'f': {
        if (is->available() < 4 || readChar() != 'a' || readChar() != 'l' || readChar() != 's' ||
            readChar() != 'e')
        {
          OZ_PARSE_ERROR(-4, "Unknown value type");
        }

        return Json(false);
      }
      case 't': {
        if (is->available() < 4 || readChar() != 'r' || readChar() != 'u' || readChar() != 'e') {
          OZ_PARSE_ERROR(-3, "Unknown value type");
        }

        return Json(true);
      }
      default: { // Number.
        SList<char, 32> chars;
        chars.add(ch);

        while (is->available() != 0) {
          ch = readChar();

          if (String::isBlank(ch) || ch == ',' || ch == '}' || ch == ']') {
            backChar();
            break;
          }
          if (chars.length() >= 31) {
            OZ_PARSE_ERROR(-chars.length(), "Too long number");
          }
          chars.add(ch);
        }
        chars.add('\0');

        const char* end;
        double number = String::parseDouble(chars.begin(), &end);

        if (end == chars.begin()) {
          OZ_PARSE_ERROR(-chars.length(), "Unknown value type");
        }

        return Json(number);
      }
      case '"': {
        return Json(parseString());
      }
      case '{': {
        return parseObject();
      }
      case '[': {
        return parseArray();
      }
    }
  }

  OZ_INTERNAL
  Json parseArray()
  {
    Json arrayValue(ARRAY);
    List<Json>& list = static_cast<ArrayData*>(arrayValue.data)->list;

    char ch = skipBlanks();
    if (ch != ']') {
      backChar();
    }

    while (ch != ']') {
      list.add(parseValue());

      ch = skipBlanks();

      if (ch != ',' && ch != ']') {
        OZ_PARSE_ERROR(0, "Expected ',' or ']' while parsing array (Is ',' is missing?)");
      }
    }

    return arrayValue;
  }

  OZ_INTERNAL
  Json parseObject()
  {
    Json objectValue(OBJECT);
    Map<String, Json>& map = static_cast<ObjectData*>(objectValue.data)->map;

    char ch = skipBlanks();
    if (ch != '}') {
      backChar();
    }

    while (ch != '}') {
      ch = skipBlanks();
      if (ch != '"') {
        OZ_PARSE_ERROR(0, "Expected key while parsing object (Is there ',' after last entry?)");
      }

      String key = parseString();

      ch = skipBlanks();
      if (ch != ':') {
        OZ_PARSE_ERROR(0, "Expected ':' after key in object entry");
      }

      map.add(static_cast<String&&>(key), parseValue());

      ch = skipBlanks();

      if (ch != ',' && ch != '}') {
        OZ_PARSE_ERROR(0, "Expected ',' or '}' while parsing object entry");
      }
    }

    return objectValue;
  }

  OZ_INTERNAL
  void finish()
  {
    while (is->available() != 0) {
      char ch = readChar();

      if (!String::isBlank(ch)) {
        OZ_PARSE_ERROR(0, "End of file expected but some content found after");
      }
    }
  }
};

struct Json::Formatter
{
  Stream*       os;
  const Format* format;
  int           lineEndLength;
  int           indentLevel;

  OZ_INTERNAL
  int writeString(const String& string)
  {
    int length = string.length() + 2;

    os->writeChar('"');

    for (int i = 0; i < string.length(); ++i) {
      char ch = string[i];

      switch (ch) {
        case '\\': {
          os->write("\\\\", 2);
          ++length;
          break;
        }
        case '"': {
          os->write("\\\"", 2);
          ++length;
          break;
        }
        case '\b': {
          os->write("\\b", 2);
          ++length;
          break;
        }
        case '\f': {
          os->write("\\f", 2);
          ++length;
          break;
        }
        case '\n': {
          os->write("\\n", 2);
          ++length;
          break;
        }
        case '\r': {
          os->write("\\r", 2);
          ++length;
          break;
        }
        case '\t': {
          os->write("\\t", 2);
          ++length;
          break;
        }
        default: {
          os->writeChar(ch);
          break;
        }
      }
    }

    os->writeChar('"');

    return length;
  }

  OZ_INTERNAL
  void writeValue(const Json& value)
  {
    switch (value.valueType) {
      case NIL: {
        os->write("null", 4);
        break;
      }
      case BOOLEAN: {
        if (value.boolean) {
          os->write("true", 4);
        }
        else {
          os->write("false", 5);
        }
        break;
      }
      case NUMBER: {
        String s = String(value.number, format->numberFormat);
        os->write(s, s.length());
        break;
      }
      case STRING: {
        const String& string = static_cast<const StringData*>(value.data)->value;

        writeString(string);
        break;
      }
      case ARRAY: {
        writeArray(value);
        break;
      }
      case OBJECT: {
        writeObject(value);
        break;
      }
    }
  }

  OZ_INTERNAL
  void writeArray(const Json& value)
  {
    const List<Json>& list = static_cast<const ArrayData*>(value.data)->list;

    if (list.isEmpty()) {
      os->write("[]", 2);
      return;
    }

    os->writeChar('[');
    os->write(format->lineEnd, lineEndLength);

    ++indentLevel;

    for (int i = 0; i < list.length(); ++i) {
      if (i != 0) {
        os->writeChar(',');
        os->write(format->lineEnd, lineEndLength);
      }

      for (int j = 0; j < indentLevel; ++j) {
        os->write("  ", 2);
      }

      writeValue(list[i]);
    }

    os->write(format->lineEnd, lineEndLength);

    --indentLevel;
    for (int j = 0; j < indentLevel; ++j) {
      os->write("  ", 2);
    }

    os->writeChar(']');
  }

  OZ_INTERNAL
  void writeObject(const Json& value)
  {
    const Map<String, Json>& map = static_cast<const ObjectData*>(value.data)->map;

    if (map.isEmpty()) {
      os->write("{}", 2);
      return;
    }

    os->writeChar('{');
    os->write(format->lineEnd, lineEndLength);

    ++indentLevel;

    for (int i = 0; i < map.length(); ++i) {
      if (i != 0) {
        os->writeChar(',');
        os->write(format->lineEnd, lineEndLength);
      }

      for (int j = 0; j < indentLevel; ++j) {
        os->write("  ", 2);
      }

      const String& entryKey   = map[i].key;
      const Json&   entryValue = map[i].value;

      int keyLength = writeString(entryKey);
      os->writeChar(':');

      if (entryValue.valueType == ARRAY || entryValue.valueType == OBJECT) {
        os->write(format->lineEnd, lineEndLength);

        for (int j = 0; j < indentLevel; ++j) {
          os->write("  ", 2);
        }
      }
      else {
        int column = indentLevel * 2 + keyLength + 1;

        // Align to 24-th column.
        for (int j = column; j < format->alignmentColumn; ++j) {
          os->writeChar(' ');
        }
      }

      writeValue(entryValue);
    }

    os->write(format->lineEnd, lineEndLength);

    --indentLevel;
    for (int j = 0; j < indentLevel; ++j) {
      os->write("  ", 2);
    }

    os->writeChar('}');
  }
};

const Json::Format Json::DEFAULT_FORMAT = { 2, 32, "%.9g", "\n" };

OZ_INTERNAL
Json::Json(const float* vector, int count) :
  data(new ArrayData{ List<Json>(count) }), valueType(ARRAY)
{
  List<Json>& list = static_cast<ArrayData*>(data)->list;

  for (int i = 0; i < count; ++i) {
    list[i].number = vector[i];
  }
}

OZ_INTERNAL
bool Json::getVector(float* vector, int count) const
{
  if (valueType != ARRAY) {
    return false;
  }

  const List<Json>& list = static_cast<const ArrayData*>(data)->list;

  if (list.length() != count) {
    return false;
  }

  for (int i = 0; i < count; ++i) {
    vector[i] = float(list[i].get(0.0));
  }
  return true;
}

Json::Json(Type type) :
  valueType(type)
{
  switch (type) {
    default: {
      break;
    }
    case STRING: {
      data = new StringData();
      break;
    }
    case ARRAY: {
      data = new ArrayData();
      break;
    }
    case OBJECT: {
      data = new ObjectData();
      break;
    }
  }
}

Json::Json(nullptr_t)
{}

Json::Json(bool value) :
  boolean(value), valueType(BOOLEAN)
{}

Json::Json(int value) :
  Json(double(value))
{}

Json::Json(float value) :
  Json(double(value))
{}

Json::Json(double value) :
  number(value), valueType(NUMBER)
{}

Json::Json(const String& value) :
  data(new StringData{ value }), valueType(STRING)
{}

Json::Json(const char* value) :
  data(new StringData{ value }), valueType(STRING)
{}

Json::Json(const Vec3& v) :
  Json(v, 3)
{}

Json::Json(const Point& p) :
  Json(p, 3)
{}

Json::Json(const Plane& p) :
  Json(Vec4(p.n.x, p.n.y, p.n.y, p.d))
{}

Json::Json(const Vec4& v) :
  Json(v, 4)
{}

Json::Json(const Quat& q) :
  Json(q, 4)
{}

Json::Json(const Mat3& m) :
  Json(m, 9)
{}

Json::Json(const Mat4& m) :
  Json(m, 16)
{}

Json::Json(InitialiserList<Json> l) :
  data(new ArrayData()), valueType(ARRAY), wasAccessed(false)
{
  List<Json>& list = static_cast<ArrayData*>(data)->list;

  list.addAll(l.begin(), int(l.size()));
}

Json::Json(InitialiserList<Pair> l) :
  data(new ObjectData()), valueType(OBJECT), wasAccessed(false)
{
  Map<String, Json>& map = static_cast<ObjectData*>(data)->map;

  for (const auto& i : l) {
    map.add(i.key, i.value);
  }
}

Json::Json(const File& file) :
  valueType(NIL), wasAccessed(false)
{
  load(file);
}

Json::~Json()
{
  clear();
}

Json::Json(const Json& j) :
  valueType(j.valueType), wasAccessed(j.wasAccessed)
{
  switch (valueType) {
    default: {
      break;
    }
    case BOOLEAN: {
      boolean = j.boolean;
      break;
    }
    case NUMBER: {
      number = j.number;
      break;
    }
    case STRING: {
      data = new StringData(*static_cast<const StringData*>(j.data));
      break;
    }
    case ARRAY: {
      data = new ArrayData(*static_cast<const ArrayData*>(j.data));
      break;
    }
    case OBJECT: {
      data = new ObjectData(*static_cast<const ObjectData*>(j.data));
      break;
    }
  }
}

Json::Json(Json&& j) :
  number(j.number), valueType(j.valueType), wasAccessed(j.wasAccessed)
{
  j.number      = 0.0;
  j.valueType   = NIL;
  j.wasAccessed = true;
}

Json& Json::operator = (const Json& j)
{
  if (&j != this) {
    clear();

    valueType   = j.valueType;
    wasAccessed = j.wasAccessed;

    switch (valueType) {
      default: {
        data = nullptr;
        break;
      }
      case BOOLEAN: {
        boolean = j.boolean;
        break;
      }
      case NUMBER: {
        number = j.number;
        break;
      }
      case STRING: {
        data = new StringData(*static_cast<const StringData*>(j.data));
        break;
      }
      case ARRAY: {
        data = new ArrayData(*static_cast<const ArrayData*>(j.data));
        break;
      }
      case OBJECT: {
        data = new ObjectData(*static_cast<const ObjectData*>(j.data));
        break;
      }
    }
  }
  return *this;
}

Json& Json::operator = (Json&& j)
{
  if (&j != this) {
    clear();

    number      = j.number;
    valueType   = j.valueType;
    wasAccessed = j.wasAccessed;

    j.number      = 0.0;
    j.valueType   = NIL;
    j.wasAccessed = true;
  }
  return *this;
}

bool Json::operator == (const Json& j) const
{
  if (valueType != j.valueType) {
    return false;
  }

  switch (valueType) {
    default: {
      return true;
    }
    case BOOLEAN: {
      return boolean == j.boolean;
    }
    case NUMBER: {
      return number == j.number;
    }
    case STRING: {
      const String& s1 = static_cast<const StringData*>(data)->value;
      const String& s2 = static_cast<const StringData*>(j.data)->value;

      return s1 == s2;
    }
    case ARRAY: {
      const List<Json>& l1 = static_cast<const ArrayData*>(data)->list;
      const List<Json>& l2 = static_cast<const ArrayData*>(j.data)->list;

      return l1 == l2;
    }
    case OBJECT: {
      const Map<String, Json>& m1 = static_cast<const ObjectData*>(data)->map;
      const Map<String, Json>& m2 = static_cast<const ObjectData*>(j.data)->map;

      return m1 == m2;
    }
  }
}

bool Json::operator != (const Json& j) const
{
  return !operator == (j);
}

Json::ArrayCIterator Json::arrayCIter() const
{
  if (valueType == ARRAY) {
    const List<Json>& list = static_cast<const ArrayData*>(data)->list;

    wasAccessed = true;
    return list.citerator();
  }
  else {
    wasAccessed |= valueType == NIL;
    return ArrayCIterator();
  }
}

Json::ArrayIterator Json::arrayIter()
{
  if (valueType == ARRAY) {
    List<Json>& list = static_cast<ArrayData*>(data)->list;

    wasAccessed = true;
    return list.iterator();
  }
  else {
    wasAccessed |= valueType == NIL;
    return ArrayIterator();
  }
}

Json::ObjectCIterator Json::objectCIter() const
{
  if (valueType == OBJECT) {
    const Map<String, Json>& map = static_cast<const ObjectData*>(data)->map;

    wasAccessed = true;
    return map.citerator();
  }
  else {
    wasAccessed |= valueType == NIL;
    return ObjectCIterator();
  }
}

Json::ObjectIterator Json::objectIter()
{
  if (valueType == OBJECT) {
    Map<String, Json>& map = static_cast<ObjectData*>(data)->map;

    wasAccessed = true;
    return map.iterator();
  }
  else {
    wasAccessed |= valueType == NIL;
    return ObjectIterator();
  }
}

int Json::length() const
{
  switch (valueType) {
    default: {
      wasAccessed |= valueType == NIL;
      return -1;
    }
    case ARRAY: {
      const List<Json>& list = static_cast<const ArrayData*>(data)->list;

      wasAccessed = true;
      return list.length();
    }
    case OBJECT: {
      const Map<String, Json>& map = static_cast<const ObjectData*>(data)->map;

      wasAccessed = true;
      return map.length();
    }
  }
}

const Json& Json::operator [] (int i) const
{
  if (valueType != ARRAY) {
    wasAccessed |= valueType == NIL;
    return NIL_VALUE;
  }

  const List<Json>& list = static_cast<const ArrayData*>(data)->list;

  wasAccessed = true;

  if (uint(i) >= uint(list.length())) {
    return NIL_VALUE;
  }

  list[i].wasAccessed = true;
  return list[i];
}

const Json& Json::operator [] (const char* key) const
{
  if (valueType != OBJECT) {
    wasAccessed |= valueType == NIL;
    return NIL_VALUE;
  }

  const Map<String, Json>& map = static_cast<const ObjectData*>(data)->map;
  const Json* value = map.find(key);

  wasAccessed = true;

  if (value == nullptr) {
    return NIL_VALUE;
  }

  value->wasAccessed = true;
  return *value;
}

bool Json::contains(const char* key) const
{
  if (valueType != OBJECT) {
    wasAccessed |= valueType == NIL;
    return false;
  }

  const Map<String, Json>& map = static_cast<const ObjectData*>(data)->map;
  const Json* value = map.find(key);

  wasAccessed = true;

  if (value == nullptr) {
    return false;
  }

  value->wasAccessed = true;
  return true;
}

bool Json::get(bool defaultValue) const
{
  if (valueType != BOOLEAN) {
    return defaultValue;
  }

  wasAccessed = true;
  return boolean;
}

double Json::get(double defaultValue) const
{
  if (valueType != NUMBER) {
    return defaultValue;
  }

  wasAccessed = true;
  return number;
}

const String& Json::get(const String& defaultValue) const
{
  if (valueType != STRING) {
    return defaultValue;
  }

  wasAccessed = true;
  return static_cast<const StringData*>(data)->value;
}

const char* Json::get(const char* defaultValue) const
{
  if (valueType != STRING) {
    return defaultValue;
  }

  wasAccessed = true;
  return static_cast<const StringData*>(data)->value.c();
}

Vec3 Json::get(const Vec3& defaultValue) const
{
  Vec3 v;
  return getVector(v, 3) ? v : defaultValue;
}

Point Json::get(const Point& defaultValue) const
{
  Point p;
  return getVector(p, 3) ? p : defaultValue;
}

Plane Json::get(const Plane& defaultValue) const
{
  Vec4 v;
  return getVector(v, 4) ? Plane(v.x, v.y, v.z, v.w) : defaultValue;
}

Vec4 Json::get(const Vec4& defaultValue) const
{
  Vec4 v;
  return getVector(v, 4) ? v : defaultValue;
}

Quat Json::get(const Quat& defaultValue) const
{
  Quat q;
  return getVector(q, 4) ? q : defaultValue;
}

Mat3 Json::get(const Mat3& defaultValue) const
{
  Mat3 m;
  return getVector(m, 9) ? m : defaultValue;
}

Mat4 Json::get(const Mat4& defaultValue) const
{
  Mat4 m;
  return getVector(m, 16) ? m : defaultValue;
}

Json& Json::add(const Json& json)
{
  if (valueType != ARRAY) {
    OZ_ERROR("oz::Json: Tried to add a value to a non-array JSON value: %s", toString().c());
  }

  List<Json>& list = static_cast<ArrayData*>(data)->list;
  return list.add(json);
}

Json& Json::add(Json&& json)
{
  if (valueType != ARRAY) {
    OZ_ERROR("oz::Json: Tried to add a value to a non-array JSON value: %s", toString().c());
  }

  List<Json>& list = static_cast<ArrayData*>(data)->list;
  return list.add(static_cast<Json&&>(json));
}

Json& Json::add(const char* key, const Json& json)
{
  if (valueType != OBJECT) {
    OZ_ERROR("oz::Json: Tried to add a key-value pair '%s' to a non-object JSON value: %s",
             key, toString().c());
  }

  Map<String, Json>& map = static_cast<ObjectData*>(data)->map;
  return map.add(key, json).value;
}

Json& Json::add(const char* key, Json&& json)
{
  if (valueType != OBJECT) {
    OZ_ERROR("oz::Json: Tried to add a key-value pair '%s' to a non-object JSON value: %s",
             key, toString().c());
  }

  Map<String, Json>& map = static_cast<ObjectData*>(data)->map;
  return map.add(key, static_cast<Json&&>(json)).value;
}

Json& Json::include(const char* key, const Json& json)
{
  if (valueType != OBJECT) {
    OZ_ERROR("oz::Json: Tried to include a key-value pair '%s' in a non-object JSON value: %s",
             key, toString().c());
  }

  Map<String, Json>& map = static_cast<ObjectData*>(data)->map;
  return map.include(key, json).value;
}

Json& Json::include(const char* key, Json&& json)
{
  if (valueType != OBJECT) {
    OZ_ERROR("oz::Json: Tried to include a key-value pair '%s' in a non-object JSON value: %s",
             key, toString().c());
  }

  Map<String, Json>& map = static_cast<ObjectData*>(data)->map;
  return map.include(key, static_cast<Json&&>(json)).value;
}

bool Json::erase(int index)
{
  if (valueType != ARRAY) {
    OZ_ERROR("oz::Json: Tried to erase a value from a non-array JSON value: %s",
             toString().c());
  }

  List<Json>& list = static_cast<ArrayData*>(data)->list;

  if (uint(index) >= uint(list.length())) {
    return false;
  }

  list.erase(index);
  return true;
}

bool Json::exclude(const char* key)
{
  if (valueType != OBJECT) {
    OZ_ERROR("oz::Json: Tried to exclude and entry form a non-object JSON value: %s",
             toString().c());
  }

  Map<String, Json>& map = static_cast<ObjectData*>(data)->map;

  return map.exclude(key) >= 0;
}

bool Json::clear(bool warnUnused)
{
  bool hasUnused = false;

  if (warnUnused && !wasAccessed) {
    Log::println("oz::Json: unused value: %s", toString().c());
    System::bell();

    hasUnused  = true;
    warnUnused = false;
  }

  switch (valueType) {
    default: {
      break;
    }
    case STRING: {
      delete static_cast<StringData*>(data);
      break;
    }
    case ARRAY: {
      ArrayData* arrayData = static_cast<ArrayData*>(data);

      if (warnUnused) {
        for (Json& i : arrayData->list) {
          hasUnused |= i.clear(true);
        }
      }

      delete arrayData;
      break;
    }
    case OBJECT: {
      ObjectData* objectData = static_cast<ObjectData*>(data);

      if (warnUnused) {
        for (auto& i : objectData->map) {
          hasUnused |= i.value.clear(true);
        }
      }

      delete objectData;
      break;
    }
  }

  valueType   = NIL;
  wasAccessed = true;

  return hasUnused;
}

String Json::toString() const
{
  switch (valueType) {
    default: {
      return "null";
    }
    case BOOLEAN: {
      return boolean ? "true" : "false";
    }
    case NUMBER: {
      return String(number);
    }
    case STRING: {
      const String& string = static_cast<const StringData*>(data)->value;

      return "\"" + string + "\"";
    }
    case ARRAY: {
      const List<Json>& list = static_cast<const ArrayData*>(data)->list;

      if (list.isEmpty()) {
        return "[]";
      }

      String s = "[ ";

      for (int i = 0; i < list.length(); ++i) {
        if (i != 0) {
          s += ", ";
        }

        s += list[i].toString();
      }

      return s + " ]";
    }
    case OBJECT: {
      const Map<String, Json>& map = static_cast<const ObjectData*>(data)->map;

      if (map.isEmpty()) {
        return "{}";
      }

      String s = "{ ";

      bool isFirst = true;
      for (const auto& i : map) {
        s += String::format(isFirst ? "\"%s\": %s" : ", \"%s\": %s",
                            i.key.c(), i.value.toString().c());
        isFirst = false;
      }

      return s + " }";
    }
  }
}

String Json::toFormattedString(const Format& format) const
{
  Stream    os(0);
  Formatter formatter = { &os, &format, String::length(format.lineEnd), 0 };

  formatter.writeValue(*this);
  os.write(format.lineEnd, formatter.lineEndLength);

  return String(os.begin(), os.tell());
}

bool Json::load(const File& file)
{
  Stream is = file.read();
  if (is.available() == 0) {
    return false;
  }

  *this = Parser::parse(&is, file);
  return true;
}

bool Json::save(const File& file, const Format& format) const
{
  Stream os(0);
  Formatter formatter = { &os, &format, String::length(format.lineEnd), 0 };

  formatter.writeValue(*this);
  os.write(format.lineEnd, formatter.lineEndLength);

  return file.write(os.begin(), os.tell());
}

}
