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
 * @file ozCore/common.hh
 *
 * Essential includes, macros, types and templates.
 */

#pragma once

#include <ozCore/config.hh>

#include <cstdarg>
#include <cstddef>
#include <climits>
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
#define OZ_ALWAYS_INLINE __attribute__((always_inline))

/**
 * @def OZ_BYTE_ORDER
 * Byte order inside a 32-bit integer. 1234 for little endian and 4321 for big endian machines.
 */
#define OZ_BYTE_ORDER __BYTE_ORDER__

/**
 * @def OZ_INTERNAL
 * Compiler-specific attribute for functions that should not be exported from a shared library and
 * are only called from withing their compilation units.
 */
#ifdef _WIN32
# define OZ_INTERNAL
#else
# define OZ_INTERNAL __attribute__((visibility("internal")))
#endif

/**
 * @def OZ_NORETURN
 * Compiler-specific attribute that marks a function as no-return.
 */
#define OZ_NORETURN __attribute__((noreturn))

/**
 * @def OZ_PRINTF_FORMAT
 * Compiler-specific attribute that specifies checking of printf-like arguments.
 */
#ifdef _WIN32
# define OZ_PRINTF_FORMAT(s, first) __attribute__((format(gnu_printf, s, first)))
#else
# define OZ_PRINTF_FORMAT(s, first) __attribute__((format(printf, s, first)))
#endif

/**
 * @def OZ_SIZEOF_LONG
 * Size of `long` type.
 */
#define OZ_SIZEOF_LONG __SIZEOF_LONG__

/**
 * @def OZ_WEAK
 * Compiler-specific attribute specifying a weak symbol.
 */
#define OZ_WEAK __attribute__((weak))

/**
 * @def soft_assert
 * If condition fails, raise SIGTRAP and print error using global log.
 */
