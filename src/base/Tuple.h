/*
 *  Tuple.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

namespace oz
{

  template <typename Head, class Tail = Nil>
  struct TupleList
  {
    Head head;
    Tail tail;

    TupleList() : head(), tail()
    {}

    template <typename Type0, typename Type1, typename Type2, typename Type3>
    TupleList( Type0& t0, Type1& t1, Type2& t2, Type3& t3 ) :
	head( t0 ), tail( t1, t2, t3, Nil() )
    {}

    template <typename Type0>
    TupleList( Type0& t0, const Nil&, const Nil&, const Nil& ) : head( t0 )
    {}

    template <typename Head_, class Tail_>
    TupleList( const TupleList<Head_, Tail_>& tl ) : head( tl.head ), tail( tl.tail )
    {}

    template <typename Head_, class Tail_>
    TupleList& operator = ( const TupleList<Head_, Tail_>& tl )
    {
      head = tl.head;
      tail = tl.tail;
      return *this;
    }

    TupleList& operator = ( const TupleList& tl )
    {
      head = tl.head;
      tail = tl.tail;
      return *this;
    }

    template <typename Head_, class Tail_>
    bool operator == ( const TupleList<Head_, Tail_>& tl ) const
    {
      return head == tl.head && tail == tl.tail;
    }

    template <typename Head_, class Tail_>
    bool operator != ( const TupleList<Head_, Tail_>& tl ) const
    {
      return head != tl.head || tail != tl.tail;
    }

  };

  template <typename Type0, typename Type1, typename Type2, typename Type3>
  struct ConstructTupleList
  {
    typedef TupleList<Type0, typename ConstructTupleList<Type1, Type2, Type3, Nil>::Type> Type;
  };

  template <typename Type0>
  struct ConstructTupleList<Type0, Nil, Nil, Nil>
  {
    typedef TupleList<Type0> Type;
  };

  template <typename Type>
  struct TupleTypeCtor
  {
    static Type ctor()
    {
      return Type();
    }
  };

  template <typename Type>
  struct TupleTypeConvert
  {
    typedef	  Type  Plain;
    typedef	  Type& Ref;
    typedef const Type& CRef;
  };

  template <typename Type>
  struct TupleTypeConvert<Type&>
  {
    typedef       Type  Plain;
    typedef	  Type& Ref;
    typedef const Type& CRef;
  };

  template <typename Type0, typename Type1 = Nil, typename Type2 = Nil, typename Type3 = Nil>
  struct Tuple : ConstructTupleList<Type0, Type1, Type2, Type3>::Type
  {
    Tuple() {}
    Tuple( const Type0& t0 = TupleTypeCtor<Type0>::ctor(),
	   const Type1& t1 = TupleTypeCtor<Type1>::ctor(),
	   const Type2& t2 = TupleTypeCtor<Type2>::ctor(),
	   const Type3& t3 = TupleTypeCtor<Type3>::ctor() ) :
        ConstructTupleList<Type0, Type1, Type2, Type3>::Type( t0, t1, t2, t3 )
    {}

    template <typename Type0_, typename Type1_, typename Type2_, typename Type3_>
    Tuple( const Tuple<Type0_, Type1_, Type2_, Type3_>& t ) :
        ConstructTupleList<Type0, Type1, Type2, Type3>::Type( t )
    {}

    template <typename Type0_, typename Type1_, typename Type2_, typename Type3_>
    Tuple& operator = ( const Tuple<Type0_, Type1_, Type2_, Type3_>& t )
    {
      ConstructTupleList<Type0, Type1, Type2, Type3>::Type::operator = ( t );
      return *this;
    }
  };

  template <int INDEX, class TupleList>
  struct ResolveTupleElemType;

  template <int INDEX, typename Head, class Tail>
  struct ResolveTupleElemType<INDEX, TupleList<Head, Tail> >
  {
    typedef typename ResolveTupleElemType<INDEX - 1, Tail>::Type Type;
  };

  template <typename Head, class Tail>
  struct ResolveTupleElemType<0, TupleList<Head, Tail> >
  {
    typedef Head Type;
  };

  template <int INDEX>
  struct TupleGetHelper
  {
    template <typename Head, class Tail>
    static const typename ResolveTupleElemType<INDEX, TupleList<Head, Tail> >::Type&
    get( const TupleList<Head, Tail>& tl )
    {
      return TupleGetHelper<INDEX - 1>::get( tl.tail );
    }
    template <typename Head, class Tail>
    static typename ResolveTupleElemType<INDEX, TupleList<Head, Tail> >::Type&
    get( TupleList<Head, Tail>& tl )
    {
      return TupleGetHelper<INDEX - 1>::get( tl.tail );
    }
  };

  template <>
  struct TupleGetHelper<0>
  {
    template <typename Head, class Tail>
    static const typename ResolveTupleElemType<0, TupleList<Head, Tail> >::Type&
    get( const TupleList<Head, Tail>& tl )
    {
      return tl.head;
    }

    template <typename Head, class Tail>
    static typename ResolveTupleElemType<0, TupleList<Head, Tail> >::Type&
    get( TupleList<Head, Tail>& tl )
    {
      return tl.head;
    }
  };

  template <int INDEX, typename Head, class Tail>
  const typename ResolveTupleElemType<INDEX, TupleList<Head, Tail> >::Type&
  get( const TupleList<Head, Tail>& tb )
  {
    return TupleGetHelper<INDEX>::get( tb );
  }

  template <int INDEX, typename Head, class Tail>
  typename ResolveTupleElemType<INDEX, TupleList<Head, Tail> >::Type&
  get( TupleList<Head, Tail>& tb )
  {
    return TupleGetHelper<INDEX>::get( tb );
  }

  template <typename Type0, typename Type1, typename Type2, typename Type3>
  inline Tuple<Type0, Type1, Type2, Type3>
  tuple( const Type0& e0 = TupleTypeCtor<Type0>::ctor(),
	 const Type1& e1 = TupleTypeCtor<Type1>::ctor(),
	 const Type2& e2 = TupleTypeCtor<Type2>::ctor(),
	 const Type3& e3 = TupleTypeCtor<Type3>::ctor() )
  {
    return Tuple<Type0, Type1, Type2, Type3>( e0, e1, e2, e3 );
  }

  template <typename Type0, typename Type1, typename Type2>
  inline Tuple<Type0, Type1, Type2>
  tuple( const Type0& e0 = TupleTypeCtor<Type0>::ctor(),
	 const Type1& e1 = TupleTypeCtor<Type1>::ctor(),
	 const Type2& e2 = TupleTypeCtor<Type2>::ctor() )
  {
    return Tuple<Type0, Type1, Type2>( e0, e1, e2 );
  }

  template <typename Type0, typename Type1>
  inline Tuple<Type0, Type1>
  tuple( const Type0& e0 = TupleTypeCtor<Type0>::ctor(),
	 const Type1& e1 = TupleTypeCtor<Type1>::ctor() )
  {
    return Tuple<Type0, Type1>( e0, e1 );
  }

  template <typename Type0>
  inline Tuple<Type0>
  tuple( const Type0& e0 = TupleTypeCtor<Type0>::ctor() )
  {
    return Tuple<Type0>( e0 );
  }

  template <typename Type0, typename Type1, typename Type2, typename Type3>
  inline Tuple<Type0&, Type1&, Type2&, Type3&>
  tie( Type0& e0, Type1& e1, Type2& e2, Type3& e3 )
  {
    return Tuple<Type0&, Type1&, Type2&, Type3&>( e0, e1, e2, e3 );
  }

  template <typename Type0, typename Type1, typename Type2>
  inline Tuple<Type0&, Type1&, Type2&>
  tie( Type0& e0, Type1& e1, Type2& e2 )
  {
    return Tuple<Type0&, Type1&, Type2&>( e0, e1, e2 );
  }

  template <typename Type0, typename Type1>
  inline Tuple<Type0&, Type1&>
  tie( Type0& e0, Type1& e1 )
  {
    return Tuple<Type0&, Type1&>( e0, e1 );
  }

  template <typename Type0>
  inline Tuple<Type0&>
  tie( Type0& e0 )
  {
    return Tuple<Type0&>( e0 );
  }
}
