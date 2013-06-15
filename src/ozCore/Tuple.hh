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

template <class Head, typename... Tail>
class Tuple;

namespace detail
{

/**
 * Helper to derive the N-th member type of a tuple.
 */
template <int N, typename Head, typename... Tail>
struct TupleTypeN
{
  /**
   * Type of the N-th tuple member.
   */
  typedef typename TupleTypeN<N - 1, Tail...>::Type Type;
};

/**
 * Helper to derive the N-th member type of a tuple (specialisation for N = 0).
 */
template <typename Head, typename... Tail>
struct TupleTypeN<0, Head, Tail...>
{
  /**
   * Type of the first tuple member.
   */
  typedef Head Type;
};

/**
 * Helper class template for `Tuple::get<N>()` function.
 */
template <int N, typename Head, typename... Tail>
struct TupleGetter
{
  /**
   * Type of the N-th tuple member.
   */
  typedef typename TupleTypeN<N, Head, Tail...>::Type Type;

  /**
   * Constant reference the N-th member of a given tuple.
   */
  static const Type& get( const Tuple<Head, Tail...>& t )
  {
    return TupleGetter<N - 1, Tail...>::get( static_cast< const Tuple<Tail...>& >( t ) );
  }

  /**
   * Reference the N-th member of a given tuple.
   */
  static Type& get( Tuple<Head, Tail...>& t )
  {
    return TupleGetter<N - 1, Tail...>::get( static_cast< Tuple<Tail...>& >( t ) );
  }
};

/**
 * Helper class template for `Tuple::get<N>()` function (specialisation for N = 0).
 */
template <typename Head, typename... Tail>
struct TupleGetter<0, Head, Tail...>
{
  /**
   * Type of the first tuple member.
   */
  typedef Head Type;

  /**
   * Constant reference the N-th member of a given tuple.
   */
  static const Type& get( const Tuple<Head, Tail...>& t )
  {
    return t.head;
  }

  /**
   * Reference the first member of a given tuple.
   */
  static Type& get( Tuple<Head, Tail...>& t )
  {
    return t.head;
  }
};

}

/**
 * %Tuple of an arbitrary number of elements.
 */
template <typename Head, typename... Tail>
class Tuple : public Tuple<Tail...>
{
  friend struct detail::TupleGetter<0, Head, Tail...>;

  private:

    Head head; ///< First element in (the rest of) the tuple.

  public:

    /**
     * Default constructor.
     */
    explicit Tuple() = default;

    /**
     * Construct tuple from given elements.
     */
    template <typename Head_ = Head, typename... Tail_>
    Tuple( Head&& h, Tail_&&... t ) :
      Tuple<Tail...>( static_cast<Tail_&&>( t )... ), head( static_cast<Head_&&>( h ) )
    {}

    /**
     * Constant reference to the N-th element in the tuple.
     */
    template <int N>
    const typename detail::TupleGetter<N, Head, Tail...>::Type& get() const
    {
      return detail::TupleGetter<N, Head, Tail...>::get( *this );
    }

    /**
     * Reference to the N-th element in the tuple.
     */
    template <int N>
    typename detail::TupleGetter<N, Head, Tail...>::Type& get()
    {
      return detail::TupleGetter<N, Head, Tail...>::get( *this );
    }

};

/**
 * `Tuple` base class, specialisation for one element.
 */
template <typename Head>
class Tuple<Head>
{
  friend struct detail::TupleGetter<0, Head>;

  private:

    Head head; ///< The only element in (the rest of) the tuple.

  public:

    /**
     * Default constructor.
     */
    explicit Tuple() = default;

    /**
     * Construct tuple from a given element.
     */
    template <typename Head_ = Head>
    Tuple( Head_&& h ) :
      head( static_cast<Head_&&>( h ) )
    {}

    /**
     * Constant reference to the only element the tuple.
     */
    template <int N>
    const typename detail::TupleGetter<N, Head>::Type& get() const
    {
      return detail::TupleGetter<N, Head>::get( *this );
    }

    /**
     * Reference to the only element in the tuple.
     */
    template <int N>
    typename detail::TupleGetter<N, Head>::Type& get()
    {
      return detail::TupleGetter<N, Head>::get( *this );
    }

};

}
