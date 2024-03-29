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

/**
 * @file ozCore/String.hh
 *
 * `String` class.
 */

#pragma once

#include "List.hh"

namespace oz
{

/**
 * String.
 *
 * Class has static storage of `STATIC_SIZE` bytes, if the string is larger it is stored in a
 * dynamically allocated storage. To deallocate storage just assign an empty string.
 */
class String
{
public:

  /// Size of static buffer for short string optimisation.
  static constexpr int STATIC_SIZE = 32 - sizeof(int);

  /// Empty string. Useful when a function needs to return a reference to an empty string.
  static const String EMPTY;

public:

  /**
   * %Range with constant access to characters.
   */
  using CRange = Arrays::CRange<const char>;

  /**
   * %Range with non-constant access to characters.
   */
  using Range = Arrays::Range<char>;

private:

  int     size_                    = 0;       ///< Length in bytes (without the final null char).
  union
  {
    char* data_                    = nullptr; ///< Pointer to the current buffer (iff not static).
    char  staticData_[STATIC_SIZE];           ///< Static buffer.
  };

  /**
   * Resize storage if necessary and set `count` to `newCount`.
   */
  OZ_INTERNAL
  char* resize(int newSize, bool keepContents);

  /**
   * Helper function for assignment operators.
   */
  void assign(const char* other, int length);

public:

  /*
   * C string functions.
   */

  /**
   * Compare two C strings per-byte (same as `strcmp()`).
   */
  static int compare(const char* a, const char* b)
  {
    return __builtin_strcmp(a, b);
  }

  /**
   * Equality.
   */
  static bool equals(const char* a, const char* b)
  {
    return __builtin_strcmp(a, b) == 0;
  }

  /**
   * Length of a C string.
   */
  static int length(const char* s)
  {
    return int(__builtin_strlen(s));
  }

  /**
   * True iff the C string is empty.
   */
  OZ_ALWAYS_INLINE
  static bool isEmpty(const char* s) noexcept
  {
    return s[0] == '\0';
  }

  /**
   * First character or null character if an empty string.
   */
  OZ_ALWAYS_INLINE
  static char first(const char* s)
  {
    return s[0];
  }

  /**
   * Last character or null character if an empty string.
   */
  static char last(const char* s)
  {
    size_t sCount = __builtin_strlen(s);
    return s[sCount - size_t(sCount != 0)];
  }

  /**
   * Index of the first occurrence of a character from a given index (inclusive).
   */
  static int index(const char* s, char ch, int start = 0);

  /**
   * Index of the last occurrence of a character before a given index (not inclusive).
   */
  static int lastIndex(const char* s, char ch, int end);

  /**
   * Index of the last occurrence of a character.
   */
  static int lastIndex(const char* s, char ch);

  /**
   * Pointer to the first occurrence of a character from a given index (inclusive).
   */
  static const char* find(const char* s, char ch, int start = 0);

  /**
   * Pointer to the last occurrence of a character before a given index (not inclusive).
   */
  static const char* findLast(const char* s, char ch, int end);

  /**
   * Pointer to the last occurrence of a character.
   */
  static const char* findLast(const char* s, char ch);

  /**
   * True iff string begins with given characters.
   */
  static bool beginsWith(const char* s, const char* sub);

  /**
   * True iff string ends with given characters.
   */
  static bool endsWith(const char* s, const char* sub);

  /**
   * Substring from `start` character (inclusively).
   */
  static String substring(const char* s, int start);

  /**
   * Substring between `start` (inclusively) and `end` (not inclusively) character.
   */
  static String substring(const char* s, int start, int end);

  /**
   * Create string with stripped leading and trailing blanks.
   */
  static String trim(const char* s);

  /**
   * Create a copy that has all instances of `whatChar` replaced by `withChar`.
   */
  static String replace(const char* s, char whatChar, char withChar);

  /**
   * Return array of substrings between occurrences of a given delimiter.
   *
   * Empty strings between two immediate delimiter occurrences or between a delimiter and
   * beginning/end of the original string are included.
   */
  static List<String> split(const char* s, char delimiter);

  /**
   * True iff character is an ASCII digit.
   */
  OZ_ALWAYS_INLINE
  static bool isDigit(char c) noexcept
  {
    return '0' <= c && c <= '9';
  }

  /**
   * True iff character is an ASCII letter.
   */
  OZ_ALWAYS_INLINE
  static bool isLetter(char c) noexcept
  {
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
  }

  /**
   * True iff character is a space.
   */
  OZ_ALWAYS_INLINE
  static bool isSpace(char c) noexcept
  {
    return c == ' ' || c == '\t';
  }

  /**
   * True iff character is a space, horizontal tab or newline.
   */
  OZ_ALWAYS_INLINE
  static bool isBlank(char c) noexcept
  {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
  }

