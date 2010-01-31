/*
 *  Tuple.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{

  template <typename Head, typename Tail = Nil>
  class TupleBase
  {
    public:

      /**
       * Recursive templates to resolve type of N-th element of Tuple:
       * ResolveType&lt;N, Tuple&lt;Head, Tail&gt;&gt; -&gt; ResolveType&lt;N - 1, Tail&gt;
       * ResolveType&lt;0, Tuple&lt;Head, Tail&gt;&gt; -&gt; Head
       * ResolveType&lt;N, Nil&gt; -&gt; void
       * The last rule is used to print out less confusing error message if requested INDEX is out
       * of bounds.
       */
      template <int INDEX, class TupleBase = void>
      struct ResolveType;

      template <int INDEX, typename HeadType, class TailType>
      struct ResolveType<INDEX, TupleBase<HeadType, TailType> >
      {
        typedef typename ResolveType<INDEX - 1, TailType>::Type Type;
      };

      template <typename HeadType, class TailType>
      struct ResolveType<0, TupleBase<HeadType, TailType> >
      {
        typedef HeadType Type;
      };

      template <int INDEX>
      struct ResolveType<INDEX, Nil>
      {
        typedef void Type;
      };

      template <int INDEX, typename Dummy = void>
      struct GetHelper
      {
        static typename ResolveType<INDEX, TupleBase>::Type&
        get( TupleBase& tb )
        {
          return Tail::template GetHelper<INDEX - 1>::get( tb.tail );
        }
      };

      template <typename Dummy>
      struct GetHelper<0, Dummy>
      {
        static typename ResolveType<0, TupleBase>::Type&
        get( TupleBase& tb )
        {
          return tb.head;
        }
      };

      Head head;
      Tail tail;

      TupleBase() {}
      TupleBase( const Head& head_ ) : head( head_ ) {}
      TupleBase( const Head& head_, const Tail& tail_ ) : head( head_ ), tail( tail_ ) {}

      bool operator == ( const TupleBase& tb ) const
      {
        return head == tb.head && tail == tb.tail;
      }

      bool operator != ( const TupleBase& tb ) const
      {
        return head != tb.head || tail != tb.tail;
      }
  };

  template <typename TypeA, typename TypeB = void, typename TypeC = void, typename TypeD = void>
  struct Tuple : TupleBase<TypeA, TupleBase<TypeB, TupleBase<TypeC, TupleBase<TypeD> > > >
  {
    Tuple() {}
    Tuple( const TypeA& a, const TypeB& b, const TypeC& c, const TypeD& d ) :
        TupleBase<TypeA, TupleBase<TypeB, TupleBase<TypeC, TupleBase<TypeD> > > >
        ( a, TupleBase<TypeB, TupleBase<TypeC, TupleBase<TypeD> > >
        ( b, TupleBase<TypeC, TupleBase<TypeD> >
        ( c, TupleBase<TypeD>
        ( d ) ) ) ) {}
  };

  template <typename TypeA, typename TypeB, typename TypeC>
  struct Tuple<TypeA, TypeB, TypeC> : TupleBase<TypeA, TupleBase<TypeB, TupleBase<TypeC> > >
  {
    Tuple() {}
    Tuple( const TypeA& a, const TypeB& b, const TypeC &c ) :
        TupleBase<TypeA, TupleBase<TypeB, TupleBase<TypeC> > >
        ( a, TupleBase<TypeB, TupleBase<TypeC> >
        ( b, TupleBase<TypeC>
        ( c ) ) ) {}
  };

  template <typename TypeA, typename TypeB>
  struct Tuple<TypeA, TypeB> : TupleBase<TypeA, TupleBase<TypeB> >
  {
    Tuple() {}
    Tuple( const TypeA& a, const TypeB& b ) :
        TupleBase<TypeA, TupleBase<TypeB> >( a, TupleBase<TypeB>( b ) ) {}
  };

  template <typename TypeA>
  struct Tuple<TypeA> : TupleBase<TypeA>
  {
    Tuple() {}
    Tuple( const TypeA& a ) : TupleBase<TypeA>( a ) {}
  };

  /**
   * Get INDEX-th element of Tuple.
   * Use as:
   * <code>
   * Tuple&lt;int, char&gt; t( 1, 2 );
   * get&lt;1&gt;( t );
   * @param t
   * @return
   */
  template <int INDEX, typename Head, class Tail>
  inline typename TupleBase<Head, Tail>::template ResolveType<INDEX, TupleBase<Head, Tail> >::Type&
  get( TupleBase<Head, Tail>& t )
  {
    return TupleBase<Head, Tail>::template GetHelper<INDEX>::get( t );
  }

}
