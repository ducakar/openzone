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
 * @file ozCore/common.hh
 *
 * Essential includes, macros, types and templates.
 */

#pragma once

#include <ozCore/config.hh>

#include <climits>
#include <cstdarg>
#include <cstddef>
#include <initializer_list>
#include <new>

/**
 * @def OZ_ALIGNMENT
 * Alignment of allocated storage returned by the `new` operator.
 */
#ifdef OZ_SIMD
# define OZ_ALIGNMENT sizeof(float[4])
#else
# define OZ_ALIGNMENT sizeof(void*)
#endif

/**
 * @def OZ_ALWAYS_INLINE
 * Compiler-specific attribute that suggests function inlining even in debug mode.
 */
#define OZ_ALWAYS_INLINE [[gnu::always_inline]]

/**
 * @def OZ_BYTE_ORDER
 * Byte order inside a 32-bit integer. 1234 for little endian and 4321 for big endian machines.
 */
#define OZ_BYTE_ORDER __BYTE_ORDER__

/**
 * @def OZ_HIDDEN
 * Compiler-specific attribute for functions that should not be exported from a shared library.
 */
#ifdef _WIN32
# define OZ_HIDDEN
#else
# define OZ_HIDDEN [[gnu::visibility("hidden")]]
#endif

/**
 * @def OZ_INTERNAL
 * Same as `OZ_HIDDEN` but function pointer is only valid inside function's compilation unit.
 */
#ifdef _WIN32
# define OZ_INTERNAL
#else
# define OZ_INTERNAL [[gnu::visibility("internal")]]
#endif

/**
 * @def OZ_NORETURN
 * Compiler-specific attribute that marks a function as no-return.
 */
#define OZ_NORETURN [[noreturn]]

/**
 * @def OZ_PRINTF_FORMAT
 * Compiler-specific attribute that specifies checking of printf-like arguments.
 */
#ifdef _WIN32
# define OZ_PRINTF_FORMAT(s, first) [[gnu::format(gnu_printf, s, first)]]
#else
# define OZ_PRINTF_FORMAT(s, first) [[gnu::format(printf, s, first)]]
#endif

/**
 * @def OZ_WEAK
 * Compiler-specific attribute specifying a weak symbol.
 */
#define OZ_WEAK [[gnu::weak]]

/**
 * Top-level OpenZone namespace.
 */
namespace oz
{

/**
 * Internal helpers.
 */
namespace detail {}

/**
 * Null pointer type.
 */
using nullptr_t = std::nullptr_t;

/**
 * Platform-dependent unsigned integer type for memory offsets and sizes.
 */
using size_t = std::size_t;

/**
 * Platform-dependent signed integer type for memory offsets and pointer differences.
 */
using ptrdiff_t = std::ptrdiff_t;

/**
 * Initialiser list.
 */
template <typename Elem>
using InitialiserList = std::initializer_list<Elem>;

/**
 * Signed byte.
 */
using byte = signed char;

/**
 * Unsigned byte.
 */
using ubyte = unsigned char;

/**
 * Unsigned short integer.
 */
using ushort = unsigned short;

/**
 * Unsigned integer.
 */
using uint = unsigned int;

/**
 * Unsigned long integer.
 */
using ulong = unsigned long;

/**
 * Signed 64-bit integer.
 */
using int64 = signed long long;

/**
 * Unsigned 64-bit integer.
 */
using uint64 = unsigned long long;

/**
 * Unit type.
 */
struct Void
{
  /**
   * All instances are identical.
   */
  OZ_ALWAYS_INLINE
  constexpr bool operator==(const Void&) const
  {
    return true;
  }