  /**
   * Parse boolean value.
   *
   * @return true if "true", false if "false" or anything else (parsing fails).
   */
  static bool parseBool(const char* s, const char** end = nullptr);

  /**
   * Parse integer value, wrapper for `strtol`.
   */
  static int parseInt(const char* s, const char** end = nullptr, int base = 10);

  /**
   * Parse floating-point value, wrapper for `strtod`.
   */
  static double parseDouble(const char* s, const char** end = nullptr);

  /**
   * Create a string in sprintf-like way.
   */
  OZ_PRINTF_FORMAT(1, 2)
  static String format(const char* s, ...);

  /**
   * Create a string in vsprintf-like way.
   */
  OZ_PRINTF_FORMAT(1, 0)
  static String vformat(const char* s, va_list ap);

  /**
   * Generate a string representing the number and optionally add a SI unit prefix.
   *
   * It adds a space after the number and, when necessary, it multiplies the number with an exponent
   * of 1000 and adds a 'm', 'k', 'M' or 'G' unit prefix after the space.
   */
  OZ_PRINTF_FORMAT(2, 0)
  static String si(double e, const char* format = "%.3g");

  /*
   * Functions that operate on a String object.
   */

  /**
   * Empty string.
   */
  String() = default;

  /**
   * Create string form a given C string.
   */
  String(const char* s)
    : String(s, int(__builtin_strlen(s)))
  {}

  /**
   * Create string form a given C string with a known length.
   *
   * The given string doesn't need to be null-terminated.
   *
   * @param s C string.
   * @param length length in bytes without the terminating null character.
   */
  explicit String(const char* s, int length);

  /**
   * Create string by concatenating two C strings.
   */
  explicit String(const char* s, const char* t);

  /**
   * Create string by concatenating two C strings with known lengths.
   */
  explicit String(const char* s, int sLength, const char* t, int tLength);

  /**
   * Create either "true" or "false" string.
   */
  explicit String(bool b);

  /**
   * Create a string representing a given int value using `snprintf()`.
   */
  explicit String(int i, const char* format = "%d");

  /**
   * Create a string representing a given double value using `snprintf()`.
   */
  explicit String(double d, const char* format = "%.9g");

  /**
   * Destructor.
   */
  ~String();

  /**
   * Copy constructor.
   */
  String(const String& other);

  /**
   * Move constructor.
   */
  String(String&& other) noexcept;

  /**
   * Copy operator.
   *
   * Existing storage is reused if its size matches.
   */
  String& operator=(const String& other);

  /**
   * Move operator.
   */
  String& operator=(String&& other) noexcept;

  /**
   * Replace current string with a given C string (nullptr is permitted, equals "").
   */
  String& operator=(const char* other)
  {
    if (other != begin()) {
      assign(other, length(other));
    }
    return *this;
  }

  /**
   * Same as `strcmp() == 0`.
   */
  bool operator==(const String& other) const
  {
    return size_ == other.size_ && compare(begin(), other.begin()) == 0;
  }

  /**
   * Same as `strcmp() == 0`.
   */
  bool operator==(const char* other) const
  {
    return compare(begin(), other) == 0;
  }

  /**
   * Same as `strcmp() == 0`.
   */
  friend bool operator==(const char* a, const String& b)
  {
    return compare(a, b.begin()) == 0;
  }

  /**
   * Same as `strcmp() < 0`.
   */
  bool operator<(const String& other) const
  {
    return compare(begin(), other.begin()) < 0;
  }

  /**
   * Same as `strcmp() < 0`.
   */
  bool operator<(const char* other) const
  {
    return compare(begin(), other) < 0;
  }

  /**
   * Same as `strcmp() < 0`.
   */
  friend bool operator<(const char* a, const String& b)
  {
    return compare(a, b.begin()) < 0;
  }

  /**
   * Constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  const char* cbegin() const noexcept
  {
    return size_ < STATIC_SIZE ? staticData_ : data_;
  }

  /**
   * Constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  const char* begin() const noexcept
  {
    return size_ < STATIC_SIZE ? staticData_ : data_;
  }

  /**
   * Begin iterator.
   */
  OZ_ALWAYS_INLINE
  char* begin() noexcept
  {
    return size_ < STATIC_SIZE ? staticData_ : data_;
  }

  /**
   * Constant end iterator.
   */
  OZ_ALWAYS_INLINE
  const char* cend() const noexcept
  {
    return begin() + size_;
  }

  /**
   * Constant end iterator.
   */
  OZ_ALWAYS_INLINE
  const char* end() const noexcept
  {
    return begin() + size_;
  }

  /**
   * End iterator.
   */
  OZ_ALWAYS_INLINE
  char* end() noexcept
  {
    return begin() + size_;
  }

