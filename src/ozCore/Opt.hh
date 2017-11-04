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
 * @file ozCore/Opt.hh
 *
 * `Opt` class template.
 */

#pragma once

#include "System.hh"

/**
 * @def OZ_UNWRAP
 * Wrapper for `oz::Opt::unwrap()`, filling in the current function, file and line parameters.
 */
#define OZ_UNWRAP(...) \
  unwrap(__PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

namespace oz
{

/**
 * Optional value.
 */
template <typename Type>
class Opt
{
private:

  char data_[sizeof(Type)];         ///< Value.
  bool hasValue_           = false; ///< True iff has value.

private:

  /**
   * Internal helper to access the value.
   */
  OZ_ALWAYS_INLINE
  const Type& asValue() const
  {
    return *reinterpret_cast<const Type*>(data_);
  }

  /**
   * Internal helper to access the value.
   */
  OZ_ALWAYS_INLINE
  Type& asValue()
  {
    return *reinterpret_cast<Type*>(data_);
  }

public:

  /**
   * Create a value-less instance.
   */
  OZ_ALWAYS_INLINE
  Opt() = default;

  /**
   * Create a value-less instance.
   */
  OZ_ALWAYS_INLINE
  Opt(Void) noexcept
  {}

  /**
   * Create from a valid value.
   */
  template <typename Type_>
  OZ_ALWAYS_INLINE
  Opt(Type_&& value)
    : hasValue_(true)
  {
    new(data_) Type(static_cast<Type_&&>(value));
  }

  /**
   * Copy constructor.
   *
   * - If `other` has value this instance's value is copy-constructed.
   */
  OZ_ALWAYS_INLINE
  Opt(const Opt& other)
    : hasValue_(other.hasValue_)
  {
    if (other.hasValue_) {
      new(data_) Type(other.asValue());
    }
  }

  /**
   * Copy constructor.
   *
   * - If `other` has value this instance's value is move-constructed.
   */
  OZ_ALWAYS_INLINE
  Opt(Opt&& other) noexcept
    : hasValue_(other.hasValue_)
  {
    if (other.hasValue_) {
      new(data_) Type(static_cast<Type&&>(other.asValue()));

      other.hasValue_ = false;
    }
  }

  /**
   * Copy operator.
   *
   * - If both instances have values copy operator is used.
   * - If only this instance has value it is destructed.
   * - If only `other` has value this instance's value is copy-constructed.
   */
  OZ_ALWAYS_INLINE
  Opt& operator=(const Opt& other)
  {
    if (&other != this) {
      if (hasValue_ && other.hasValue_) {
        asValue() = other.asValue();
      }
      else if (hasValue_) {
        asValue().~Type();
        hasValue_ = false;
      }
      else if (other.hasValue_) {
        new(data_) Type(other.asValue());
        hasValue_ = true;
      }
    }
    return *this;
  }

  /**
   * Move operator.
   *
   * - If both instances have values move operator is used.
   * - If only this instance has value it is destructed.
   * - If only `other` has value this instance's value is move-constructed.
   */
  OZ_ALWAYS_INLINE
  Opt& operator=(Opt&& other) noexcept
  {
    if (&other != this) {
      if (hasValue_ && other.hasValue_) {
        asValue() = static_cast<Type&&>(other.asValue());

        other.hasValue_ = false;
      }
      else if (hasValue_) {
        asValue().~Type();
        hasValue_ = false;
      }
      else if (other.hasValue_) {
        new(data_) Type(static_cast<Type&&>(other.asValue()));
        hasValue_ = true;

        other.hasValue_ = false;
      }
    }
    return *this;
  }

  /**
   * Destroy value if exists and make value-less.
   */
  OZ_ALWAYS_INLINE
  Opt& operator=(Void) noexcept
  {
    if (hasValue_) {
      asValue().~Type();
      hasValue_ = false;
    }
    return *this;
  }

  /**
   * Assign a value.
   *
   * - If it has value copy/move operator is used.
   * - If it is value-less the value is copy/move-constructed.
   */
  template <typename Type_>
  OZ_ALWAYS_INLINE
  Opt& operator=(Type_&& value)
  {
    if (hasValue_) {
      asValue() = static_cast<Type_&&>(value);
    }
    else {
      new(data_) Type(static_cast<Type_&&>(value));
      hasValue_ = true;
    }
    return *this;
  }

  /**
   * Both instances are value-less or values are equal.
   */
  OZ_ALWAYS_INLINE
  bool operator==(const Opt& other)
  {
    return (hasValue_ == other.hasValue_) && (!hasValue_ || asValue() == other.asValue());
  }

  /**
   * Instance has value and is equal to a given value.
   */
  template <typename Type_>
  OZ_ALWAYS_INLINE
  bool operator==(const Type_& v)
  {
    return hasValue_ && asValue() == v;
  }

  /**
   * This instance is value-less and `other` is not or this instance's value is less than `other`'s.
   */
  OZ_ALWAYS_INLINE
  bool operator<(const Opt& other)
  {
    return other.hasValue_ && (!hasValue_ || asValue() < other.asValue());
  }

  /**
   * This instance is value-less or less than a given value.
   */
  template <typename Type_>
  OZ_ALWAYS_INLINE
  bool operator<(const Type_& v)
  {
    return asValue() < v;
  }

  /**
   * This instance has value and is greater than a given one.
   */
  template <typename Type_>
  OZ_ALWAYS_INLINE
  friend bool operator<(const Type_& a, const Opt& b)
  {
    return b.hasValue_ && a < b.asValue();
  }

  /**
   * Constant access to member.
   */
  OZ_ALWAYS_INLINE
  const Type* operator->() const
  {
    OZ_ASSERT(hasValue_);

    return &asValue();
  }

  /**
   * Access to member.
   */
  OZ_ALWAYS_INLINE
  Type* operator->()
  {
    OZ_ASSERT(hasValue_);

    return &asValue();
  }

  /**
   * Constant reference to value.
   */
  OZ_ALWAYS_INLINE
  const Type& operator*() const&
  {
    OZ_ASSERT(hasValue_);

    return asValue();
  }

  /**
   * Reference to value.
   */
  OZ_ALWAYS_INLINE
  Type& operator*() &
  {
    OZ_ASSERT(hasValue_);

    return asValue();
  }

  /**
   * Constant reference to value on constant temporary.
   */
  OZ_ALWAYS_INLINE
  const Type&& operator*() const&&
  {
    OZ_ASSERT(hasValue_);

    return static_cast<Type&&>(asValue());
  }

  /**
   * Reference to value on temporary.
   */
  OZ_ALWAYS_INLINE
  Type&& operator*() &&
  {
    OZ_ASSERT(hasValue_);

    return static_cast<Type&&>(asValue());
  }

  /**
   * True iff has value.
   */
  OZ_ALWAYS_INLINE
  explicit operator bool() const noexcept
  {
    return hasValue_;
  }

  /**
   * Value or `defaultValue` if value-less.
   */
  template <typename Type_ = Type>
  OZ_ALWAYS_INLINE
  Type valueOr(Type_&& defaultValue = Type()) const&
  {
    return hasValue_ ? asValue() : static_cast<Type>(static_cast<Type_&&>(defaultValue));
  }

  /**
   * Value or `defaultValue` if value-less.
   */
  template <typename Type_ = Type>
  OZ_ALWAYS_INLINE
  Type valueOr(Type_&& defaultValue = Type()) &&
  {
    return hasValue_
        ? static_cast<Type&&>(asValue())
        : static_cast<Type&&>(static_cast<Type_&&>(defaultValue));
  }

  /**
   * Return value if exists or crash with error otherwise.
   */
  const Type& unwrap() const&
  {
    if (!hasValue_) {
      OZ_ERROR("oz::Opt: Missing value");
    }
    return asValue();
  }

  /**
   * Return value if exists or crash with error otherwise.
   */
  Type& unwrap() &
  {
    if (!hasValue_) {
      OZ_ERROR("oz::Opt: Missing value");
    }
    return asValue();
  }

  /**
   * Return value if exists or crash with error otherwise.
   */
  const Type&& unwrap() const&&
  {
    if (!hasValue_) {
      OZ_ERROR("oz::Opt: Missing value");
    }
    return static_cast<Type&&>(asValue());
  }

  /**
   * Return value if exists or crash with error otherwise.
   */
  Type&& unwrap() &&
  {
    if (!hasValue_) {
      OZ_ERROR("oz::Opt: Missing value");
    }
    return static_cast<Type&&>(asValue());
  }

  /**
   * Return value if exists or crash with a given error message otherwise.
   */
  OZ_PRINTF_FORMAT(5, 6)
  const Type& unwrap(const char* function, const char* file, int line,
                     const char* message, ...) const&
  {
    if (!hasValue_) {
      va_list ap;
      va_start(ap, message);

      System::verror(function, file, line, 1, message, ap);
    }
    return asValue();
  }

  /**
   * Return value if exists or crash with a given error message otherwise.
   */
  OZ_PRINTF_FORMAT(5, 6)
  Type& unwrap(const char* function, const char* file, int line, const char* message, ...) &
  {
    if (!hasValue_) {
      va_list ap;
      va_start(ap, message);

      System::verror(function, file, line, 1, message, ap);
    }
    return asValue();
  }

  /**
   * Return value if exists or crash with a given error message otherwise.
   */
  OZ_PRINTF_FORMAT(5, 6)
  const Type&& unwrap(const char* function, const char* file, int line,
                      const char* message, ...) const&&
  {
    if (!hasValue_) {
      va_list ap;
      va_start(ap, message);

      System::verror(function, file, line, 1, message, ap);
    }
    return static_cast<Type&&>(asValue());
  }

  /**
   * Return value if exists or crash with a given error message otherwise.
   */
  OZ_PRINTF_FORMAT(5, 6)
  Type&& unwrap(const char* function, const char* file, int line, const char* message, ...) &&
  {
    if (!hasValue_) {
      va_list ap;
      va_start(ap, message);

      System::verror(function, file, line, 1, message, ap);
    }
    return static_cast<Type&&>(asValue());
  }

};

}