  /**
   * All instances are identical.
   */
  OZ_ALWAYS_INLINE
  constexpr bool operator!=(const Void&) const
  {
    return false;
  }
};

static_assert(sizeof(char  ) == 1, "sizeof(char) should be 1"  );
static_assert(sizeof(short ) == 2, "sizeof(short) should be 2" );
static_assert(sizeof(int   ) == 4, "sizeof(int) should be 4"   );
static_assert(sizeof(int64 ) == 8, "sizeof(int64) should be 8" );
static_assert(sizeof(float ) == 4, "sizeof(float) should be 4" );
static_assert(sizeof(double) == 8, "sizeof(double) should be 8");

/**
 * operator!= expressed in terms of operator==.
 */
template <typename Type>
inline constexpr bool operator!=(const Type& a, const Type& b)
{
  return !(a == b);
}

/**
 * operator<= expressed in terms of operator<.
 */
template <typename Type>
inline constexpr bool operator<=(const Type& a, const Type& b)
{
  return !(b < a);
}

/**
 * operator> expressed in terms of operator<.
 */
template <typename Type>
inline constexpr bool operator>(const Type& a, const Type& b)
{
  return b < a;
}

/**
 * operator>= expressed in terms of operator<.
 */
template <typename Type>
inline constexpr bool operator>=(const Type& a, const Type& b)
{
  return !(a < b);
}

/**
 * Swap values of two variables.
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline void swap(Value& a, Value& b) noexcept
{
  Value t(static_cast<Value&&>(a));

  a = static_cast<Value&&>(b);
  b = static_cast<Value&&>(t);
}

/**
 * Absolute value.
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline constexpr Value abs(const Value& a)
{
  return a < 0 ? -a : a;
}

/**
 * `a` if `a <= b`, `b` otherwise.
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline constexpr const Value& min(const Value& a, const Value& b)
{
  return b < a ? b : a;
}

/**
 * Minimum of more than two arguments.
 */
template <typename Value, typename ...Args>
OZ_ALWAYS_INLINE
inline constexpr const Value& min(const Value& a, const Value& b, const Args&... args)
{
  return min<Args...>(min<Value>(a, b), args...);
}

/**
 * `a` if `a >= b`, `b` otherwise.
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline constexpr const Value& max(const Value& a, const Value& b)
{
  return a < b ? b : a;
}
/**
 * Maximum of more than two arguments.
 */
template <typename Value, typename ...Args>
OZ_ALWAYS_INLINE
inline constexpr const Value& max(const Value& a, const Value& b, const Args&... args)
{
  return max<Args...>(max<Value>(a, b), args...);
}

/**
 * `c` if `a <= c <= b`, respective boundary otherwise.
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline constexpr const Value& clamp(const Value& c, const Value& a, const Value& b)
{
  return c < a ? a : (b < c ? b : c);
}

/**
 * Generic `Less` function object, defaults to `operator<`.
 */
template <typename Type = void>
struct Less
{
  /**
   * Compare using `operator<`.
   */
  template <typename TypeA, typename TypeB>
  OZ_ALWAYS_INLINE
  constexpr bool operator()(const TypeA& a, const TypeB& b) const
  {
    return a < b;
  }
};

/**
 * Generic greater function object, calls inverted `Less` function.
 */
template <typename Type = void>
struct Greater
{
  /**
   * Compare using appropriate `Less` function object.
   */
  template <typename TypeA, typename TypeB>
  OZ_ALWAYS_INLINE
  constexpr bool operator()(const TypeA& a, const TypeB& b) const
  {
    return Less<Type>()(b, a);
  }
};

/**
 * `Less` function object for string comparison.
 */
template <>
struct Less<const char*>
{
  /**
   * Compare using `strcmp`.
   */
  OZ_ALWAYS_INLINE
  bool operator()(const char* a, const char* b) const
  {
    return __builtin_strcmp(a, b) < 0;
  }
};

/**
 * Generic hash function object for integers and pointers.
 */
template <typename Number>
struct Hash
{
  /**
   * Return value as integer.
   */
  OZ_ALWAYS_INLINE
  constexpr int operator()(const Number& value) const
  {
    return int(value);
  }
};

/**
 * `Hash` function object for strings.
 */
template <>
struct Hash<const char*>
{
  /// %Hash value for an empty string.
  static const int EMPTY = int(2166136261);

  /**
   * FNV hash function.
   */
  int operator()(const char* s) const
  {
    uint value = EMPTY;

    while (*s != '\0') {
      value = (value * 16777619) ^ int(*s);
      ++s;
    }
    return value;
  }
};

}