  /**
   * Compare strings per-byte.
   */
  int compare(const String& other) const
  {
    return compare(begin(), other.begin());
  }

  /**
   * Compare strings per-byte.
   */
  int compare(const char* other) const
  {
    return compare(begin(), other);
  }

  /**
   * Cast to a C string.
   */
  OZ_ALWAYS_INLINE
  operator const char*() const noexcept
  {
    return begin();
  }

  /**
   * Cast to a C string.
   */
  OZ_ALWAYS_INLINE
  const char* c() const noexcept
  {
    return begin();
  }

  /**
   * Length.
   */
  OZ_ALWAYS_INLINE
  int length() const noexcept
  {
    return size_;
  }

  /**
   * True iff the string is empty.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const noexcept
  {
    return size_ == 0;
  }

  /**
   * Constant reference to the `i`-th byte.
   */
  OZ_ALWAYS_INLINE
  const char& operator[](int i) const
  {
    OZ_ASSERT(0 <= i && i <= size_);

    return begin()[i];
  }

  /**
   * Reference to the `i`-th byte.
   */
  OZ_ALWAYS_INLINE
  char& operator[](int i)
  {
    OZ_ASSERT(0 <= i && i <= size_);

    return begin()[i];
  }

  /**
   * Constant reference to the first character or null character if an empty string.
   */
  OZ_ALWAYS_INLINE
  const char& first() const
  {
    return begin()[0];
  }

  /**
   * Reference to the first character or null character if an empty string.
   */
  OZ_ALWAYS_INLINE
  char& first()
  {
    return begin()[0];
  }

  /**
   * Constant reference to the last character or null character if an empty string.
   */
  OZ_ALWAYS_INLINE
  const char& last() const
  {
    return begin()[size_ - int(size_ != 0)];
  }

  /**
   * Reference to the last character or null character if an empty string.
   */
  OZ_ALWAYS_INLINE
  char& last()
  {
    return begin()[size_ - int(size_ != 0)];
  }

  /**
   * Index of the first occurrence of a character from a given index (inclusive).
   */
  int index(char ch, int start = 0) const;

  /**
   * Index of the last occurrence of a character before a given index (not inclusive).
   */
  int lastIndex(char ch, int end) const;

  /**
   * Index of the last occurrence of a character.
   */
  int lastIndex(char ch) const;

  /**
   * Pointer to the first occurrence of a character from a given index (inclusive).
   */
  const char* find(char ch, int start = 0) const;

  /**
   * Pointer to the last occurrence of a character before a given index (not inclusive).
   */
  const char* findLast(char ch, int end) const;

  /**
   * Pointer to the last occurrence of a character.
   */
  const char* findLast(char ch) const;

  /**
   * True iff string begins with given characters.
   */
  bool beginsWith(const char* sub) const;

  /**
   * True iff string ends with given characters.
   */
  bool endsWith(const char* sub) const;

  /**
   * Parse bool value, wraps `parseBool(const char* s, const char** end)`.
   */
  bool parseBool(const char** end = nullptr) const;

  /**
   * Parse int value, wraps `parseInt(const char* s, const char** end)`.
   */
  int parseInt(const char** end = nullptr) const;

  /**
   * Parse double value, wraps `parseDouble(const char* s, const char** end)`.
   */
  double parseDouble(const char** end = nullptr) const;

  /**
   * Create concatenated string.
   */
  String operator+(const String& s) const;

  /**
   * Create concatenated string.
   */
  String operator+(const char* s) const;

  /**
   * Create concatenated string.
   */
  friend String operator+(const char* s, const String& t);

  /**
   * Replace with concatenated string.
   */
  String& operator+=(const String& s);

  /**
   * Replace with concatenated string.
   */
  String& operator+=(const char* s);

  /**
   * Substring from `start` character (inclusively).
   */
  String substring(int start) const;

  /**
   * Substring between `start` (inclusively) and `end` (not inclusively) character.
   */
  String substring(int start, int end) const;

  /**
   * Create string with stripped leading and trailing blanks.
   */
  String trim() const;

  /**
   * Create a copy that has all instances of `whatChar` replaced by `withChar`.
   */
  String replace(char whatChar, char withChar) const;

  /**
   * Return array of substrings between occurrences of a given delimiter.
   *
   * Empty strings between two immediate delimiter occurrences or between a delimiter and
   * beginning/end of the original string are included.
   */
  List<String> split(char delimiter) const;

};

/**
 * `Less` function object is the same as for C strings.
 */
template <>
struct Less<String> : Less<const char*>
{};

/**
 * `Hash` function object is the same as for C strings.
 */
template <>
struct Hash<String> : Hash<const char*>
{};

}
