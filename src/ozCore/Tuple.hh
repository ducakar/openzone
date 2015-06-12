/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/Tuple.hh
 *
 * `Tuple` class template.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %Tuple of an arbitrary number of elements.
 */
template <typename Head, typename... Tail>
struct Tuple : public Tuple<Tail...>
{
  Head head; ///< First element in (the rest of) the tuple.

  /**
   * Default constructor.
   */
  OZ_ALWAYS_INLINE
  constexpr Tuple() = default;

  /**
   * Construct tuple from given elements.
   */
  template <typename Head_ = Head, typename... Tail_>
  OZ_ALWAYS_INLINE
  constexpr Tuple(Head&& h, Tail_&&... t) :
    Tuple<Tail...>(static_cast<Tail_&&>(t)...), head(static_cast<Head_&&>(h))
  {}

  /**
   * Construct from a tuple of differing but compatible types.
   */
  template <typename Head_, typename... Tail_>
  OZ_ALWAYS_INLINE
  constexpr Tuple(const Tuple<Head_, Tail_...>& t) :
    Tuple<Tail...>(static_cast<const Tuple<Tail_...>&>(t)), head(t.head)
  {}

  /**
   * Construct from a tuple of differing but compatible types.
   */
  template <typename Head_, typename... Tail_>
  OZ_ALWAYS_INLINE
  constexpr Tuple(Tuple<Head_, Tail_...>&& t) :
    Tuple<Tail...>(static_cast<Tuple<Tail_...>&&>(t)), head(static_cast<Head_&&>(t.head))
  {}

  /**
   * Assign from a tuple of differing but compatible types.
   */
  template <typename Head_, typename... Tail_>
  OZ_ALWAYS_INLINE
  Tuple& operator = (const Tuple<Head_, Tail_...>& t)
  {
    head = t.head;
    Tuple<Tail...>::operator = (static_cast<const Tuple<Tail_...>&>(t));
    return *this;
  }

  /**
   * Assign from a tuple of differing but compatible types.
   */
  template <typename Head_, typename... Tail_>
  OZ_ALWAYS_INLINE
  Tuple& operator = (Tuple<Head_, Tail_...>&& t)
  {
    head = static_cast<Head_&&>(t.head);
    Tuple<Tail...>::operator = (static_cast<Tuple<Tail_...>&&>(t));
    return *this;
  }

  /**
   * Per-element equality operator.
   */
  OZ_ALWAYS_INLINE
  constexpr bool operator == (const Tuple& t) const
  {
    return head == t.head && Tuple<Tail...>::operator == (t);
  }

  /**
   * Per-element inequality operator.
   */
  OZ_ALWAYS_INLINE
  constexpr bool operator != (const Tuple& t) const
  {
    return !operator == (t);
  }

};

/**
 * `Tuple` base class, specialisation for one element.
 */
template <typename Head>
struct Tuple<Head>
{
  Head head; ///< The only element in the tuple.

  /**
   * Default constructor.
   */
  OZ_ALWAYS_INLINE
  constexpr Tuple() = default;

  /**
   * Construct tuple from a given element.
   */
  template <typename Head_ = Head>
  OZ_ALWAYS_INLINE
  constexpr Tuple(Head_&& h) :
    head(static_cast<Head_&&>(h))
  {}

  /**
   * Construct from a tuple of differing but compatible types.
   */
  template <typename Head_>
  OZ_ALWAYS_INLINE
  constexpr Tuple(const Tuple<Head_>& t) :
    head(t.head)
  {}

  /**
   * Construct from a tuple of differing but compatible types.
   */
  template <typename Head_>
  OZ_ALWAYS_INLINE
  constexpr Tuple(Tuple<Head_>&& t) :
    head(static_cast<Head_&&>(t.head))
  {}

  /**
   * Assign from a tuple of differing but compatible types.
   */
  template <typename Head_>
  OZ_ALWAYS_INLINE
  Tuple& operator = (const Tuple<Head_>& t)
  {
    head = t.head;
    return *this;
  }

  /**
   * Assign from a tuple of differing but compatible types.
   */
  template <typename Head_>
  OZ_ALWAYS_INLINE
  Tuple& operator = (Tuple<Head_>&& t)
  {
    head = static_cast<Head_&&>(t.head);
    return *this;
  }

  /**
   * Per-element equality operator.
   */
  OZ_ALWAYS_INLINE
  constexpr bool operator == (const Tuple& t) const
  {
    return head == t.head;
  }

  /**
   * Per-element inequality operator.
   */
  OZ_ALWAYS_INLINE
  constexpr bool operator != (const Tuple& t) const
  {
    return !operator == (t);
  }

};

namespace detail
{

/**
 * Helper template class for `get<N>()` function.
 */
template <int N, typename Head, typename... Tail>
struct TupleGet
{
  /**
   * Type of the N-th tuple member.
   */
  typedef typename TupleGet<N - 1, Tail...>::Type Type;

  /**
   * Constant reference the N-th member of a given tuple.
   */
  OZ_ALWAYS_INLINE
  static constexpr const Type& get(const Tuple<Head, Tail...>& t)
  {
    return TupleGet<N - 1, Tail...>::get(t);
  }

  /**
   * Reference the N-th member of a given tuple.
   */
  OZ_ALWAYS_INLINE
  static constexpr Type& get(Tuple<Head, Tail...>& t)
  {
    return TupleGet<N - 1, Tail...>::get(t);
  }
};

/**
 * Helper template class for `get<N>()` function (specialisation for N = 0).
 */
template <typename Head, typename... Tail>
struct TupleGet<0, Head, Tail...>
{
  /**
   * Type of the first tuple member.
   */
  typedef Head Type;

  /**
   * Constant reference the N-th member of a given tuple.
   */
  OZ_ALWAYS_INLINE
  static constexpr const Type& get(const Tuple<Head, Tail...>& t)
  {
    return t.head;
  }

  /**
   * Reference the first member of a given tuple.
   */
  OZ_ALWAYS_INLINE
  static constexpr Type& get(Tuple<Head, Tail...>& t)
  {
    return t.head;
  }
};

}

/**
 * Constant reference to the N-th element in a tuple.
 */
template <int N, typename... Args>
OZ_ALWAYS_INLINE
inline constexpr const typename detail::TupleGet<N, Args...>::Type& get(const Tuple<Args...>& tuple)
{
  return detail::TupleGet<N, Args...>::get(tuple);
}

/**
 * Reference to the N-th element in a tuple.
 */
template <int N, typename... Args>
OZ_ALWAYS_INLINE
inline constexpr typename detail::TupleGet<N, Args...>::Type& get(Tuple<Args...>& tuple)
{
  return detail::TupleGet<N, Args...>::get(tuple);
}

/**
 * Construct a tuple from parametrs list.
 */
template <typename... Args>
OZ_ALWAYS_INLINE
inline constexpr Tuple<Args...> tuple(Args&&... args)
{
  return Tuple<Args...>(static_cast<Args&&>(args)...);
}

/**
 * Tie variables into a tuple of references.
 */
template <typename... Args>
OZ_ALWAYS_INLINE
inline constexpr Tuple<Args&...> tie(Args&... args)
{
  return Tuple<Args&...>(args...);
}

}