#ifdef NDEBUG
# define soft_assert(cond) void(0)
#else
# define soft_assert(cond) \
  ((cond) ? \
   void(0) : oz::detail::softAssertHelper(__PRETTY_FUNCTION__, __FILE__, __LINE__, #cond))
#endif

/**
 * @def hard_assert
 * If condition fails, raise SIGTRAP, print error using global log and abort program.
 */
#ifdef NDEBUG
# define hard_assert(cond) void(0)
#else
# define hard_assert(cond) \
  ((cond) ? \
   void(0) : oz::detail::hardAssertHelper(__PRETTY_FUNCTION__, __FILE__, __LINE__, #cond))
#endif

/**
 * Top-level OpenZone namespace.
 */
namespace oz
{

/**
 * Null pointer type.
 */
typedef std::nullptr_t nullptr_t;

/**
 * Platform-dependent unsigned integer type for memory offsets and sizes.
 */
typedef std::size_t size_t;

/**
 * Platform-dependent signed integer type for memory offsets and pointer differences.
 */
typedef std::ptrdiff_t ptrdiff_t;

/**
 * Initialiser list.
 */
template <typename Elem>
using InitialiserList = std::initializer_list<Elem>;

/**
 * Signed byte.
 */
typedef signed char byte;

/**
 * Unsigned byte.
 */
typedef unsigned char ubyte;

/**
 * Unsigned short integer.
 */
typedef unsigned short ushort;

/**
 * Unsigned integer.
 */
typedef unsigned int uint;

/**
 * Unsigned long integer.
 */
typedef unsigned long ulong;

/**
 * Signed 64-bit integer.
 */
typedef long long long64;

/**
 * Unsigned 64-bit integer.
 */
typedef unsigned long long ulong64;

static_assert(sizeof(char  ) == 1, "sizeof(char) should be 1"  );
static_assert(sizeof(short ) == 2, "sizeof(short) should be 2" );
static_assert(sizeof(int   ) == 4, "sizeof(int) should be 4"   );
static_assert(sizeof(long64) == 8, "sizeof(long64) should be 8");
static_assert(sizeof(float ) == 4, "sizeof(float) should be 4" );
static_assert(sizeof(double) == 8, "sizeof(double) should be 8");

/**
 * Internal helpers.
 */
namespace detail
{

/**
 * Helper function for `soft_assert` macro.
 */
void softAssertHelper(const char* function, const char* file, int line, const char* message);

/**
 * Helper function for `hard_assert` macro.
 */
OZ_NORETURN
void hardAssertHelper(const char* function, const char* file, int line, const char* message);

/**
 * Base class for iterators.
 *
 * It should only be used as a base class. The following functions have to be implemented:
 * - default constructor that creates an invalid iterator and
 * - `Iterator& operator ++ ()`.
 */
template <typename Elem>
class IteratorBase
{
public:

  /**
   * Element type.
   */
  typedef Elem ElemType;

protected:

  Elem* elem = nullptr; ///< The element the iterator is currently pointing at.

protected:

  /**
   * Create an invalid iterator.
   */
  IteratorBase() = default;

  /**
   * Create an iterator pointing to a given element.
   */
  OZ_ALWAYS_INLINE
  explicit IteratorBase(Elem* first) :
    elem(first)
  {}

public:

  /**
   * True iff iterators point to the same element.
   */
  OZ_ALWAYS_INLINE
  bool operator == (const IteratorBase& i) const
  {
    return elem == i.elem;
  }

  /**
   * False iff iterators point to the same element.
   */
  OZ_ALWAYS_INLINE
  bool operator != (const IteratorBase& i) const
  {
    return elem != i.elem;
  }

  /**
   * True as long as iterator has not passed all elements.
   */
  OZ_ALWAYS_INLINE
  bool isValid() const
  {
    return elem != nullptr;
  }

  /**
   * Pointer to the current element.
   */
  OZ_ALWAYS_INLINE
  operator Elem* () const
  {
    return elem;
  }

  /**
   * Reference to the current element.
   */
  OZ_ALWAYS_INLINE
  Elem& operator * () const
  {
    return *elem;
  }

  /**
   * Access to the current element's member.
   */
  OZ_ALWAYS_INLINE
  Elem* operator -> () const
  {
    return elem;
  }

  /**
   * Advance to the next element; should be implemented in derived classes.
   */
  IteratorBase& operator ++ () = delete;

  /**
   * STL-style begin iterator; should be implemented in derived classes.
   */
  IteratorBase begin() const = delete;

  /**
   * STL-style end iterator; should be implemented in derived classes.
   */
  IteratorBase end() const = delete;

};

}

/**
 * Iterator with constant element access for a container (same as `container.citerator()`).
 */
template <class Container>
OZ_ALWAYS_INLINE
inline typename Container::CIterator citerator(const Container& container)
{
  return container.citerator();
}

/**
 * Iterator with non-constant element access for a container (same as `container.iterator()`).
 */
template <class Container>
OZ_ALWAYS_INLINE
inline typename Container::Iterator iterator(Container& container)
{
  return container.iterator();
}

/**
 * Swap values of variables.
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline void swap(Value& a, Value& b)
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
 * `a` if `a >= b`, `b` otherwise.
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline constexpr const Value& max(const Value& a, const Value& b)
{
  return a < b ? b : a;
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
 * Generic less function object, defaults to `operator <`.
 */
template <typename Type = void>
struct Less
{
  /**
   * Compare using `operator <`.
   */
  template <typename TypeA, typename TypeB>
  OZ_ALWAYS_INLINE
  constexpr bool operator () (const TypeA& a, const TypeB& b) const
  {
    return a < b;
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
  constexpr int operator () (const Number& value) const
  {
    return int(value);
  }
};

/**
 * Hash function for strings.
 */
template <>
struct Hash<const char*>
{
  /// %Hash value for an empty string.
  static const int EMPTY = 5381;

  /**
   * (Modified) Bernstein's hash.
   */
  OZ_ALWAYS_INLINE
  int operator () (const char* s) const
  {
    uint value = EMPTY;

    while (*s != '\0') {
      value = (value * 33) ^ int(*s);
      ++s;
    }
    return value;
  }
};

}
